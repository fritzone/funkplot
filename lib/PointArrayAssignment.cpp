#include "PointArrayAssignment.h"


bool PointArrayAssignment::execute(RuntimeProvider *rp)
{
    resolvePrecalculatedPointsForIndexedAccessWithList(nullptr, rp);
    return true;
}

QString PointArrayAssignment::toString()
{
    if(precalculatedPoints.empty())
    {
        return "[]";
    }
    else
    {
        QString result = "[";
        QStringList points;
        for(const auto& pcp : precalculatedPoints)
        {
            points.append("Point(" + QString::number(pcp.x(), 'f', 6) + ", " + QString::number(pcp.y(), 'f', 6) + ")");
        }
        result += points.join(",") + "]";
        return result;
    }

}
