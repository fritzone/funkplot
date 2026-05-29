#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "Function.h"
#include "RuntimeProvider.h"
#include "StatementHandler.h"
#include "util.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <QLineF>
#include <catch2/catch_all.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg: [[fallthrough]];
    case QtWarningMsg: [[fallthrough]];
    case QtInfoMsg:
        break;
    case QtCriticalMsg:  [[fallthrough]];
    case QtFatalMsg:
        fprintf(stderr, "%s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    }
}


static std::tuple<RuntimeProvider::CB_ErrorReporter, RuntimeProvider::CB_StringPrinter, RuntimeProvider::CB_PointDrawer, RuntimeProvider::CB_LineDrawer, RuntimeProvider::CB_StatementTracker, RuntimeProvider::CB_PenSetter, RuntimeProvider::CB_PlotDrawer, RuntimeProvider::CB_TextDrawer> RuntimeProviderParameterProvider()
{
    qInstallMessageHandler(myMessageOutput);

    std::tuple<RuntimeProvider::CB_ErrorReporter, RuntimeProvider::CB_StringPrinter, RuntimeProvider::CB_PointDrawer, RuntimeProvider::CB_LineDrawer, RuntimeProvider::CB_StatementTracker, RuntimeProvider::CB_PenSetter, RuntimeProvider::CB_PlotDrawer, RuntimeProvider::CB_TextDrawer> r =
        {
            [](int l, int c, QString e) { qWarning() << "ERROR" << c << "AT" << l << e;},
            [](QString s) { qWarning() << "PRINT" << s;},
            [](double x, double y, int s) {qInfo() << "POINT AT" << QPointF(x ,y) << "SIZE:" << s; },
            [](double x1, double y1, double x2, double y2, int s) {qInfo() << "LINE AT" << QLineF(x1 ,y1, x2, y2) << "SIZE:" << s; },
            [](QString s) { qInfo() << "CURRENT STATEMENT"<<s;},
            [](int r, int g, int b, int a, int s) { qInfo() << "COLOR" << r << g << b << a << "SIZE:" <<  s; },
            [](QSharedPointer<Plot> p) { qInfo() << "PLOTTING" <<p->plotTarget; },
            [](double, double, const QString&) {}
        };

    return r;
}

TEST_CASE( "Function handling", "[functions]" )
{

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    registerClasses();

    SECTION("Simple function with 2 variables") {
        std::unique_ptr<Function> f  { new Function("f(x,y) = (x+y)/3", nullptr) };
        f->SetVariable(X,3);
        f->SetVariable(Y,3);
        double cc=f->Calculate();
        REQUIRE_THAT(cc, Catch::Matchers::WithinRel(2.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001) );
        REQUIRE(f->get_name() == "f");
    }

    SECTION( "Simple function with 1 variable")
    {
        std::unique_ptr<Function> f  { new Function("f(x) = x*x + 3*x + 2", nullptr) };
        f->SetVariable(X, 1);

        double cc=f->Calculate();
        REQUIRE_THAT(cc, Catch::Matchers::WithinRel(6.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001) );

        REQUIRE(f->get_name() == "f");
    }

    SECTION("Function is a number")
    {
        std::unique_ptr<Function> f  { new Function("f(x) = 3", nullptr) };
        f->SetVariable(X, 1);

        double cc=f->Calculate();
        REQUIRE_THAT(cc, Catch::Matchers::WithinRel(3.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001) );

        REQUIRE(f->get_name() == "f");
    }

    SECTION("Formula has division by zero")
    {
        std::unique_ptr<Function> f  { new Function("f(x) = (x-1)/(x+1)", nullptr) };
        f->SetVariable(X, -1);

        double cc=f->Calculate();

        REQUIRE( std::isnan(cc) );
        REQUIRE(f->get_name() == "f");
    }

    SECTION("Power of sin")
    {
        std::unique_ptr<Function> f  { new Function("f(x) = sin^2(x)", nullptr) };
        f->SetVariable(X, 1.570795); // PI/2
        double cc=f->Calculate();
        REQUIRE_THAT(cc, Catch::Matchers::WithinRel(1.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001) );
        REQUIRE(f->get_name() == "f");
    }

    SECTION("Simple power")
    {
        std::unique_ptr<Function> f  { new Function("f(x) = x^2", nullptr) };
        f->SetVariable(X, 2); // PI/2
        double cc=f->Calculate();
        REQUIRE_THAT(cc, Catch::Matchers::WithinRel(4.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001) );
        REQUIRE(f->get_name() == "f");
    }

    SECTION("Preverifier")
    {
        std::unique_ptr<Function> f  { new Function("f(x) =     a * ( 2 * cos(t) - cos(2 * t) )", nullptr) };
        std::string p = f->getPreverified();
        REQUIRE(p == "a*(2*cos(t)+(0-1)*cos(2*t))");
        REQUIRE(f->get_name() == "f");
    }

    SECTION("Preverifier 2")
    {
        std::unique_ptr<Function> f  { new Function("f(t)=(a+b)*sin(t)-b*sin((a/b+1)*t)", nullptr) };
        std::string p = f->getPreverified();
        REQUIRE(p == "(a+b)*sin(t)+(0-1)*b*sin((a/b+1)*t)");
        REQUIRE(f->get_name() == "f");
    }

    SECTION("Preverifier 3")
    {
        std::unique_ptr<Function> f  { new Function("f(x) = t*sqrt( 1 - ((b^4 - 4 * a^2*t^2 - t^4) / (4 * a * t^3))^2 )", nullptr) };
        std::string p = f->getPreverified();
        REQUIRE(p == "t*sqrt(1+(0-1)*((b^4+(0-1)*4*a^2*t^2+(0-1)*t^4)/(4*a*t^3))^2)");
        REQUIRE(f->get_name() == "f");
    }
}

