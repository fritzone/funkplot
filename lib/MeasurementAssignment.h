#ifndef MEASUREMENTASSIGNMENT_H
#define MEASUREMENTASSIGNMENT_H

#include "Assignment.h"

enum class MeasurementForm { DistanceFromTo, LengthOfSegment };

struct MeasurementAssignment : public Assignment
{
    MeasurementAssignment() = default;
    explicit MeasurementAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    bool execute(RuntimeProvider* rp) override;

    MeasurementForm form = MeasurementForm::DistanceFromTo;

    // DistanceFromTo: distance between two named points
    QString p1Name, p2Name;

    // LengthOfSegment: length of a named segment
    QString segName;
};

#endif // MEASUREMENTASSIGNMENT_H
