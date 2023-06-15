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


static std::tuple<RuntimeProvider::CB_ErrorReporter, RuntimeProvider::CB_StringPrinter, RuntimeProvider::CB_PointDrawer, RuntimeProvider::CB_LineDrawer, RuntimeProvider::CB_StatementTracker, RuntimeProvider::CB_PenSetter, RuntimeProvider::CB_PlotDrawer> RuntimeProviderParameterProvider()
{
    qInstallMessageHandler(myMessageOutput);

    std::tuple<RuntimeProvider::CB_ErrorReporter, RuntimeProvider::CB_StringPrinter, RuntimeProvider::CB_PointDrawer, RuntimeProvider::CB_LineDrawer, RuntimeProvider::CB_StatementTracker, RuntimeProvider::CB_PenSetter, RuntimeProvider::CB_PlotDrawer> r =
        {
            [](int l, int c, QString e) { qWarning() << "ERROR" << c << "AT" << l << e;},
            [](QString s) { qWarning() << "PRINT" << s;},
            [](double x, double y) {qInfo() << "POINT AT" << QPointF(x ,y); },
            [](double x1, double y1, double x2, double y2) {qInfo() << "LINE AT" << QLineF(x1 ,y1, x2, y2); },
            [](QString s) { qInfo() << "CURRENT STATEMENT"<<s;},
            [](int r, int g, int b, int a, int s) { qInfo() << "COLOR" << r << g << b << a << "SIZE:" <<  s; },
            [](QSharedPointer<Plot> p) { qInfo() << "PLOTTING" <<p->plotTarget; }
        };

    return r;
}

TEST_CASE( "Function handling", "[functions]" )
{

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    registerClasses();

    SECTION("Simple function with 2 variables") {
        std::unique_ptr<Function> f  { new Function("f(x,y) = (x+y)/3", nullptr) };
        f->SetVariable("x",3);
        f->SetVariable("y",3);
        double cc=f->Calculate();
        REQUIRE_THAT(cc, Catch::Matchers::WithinRel(2.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001) );
        REQUIRE(f->get_name() == "f");
    }

    SECTION( "Simple function with 1 variable")
    {
        std::unique_ptr<Function> f  { new Function("f(x) = x*x + 3*x + 2", nullptr) };
        f->SetVariable("x", 1);

        double cc=f->Calculate();
        REQUIRE_THAT(cc, Catch::Matchers::WithinRel(6.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001) );

        REQUIRE(f->get_name() == "f");
    }

    SECTION("Function is a number")
    {
        std::unique_ptr<Function> f  { new Function("f(x) = 3", nullptr) };
        f->SetVariable("x", 1);

        double cc=f->Calculate();
        REQUIRE_THAT(cc, Catch::Matchers::WithinRel(3.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001) );

        REQUIRE(f->get_name() == "f");
    }

    SECTION("Formula has division by zero")
    {
        std::unique_ptr<Function> f  { new Function("f(x) = (x-1)/(x+1)", nullptr) };
        f->SetVariable("x", -1);

        double cc=f->Calculate();

        REQUIRE( std::isnan(cc) );
        REQUIRE(f->get_name() == "f");
    }

    SECTION("Power of sin")
    {
        std::unique_ptr<Function> f  { new Function("f(x) = sin^2(x)", nullptr) };
        f->SetVariable("x", 1.570795); // PI/2
        double cc=f->Calculate();
        REQUIRE_THAT(cc, Catch::Matchers::WithinRel(1.0, 0.001) || Catch::Matchers::WithinAbs(0, 0.000001) );
        REQUIRE(f->get_name() == "f");
    }

    SECTION("Simple power")
    {
        std::unique_ptr<Function> f  { new Function("f(x) = x^2", nullptr) };
        f->SetVariable("x", 2); // PI/2
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
        REQUIRE(p == "(a+b)*sin(t)-b*sin((a/b+1)*t)");
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
        f->SetVariable("x", 4); // PI/2
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
        REQUIRE(rp.value("x") == 1);
        REQUIRE(rp.value("y") == 2);
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

        REQUIRE(rp.value("p", "x") == 1);
        REQUIRE(rp.value("p", "y") == 2);

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

        REQUIRE(rp.value("p", "x") == 3);
        REQUIRE(rp.value("p", "y") == 4);
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

        REQUIRE(rp.value("p", "x") == 5);
        REQUIRE(rp.value("p", "y") == 6);
    };

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

        REQUIRE(rp.value("p", "x") == 1);
        REQUIRE(rp.value("p", "y") == 2);
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

        REQUIRE(rp.value("p", "x") == 3);
        REQUIRE(rp.value("p", "y") == 6);
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

        REQUIRE(rp.value("p", "x") == 6);
        REQUIRE(rp.value("p", "y") == 12);
    };

}
