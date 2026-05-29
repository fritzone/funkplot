#ifndef EXTENDEDSEGMENTASSIGNMENT_H
#define EXTENDEDSEGMENTASSIGNMENT_H

#include "SegmentAssignment.h"

enum class ExtendedSegmentForm {
    FromPointLengthAngle,      // segment from p length L at <angle>
    CenteredLengthAngle,       // segment centered at p length L at <angle>
    ThroughParallel,           // segment centered at p parallel to s length L
    ThroughPerpendicular,      // segment centered at p perpendicular to s length L
    CongruentFromParallel,     // segment congruent to src from p parallel to s
    CongruentFromPerpendicular,// segment congruent to src from p perpendicular to s
    PerpendicularFromPoint,    // segment from p perpendicular to s  (foot of altitude)
    FromThroughLength,         // segment from A through B length L  (starts at A, direction A→B)
};

struct ExtendedSegmentAssignment : public SegmentAssignment
{
    ExtendedSegmentAssignment() = default;
    explicit ExtendedSegmentAssignment(int ln, const QString& s) : SegmentAssignment(ln, s) {}

    bool execute(RuntimeProvider* rp) override;

    ExtendedSegmentForm form = ExtendedSegmentForm::FromPointLengthAngle;

    // Reference point parsed by parseEndpoint
    QSharedPointer<Function> refPx, refPy;
    QString refPointName;

    // Length expression
    QSharedPointer<Function> lengthFn;

    // Angle: either a named angle variable (already in radians) or a literal expression + unit flag
    QString    angleVarName;       // non-empty → look up rp->value(angleVarName) in radians
    QSharedPointer<Function> angleLiteral;
    bool       angleInDegrees = true;

    // Reference segment (for parallel/perpendicular direction, and congruent source)
    QString refSegmentName;
    // Non-empty: take length from this segment instead of lengthFn
    QString lengthFromSegmentName;
    // For FromThroughLength: name of the through-point (direction from refPoint to throughPoint)
    QString throughPointName;

private:
    double resolveAngle(RuntimeProvider* rp);
    QPointF resolveRefPoint(RuntimeProvider* rp);
};

#endif // EXTENDEDSEGMENTASSIGNMENT_H
