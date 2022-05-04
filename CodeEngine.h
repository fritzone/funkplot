#ifndef CODEENGINE_H
#define CODEENGINE_H

#include "Keywords.h"
#include "Function.h"
#include "Statement.h"
#include "Stepped.h"

#include <QString>
#include <QVector>
#include <QSharedPointer>

class RuntimeProvider;

struct Done : public Statement
{
    explicit Done(const QString& s) : Statement(s) {}

    bool execute(RuntimeProvider* mw) override { return true; }
    QString keyword() const override
    {
        return Keywords::KW_DONE;
    }
};

struct Loop;
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
        return Keywords::KW_FOR;
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

struct Assignment : public Stepped, public Statement, public QEnableSharedFromThis<Assignment>
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

    void resolvePrecalculatedPointsForIndexedAccess(QSharedPointer<Plot> plot, QSharedPointer<Function> funToUse, RuntimeProvider *rp);

    QVector<QPointF> precalculatedPoints;
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
            return {temporaryFunction(spx, this), temporaryFunction(spy, this)};
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
    QString unit = "radians";

    QPointF around;

    QString aroundX;
    QString aroundY;

    bool execute(RuntimeProvider* rp) override;
    QString keyword() const override
    {
        return Keywords::KW_ROTATE;
    }

};

#endif // CODEENGINE_H
