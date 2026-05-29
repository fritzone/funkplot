#include "ExtendedSegmentAssignment.h"
#include "RuntimeProvider.h"
#include "Function.h"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Resolve a segment name to its four endpoints.
// Handles both a real SegmentAssignment and a compact two-point form like "AB"
// where A and B are known point variables.
static bool resolveSegmentEndpoints(RuntimeProvider* rp, const QString& name,
                                    double& sx1, double& sy1, double& sx2, double& sy2)
{
    auto sa = rp->getAssignmentAs<SegmentAssignment>(name);
    if (sa && sa->x1 && sa->y1 && sa->x2 && sa->y2) {
        sx1 = sa->x1->Calculate(); sy1 = sa->y1->Calculate();
        sx2 = sa->x2->Calculate(); sy2 = sa->y2->Calculate();
        return true;
    }
    // Try compact two-point form: split "AB" into "A" + "B"
    auto readLabel = [&](int pos) -> std::pair<QString,int> {
        if (pos >= name.length() || !name[pos].isLetter()) return {{}, pos};
        QString lbl; lbl += name[pos++];
        while (pos < name.length() && name[pos].isDigit()) lbl += name[pos++];
        return {lbl, pos};
    };
    int p = 0;
    auto [la, p1] = readLabel(p); p = p1;
    auto [lb, p2] = readLabel(p); p = p2;
    if (la.isEmpty() || lb.isEmpty() || p != name.length()) return false;

    auto aA = rp->getAssignment(la);
    auto aB = rp->getAssignment(lb);
    if (!aA || !aB) return false;
    auto [axf, ayf] = aA->fullCoordProvider(rp);
    auto [bxf, byf] = aB->fullCoordProvider(rp);
    if (!axf || !ayf || !bxf || !byf) return false;
    sx1 = axf->Calculate(); sy1 = ayf->Calculate();
    sx2 = bxf->Calculate(); sy2 = byf->Calculate();
    return true;
}

QPointF ExtendedSegmentAssignment::resolveRefPoint(RuntimeProvider* rp)
{
    if (!refPointName.isEmpty())
    {
        auto a = rp->getAssignment(refPointName);
        if (a)
        {
            auto [xf, yf] = a->fullCoordProvider(rp);
            if (xf && yf)
                return {xf->Calculate(), yf->Calculate()};
        }
    }
    return {refPx->Calculate(), refPy->Calculate()};
}

double ExtendedSegmentAssignment::resolveAngle(RuntimeProvider* rp)
{
    if (!angleVarName.isEmpty())
        return rp->value(angleVarName.toStdString());   // already in radians
    double v = angleLiteral->Calculate();
    return angleInDegrees ? v * M_PI / 180.0 : v;
}

bool ExtendedSegmentAssignment::execute(RuntimeProvider* rp)
{
    QPointF p = resolveRefPoint(rp);

    double L = 0.0;
    if (form != ExtendedSegmentForm::PerpendicularFromPoint) {
        if (!lengthFromSegmentName.isEmpty()) {
            auto sa = rp->getAssignmentAs<SegmentAssignment>(lengthFromSegmentName);
            if (!sa) return false;
            double lx1 = sa->x1->Calculate(), ly1 = sa->y1->Calculate();
            double lx2 = sa->x2->Calculate(), ly2 = sa->y2->Calculate();
            L = std::sqrt((lx2-lx1)*(lx2-lx1) + (ly2-ly1)*(ly2-ly1));
        } else {
            L = lengthFn->Calculate();
        }
    }

    double ex1, ey1, ex2, ey2;

    switch (form)
    {
    case ExtendedSegmentForm::FromPointLengthAngle:
    {
        double a = resolveAngle(rp);
        ex1 = p.x();
        ey1 = p.y();
        ex2 = p.x() + L * std::cos(a);
        ey2 = p.y() + L * std::sin(a);
        break;
    }
    case ExtendedSegmentForm::CenteredLengthAngle:
    {
        double a = resolveAngle(rp);
        ex1 = p.x() - L / 2.0 * std::cos(a);
        ey1 = p.y() - L / 2.0 * std::sin(a);
        ex2 = p.x() + L / 2.0 * std::cos(a);
        ey2 = p.y() + L / 2.0 * std::sin(a);
        break;
    }
    case ExtendedSegmentForm::ThroughParallel:
    case ExtendedSegmentForm::ThroughPerpendicular:
    {
        double rx1, ry1, rx2, ry2;
        if (!resolveSegmentEndpoints(rp, refSegmentName, rx1, ry1, rx2, ry2)) return false;
        double a = std::atan2(ry2 - ry1, rx2 - rx1);
        if (form == ExtendedSegmentForm::ThroughPerpendicular)
            a += M_PI / 2.0;
        ex1 = p.x() - L / 2.0 * std::cos(a);
        ey1 = p.y() - L / 2.0 * std::sin(a);
        ex2 = p.x() + L / 2.0 * std::cos(a);
        ey2 = p.y() + L / 2.0 * std::sin(a);
        break;
    }
    case ExtendedSegmentForm::CongruentFromParallel:
    case ExtendedSegmentForm::CongruentFromPerpendicular:
    {
        double rx1, ry1, rx2, ry2;
        if (!resolveSegmentEndpoints(rp, refSegmentName, rx1, ry1, rx2, ry2)) return false;
        double a = std::atan2(ry2 - ry1, rx2 - rx1);
        if (form == ExtendedSegmentForm::CongruentFromPerpendicular)
            a += M_PI / 2.0;
        ex1 = p.x();
        ey1 = p.y();
        ex2 = p.x() + L * std::cos(a);
        ey2 = p.y() + L * std::sin(a);
        break;
    }
    case ExtendedSegmentForm::PerpendicularFromPoint:
    {
        double sx1, sy1, sx2, sy2;
        if (!resolveSegmentEndpoints(rp, refSegmentName, sx1, sy1, sx2, sy2)) return false;
        double dx = sx2 - sx1, dy = sy2 - sy1;
        double len2 = dx * dx + dy * dy;
        if (len2 < 1e-12) return false;
        double t = ((p.x() - sx1) * dx + (p.y() - sy1) * dy) / len2;
        ex1 = p.x();
        ey1 = p.y();
        ex2 = sx1 + t * dx;
        ey2 = sy1 + t * dy;
        break;
    }
    case ExtendedSegmentForm::FromThroughLength:
    {
        // Segment starts at refPoint, goes in direction (refPoint → throughPoint) for length L
        auto aB = rp->getAssignment(throughPointName);
        if (!aB) return false;
        auto [bxf, byf] = aB->fullCoordProvider(rp);
        if (!bxf || !byf) return false;
        double bx = bxf->Calculate(), by = byf->Calculate();
        double dx = bx - p.x(), dy = by - p.y();
        double len = std::sqrt(dx*dx + dy*dy);
        if (len < 1e-12) return false;
        ex1 = p.x();
        ey1 = p.y();
        ex2 = p.x() + L * dx / len;
        ey2 = p.y() + L * dy / len;
        break;
    }
    }

    // Publish endpoints as constant Functions so the rest of the system can use them
    x1 = Function::temporaryFunction(QString::number(ex1, 'f', 15), this);
    y1 = Function::temporaryFunction(QString::number(ey1, 'f', 15), this);
    x2 = Function::temporaryFunction(QString::number(ex2, 'f', 15), this);
    y2 = Function::temporaryFunction(QString::number(ey2, 'f', 15), this);
    return true;
}
