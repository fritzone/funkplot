#ifndef FUNCTIONDEFINITION_H
#define FUNCTIONDEFINITION_H

#include "Keywords.h"
#include "Statement.h"
#include "StatementHandler.h"

class Function;

/**
 * @brief The FunctionDefinition struct represents a function definition statement
 */
struct FunctionDefinition : public Statement
{
    FunctionDefinition() = default;
    explicit FunctionDefinition(int ln, QString a) : Statement(ln, a) {}

    QString keyword() const override
    {
        return kw();
    }

    static QString kw()
    {
        return Keywords::KW_FUNCTION;
    }

    static QVector<QSharedPointer<Statement> > create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback);

    QSharedPointer<Function> f; // the actual function object

    static bool regFunctionDefinition;
};

REGISTER_STATEMENTHANDLER(FunctionDefinition)


#endif // FUNCTIONDEFINITION_H
