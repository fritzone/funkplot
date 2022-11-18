#ifndef ELSE_H
#define ELSE_H

#include "Statement.h"
#include "Keywords.h"

class RuntimeProvider;

/**
 * @brief The Done struct represents the DONE keyword
 */
struct Else : public Statement
{
    explicit Else(int ln, const QString& s) : Statement(ln, s) {}

    bool execute(RuntimeProvider* mw) override
    {
        return true;
    }

    QString keyword() const override
    {
        return Keywords::KW_ELSE;
    }
};

#endif // ELSE_H
