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
};

#endif // LINEASSIGNMENT_H
