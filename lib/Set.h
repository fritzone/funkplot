#ifndef SET_H
#define SET_H

#include "Keywords.h"
#include "Statement.h"
#include "StatementHandler.h"

/**
 * @brief The Set struct class represents the "set" keyword, ie. set something to something
 */
struct Set : public Statement
{
    Set() = default;
    explicit Set(int ln, const QString& s) : Statement(ln, s) {}

    bool execute(RuntimeProvider* rp) override;
    QString keyword() const override
    {
        return Keywords::KW_SET;
    }

    static QString kw()
    {
        return Keywords::KW_SET;
    }

    void setColor(RuntimeProvider* rp);

    static QVector<QSharedPointer<Statement>> create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback srcb);

    QString what;
    QString value;
};

REGISTER_STATEMENTHANDLER(Set)

#endif // SET_H
