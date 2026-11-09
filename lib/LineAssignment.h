#ifndef LINEASSIGNMENT_H
#define LINEASSIGNMENT_H

#include "Assignment.h"
#include "Function.h"

struct LineAssignment : public Assignment
{
    LineAssignment() = default;
    explicit LineAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    std::tuple<QSharedPointer<Function>, QSharedPointer<Function>> fullCoordProvider(RuntimeProvider* rp) override
    {
        return {nullptr, nullptr}; // Lines are infinite, can't provide a single point easily without params
    }

    QSharedPointer<Function> x1, y1, x2, y2;

    // The defining points of the extended forms are computed only when the defining let statement runs,
    // so they are null if the line is used before that, or if its definition could not be resolved
    void requireEndpoints() const
    {
        if(!x1 || !y1 || !x2 || !y2)
        {
            throw funkplot::syntax_error_exception(ERRORCODE(49), "Line <b>%s</b> is used before it is known (it was not assigned yet, or its definition could not be computed)", varName.toStdString());
        }
    }
};

#endif // LINEASSIGNMENT_H
