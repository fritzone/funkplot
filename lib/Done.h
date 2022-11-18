#ifndef DONE_H
#define DONE_H

#include "Statement.h"
#include "Keywords.h"

class RuntimeProvider;

/**
 * @brief The Done struct represents the DONE keyword
 */
struct Done : public Statement
{
    explicit Done(int ln, const QString& s) : Statement(ln, s) {}

    bool execute(RuntimeProvider* mw) override
    {
        return true;
    }

    QString keyword() const override
    {
        return Keywords::KW_DONE;
    }
};


#endif // DONE_H
