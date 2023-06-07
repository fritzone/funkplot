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
