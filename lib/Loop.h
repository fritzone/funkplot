#ifndef LOOP_H
#define LOOP_H

#include "Keywords.h"
#include "Statement.h"

struct LoopTarget;

/**
 * @brief The Loop struct is a loop as written in the code with the "for" keyword
 */
struct Loop : public Statement
{

    explicit Loop(int ln, const QString& s) : Statement(ln, s) {}

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

#endif // LOOP_H
