#ifndef CODEENGINE_H
#define CODEENGINE_H

#include "Function.h"

#include <QString>
#include <QVector>
#include <QSharedPointer>

class RuntimeProvider;

namespace Keywords
{
const QString KW_ROTATE = "rotate";        // rotate <scene|point|assignment_of_points|object> with X [degrees|radians] [around <point>]
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

    bool rotated = false;

    double rotated_x = 0.0;
    double rotated_y = 0.0;

    bool execute(RuntimeProvider* rp) override;
    std::tuple<QSharedPointer<Function>, QSharedPointer<Function>> fullCoordProvider() override
    {
        if(rotated)
        {
            QString spx = QString::number(rotated_x, 'f', 10);
            QString spy = QString::number(rotated_y, 'f', 10);
            return {temporaryFunction(spx), temporaryFunction(spy)};
        }
        else
        {
            return {x, y};
        }
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

struct Rotation : public Statement
{
    explicit Rotation(QString a) : Statement(a) {}

    QSharedPointer<Function> degree;
    QString what;
    QString unit;

    bool execute(RuntimeProvider* rp) override;
    QString keyword() const override
    {
        return Keywords::KW_ROTATE;
    }

};

#endif // CODEENGINE_H
