#ifndef POINTARRAYASSIGNMENT_H
#define POINTARRAYASSIGNMENT_H

#include "Assignment.h"
#include "Function.h"

#include <tuple>

/**
 * @brief The PointArrayAssignment class is an assignment in which we assign to a
 * point list variable a list of points, like: let l = list[(1,2), (3,4)]
 */
struct PointArrayAssignment : public Assignment
{
    explicit PointArrayAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    bool execute(RuntimeProvider *rp) override;

    QString toString() override;

    void resolvePrecalculatedPointsForIndexedAccessWithList(QSharedPointer<Plot>, RuntimeProvider *rp) override;

    void rotate(std::tuple<double, double> rp, double angle);

public:

    QVector<std::tuple<QSharedPointer<Function>,QSharedPointer<Function>>> m_elements;


};

#endif // POINTARRAYASSIGNMENT_H