TEST_CASE("Power", "[power]")
{

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    registerClasses();

    SECTION("PowerTest")
    {
        std::unique_ptr<Function> f  { new Function("f(x) = (x)^(-1/2)", nullptr) };
        f->SetVariable(X, 4); // PI/2
        double cc=f->Calculate();
        REQUIRE(f->get_name() == "f");
        REQUIRE(cc == 0.5);
    }

}

TEST_CASE( "Parsing", "[parser]" )
{

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    registerClasses();

    SECTION("Expression extractor")
    {
        std::string s = "sin(x) over";
        const char* b = &s[0];
        std::string fnai("over");
        std::string ex = extract_proper_expression(b, fnai, std::set<char>{} ,std::set<std::string>{"over"} );

        REQUIRE(ex == "sin(x)");
    }


    SECTION("Extracter")
    {
        QString fnai;
        QString codeli = "ps[12]";
        QString funToPlot = extract_proper_expression(codeli, fnai, {' ', '['}, {Keywords::KW_COUNTS, Keywords::KW_OVER, Keywords::KW_CONTINUOUS, Keywords::KW_FOR}, false);

        REQUIRE(funToPlot.toStdString() == "ps");

    }
}

TEST_CASE( "Compiler operations", "[compiler]" )
{

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    registerClasses();

    SECTION("list declaration")
    {
        QString s{
            R"(
               var ps list of points
               var xs ys list of numbers
            )"
        };

        QStringList codelines = s.split("\n");
        rp.parse(codelines);
        rp.execute();

        REQUIRE(rp.getAllVariables().size() == 3);
        REQUIRE(rp.typeOfVariable("ps") == Types::TYPE_LIST);
        REQUIRE(rp.domainOfVariable("ps") == Domains::DOMAIN_POINTS);
    }

    SECTION( "lists assignment")
    {
        QString s{
            R"(
              var xs list of numbers
              let xs = list [1, 2, 3, 4]
              let xs[2] = 9
            )"
        };

        QStringList codelines = s.split("\n");
        rp.parse(codelines);
        rp.execute();

        REQUIRE(rp.getAllVariables().size() == 1);
        REQUIRE(rp.typeOfVariable("xs") == Types::TYPE_LIST);
        REQUIRE(rp.domainOfVariable("xs") == Domains::DOMAIN_NUMBERS);
        REQUIRE(rp.getIndexedVariableValue("xs", 0) == 1);
        REQUIRE(rp.getIndexedVariableValue("xs", 1) == 2);
        REQUIRE(rp.getIndexedVariableValue("xs", 2) == 9);
        REQUIRE(rp.getIndexedVariableValue("xs", 3) == 4);
    }

    SECTION( "Point assignment")
    {
        QString s{
            R"(
              var p point
              var x y number
              let p = point at (1,2)
              let x = p.x
              let y = p.y
            )"
        };

        QStringList codelines = s.split("\n");
        rp.parse(codelines);
        rp.execute();

        REQUIRE(rp.getAllVariables().size() == 3);
        REQUIRE(rp.value(X) == 1);
        REQUIRE(rp.value(Y) == 2);
    }

    SECTION ("More Complex assignments")
    {
        QString s{
            R"(
                var b number
                let b = 99
                var xs list of numbers
                let xs = list [1,2,3,4]
                let xs[2] = b + 1
            )"
        };

        QStringList codelines = s.split("\n");
        rp.parse(codelines);
        rp.execute();

        REQUIRE(rp.getAllVariables().size() == 2);
        REQUIRE(rp.typeOfVariable("xs") == Types::TYPE_LIST);
        REQUIRE(rp.domainOfVariable("xs") == Domains::DOMAIN_NUMBERS);

        REQUIRE(rp.getIndexedVariableValue("xs", 0) == 1);
        REQUIRE(rp.getIndexedVariableValue("xs", 1) == 2);
        REQUIRE(rp.getIndexedVariableValue("xs", 2) == 100);
        REQUIRE(rp.getIndexedVariableValue("xs", 3) == 4);
    }

    SECTION( "list append numbers" )
    {
        QString s{
            R"(var l list of numbers
              let l = list [1,2,3,4]
              append to l numbers 5,6,7
            )"
        };

        QStringList codelines = s.split("\n");
        rp.parse(codelines);
        rp.execute();

        REQUIRE(rp.getIndexedVariableValue("l", 0) == 1);
        REQUIRE(rp.getIndexedVariableValue("l", 1) == 2);
        REQUIRE(rp.getIndexedVariableValue("l", 2) == 3);
        REQUIRE(rp.getIndexedVariableValue("l", 3) == 4);
        REQUIRE(rp.getIndexedVariableValue("l", 4) == 5);
        REQUIRE(rp.getIndexedVariableValue("l", 5) == 6);
        REQUIRE(rp.getIndexedVariableValue("l", 6) == 7);

    }

    SECTION( "function values")
    {
        QString s{
          R"(
            function f(x) = x
            var n number
            let n = f(2)
          )"
        };

        QStringList codelines = s.split("\n");
        rp.parse(codelines);
        rp.execute();

        REQUIRE(rp.value("n") == 2);
    }


    SECTION( "if keyword" )
    {
        QString s{
            R"(
                var n c number
                let c = 4
                let n = 2
                if n == 2 do
                  let c = 6
                done
            )"
        };

        QStringList codelines = s.split("\n");
        rp.parse(codelines);
        rp.execute();

        REQUIRE(rp.value("c") == 6);
    }



    SECTION( "if with and and or")
    {
        QString s{
            R"(
                var a b c d number
                let a = 1
                let b = 2
                let c = 3
                let d = 4
                if a == 3 and c == 3 and d == 4 or b == 2 do
                  let d = 5
                done
            )"
        };



        QStringList codelines = s.split("\n");
        rp.parse(codelines);
        rp.execute();

        REQUIRE(rp.value("d") == 5);

    }


    SECTION( "point assignment" )
    {
        QString s{
            R"(
                var p point, ps list of points
                let ps = list [(1,2)]
                let p = ps[0]
            )"
        };

        QStringList codelines = s.split("\n");
        rp.parse(codelines);
        rp.execute();

        REQUIRE(rp.value("p", X) == 1);
        REQUIRE(rp.value("p", Y) == 2);

    }

    SECTION("list of points_assignment")
    {
        QString s{
            R"(
                var l list of points
                var p point
                let l = list [ (1,2) , (3,4) ]
                let p = l[1]

            )"
        };

        QStringList codelines = s.split("\n");
        rp.parse(codelines);
        rp.execute();

        REQUIRE(rp.value("p", X) == 3);
        REQUIRE(rp.value("p", Y) == 4);
    }

    SECTION( "list append points")
    {
        QString s{
            R"(var l list of points, p point
                let l = list [ (1,2) , (3,4) ]
                append to l points (5,6)
                let p = l[2]
            )"
        };

        QStringList codelines = s.split("\n");
        rp.parse(codelines);
        rp.execute();

        REQUIRE(rp.value("p", X) == 5);
        REQUIRE(rp.value("p", Y) == 6);
    };

    SECTION( "plot with expressions in over" )
    {
        QString s{
            "function f(x) = x\n"
            "plot f over (cos(0) + 1, 2 + 2)"
        };

        QStringList codelines = s.split("\n");
        bool ok = rp.parse(codelines);
        if(!ok) {
            qDebug() << "Parse failed!";
        }
        REQUIRE(ok);
        auto statements = rp.getStatements();

        bool foundPlot = false;
        for(auto& stmt : statements)
        {
            auto p = qSharedPointerDynamicCast<Plot>(stmt);
            if(p)
            {
                foundPlot = true;
                REQUIRE(p->start != nullptr);
                REQUIRE(p->end != nullptr);

                REQUIRE(p->start->Calculate() == 2.0);
                REQUIRE(p->end->Calculate() == 4.0);
            }
        }
        REQUIRE(foundPlot);
    }


}

