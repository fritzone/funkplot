#include "PointOnSegmentAssignment.h"
#include "RuntimeProvider.h"
#include "SegmentAssignment.h"
#include "Keywords.h"
#include "Function.h"
#include "util.h"

#include <cmath>
#include <limits>
#include <QtMath>

bool PointOnSegmentAssignment::execute(RuntimeProvider *rp)
{
    return true;
}

QPointF PointOnSegmentAssignment::compute(RuntimeProvider *rp)
{
    const double nan = std::numeric_limits<double>::quiet_NaN();

    double x1, y1, x2, y2;

    auto sa = rp->getAssignmentAs<SegmentAssignment>(segmentName);
    if (sa && sa->x1 && sa->y1 && sa->x2 && sa->y2)
    {
        x1 = sa->x1->Calculate();
        y1 = sa->y1->Calculate();
        x2 = sa->x2->Calculate();
        y2 = sa->y2->Calculate();
    }
    else
    {
        // Try compact two-point form: segmentName == "AB" → point A and point B
        QString la, lb, lc;
        auto split = [](const QString& s, QString& l1, QString& l2, QString& l3) -> int {
            auto readOne = [&](int pos) -> std::pair<QString,int> {
                if (pos >= s.length() || !s[pos].isLetter()) return {{}, pos};
                QString lbl; lbl += s[pos++];
                while (pos < s.length() && s[pos].isDigit()) lbl += s[pos++];
                return {lbl, pos};
            };
            int p = 0;
            auto [a,p1] = readOne(p); l1=a; p=p1;
            auto [b,p2] = readOne(p); l2=b; p=p2;
            auto [c,p3] = readOne(p); l3=c;
            return (!l1.isEmpty()?1:0)+(!l2.isEmpty()?1:0)+(!l3.isEmpty()?1:0);
        };
        if (split(segmentName, la, lb, lc) == 2)
        {
            auto aA = rp->getAssignment(la);
            auto aB = rp->getAssignment(lb);
            if (!aA || !aB) return QPointF(nan, nan);
            auto [axf, ayf] = aA->fullCoordProvider(rp);
            auto [bxf, byf] = aB->fullCoordProvider(rp);
            if (!axf || !ayf || !bxf || !byf) return QPointF(nan, nan);
            x1 = axf->Calculate(); y1 = ayf->Calculate();
            x2 = bxf->Calculate(); y2 = byf->Calculate();
        }
        else
        {
            return QPointF(nan, nan);
        }
    }

    if (mode == PointOnSegmentMode::Midpoint)
        return QPointF((x1 + x2) / 2.0, (y1 + y2) / 2.0);

    // Resolve fromRef to a direction: false = from P1 toward P2, true = from P2 toward P1
    bool fromEnd = false;
    if (fromRef == Keywords::KW_END)
        fromEnd = true;
    else if (fromRef != Keywords::KW_START && !fromRef.isEmpty())
    {
        // named point reference — match against the segment's endpoint variable names
        if (fromRef == sa->p2Name)
            fromEnd = true;
        // if fromRef == sa->p1Name or unrecognised, keep fromEnd = false (treat as start)
    }

    if (mode == PointOnSegmentMode::Percentage)
    {
        double pct = param->Calculate() / 100.0;
        double t = fromEnd ? (1.0 - pct) : pct;
        t = qBound(0.0, t, 1.0);
        return QPointF(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
    }

    if (mode == PointOnSegmentMode::Distance)
    {
        double dist = param->Calculate();
        double len = std::hypot(x2 - x1, y2 - y1);
        if (len < 1e-12)
            return QPointF(x1, y1);
        double t = dist / len;
        if (fromEnd) t = 1.0 - t;
        t = qBound(0.0, t, 1.0);
        return QPointF(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
    }

    if (mode == PointOnSegmentMode::ParametricT)
    {
        double t = qBound(0.0, param->Calculate(), 1.0);
        return QPointF(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
    }

    if (mode == PointOnSegmentMode::Nearest)
    {
        auto qa = rp->getAssignment(nearestToPoint);
        if (!qa) return QPointF(nan, nan);
        auto fcp = qa->fullCoordProvider(rp);
        if (!std::get<0>(fcp) || !std::get<1>(fcp)) return QPointF(nan, nan);
        const double qx = std::get<0>(fcp)->Calculate();
        const double qy = std::get<1>(fcp)->Calculate();

        const double dx = x2 - x1, dy = y2 - y1;
        const double len2 = dx * dx + dy * dy;
        if (len2 < 1e-24)
            return QPointF(x1, y1);
        double t = ((qx - x1) * dx + (qy - y1) * dy) / len2;
        t = qBound(0.0, t, 1.0);
        return QPointF(x1 + t * dx, y1 + t * dy);
    }

    if (mode == PointOnSegmentMode::AtX)
    {
        const double X = param->Calculate();
        const double dx = x2 - x1;
        if (std::abs(dx) < 1e-12) return QPointF(nan, nan);
        const double t = (X - x1) / dx;
        if (t < 0.0 || t > 1.0) return QPointF(nan, nan);
        return QPointF(X, y1 + t * (y2 - y1));
    }

    if (mode == PointOnSegmentMode::AtY)
    {
        const double Y = param->Calculate();
        const double dy = y2 - y1;
        if (std::abs(dy) < 1e-12) return QPointF(nan, nan);
        const double t = (Y - y1) / dy;
        if (t < 0.0 || t > 1.0) return QPointF(nan, nan);
        return QPointF(x1 + t * (x2 - x1), Y);
    }

    return QPointF(nan, nan);
}

std::tuple<QSharedPointer<Function>, QSharedPointer<Function>> PointOnSegmentAssignment::fullCoordProvider(RuntimeProvider *rp)
{
    if (rotated)
    {
        QString spx = QString::number(rotated_x, 'f', 10);
        QString spy = QString::number(rotated_y, 'f', 10);
        return {Function::temporaryFunction(spx, this), Function::temporaryFunction(spy, this)};
    }

    QPointF pt = compute(rp);
    if (std::isnan(pt.x()) || std::isnan(pt.y()))
        return {nullptr, nullptr};

    QString sx = QString::number(pt.x(), 'f', 10);
    QString sy = QString::number(pt.y(), 'f', 10);
    return {Function::temporaryFunction(sx, this), Function::temporaryFunction(sy, this)};
}

QString PointOnSegmentAssignment::toString()
{
    auto fcp = fullCoordProvider(RuntimeProvider::get());
    auto x_provider = std::get<0>(fcp);
    auto y_provider = std::get<1>(fcp);
    if (x_provider && y_provider)
    {
        double x = x_provider->Calculate();
        double y = y_provider->Calculate();
        return "Point(" + QString::number(x, 'f', 6) + ", " + QString::number(y, 'f', 6) + ")";
    }
    return "Point(0.0, 0.0)";
}

void PointOnSegmentAssignment::rotate(std::tuple<double, double> rp, double angle)
{
    rotated = false;
    QPointF pt = compute(RuntimeProvider::get());
    if (!std::isnan(pt.x()) && !std::isnan(pt.y()))
    {
        auto p = rotatePoint(rp, angle, pt);
        rotated_x = p.x();
        rotated_y = p.y();
        rotated = true;
    }
}
