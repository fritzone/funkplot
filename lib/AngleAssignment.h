#ifndef ANGLEASSIGNMENT_H
#define ANGLEASSIGNMENT_H

#include "Assignment.h"
#include "Function.h"

enum class AngleSource { Literal, OfSegment, ThreePoints };

struct AngleAssignment : public Assignment
{
    AngleAssignment() = default;
    explicit AngleAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    bool execute(RuntimeProvider* rp) override;
    QString toString() override;

    AngleSource source = AngleSource::Literal;

    // Literal: numeric expression + unit flag
    QSharedPointer<Function> expr;
    bool inDegrees = true;          // false → expression is already in radians

    // OfSegment: angle the segment makes with the positive x-axis
    QString segmentName;

    // ThreePoints: interior angle at vertex between rays to p1 and p3
    QString p1Name, vertexName, p3Name;
};

#endif // ANGLEASSIGNMENT_H
