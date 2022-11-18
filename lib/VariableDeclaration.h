#ifndef VARIABLEDECLARATION_H
#define VARIABLEDECLARATION_H

#include "Keywords.h"
#include "Statement.h"
#include "RuntimeProvider.h"

struct VariableDeclaration : public Statement
{
    explicit VariableDeclaration(int ln, const QString& s) : Statement(ln, s), lineNumber(ln) {}

    bool execute(RuntimeProvider* rp) override
    {
        return false;
    }
    QString keyword() const override
    {
        return Keywords::KW_FOR;
    }

    QString type;
    QString name;
    int lineNumber;

};

#endif // VARIABLEDECLARATION_H
