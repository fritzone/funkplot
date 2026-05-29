#ifndef POINTONSEGMENTASSIGNMENT_H
#define POINTONSEGMENTASSIGNMENT_H

#include "Assignment.h"
#include <QPointF>

enum class PointOnSegmentMode {
    Percentage,
    Distance,
    ParametricT,
    Nearest,
    AtX,
    AtY,
    Midpoint
};

struct PointOnSegmentAssignment : public Assignment
{
    explicit PointOnSegmentAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    bool execute(RuntimeProvider* rp) override;
    std::tuple<QSharedPointer<Function>, QSharedPointer<Function>> fullCoordProvider(RuntimeProvider* rp) override;
    QString toString() override;
    void rotate(std::tuple<double, double> rp, double angle);

    PointOnSegmentMode mode = PointOnSegmentMode::Midpoint;
    QString segmentName;
    QString fromRef;            // "start", "end", or a named point variable
    QSharedPointer<Function> param;  // value for Percentage/Distance/ParametricT/AtX/AtY
    QString nearestToPoint;     // for Nearest mode

    bool rotated = false;
    double rotated_x = 0.0;
    double rotated_y = 0.0;

private:
    QPointF compute(RuntimeProvider* rp);
};

#endif // POINTONSEGMENTASSIGNMENT_H
