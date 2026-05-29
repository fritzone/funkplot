#include "AngleAssignment.h"
#include "RuntimeProvider.h"
#include "SegmentAssignment.h"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static QPointF resolvePointCoords(const QString& name, RuntimeProvider* rp)
{
    auto a = rp->getAssignment(name);
    if (!a) return {};
    auto [xf, yf] = a->fullCoordProvider(rp);
    if (!xf || !yf) return {};
    return {xf->Calculate(), yf->Calculate()};
}

bool AngleAssignment::execute(RuntimeProvider* rp)
{
    double radians = 0.0;

    switch (source)
    {
    case AngleSource::Literal:
    {
        double v = expr->Calculate();
        radians = inDegrees ? v * M_PI / 180.0 : v;
        break;
    }
    case AngleSource::OfSegment:
    {
        auto sa = rp->getAssignmentAs<SegmentAssignment>(segmentName);
        if (!sa) return false;
        double dx = sa->x2->Calculate() - sa->x1->Calculate();
        double dy = sa->y2->Calculate() - sa->y1->Calculate();
        radians = std::atan2(dy, dx);
        break;
    }
    case AngleSource::ThreePoints:
    {
        QPointF a = resolvePointCoords(p1Name,    rp);
        QPointF v = resolvePointCoords(vertexName, rp);
        QPointF b = resolvePointCoords(p3Name,    rp);
        double ax = a.x() - v.x(), ay = a.y() - v.y();
        double bx = b.x() - v.x(), by = b.y() - v.y();
        double lenA = std::sqrt(ax*ax + ay*ay);
        double lenB = std::sqrt(bx*bx + by*by);
        if (lenA < 1e-12 || lenB < 1e-12) return false;
        double cosA = std::max(-1.0, std::min(1.0, (ax*bx + ay*by) / (lenA * lenB)));
        radians = std::acos(cosA);
        break;
    }
    }

    rp->setValue(varName, radians);
    return true;
}

QString AngleAssignment::toString()
{
    double rad = RuntimeProvider::get()->value(varName.toStdString());
    return QString::number(rad * 180.0 / M_PI, 'f', 4) + QString::fromUtf8("°");
}
