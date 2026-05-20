#include "PointsOfObjectAssignment.h"
#include "Plot.h"
#include "RuntimeProvider.h"
#include "StatementHandler.h"
#include "Function.h"

#include <QPointF>
#include <QDebug>

class RuntimeProvider;

bool PointsOfObjectAssignment::execute(RuntimeProvider *rp)
{
    if (ofWhat == Keywords::KW_LINE || ofWhat == Keywords::KW_SEGMENT) {
        double vx1 = x1->Calculate();
        double vy1 = y1->Calculate();
        double vx2 = x2->Calculate();
        double vy2 = y2->Calculate();

        double draw_x1 = vx1, draw_y1 = vy1, draw_x2 = vx2, draw_y2 = vy2;

        if (ofWhat == Keywords::KW_LINE) {
            double x_min = rp->coordStartX();
            double x_max = rp->coordEndX();
            double y_min = rp->coordStartY();
            double y_max = rp->coordEndY();

            double dx = vx2 - vx1;
            double dy = vy2 - vy1;

            if (dx == 0 && dy == 0) return true;

            auto get_y = [&](double x) { return vy1 + (dy / dx) * (x - vx1); };
            auto get_x = [&](double y) { return vx1 + (dx / dy) * (y - vy1); };

            QVector<QPointF> intersections;
            if (dx != 0) {
                double y_at_min = get_y(x_min);
                if (y_at_min >= y_min && y_at_min <= y_max) intersections.append({x_min, y_at_min});
                double y_at_max = get_y(x_max);
                if (y_at_max >= y_min && y_at_max <= y_max) intersections.append({x_max, y_at_max});
            }
            if (dy != 0) {
                double x_at_min = get_x(y_min);
                if (x_at_min >= x_min && x_at_min <= x_max) intersections.append({x_at_min, y_min});
                double x_at_max = get_x(y_max);
                if (x_at_max >= x_min && x_at_max <= x_max) intersections.append({x_at_max, y_max});
            }

            if (intersections.size() >= 2) {
                draw_x1 = intersections[0].x();
                draw_y1 = intersections[0].y();
                draw_x2 = intersections[1].x();
                draw_y2 = intersections[1].y();
            } else {
                return true;
            }
        }

        int count = counted ? static_cast<int>(step->Calculate()) : 100;
        if (count < 1) count = 1;

        QVector<QPointF> allPoints;
        for (int i = 0; i < count; ++i) {
            double t = (count > 1) ? static_cast<double>(i) / (count - 1) : 0.0;
            double px = draw_x1 + t * (draw_x2 - draw_x1);
            double py = draw_y1 + t * (draw_y2 - draw_y1);
            allPoints.append(QPointF(px, py));
        }
        setPrecalculatedPoints(allPoints);
        setPrecalculatedSetForce(true);
        return true;
    }

    // create a temporary plot
    QString tempPlotCode = "plot " + ofWhat + ((start && end) ? " over ("
                                                              + QString::number(start->Calculate(), 'f', 6) + ", "
                                                              + QString::number(end->Calculate(), 'f', 6) + ") " : "") +  (counted ? " counts " + QString::number(static_cast<int>(step->Calculate())) : "");
    qDebug() << "Here and done nothing:" << tempPlotCode;

    auto ph = HandlerStore::instance().getHandler("plot");

    QStringList e {};
    Statement::StatementCallback exec = [](QSharedPointer<Statement>)->bool{return true;};
    StatementReaderCallback srexec = [](int&, QStringList&, QVector<QSharedPointer<Statement>>&, QSharedPointer<Statement>) { return nullptr;};
    auto p = ph->execute(lineNumber, tempPlotCode, e, exec, srexec);

    QSharedPointer<Assignment> unused;
    auto fun = rp->getNameFunctionOrAssignment(ofWhat, unused);

    this->resolvePrecalculatedPointsForIndexedAccessWithFunction(p.at(0).dynamicCast<Plot>(), fun, rp);
    return true;
}

QString PointsOfObjectAssignment::providedFunction()
{
    return ofWhat;
}

QString PointsOfObjectAssignment::toString()
{
    execute(runtimeProvider);
    auto pcp = getPrecalculatedPoints();
    if(!pcp.empty())
    {
        QString result = "[";
        QStringList points;
        for(const auto& pcp : pcp)
        {
            points.append("Point(" + QString::number(pcp.x(), 'f', 6) + ", " + QString::number(pcp.y(), 'f', 6) + ")");
        }
        result += points.join(",") + "]";
        return result;
    }
    else
    {
        return "[]";
    }
}

QSharedPointer<Function> PointsOfObjectAssignment::startValueProvider()
{
    return start;
}

QSharedPointer<Function> PointsOfObjectAssignment::endValueProvider()
{
    return end;
}

void PointsOfObjectAssignment::rotate(std::tuple<double, double> rp, double angle)
{
    // firstly let's populate the points
    execute(RuntimeProvider::get());
    auto pcp = getPrecalculatedPoints();

    // then rotate all the precalculated points
    if(!pcp.empty())
    {
        QVector<QPointF> rotated;
        for(const auto& pcp : qAsConst(pcp))
        {
            rotated.append( rotatePoint(rp, angle, pcp) );
        }
        pcp.swap(rotated);
        setPrecalculatedSetForce(true);
    }

}
