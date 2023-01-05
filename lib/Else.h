#ifndef ELSE_H
#define ELSE_H

#include "Statement.h"
#include "Keywords.h"
#include "StatementHandler.h"

class RuntimeProvider;

/**
 * @brief The Done struct represents the DONE keyword
 */
struct Else : public Statement
{
    Else() = default;
    explicit Else(int ln, const QString& s) : Statement(ln, s) {}

    bool execute(RuntimeProvider* mw) override
    {
        return true;
    }

    QString keyword() const override
    {
        return Keywords::KW_ELSE;
    }

    static QString kw()
    {
        return Keywords::KW_ELSE;
    }

    static QVector<QSharedPointer<Statement>> create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback srcb);

};

REGISTER_STATEMENTHANDLER(Else)

#endif // ELSE_H
