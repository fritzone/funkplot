#ifndef POINTSOFOBJECTASSIGNMENT_H
#define POINTSOFOBJECTASSIGNMENT_H

#include "Assignment.h"

class Function;

/**
 * @brief The PointsOfObjectAssignment struct is representing the assignment to the points of
 * a geometric object
 */
struct PointsOfObjectAssignment : public Assignment
{
    explicit PointsOfObjectAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    bool execute(RuntimeProvider* rp) override;
    QString providedFunction() override;
    QString toString() override;

    QSharedPointer<Function> startValueProvider() override;
    QSharedPointer<Function> endValueProvider() override;

    void rotate(std::tuple<double, double> rp, double angle);

public:

    QString ofWhat;                     // can be a function for now (or a circle, etc... later)
    QSharedPointer<Function> ofWhatFun; // what, the function

};


#endif // POINTSOFOBJECTASSIGNMENT_H
