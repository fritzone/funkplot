#include "PointArrayAssignment.h"
#include "RuntimeProvider.h"


bool PointArrayAssignment::execute(RuntimeProvider *rp)
{
    resolvePrecalculatedPointsForIndexedAccessWithList(nullptr, rp);
    return true;
}

QString PointArrayAssignment::toString()
{
    if(getPrecalculatedPoints().empty())
    {
        return "[]";
    }
    else
    {
        QString result = "[";
        QStringList points;
        for(const auto& pcp : qAsConst(getPrecalculatedPoints()))
        {
            points.append("Point(" + QString::number(pcp.x(), 'f', 6) + ", " + QString::number(pcp.y(), 'f', 6) + ")");
        }
        result += points.join(",") + "]";
        return result;
    }

}

void PointArrayAssignment::resolvePrecalculatedPointsForIndexedAccessWithList(QSharedPointer<Plot>, RuntimeProvider *rp)
{
    QVector<QPointF> allPoints;

    if(m_elements.size() > 0)
    {
        for(const auto& p : qAsConst(m_elements))
        {
            double x = std::get<0>(p)->Calculate();
            double y = std::get<1>(p)->Calculate();

            allPoints.append(QPointF{x, y});
        }
    }
    else
    {
        auto pcp = getPrecalculatedPoints();
        if(pcp.size() > 0)
        {
            for(const auto& p : qAsConst(pcp))
            {
                allPoints.append(p);
            }
        }
    }

    if(allPoints.size() > 0)
    {
        setPrecalculatedPoints(allPoints);
    }

}

void PointArrayAssignment::rotate(std::tuple<double, double> rp, double angle)
{
    QVector<std::tuple<QSharedPointer<Function>,QSharedPointer<Function>>> rotatedPoints;

    for(int idx_v = 0; idx_v < m_elements.size(); idx_v ++)
    {
        auto p = m_elements[idx_v];

        double x = std::get<0>(p)->Calculate();
        double y = std::get<1>(p)->Calculate();

        auto pRotated = rotatePoint(rp, angle, {x, y});

        auto fx = Function::temporaryFunction(pRotated.x(), this);
        auto fy = Function::temporaryFunction(pRotated.y(), this);

        rotatedPoints.append({fx, fy});
    }
    m_elements.swap(rotatedPoints);
    resolvePrecalculatedPointsForIndexedAccessWithList(nullptr, RuntimeProvider::get());
    setPrecalculatedSetForce(true);
}