TEST_CASE( "Parametric functions", "[compiler]" )
{

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    registerClasses();


    SECTION( "parametric function simple assignment")
    {
        QString s{
            R"(parametric function f(t)
                    x = t
                    y = 2 * t
                end

                var p point
                let p = f(1)
            )"
        };

        QStringList codelines = s.split("\n");
        rp.parse(codelines);
        rp.execute();

        REQUIRE(rp.value("p", X) == 1);
        REQUIRE(rp.value("p", Y) == 2);
    };

    SECTION( "parametric function simple assignment to sum of functions")
    {
        QString s{
            R"(parametric function f(t)
                    x = t
                    y = 2 * t
                end

                var p point
                let p = f(1) + f(2)
            )"
        };

        QStringList codelines = s.split("\n");
        rp.parse(codelines);
        rp.execute();

        REQUIRE(rp.value("p", X) == 3);
        REQUIRE(rp.value("p", Y) == 6);
    };

    SECTION( "parametric function simple assignment to multiplication")
    {
        QString s{
            R"(parametric function f(t)
                    x = t
                    y = 2 * t
                end

                var p point
                let p =3 * f(2)
            )"
        };

        QStringList codelines = s.split("\n");
        rp.parse(codelines);
        rp.execute();

        REQUIRE(rp.value("p", X) == 6);
        REQUIRE(rp.value("p", Y) == 12);
    };

}

