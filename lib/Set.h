#ifndef SET_H
#define SET_H

#include "Keywords.h"
#include "Statement.h"

/**
 * @brief The Set struct class represents the "set" keyword, ie. set something to something
 */
struct Set : public Statement
{

    explicit Set(int ln, const QString& s) : Statement(ln, s) {}

    bool execute(RuntimeProvider* rp) override;
    QString keyword() const override
    {
        return Keywords::KW_SET;
    }

    void setColor(RuntimeProvider* rp);

    QString what;
    QString value;
};


#endif // SET_H
