#ifndef ARITHMETICASSIGNMENT_H
#define ARITHMETICASSIGNMENT_H

#include "Assignment.h"

class Function;

/**
 * @brief The ArithmeticAssignment struct represents an assignment to a numeric value
 */
struct ArithmeticAssignment : public Assignment
{
    explicit ArithmeticAssignment(int ln, const QString& s) : Assignment(ln, s) {}
    bool execute(RuntimeProvider* rp) override;
    QString toString() override;

    QSharedPointer<Function> arithmetic;       // if this is an arithmetic assignment this is the function
};

#endif // ARITHMETICASSIGNMENT_H
