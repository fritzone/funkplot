#ifndef APPEND_H
#define APPEND_H

#include "Statement.h"
#include "Keywords.h"
#include "Function.h"
#include "StatementHandler.h"

struct Append : public Statement
{
    Append() = default;
    explicit Append(int ln, const QString& code) : Statement(ln, code) {}

    bool execute(RuntimeProvider* rp) override;

    QString keyword() const override
    {
        return kw();
    }

    static QString kw()
    {
        return Keywords::KW_APPEND;
    }


    static QVector<QSharedPointer<Statement>> create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback);
    static bool regAppend;

    // if this is a number list the values are stored here
    QStringList numberValues;

    // otherwise the pointValues is used
    QVector<std::tuple<QSharedPointer<Function>, QSharedPointer<Function>>> pointValues;

    // what domain this append uses: numbers/points
    // TODO: This could be nicer, but a bit later
    QString domain;

    // to which variable do we append
    QString varName;
};
REGISTER_STATEMENTHANDLER(Append)

#endif // APPEND_H
