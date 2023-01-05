#ifndef IF_H
#define IF_H

#include "Function.h"
#include "Keywords.h"
#include "Statement.h"
#include "StatementHandler.h"

/**
 * @brief The If struct is a conditional expression
 */
struct If : public Statement
{
    If() = default;
    explicit If (int ln, const QString& s) : Statement(ln, s) {}

    bool execute(RuntimeProvider* rp) override;
    QString keyword() const override
    {
        return Keywords::KW_IF;
    }

    static QString kw()
    {
        return Keywords::KW_IF;
    }

    static QVector<QSharedPointer<Statement>> create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback srcb);

    QVector<QSharedPointer<Statement>> ifBody;
    QVector<QSharedPointer<Statement>> elseBody;
    QSharedPointer<Function> expression;

};

REGISTER_STATEMENTHANDLER(If)

#endif // IF_H
