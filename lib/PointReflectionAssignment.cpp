#include "PointReflectionAssignment.h"
#include "RuntimeProvider.h"
#include "GeomUtils.h"
#include "util.h"

bool PointReflectionAssignment::execute(RuntimeProvider* rp)
{
    return true;
}

std::tuple<QSharedPointer<Function>, QSharedPointer<Function>>
PointReflectionAssignment::fullCoordProvider(RuntimeProvider* rp)
{
    if (rotated) {
        return {Function::temporaryFunction(QString::number(rotated_x, 'f', 10), this),
                Function::temporaryFunction(QString::number(rotated_y, 'f', 10), this)};
    }

    auto src = rp->getAssignment(sourceName);
    if (!src) return {nullptr, nullptr};
    auto [xf, yf] = src->fullCoordProvider(rp);
    if (!xf || !yf) return {nullptr, nullptr};
    QPointF P = {xf->Calculate(), yf->Calculate()};

    QPointF refP1, refP2;
    if (!resolveObjectLine(rp, refName, refP1, refP2)) return {nullptr, nullptr};

    QPointF foot = projectPointOntoLine(P, refP1, refP2);
    double rx = 2.0 * foot.x() - P.x();
    double ry = 2.0 * foot.y() - P.y();

    return {Function::temporaryFunction(QString::number(rx, 'f', 10), this),
            Function::temporaryFunction(QString::number(ry, 'f', 10), this)};
}

QString PointReflectionAssignment::toString()
{
    auto [xf, yf] = fullCoordProvider(RuntimeProvider::get());
    if (xf && yf)
        return "Point(" + QString::number(xf->Calculate(), 'f', 6) + ", " + QString::number(yf->Calculate(), 'f', 6) + ")";
    return "Point(0.0, 0.0)";
}

void PointReflectionAssignment::rotate(std::tuple<double,double> center, double angle)
{
    rotated = false;
    auto [xf, yf] = fullCoordProvider(RuntimeProvider::get());
    if (xf && yf) {
        auto p = rotatePoint(center, angle, {xf->Calculate(), yf->Calculate()});
        rotated_x = p.x();
        rotated_y = p.y();
        rotated = true;
    }
}
