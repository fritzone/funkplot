#ifndef ARITHMETICASSIGNMENTTOARRAYELEMENT_H
#define ARITHMETICASSIGNMENTTOARRAYELEMENT_H

#include "ArithmeticAssignment.h"

/**
 * @brief The ArithmeticAssignment struct represents an assignment to a numeric value
 */
struct ArithmeticAssignmentToArrayElement : public ArithmeticAssignment
{
    explicit ArithmeticAssignmentToArrayElement(int ln, const QString& s) : ArithmeticAssignment(ln, s) {}
    bool execute(RuntimeProvider* rp) override;
    QString toString() override
    {
        return "";
    }

    QSharedPointer<Function> index;       // The index value
};


#endif // ARITHMETICASSIGNMENTTOARRAYELEMENT_H
