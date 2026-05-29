#include "MeasurementAssignment.h"
#include "RuntimeProvider.h"
#include "SegmentAssignment.h"
#include "GeomUtils.h"

#include <cmath>

bool MeasurementAssignment::execute(RuntimeProvider* rp)
{
    double result = 0.0;

    switch (form)
    {
    case MeasurementForm::DistanceFromTo:
    {
        auto aA = rp->getAssignment(p1Name);
        auto aB = rp->getAssignment(p2Name);
        if (!aA || !aB) return false;
        auto [axf, ayf] = aA->fullCoordProvider(rp);
        auto [bxf, byf] = aB->fullCoordProvider(rp);
        if (!axf || !ayf || !bxf || !byf) return false;
        double dx = bxf->Calculate() - axf->Calculate();
        double dy = byf->Calculate() - ayf->Calculate();
        result = std::sqrt(dx*dx + dy*dy);
        break;
    }
    case MeasurementForm::LengthOfSegment:
    {
        QPointF p1, p2;
        if (!resolveObjectLine(rp, segName, p1, p2)) return false;
        double dx = p2.x() - p1.x(), dy = p2.y() - p1.y();
        result = std::sqrt(dx*dx + dy*dy);
        break;
    }
    }

    rp->setValue(varName, result);
    return true;
}
