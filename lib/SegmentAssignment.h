#ifndef SEGMENTASSIGNMENT_H
#define SEGMENTASSIGNMENT_H

#include "Assignment.h"
#include "Function.h"

struct SegmentAssignment : public Assignment
{
    SegmentAssignment() = default;
    explicit SegmentAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    QSharedPointer<Function> x1, y1, x2, y2;
};

#endif // SEGMENTASSIGNMENT_H
