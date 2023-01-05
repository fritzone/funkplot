#ifndef DONE_H
#define DONE_H

#include "Statement.h"
#include "Keywords.h"
#include "StatementHandler.h"

class RuntimeProvider;

/**
 * @brief The Done struct represents the DONE keyword
 */
struct Done : public Statement
{
    Done() = default;

    explicit Done(int ln, const QString& s) : Statement(ln, s) {}

    QString keyword() const override
    {
        return Keywords::KW_DONE;
    }

    static QString kw()
    {
        return Keywords::KW_DONE;
    }

    static QVector<QSharedPointer<Statement>> create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback srcb);

};

REGISTER_STATEMENTHANDLER(Done)

#endif // DONE_H
