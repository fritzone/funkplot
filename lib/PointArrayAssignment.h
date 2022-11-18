#ifndef POINTARRAYASSIGNMENT_H
#define POINTARRAYASSIGNMENT_H

#include "Assignment.h"
#include "Function.h"

#include <tuple>

struct PointArrayAssignment : public Assignment
{
    explicit PointArrayAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    QVector<std::tuple<QSharedPointer<Function>,QSharedPointer<Function>>> m_elements;

    bool execute(RuntimeProvider *rp) override;

    QString toString() override;

    void resolvePrecalculatedPointsForIndexedAccessWithList(QSharedPointer<Plot>, RuntimeProvider *rp) override
    {
        QVector<QPointF> allPoints;

        for(const auto& p : qAsConst(m_elements))
        {
            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            double x = std::get<0>(p)->Calculate(rp, ia, a);
            double y = std::get<1>(p)->Calculate(rp, ia, a);

            allPoints.append(QPointF{x, y});
        }

        precalculatedPoints = allPoints;

    }

};

#endif // POINTARRAYASSIGNMENT_H
