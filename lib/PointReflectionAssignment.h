#ifndef POINTREFLECTIONASSIGNMENT_H
#define POINTREFLECTIONASSIGNMENT_H

#include "Assignment.h"
#include "Function.h"

struct PointReflectionAssignment : public Assignment
{
    PointReflectionAssignment() = default;
    explicit PointReflectionAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    bool execute(RuntimeProvider* rp) override;
    std::tuple<QSharedPointer<Function>, QSharedPointer<Function>> fullCoordProvider(RuntimeProvider* rp) override;
    QString toString() override;
    void rotate(std::tuple<double,double> center, double angle);

    QString sourceName;  // point to reflect
    QString refName;     // line or segment to reflect across

    bool rotated = false;
    double rotated_x = 0.0, rotated_y = 0.0;
};

#endif // POINTREFLECTIONASSIGNMENT_H
