#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "util.h"
#include "Function.h"
#include "RuntimeProvider.h"
#include "AbstractDrawer.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>
/*
TEST_CASE( "SimpleFunction2var", "[simfun]" )
{
    Function *f = new Function("f(x,y) = (x+y)/3");
	f->SetVariable("x",3);
    f->SetVariable("y",3);

    double cc=f->Calculate(nullptr);
    REQUIRE(cc == 2.0);
    REQUIRE(f->get_name() == "f");
    delete f;

}

TEST_CASE( "SimpleFunction1var", "[simfun]" )
{
    Function *f = new Function("f(x) = x*x + 3*x + 2");
    f->SetVariable("x", 1);

    double cc=f->Calculate(nullptr);
    REQUIRE(cc == 6.0);
    REQUIRE(f->get_name() == "f");
    delete f;

}


TEST_CASE( "SimpleNumber", "[simfun]" )
{
    Function *f = new Function("f(x) = 3");
    f->SetVariable("x", 1);

    double cc=f->Calculate(nullptr);
    REQUIRE(cc == 3);
    REQUIRE(f->get_name() == "f");
    delete f;

}




TEST_CASE( "Divide", "[simfun]" )
{
    Function *f = new Function("f(x) = (x-1)/(x+1)");
    f->SetVariable("x", 1);

    double cc=f->Calculate(nullptr);
    REQUIRE(cc == 0);
    REQUIRE(f->get_name() == "f");
    delete f;

} */
/*
TEST_CASE( "Power", "[simfun]" )
{
    Function *f = new Function("function f(x) = pow(x, cos(x / 2) + x)");
    f->SetVariable("x", 0);

    double cc=f->Calculate(nullptr);
    REQUIRE(cc == 0);
    REQUIRE(f->get_name() == "f");
    delete f;

}*/

/*TEST_CASE( "UnwantedWordExtractor", "[parser]" )
{
    std::string s = "sin(x) over";
    const char* b = &s[0];
    std::string ex = extract_proper_expression(b, std::set<char>{} ,std::set<std::string>{"over"} );
}
*/


//TEST_CASE( "Indexed", "[simfun]" )
//{
//    Function *f = new Function("function f(x) = x[2 + i]", nullptr);
//    f->SetVariable("x", 0);
//    IndexedAccess* ia = nullptr; Assignment* a = nullptr;
//    double cc=f->Calculate(nullptr, ia, a);
//    REQUIRE(cc == 0);
//    REQUIRE(f->get_name() == "f");
//    delete f;

//}

//TEST_CASE( "Power", "[simfun]" )
//{
//    Function *f = new Function("function f(x) = sin^2(x)", nullptr);
//    f->SetVariable("x", 1.570795); // PI/2
//    IndexedAccess* ia = nullptr; Assignment* a = nullptr;
//    double cc=f->Calculate(nullptr, ia, a);
//    REQUIRE(cc == 1.0);
//    REQUIRE(f->get_name() == "f");
//    delete f;

//}

//TEST_CASE( "Power", "[numfun]" )
//{
//    Function *f = new Function("function f(x) = x^2", nullptr);
//    f->SetVariable("x", 2); // PI/2
//    IndexedAccess* ia = nullptr; Assignment* a = nullptr;
//    double cc=f->Calculate(nullptr, ia, a);
//    REQUIRE(cc == 4);
//    delete f;

//}

/*
TEST_CASE( "Power2", "[simfun]" )
{
    Function *f = new Function("function f(x) = sin^(2+3)(x)", nullptr);
    f->SetVariable("x", 1.570795); // PI/2
    IndexedAccess* ia = nullptr; Assignment* a = nullptr;
    double cc=f->Calculate(nullptr, ia, a);
    REQUIRE(cc == 1.0);
    delete f;
}
*/

static std::tuple<RuntimeProvider::CB_ErrorReporter, RuntimeProvider::CB_StringPrinter, RuntimeProvider::CB_PointDrawer, RuntimeProvider::CB_StatementTracker, RuntimeProvider::CB_PenSetter, RuntimeProvider::CB_PlotDrawer> RuntimeProviderParameterProvider()
{
    return {
        [](int l, int c, QString e) { qWarning() << "ERROR" << c << "AT" << l << e;},
        [](QString s) { qWarning() << "PRINT" << s;},
        [](double x, double y) {qInfo() << "POINT AT" << QPointF(x ,y); },
        [](QString s) { qInfo() << "CURRENT STATEMENT"<<s;},
        [](int r, int g, int b, int a, int s) { qInfo() << "COLOR" << r << g << b << a << "SIZE:" <<  s; },
        [](QSharedPointer<Plot> p) { qInfo() << "PLOTTING" <<p->plotTarget; }
    };
}
/*
TEST_CASE( "ListsOfSpecificTypes", "[compiler]" )
{
    QString s{
              R"(
var ps list of points
var xs ys list of numbers
)"
    };

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    QStringList codelines = s.split("\n");
    rp.parse(codelines);
    rp.execute();

    REQUIRE(rp.get_all_variables().size() == 3);
    REQUIRE(rp.typeOfVariable("ps") == Types::TYPE_LIST);
    REQUIRE(rp.domainOfVariable("ps") == Domains::DOMAIN_POINTS);
}


TEST_CASE( "ListsAssignment", "[compiler]" )
{
    QString s{
        R"(
var xs list of numbers
let xs = list [1, 2, 3, 4]
let xs[2] = 9
)"
    };

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    QStringList codelines = s.split("\n");
    rp.parse(codelines);
    rp.execute();

    REQUIRE(rp.get_all_variables().size() == 1);
    REQUIRE(rp.typeOfVariable("xs") == Types::TYPE_LIST);
    REQUIRE(rp.domainOfVariable("xs") == Domains::DOMAIN_NUMBERS);
    REQUIRE(rp.getIndexedVariableValue("xs", 0) == 1);
    REQUIRE(rp.getIndexedVariableValue("xs", 1) == 2);
    REQUIRE(rp.getIndexedVariableValue("xs", 2) == 9);
    REQUIRE(rp.getIndexedVariableValue("xs", 3) == 4);
}
*/

