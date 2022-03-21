#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPen>
#include <QMap>
#include <QSet>
#include <QDebug>

#include "Function.h"
#include "RuntimeProvider.h"

#include <functional>

class QGraphicsScene;

namespace Ui {
class MainWindow;
}

namespace Keywords
{
const QString KW_FUNCTION = "function";    // function f(x) = x * 2 + 5
const QString KW_PLOT = "plot";            // plot f over (-2, 2) [continuous]
const QString KW_OVER = "over";            // plot f over (-2, 2) [continuous]
const QString KW_CONTINUOUS = "continuous";// plot f over (-2, 2) [continuous|step<cnt=0.01>]
const QString KW_LET = "let";              // let something = points of f over (-2, 2) [continuous|step<cnt|0.01>] or let something = point at x, y or let something = a + b + c
const QString KW_OF  = "of";               // let something = points of f over (-2, 2) [continuous|step<cnt|0.01>]
const QString KW_STEP = "step";            // plot f over (-2, 2) [continuous|step<cnt|0.01>]
const QString KW_COUNTS = "counts";        // plot f over (-2, 2) [continuous|step<cnt|0.01>|counts<X[points|segments]> ]
const QString KW_FOREACH = "foreach";      // foreach p in something do ... done
const QString KW_SET = "set";              // set color name, set color #RGB, set color #RRGGBB, set color #RRGGBBAA, set color R,G,B[,A], set color name,alpha
const QString KW_IN = "in";                // foreach p in something do ... done
const QString KW_DO = "do";                // foreach p in something do ... done
const QString KW_DONE = "done";            // foreach p in something do ... done
const QString KW_RANGE = "range";          // foreach p in range (0, 1) do ... done
const QString KW_SEGMENTS = "segments";    // plot f over (-2, 2) [continuous|step<cnt|0.01>|counts<X[points|segments]> ]
const QString KW_POINTS = "points";        // plot f over (-2, 2) [continuous|step<cnt|0.01>|counts<X[points|segments]> ]
};

struct Statement
{
    explicit Statement(const QString& s) : statement(s) {}
    Statement() = delete;

    QString statement;
    QSharedPointer<Statement> parent = nullptr;
    RuntimeProvider* runtimeProvider = nullptr; // this is the "scope" of the running piece of code

    // executes this statement
    virtual bool execute(RuntimeProvider* mw) = 0;

    // return the keyword this statement represents
    virtual QString keyword() const = 0;
};

struct Stepped
{
    Stepped() noexcept;

    bool continuous = false;
    QSharedPointer<Function> step;
    bool counted = false;
    QSharedPointer<Function> start;
    QSharedPointer<Function> end;

};

struct Sett : public Statement
{

    explicit Sett(const QString& s) : Statement(s) {}

    bool execute(RuntimeProvider* rp) override;
    QString keyword() const override
    {
        return Keywords::KW_SET;
    }

    QString what;
    QString value;
};

struct Done : public Statement
{
    explicit Done(const QString& s) : Statement(s) {}

    bool execute(RuntimeProvider* mw) override { return true; }
    QString keyword() const override
    {
        return Keywords::KW_DONE;
    }
};

class Loop;
struct LoopTarget : public Stepped
{

    QSharedPointer<Loop> theLoop;
    QString name;

    using LooperCallback = std::function<void()>;

    virtual bool loop(LooperCallback, RuntimeProvider*) = 0;
};

struct RangeIteratorLoopTarget : public LoopTarget
{
    explicit RangeIteratorLoopTarget(QSharedPointer<Loop>);
    RangeIteratorLoopTarget() = delete;

    bool loop(LooperCallback lp, RuntimeProvider*) override;

    QSharedPointer<Function> startFun;
    QSharedPointer<Function> endFun;
    QSharedPointer<Function> stepFun;
};

struct FunctionIteratorLoopTarget : public LoopTarget
{
    explicit FunctionIteratorLoopTarget(QSharedPointer<Loop>);
    FunctionIteratorLoopTarget() = delete;

    bool loop(LooperCallback, RuntimeProvider*) override;

};

struct Loop : public Statement
{

    explicit Loop(const QString& s) : Statement(s) {}

    bool execute(RuntimeProvider* rp) override;
    QString keyword() const override
    {
        return Keywords::KW_FOREACH;
    }

    void updateLoopVariable(double);
    void updateLoopVariable(QPointF v);

    QString loop_variable;
    QSharedPointer<LoopTarget> loop_target;
    QVector<QSharedPointer<Statement>> body;
};

struct Plot : public Stepped, public Statement, public QEnableSharedFromThis<Plot>
{

    explicit Plot(const QString& s) : Statement(s){}

    QString plotTarget;

    QString keyword() const override
    {
        return Keywords::KW_PLOT;
    }