TEST_CASE("Draw Statement", "[draw]")
{
    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    registerClasses();

    SECTION("draw segment statement")
    {
        QStringList lines = {
            "draw segment from (1, 1) to (2, 2)"
        };

        rp.parse(lines);
        rp.execute();

        auto& stmts = rp.getStatements();
        REQUIRE(stmts.size() == 1);
        REQUIRE(stmts[0]->keyword() == Keywords::KW_DRAW);
    }

    SECTION("draw point statement")
    {
        QStringList lines = {
            "draw point at (1, 1)"
        };

        rp.parse(lines);
        rp.execute();

        auto& stmts = rp.getStatements();
        REQUIRE(stmts.size() == 1);
        REQUIRE(stmts[0]->keyword() == Keywords::KW_DRAW);
    }

    SECTION("points of segment assignment")
    {
        QStringList lines = {
            "var ps list of points",
            "let ps = points of segment from (0, 0) to (10, 10) counts 11",
            "var p1 p2 point",
            "let p1 = ps[0]",
            "let p2 = ps[10]"
        };

        rp.parse(lines);
        rp.execute();

        REQUIRE(rp.typeOfVariable("ps") == Types::TYPE_LIST);
        
        // Use rp.value with X and Y constants to check point coordinates
        REQUIRE(rp.value("p1", X) == 0);
        REQUIRE(rp.value("p1", Y) == 0);
        REQUIRE(rp.value("p2", X) == 10);
        REQUIRE(rp.value("p2", Y) == 10);

        // Check a middle point
        QStringList lines2 = { "let p1 = ps[5]" };
        rp.parse(lines2);
        rp.execute();
        REQUIRE(rp.value("p1", X) == 5);
        REQUIRE(rp.value("p1", Y) == 5);
    }

    SECTION("point expressions as segment endpoints")
    {
        QStringList lines = {
            "var p1 p2 point",
            "let p1 = point at (0, 0)",
            "let p2 = point at (10, 10)",
            "draw segment from p1 to p2",
            "var ps list of points",
            "let ps = points of segment from p1 to p2 counts 11",
            "var p3 point",
            "let p3 = ps[5]",
            "draw segment from ps[0] to ps[10]"
        };

        rp.parse(lines);
        rp.execute();

        REQUIRE(rp.value("p3", X) == 5);
        REQUIRE(rp.value("p3", Y) == 5);

        // Test indexed access on list as endpoint
        QStringList lines2 = {
            "let ps = points of segment from ps[0] to ps[5] counts 6"
        };
        rp.parse(lines2);
        rp.execute();
        REQUIRE(rp.value("ps[5]", X) == 5);
    }

    SECTION("draw segment with counts")
    {
        QStringList lines = {
            "draw segment from (0, 0) to (10, 10) counts 11"
        };

        rp.parse(lines);
        // Execute to ensure no crashes
        rp.execute();

        auto& stmts = rp.getStatements();
        REQUIRE(stmts.size() == 1);
        REQUIRE(stmts[0]->keyword() == Keywords::KW_DRAW);
    }

    SECTION("infinite line support")
    {
        QStringList lines = {
            "var ps list of points",
            // Visible area is default -150 to 150 X, -100 to 100 Y
            // For line y=x, it will be clipped at y=-100 and y=100
            "let ps = points of line from (0, 0) to (1, 1) counts 3"
        };

        rp.parse(lines);
        rp.execute();

        // Points should be at edges of visible area
        // Line y=x is limited by y-bounds [-100, 100]
        REQUIRE(rp.value("ps[0]", X) == -100);
        REQUIRE(rp.value("ps[2]", X) == 100);
    }

    SECTION( "line and segment variables")
    {
        QStringList lines = {
            "var l line",
            "var s segment",
            "let l = line through (0, 0) and (10, 10)",
            "let s = segment from (0, 0) to (10, 10)",
            "draw line l",
            "draw segment s",
            "var ps1 ps2 list of points",
            "let ps1 = points of l over (0, 1) counts 11",
            "let ps2 = points of s over (0, 1) counts 3"
        };

        rp.parse(lines);
        rp.execute();

        REQUIRE_THAT(rp.value("ps1[0]", X), Catch::Matchers::WithinRel(0.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001));
        REQUIRE_THAT(rp.value("ps1[5]", X), Catch::Matchers::WithinRel(5.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001));
        REQUIRE_THAT(rp.value("ps1[10]", X), Catch::Matchers::WithinRel(10.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001));

        REQUIRE_THAT(rp.value("ps2[0]", X), Catch::Matchers::WithinRel(0.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001));
        REQUIRE_THAT(rp.value("ps2[1]", X), Catch::Matchers::WithinRel(5.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001));
        REQUIRE_THAT(rp.value("ps2[2]", X), Catch::Matchers::WithinRel(10.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001));
    }
    }