TEST_CASE( "ListsOfSpecificTypesInvalidAssignment", "[compiler]" )
{
    QString s{
        R"(
var p point
let p = point at (1,2)
var xs list of numbers
let xs = list [1,2,3,4]
let xs[2] = 99
)"
    };

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    QStringList codelines = s.split("\n");
    rp.parse(codelines);
    rp.execute();

    REQUIRE(rp.getAllVariables().size() == 2);
    REQUIRE(rp.typeOfVariable("xs") == Types::TYPE_LIST);
    REQUIRE(rp.domainOfVariable("xs") == Domains::DOMAIN_NUMBERS);

    REQUIRE(rp.getIndexedVariableValue("xs", 0) == 1);
    REQUIRE(rp.getIndexedVariableValue("xs", 1) == 2);
    REQUIRE(rp.getIndexedVariableValue("xs", 2) == 99);
    REQUIRE(rp.getIndexedVariableValue("xs", 3) == 4);
}
/**/

/*TEST_CASE( "ListsOfSpecificTypesAssignment", "[compiler]" )
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

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    QStringList codelines = s.split("\n");
    rp.parse(codelines);
    rp.execute();

    REQUIRE(rp.get_all_variables().size() == 2);
    REQUIRE(rp.typeOfVariable("xs") == Types::TYPE_LIST);
    REQUIRE(rp.domainOfVariable("xs") == Domains::DOMAIN_NUMBERS);

    REQUIRE(rp.getIndexedVariableValue("xs", 0) == 1);
    REQUIRE(rp.getIndexedVariableValue("xs", 1) == 2);
    REQUIRE(rp.getIndexedVariableValue("xs", 2) == 100);
    REQUIRE(rp.getIndexedVariableValue("xs", 3) == 4);
}
*/
/*
TEST_CASE( "WeirdCode", "[compiler]" )
{
    QString s{
        R"(
var pssin pscos list of points
var i number
var x y number
var ps pc p point
let i = 0
function ps(x) = sin(x)
function pc(x) = cos(x)

let pssin = points of ps over (-3.14, 3.14) counts 16
let pscos = points of pc over (-3.14, 3.14) counts 16

for i = 0 to 15 do

  let ps = pssin[i]
  let pc = pscos[i]

  let x = ps.y
  let y = pc.y

  let p = point at (x,y)
  plot p

done
)"
    };

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    QStringList codelines = s.split("\n");
    rp.parse(codelines);
    rp.execute();

    REQUIRE(-0.912983 == rp.value("y"));

}
*/
/*
 * TEST_CASE( "PlotTest", "[compiler]" )
{
    QString s{
        R"(
function f(x) = cos(x)
plot f counts 255)"
    };

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    QStringList codelines = s.split("\n");
    rp.parse(codelines);
    rp.execute();
}


*/

/*
TEST_CASE( "list_append_numbers", "[compiler]" )
{
    QString s{
        R"(var l list of numbers
let l = list [1,2,3,4]
append to l numbers 5,6,7
)"
    };

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    QStringList codelines = s.split("\n");
    rp.parse(codelines);
    rp.execute();
}
*/

/*
TEST_CASE("list_of_points_assignment", "[compiler]")
{
    QString s{
        R"(var l list of points
let l = list [ (1,2) , (3,4) ]
plot l[1]
)"
    };

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    QStringList codelines = s.split("\n");
    rp.parse(codelines);
    rp.execute();
}

*/
/*
TEST_CASE( "list_append_points", "[compiler]" )
{
    QString s{
        R"(var l list of points, p point
let l = list [ (1,2) , (3,4) ]
append to l points (5,6)
for p in l do
 plot p
done
)"
    };

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    QStringList codelines = s.split("\n");
    rp.parse(codelines);
    rp.execute();
}
*/


//TEST_CASE( "function_values", "[compiler]" )
//{
//    QString s{
//              R"(
//function f(x) = x
//var n number
//let n = f(2)
//)"
//    };

//    RuntimeProvider rp{RuntimeProviderParameterProvider()};
//    QStringList codelines = s.split("\n");
//    rp.parse(codelines);
//    rp.execute();
//}