    QSharedPointer<Plot> f()
    {
        return sharedFromThis();
    }

    bool execute(RuntimeProvider* rp) override;
};

struct Assignment : public Stepped, public Statement
{
    explicit Assignment(const QString& s) : Statement(s) {}

    QString varName;                    // the name of the object we will refer to in later code (such as: plot assignedStuff)
    QString targetProperties;           // the name of the properties of the assigned objects, such as: points. If targetProperties is "arythmetic" then a new function is created and evaluated at run time

    virtual QString providedFunction()
    {
        return "";
    }

    virtual QSharedPointer<Function> startValueProvider()
    {
        return nullptr;
    }

    virtual QSharedPointer<Function> endValueProvider()
    {
        return nullptr;
    }

    QString keyword() const override
    {
        return Keywords::KW_LET;
    }


    virtual std::tuple<QSharedPointer<Function>, QSharedPointer<Function>> fullCoordProvider()
    {
        return {nullptr, nullptr};
    }
};

struct PointsOfObjectAssignment : public Assignment
{
    explicit PointsOfObjectAssignment(const QString& s) : Assignment(s) {}
    QString ofWhat;                     // can be a function for now (or a circle, etc... later)
    QSharedPointer<Function> ofWhatFun;
    bool execute(RuntimeProvider* rp) override;
    QString providedFunction() override
    {
        return ofWhat;
    }

    QSharedPointer<Function> startValueProvider() override
    {
        return start;
    }

    QSharedPointer<Function> endValueProvider() override
    {
        return end;
    }


};

struct ArythmeticAssignment : public Assignment
{
    explicit ArythmeticAssignment(const QString& s) : Assignment(s) {}

    QSharedPointer<Function> arythmetic;       // if this is an arythmetic assignment this is the function

    bool execute(RuntimeProvider* rp) override;


};

struct PointDefinitionAssignment : public Assignment
{
    explicit PointDefinitionAssignment(const QString& s) : Assignment(s) {}

    QSharedPointer<Function> x;       // x position
    QSharedPointer<Function> y;       // y position

    bool execute(RuntimeProvider* rp) override;
    std::tuple<QSharedPointer<Function>, QSharedPointer<Function>> fullCoordProvider() override
    {
        return {x, y};
    }

};

struct FunctionDefinition : public Statement
{
    explicit FunctionDefinition(QString a) : Statement(a) {}

    QSharedPointer<Function> f;

    bool execute(RuntimeProvider* mw) override;
    QString keyword() const override
    {
        return Keywords::KW_FUNCTION;
    }
};

struct DrawnLine
{
    QLineF line;
    QPen pen;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

    const QString STR_FUNCTION = Keywords::KW_FUNCTION + " ";    // function f(x) = x * 2 + 5
    const QString STR_PLOT = Keywords::KW_PLOT + " ";            // plot f over (-2, 2) [continuous]
    const QString STR_OVER = Keywords::KW_OVER + " ";            // plot f over (-2, 2) [continuous]
    const QString STR_LET = Keywords::KW_LET + " ";              // let something = points of f over (-2, 2) [continuous|step<cnt|0.01>]
    const QString STR_OF  = Keywords::KW_OF + " ";               // let something = points of f over (-2, 2) [continuous|step<cnt|0.01>]
    const QString STR_STEP = Keywords::KW_STEP + " ";            // plot f over (-2, 2) [continuous|step<cnt|0.01>]
    const QString STR_COUNTS = Keywords::KW_COUNTS + " ";
    const QString STR_FOREACH = Keywords::KW_FOREACH + " ";      // foreach p in something do ... done
    const QString STR_SET = Keywords::KW_SET + " ";              // set color red
    const QString STR_IN = Keywords::KW_IN + " ";                // foreach p in something do ... done


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void reportError(QString err);
    void drawPlot(QSharedPointer<Plot>);
    
    QSharedPointer<Statement> resolveCodeline(QStringList &codelines, QVector<QSharedPointer<Statement> > &statements, QSharedPointer<Statement> parentScope);

    void setCurrentStatement(const QString &newCurrentStatement);

    QSharedPointer<Function> getFunction(const QString& name, QSharedPointer<Assignment> &assignment);
    QSharedPointer<Assignment> providePointsOfDefinition(const QString &codeline, QString assignment_body, const QString &varName, const QString &targetProperties);

    QVector<QSharedPointer<Assignment> > &get_assignments();

    void resolveOverKeyword(QString codeline, QSharedPointer<Stepped>);


private slots:
    void on_splitter_splitterMoved(int pos, int index);
    void on_toolButton_clicked();

private:
    void drawCoordinateSystem();
    void consumeSpace(QString&);
    QString getDelimitedId(QString&, QSet<char>, char &delim);
    QString getDelimitedId(QString&, QSet<char> = {' '});