static int currentSize = 1;

TEST_CASE("Pixel Size State", "[pixel]")
{
    struct DrawOp {
        QString type;
        int size;
    };
    static QVector<DrawOp> ops;
    ops.clear();
    currentSize = 1;

    auto erp = [](int l, int c, QString e) {};
    auto sp = [](QString s) {};
    auto pd = [](double x, double y, int s) { ops.append({"POINT", s}); };
    auto ld = [](double x1, double y1, double x2, double y2, int s) { ops.append({"LINE", s}); };
    auto str = [](QString s) {};
    auto ps = [](int r, int g, int b, int a, int s) { currentSize = s; };
    auto pld = [](QSharedPointer<Plot> plot) { 
        auto executor = [](QSharedPointer<Plot> plot, double x, double y, bool continuous, int size)
        {
            if(continuous) {
                RuntimeProvider::get()->drawLine(x, y, x, y); // simplified, will pass m_ps internally
            } else {
                RuntimeProvider::get()->drawPoint(x, y); // simplified, will pass m_ps internally
            }
        };
        RuntimeProvider::get()->genericPlotIterator(plot, executor);
    };

    RuntimeProvider rp{erp, sp, pd, ld, str, ps, pld, {}};
    registerClasses();

    SECTION("point at intersection")
    {
        QStringList lines = {
            "var l1 l2 line",
            "let l1 = line through (0, 0) and (10, 10)",
            "let l2 = line through (0, 10) and (10, 0)",
            "var p point",
            "let p = point at intersection of l1 and l2"
        };

        rp.parse(lines);
        rp.execute();

        REQUIRE(rp.typeOfVariable("p") == Types::TYPE_POINT);
        REQUIRE_THAT(rp.value("p", X), Catch::Matchers::WithinRel(5.0, 0.001) || Catch::Matchers::WithinAbs(5.0, 0.000001));
        REQUIRE_THAT(rp.value("p", Y), Catch::Matchers::WithinRel(5.0, 0.001) || Catch::Matchers::WithinAbs(5.0, 0.000001));
    }

    SECTION("perpendicular segment drawn and intersection point found")
    {
        // A=(5,1), B=(1,2), C=(2,-1)
        // AB direction: (-4,1), foot of perp from C onto AB:
        //   t = ((2-5)*(-4) + (-1-1)*(1)) / 17 = 10/17
        //   D = (5 - 40/17, 1 + 10/17) = (45/17, 27/17)
        QVector<QPointF> drawnLines;
        QVector<QPointF> drawnPoints;

        auto erp2 = [](int l, int c, QString e) { qWarning() << "ERR" << e; };
        auto sp2  = [](QString) {};
        auto pd2  = [&](double x, double y, int s) { drawnPoints.append({x,y}); };
        auto ld2  = [&](double x1, double y1, double x2, double y2, int s) {
            drawnLines.append({x1,y1}); drawnLines.append({x2,y2});
        };
        auto str2 = [](QString) {};
        auto ps2  = [](int,int,int,int,int) {};
        auto pld2 = [](QSharedPointer<Plot>) {};
        RuntimeProvider rp2{erp2, sp2, pd2, ld2, str2, ps2, pld2, {}};
        registerClasses();

        QStringList lines = {
            "var A B C D point",
            "var AB CD segment",
            "let A = point at (5, 1)",
            "let B = point at (1, 2)",
            "let C = point at (2, -1)",
            "let AB = segment from A to B",
            "draw segment AB",
            "draw point A",
            "draw point B",
            "draw point C",
            "let CD = segment from C perpendicular to AB",
            "let D = point at intersection of CD and AB",
            "draw point D",
            "draw segment CD",
        };

        bool ok = rp2.parse(lines);
        REQUIRE(ok);
        rp2.execute();

        // D should be the foot of the perpendicular
        double Dx = 45.0 / 17.0;
        double Dy = 27.0 / 17.0;
        REQUIRE_THAT(rp2.value("D", X), Catch::Matchers::WithinAbs(Dx, 0.001));
        REQUIRE_THAT(rp2.value("D", Y), Catch::Matchers::WithinAbs(Dy, 0.001));

        // 3 segments drawn: AB, CD, plus maybe more - at least 2
        INFO("drawnLines size = " << drawnLines.size());
        REQUIRE(drawnLines.size() >= 4);  // at least 2 lines (AB and CD), each contributing 2 points

        // 4 points drawn: A, B, C, D
        INFO("drawnPoints size = " << drawnPoints.size());
        REQUIRE(drawnPoints.size() >= 4);
    }
}

