#ifndef SEGMENTASSIGNMENT_H
#define SEGMENTASSIGNMENT_H

#include "Assignment.h"
#include "Function.h"

struct SegmentAssignment : public Assignment
{
    SegmentAssignment() = default;
    explicit SegmentAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    QSharedPointer<Function> x1, y1, x2, y2;

    // The endpoints of the extended forms are computed only when the defining let statement runs,
    // so they are null if the segment is used before that, or if its definition could not be resolved
    void requireEndpoints() const
    {
        if(!x1 || !y1 || !x2 || !y2)
        {
            throw funkplot::syntax_error_exception(ERRORCODE(49), "Segment <b>%s</b> is used before its endpoints are known (it was not assigned yet, or its definition could not be computed)", varName.toStdString());
        }
    }
    QString p1Name;   // variable name of start-point if defined via a named point, else empty
    QString p2Name;   // variable name of end-point if defined via a named point, else empty
    QString ep1Label; // drawn label for start endpoint; empty = don't draw
    QString ep2Label; // drawn label for end endpoint; empty = don't draw
};

#endif // SEGMENTASSIGNMENT_H
