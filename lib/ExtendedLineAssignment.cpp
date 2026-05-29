#include "ExtendedLineAssignment.h"
#include "RuntimeProvider.h"
#include "Function.h"
#include "GeomUtils.h"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static QPointF resolveNamedPoint(const QString& name, RuntimeProvider* rp)
{
    auto a = rp->getAssignment(name);
    if (!a) return {};
    auto [xf, yf] = a->fullCoordProvider(rp);
    if (!xf || !yf) return {};
    return {xf->Calculate(), yf->Calculate()};
}

bool ExtendedLineAssignment::execute(RuntimeProvider* rp)
{
    double ex1, ey1, ex2, ey2;

    switch (form)
    {
    case ExtendedLineForm::PerpendicularBisector:
    {
        QPointF A, B;
        if (!resolveObjectLine(rp, refName, A, B)) return false;
        QPointF M = {(A.x() + B.x()) / 2.0, (A.y() + B.y()) / 2.0};
        // Direction of AB; perpendicular is (-dy, dx)
        double dx = B.x() - A.x(), dy = B.y() - A.y();
        double len = std::sqrt(dx*dx + dy*dy);
        if (len < 1e-12) return false;
        double pdx = -dy / len, pdy = dx / len;
        ex1 = M.x() + pdx; ey1 = M.y() + pdy;
        ex2 = M.x() - pdx; ey2 = M.y() - pdy;
        break;
    }
    case ExtendedLineForm::AngleBisector:
    {
        QPointF P1 = resolveNamedPoint(p1Name, rp);
        QPointF V  = resolveNamedPoint(vertexName, rp);
        QPointF P3 = resolveNamedPoint(p3Name, rp);
        double ax = P1.x() - V.x(), ay = P1.y() - V.y();
        double cx = P3.x() - V.x(), cy = P3.y() - V.y();
        double lenA = std::sqrt(ax*ax + ay*ay);
        double lenC = std::sqrt(cx*cx + cy*cy);
        if (lenA < 1e-12 || lenC < 1e-12) return false;
        // Normalize and sum to get bisector direction
        double uax = ax/lenA, uay = ay/lenA;
        double ucx = cx/lenC, ucy = cy/lenC;
        double bx = uax + ucx, by = uay + ucy;
        double lenB = std::sqrt(bx*bx + by*by);
        if (lenB < 1e-12) {
            // Antiparallel rays: bisector is perpendicular to one of them
            bx = -uay; by = uax;
            lenB = 1.0;
        }
        bx /= lenB; by /= lenB;
        ex1 = V.x() + bx; ey1 = V.y() + by;
        ex2 = V.x() - bx; ey2 = V.y() - by;
        break;
    }
    case ExtendedLineForm::ThroughParallel:
    case ExtendedLineForm::ThroughPerpendicular:
    {
        QPointF refP1, refP2;
        if (!resolveObjectLine(rp, refName, refP1, refP2)) return false;
        QPointF P;
        if (!refPointName.isEmpty())
            P = resolveNamedPoint(refPointName, rp);
        else if (x1 && y1)
            P = {x1->Calculate(), y1->Calculate()};
        else return false;

        double dx = refP2.x() - refP1.x(), dy = refP2.y() - refP1.y();
        double len = std::sqrt(dx*dx + dy*dy);
        if (len < 1e-12) return false;
        dx /= len; dy /= len;
        if (form == ExtendedLineForm::ThroughPerpendicular) {
            double tmp = dx; dx = -dy; dy = tmp;
        }
        ex1 = P.x() + dx; ey1 = P.y() + dy;
        ex2 = P.x() - dx; ey2 = P.y() - dy;
        break;
    }
    }

    x1 = Function::temporaryFunction(QString::number(ex1, 'f', 15), this);
    y1 = Function::temporaryFunction(QString::number(ey1, 'f', 15), this);
    x2 = Function::temporaryFunction(QString::number(ex2, 'f', 15), this);
    y2 = Function::temporaryFunction(QString::number(ey2, 'f', 15), this);
    return true;
}
