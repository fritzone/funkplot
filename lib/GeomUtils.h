#ifndef GEOMUTILS_H
#define GEOMUTILS_H

#include <QPointF>
#include <QString>

class RuntimeProvider;

// Resolves a named object (segment, line, or compact "AB" two-point form)
// to two reference points that define the line/segment direction.
// Returns false if the name cannot be resolved.
bool resolveObjectLine(RuntimeProvider* rp, const QString& name, QPointF& p1, QPointF& p2);

// Projects point P onto the line defined by lineP1 and lineP2.
// Returns the foot of the perpendicular from P to the line.
QPointF projectPointOntoLine(const QPointF& P, const QPointF& lineP1, const QPointF& lineP2);

#endif // GEOMUTILS_H
