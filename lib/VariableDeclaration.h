#ifndef VARIABLEDECLARATION_H
#define VARIABLEDECLARATION_H

#include "Keywords.h"
#include "Statement.h"

struct VariableDeclaration : public Statement
{
    explicit VariableDeclaration(int ln, const QString& s) : Statement(ln, s) {}

    QString keyword() const override
    {
        return Keywords::KW_FOR;
    }

    QString type;
    QString name;

};

#endif // VARIABLEDECLARATION_H