    static int nextNumber();

    int sceneX(double x);
    int sceneY(double y);
    double coordStartX();
    double coordEndX();
    double coordStartY();
    double coordEndY();
    double zoomFactor();

    QSharedPointer<Statement> createPlot(const QString &codeline);
    QSharedPointer<Statement> resolveObjectAssignment(const QString &codeline);
    QSharedPointer<Statement> createFunction(const QString& codeline);
    QSharedPointer<Statement> createSett(const QString &codeline);
    QSharedPointer<Statement> createLoop(const QString &codeline, QStringList& codelines);

    template<class E>
    void genericPlotIterator(QSharedPointer<Plot> plot, E executor)
    {
        QSharedPointer<Assignment> assignment(nullptr);
        bool continuous = true;

        // first test: see if this is a function we need to plot
        QSharedPointer<Function> funToUse = getFunction(plot->plotTarget, assignment);

        if(!funToUse)
        {
            // third: or maybe a point, we'll handle that here
            for(const auto& adef : qAsConst(assignments))
            {
                if(plot->plotTarget == adef->varName)
                {
                    auto fcp = adef->fullCoordProvider();
                    if( std::get<0>(fcp) && std::get<1>(fcp) )
                    {
                        drawPoint(fcp);
                        return;
                    }
                }
            }

            // here the situation is hopeless, no function, no assignment, no point, giving up
            reportError("Invalid data to plot: " + plot->plotTarget);
            return;
        }

        double plotStart = -1.0;
        double plotEnd = 1.0;
        bool counted = plot->counted;
        QSharedPointer<Function> stepFun = nullptr;

        if(assignment)
        {
            if(!plot->start)
            {
                if(! assignment->startValueProvider())
                {
                    reportError("Invalid plotting interval for " + assignment->varName + ". There is no clear start value defined for it.");
                    return;
                }
                else
                {
                    plotStart = assignment->startValueProvider()->Calculate(&rp);
                }
            }
            else
            {
                plotStart = plot->start->Calculate(&rp);
            }

            if(!plot->end)
            {
                if(! assignment->endValueProvider())
                {
                    reportError("Invalid plotting interval. There is no clear end value defined for it.");
                    return;
                }
                else
                {
                    plotEnd = assignment->endValueProvider()->Calculate(&rp);
                }
            }
            else
            {
                plotEnd = plot->end->Calculate(&rp);
            }
            continuous = assignment->continuous || plot->continuous;
            counted |= assignment->counted;
            stepFun = assignment->step;

        }
        else
        {
            stepFun = plot->step;
            continuous = plot->continuous;
            if(plot->start)
            {
                plotStart = plot->start->Calculate(&rp);
            }

            if(plot->end)
            {
                plotEnd = plot->end->Calculate(&rp);
            }
        }

        auto pars = funToUse->get_domain_variables();
        if(pars.size() == 1)
        {
            double t = stepFun->Calculate(&rp);
            double stepValue = t;
            qDebug() << "Current step:" << t << "for:" << stepFun->get_funBody();
            if(counted)
            {
                if(t > 1)
                {
                    stepValue = (plotEnd - plotStart) / (t - 1);
                }
                else
                {
                    stepValue = (plotEnd - plotStart);
                }
            }
            for(double x=plotStart; x<=plotEnd; x += stepValue)
            {

                funToUse->SetVariable(pars[0].c_str(), x);
                double y = funToUse->Calculate(&rp);

                executor(x, y, continuous);

            }

            if(!counted)
            {
                // the last points always goes to plotEnd
                funToUse->SetVariable(pars[0].c_str(), plotEnd);
                double y = funToUse->Calculate(&rp);

                executor(plotEnd, y, continuous);
            }
        }
        else
        {
            reportError("Invalid function to plot: " + plot->plotTarget + ". Multidomain functions are not supported yet");
            return;
        }

    }

    void resizeEvent(QResizeEvent* event);
    void redrawEverything();
    void drawPoint(std::tuple<QSharedPointer<Function>, QSharedPointer<Function>>);

    Ui::MainWindow *ui;
    QGraphicsScene* sc = nullptr;

    QVector<QSharedPointer<Plot>> plots;
    QVector<QSharedPointer<FunctionDefinition>> functions;
    QVector<QSharedPointer<Assignment>> assignments;
    QVector<QSharedPointer<Sett>> m_setts;

    QVector<QSharedPointer<Statement>> statements;

    // in case we resize/zoom/scroll the window, these objects will be used to redraw the scene
    QVector<DrawnLine> drawnLines;
    QVector<QPointF> drawnPoints;
    friend class Sett;
    friend class Assignment;
    QPen drawingPen;
    QString currentStatement;
    RuntimeProvider rp;

};

#endif // MAINWINDOW_H
