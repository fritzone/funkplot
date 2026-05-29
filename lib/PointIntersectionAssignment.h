#ifndef POINTINTERSECTIONASSIGNMENT_H
#define POINTINTERSECTIONASSIGNMENT_H

#include "Assignment.h"

struct PointIntersectionAssignment : public Assignment
{
    explicit PointIntersectionAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    bool execute(RuntimeProvider* rp) override;
    std::tuple<QSharedPointer<Function>, QSharedPointer<Function>> fullCoordProvider(RuntimeProvider* rp) override;
    QString toString() override;
    void rotate(std::tuple<double, double> rp, double angle);

    QString obj1, obj2;
    bool rotated = false;
    double rotated_x = 0.0;
    double rotated_y = 0.0;
};

#endif // POINTINTERSECTIONASSIGNMENT_H
