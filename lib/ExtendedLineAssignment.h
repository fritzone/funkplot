#ifndef EXTENDEDLINEASSIGNMENT_H
#define EXTENDEDLINEASSIGNMENT_H

#include "LineAssignment.h"

enum class ExtendedLineForm {
    PerpendicularBisector,  // perpendicular bisector of <segment>
    AngleBisector,          // angle bisector <p1> <vertex> <p3>
    ThroughParallel,        // line through <point> parallel to <line/segment>
    ThroughPerpendicular,   // line through <point> perpendicular to <line/segment>
};

struct ExtendedLineAssignment : public LineAssignment
{
    ExtendedLineAssignment() = default;
    explicit ExtendedLineAssignment(int ln, const QString& s) : LineAssignment(ln, s) {}

    bool execute(RuntimeProvider* rp) override;

    ExtendedLineForm form = ExtendedLineForm::PerpendicularBisector;

    // For PerpendicularBisector and Through*: name of the reference segment/line
    QString refName;

    // For Through*: name of the point through which the line passes
    QString refPointName;

    // For AngleBisector: the three defining points (vertex is the angle vertex)
    QString p1Name, vertexName, p3Name;
};

#endif // EXTENDEDLINEASSIGNMENT_H
