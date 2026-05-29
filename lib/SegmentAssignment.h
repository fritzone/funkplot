#ifndef SEGMENTASSIGNMENT_H
#define SEGMENTASSIGNMENT_H

#include "Assignment.h"
#include "Function.h"

struct SegmentAssignment : public Assignment
{
    SegmentAssignment() = default;
    explicit SegmentAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    QSharedPointer<Function> x1, y1, x2, y2;
    QString p1Name;   // variable name of start-point if defined via a named point, else empty
    QString p2Name;   // variable name of end-point if defined via a named point, else empty
    QString ep1Label; // drawn label for start endpoint; empty = don't draw
    QString ep2Label; // drawn label for end endpoint; empty = don't draw
};

#endif // SEGMENTASSIGNMENT_H