//TEST_CASE( "point_assignment", "[compiler]" )
//{
//    QString s{
//        R"(
//var p point, ps list of points
//let ps = list [(1,2)]
//let p = ps[0]
//plot p
//)"
//    };

//    RuntimeProvider rp{RuntimeProviderParameterProvider()};
//    QStringList codelines = s.split("\n");
//    rp.parse(codelines);
//    rp.execute();
//}
/*
class ImageDrawer : public AbstractDrawer
{

    virtual int sceneX(double x) override
    {
        return 11;
    }

    virtual int sceneY(double y) override
    {
        return 12;
    }

    virtual void redrawEverything() override
    {

    }

    virtual void drawLine(const QLineF&, const QPen&) override
    {

    }

    void setDrawingPen(int r, int g, int b, int a, int s)
    {

    }


    QVector<QPointF> drawPlot(QSharedPointer<Plot> plot)
    {
        double cx, cy;
        bool first = true;

        QVector<QPointF> points;

        auto executor = [this, &cx, &cy, &first, &points](double x, double y, bool continuous)
        {
            points.append({x, y});
            if(continuous)
            {
                if(first)
                {
                    cx = x;
                    cy = y;
                    first = false;
                }
                else
                {
                    QPoint scp1 {toScene({cx, cy})};
                    QPoint scp2 {toScene({x, y})};
                    //sc->addLine( QLine(scp1, scp2), drawingPen);
                    addLine({static_cast<qreal>(cx), static_cast<qreal>(cy), static_cast<qreal>(x), static_cast<qreal>(y)}, m_drawingPen);
                    cx = x;
                    cy = y;
                }
            }
            else
            {
                QPoint scp {toScene({x, y})};

                addPoint({x, y}, m_drawingPen, m_pixelSize);
            }
        };

        genericPlotIterator(plot, executor);

        return points;


    }

    void drawPoint(double x, double y)
    {

    }

    template<class E>
    void genericPlotIterator(QSharedPointer<Plot> plot, E executor)
    {
        m_rp->genericPlotIterator(plot, executor);
    }

    QPen m_drawingPen;                      // the pen used for drawing, holds the color of it
    size_t m_pixelSize = 1;                 // the size of a pixel (pixels are small ellipses)
    RuntimeProvider* m_rp;                  // the runtime provider of this class

};

static std::tuple<RuntimeProvider::CB_ErrorReporter, RuntimeProvider::CB_PointDrawer, RuntimeProvider::CB_StatementTracker, RuntimeProvider::CB_PenSetter, RuntimeProvider::CB_PlotDrawer> RuntimeProviderForImageDrawingParameterProvider()
{



    ImageDrawer imgDrawer;

    return {
        [](int l, int c, QString e) { qWarning() << "ERROR" << c << "AT" << l << e;},
        [](double x, double y) {qInfo() << "POINT AT" << QPointF(x ,y); },
        [](QString s) { qInfo() << "CURRENT STATEMENT"<<s;},
        [](int r, int g, int b, int a, int s) { qInfo() << "COLOR" << r << g << b << a << "SIZE:" <<  s; },
        [](QSharedPointer<Plot> p) { qInfo() << "PLOTTING" <<p->plotTarget; }
    };
}
*/


TEST_CASE( "Equality", "[compiler]" )
{
    RuntimeProvider rp{RuntimeProviderParameterProvider()};

    Function *f = new Function("f(x) = x", nullptr);
    f->SetVariable("x", 2); // PI/2
    IndexedAccess* ia = nullptr; Assignment* a = nullptr;
    double cc=f->Calculate(nullptr, ia, a);
    delete f;
    REQUIRE(cc == 2);
}


TEST_CASE( "PlotTest", "[compiler]" )
{
    QString s{
        R"(
function f(x) = cos(x)
plot f counts 255)"
    };

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    QStringList codelines = s.split("\n");
    rp.parse(codelines);
    rp.execute();
}



//TEST_CASE( "if_test_1", "[compiler]" )
//{
//    QString s{
//              R"(
//var n c number
//let c = 4
//let n = 2
//if n == 2 do
//  let c = 6
//done
//)"
//    };

//    RuntimeProvider rp{RuntimeProviderParameterProvider()};
//    QStringList codelines = s.split("\n");
//    rp.parse(codelines);
//    rp.execute();
//    QString rpv = rp.valueOfVariable("c");
//    REQUIRE(static_cast<int>(rpv.toDouble()) == 6);
//}

TEST_CASE( "if_test_1", "[compiler]" )
{
    QString s{
              R"(
var i number, cp point, ps list, cp2 point
let i = 0
function f(x) = sin(x)
let ps = points of f over (-10, 10) counts 512 continuous
for i = 0 to 255 step 2 do
  let cp = ps[i]
  rotate cp with i/3 degrees
  plot cp

  let cp2 = ps[i + 1]
  rotate cp2 with i/2 degrees
  plot cp2
done

)"
    };

    RuntimeProvider rp{RuntimeProviderParameterProvider()};
    QStringList codelines = s.split("\n");
    rp.parse(codelines);
    rp.execute();
}

