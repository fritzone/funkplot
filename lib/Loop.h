#ifndef LOOP_H
#define LOOP_H

#include "Keywords.h"
#include "Statement.h"
#include "StatementHandler.h"

struct LoopTarget;

/**
 * @brief The Loop struct is a loop as written in the code with the "for" keyword
 */
struct Loop : public Statement
{

    Loop() = default;
    explicit Loop(int ln, const QString& s) : Statement(ln, s) {}

    bool execute(RuntimeProvider* rp) override;
    QString keyword() const override
    {
        return Keywords::KW_FOR;
    }


    void updateLoopVariable(double);
    void updateLoopVariable(QPointF v);

    static QString kw()
    {
        return Keywords::KW_FOR;
    }

    static QVector<QSharedPointer<Statement>> create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback srcb);

    QString loop_variable;
    QSharedPointer<LoopTarget> loop_target;
    QVector<QSharedPointer<Statement>> body;
};

REGISTER_STATEMENTHANDLER(Loop)

#endif // LOOP_H
