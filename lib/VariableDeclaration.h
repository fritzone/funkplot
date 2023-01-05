#ifndef VARIABLEDECLARATION_H
#define VARIABLEDECLARATION_H

#include "Keywords.h"
#include "Statement.h"
#include "StatementHandler.h"

struct VariableDeclaration : public Statement
{
    VariableDeclaration() =default;
    explicit VariableDeclaration(int ln, const QString& s) : Statement(ln, s) {}

    static bool regVariableDeclaration;

    QString keyword() const override
    {
        return kw();
    }

    static QString kw()
    {
        return Keywords::KW_VAR;
    }

    static QVector<QSharedPointer<Statement>> create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback srcb);

    QString type;
    QString name;

};
REGISTER_STATEMENTHANDLER(VariableDeclaration)

#endif // VARIABLEDECLARATION_H
