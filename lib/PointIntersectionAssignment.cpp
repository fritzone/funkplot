#include "PointIntersectionAssignment.h"
#include "RuntimeProvider.h"
#include "LineAssignment.h"
#include "SegmentAssignment.h"
#include "Function.h"

#include <QLineF>
#include <QDebug>
#include <cmath>

bool PointIntersectionAssignment::execute(RuntimeProvider *rp)
{
    return true;
}

std::tuple<QSharedPointer<Function>, QSharedPointer<Function>> PointIntersectionAssignment::fullCoordProvider(RuntimeProvider *rp)
{
    if (rotated) {
        QString spx = QString::number(rotated_x, 'f', 10);
        QString spy = QString::number(rotated_y, 'f', 10);
        return {Function::temporaryFunction(spx, this), Function::temporaryFunction(spy, this)};
    }

    auto getLine = [&](const QString& name) -> QLineF {
        auto a = rp->getAssignment(name);
        if (!a) return QLineF();
        
        QSharedPointer<Function> x1, y1, x2, y2;
        if (auto la = qSharedPointerDynamicCast<LineAssignment>(a)) {
            x1 = la->x1; y1 = la->y1; x2 = la->x2; y2 = la->y2;
        } else if (auto sa = qSharedPointerDynamicCast<SegmentAssignment>(a)) {
            x1 = sa->x1; y1 = sa->y1; x2 = sa->x2; y2 = sa->y2;
        } else {
            return QLineF();
        }
        
        if (x1 && y1 && x2 && y2) {
            return QLineF(x1->Calculate(), y1->Calculate(), x2->Calculate(), y2->Calculate());
        }
        return QLineF();
    };

    QLineF l1 = getLine(obj1);
    QLineF l2 = getLine(obj2);

    if (l1.isNull() || l2.isNull()) {
        return {nullptr, nullptr};
    }

    QPointF intersection;
    auto itype = l1.intersect(l2, &intersection);
    if (itype != QLineF::NoIntersection) {
        QString sx = QString::number(intersection.x(), 'f', 10);
        QString sy = QString::number(intersection.y(), 'f', 10);
        return {Function::temporaryFunction(sx, this), Function::temporaryFunction(sy, this)};
    }

    // Parallel lines, return NaN
    QString snan = "NaN";
    return {Function::temporaryFunction(snan, this), Function::temporaryFunction(snan, this)};
}

QString PointIntersectionAssignment::toString()
{
    auto fcp = fullCoordProvider(RuntimeProvider::get());
    auto x_provider = std::get<0>(fcp);
    auto y_provider = std::get<1>(fcp);
    if( x_provider && y_provider )
    {
        double x = x_provider->Calculate();
        double y = y_provider->Calculate();

        QString r = "Point(" + QString::number(x, 'f', 6) + ", " + QString::number(y, 'f', 6) + ")";
        return r;
    }
    else
    {
        return "Point(0.0, 0.0)";
    }
}

void PointIntersectionAssignment::rotate(std::tuple<double, double> rp, double angle)
{
    rotated = false;

    auto fcp = fullCoordProvider(RuntimeProvider::get());
    if( std::get<0>(fcp) && std::get<1>(fcp) )
    {
        double x = std::get<0>(fcp)->Calculate();
        double y = std::get<1>(fcp)->Calculate();

        auto p = rotatePoint(rp, angle, {x, y});

        rotated_x = p.x();
        rotated_y = p.y();
        rotated = true;
    }
}
