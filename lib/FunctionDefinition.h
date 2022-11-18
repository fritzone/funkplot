#ifndef FUNCTIONDEFINITION_H
#define FUNCTIONDEFINITION_H

#include "Keywords.h"
#include "Statement.h"

class Function;

/**
 * @brief The FunctionDefinition struct represents a function definition statement
 */
struct FunctionDefinition : public Statement
{
    explicit FunctionDefinition(int ln, QString a) : Statement(ln, a) {}

    bool execute(RuntimeProvider* mw) override;

    QString keyword() const override
    {
        return Keywords::KW_FUNCTION;
    }

    QSharedPointer<Function> f; // the actual function object
};


#endif // FUNCTIONDEFINITION_H
