#include "Assignment.h"
#include "Keywords.h"
#include "Function.h"
#include "RuntimeProvider.h"
#include "Executor.h"
#include "ArrayAssignment.h"
#include "Plot.h"

#include "PointsOfObjectAssignment.h"
#include "constants.h"

#include <QVector>
#include <QSharedPointer>

Assignment::Assignment(int ln, const QString &s) : Statement(ln, s)
{
    qDebug() << "Create:" << s << "At:" << (void*)this;
}

QString Assignment::providedFunction()
{
    return "";
}

QSharedPointer<Function> Assignment::startValueProvider()
{
    return nullptr;
}

QSharedPointer<Function> Assignment::endValueProvider()
{
    return nullptr;
}

QString Assignment::keyword() const
{
    return Keywords::KW_LET;
}

std::tuple<QSharedPointer<Function>, QSharedPointer<Function> > Assignment::fullCoordProvider(RuntimeProvider *rp)
{
    return {nullptr, nullptr};
}

void Assignment::resolvePrecalculatedPointsForIndexedAccessWithFunction(QSharedPointer<Plot> plot, QSharedPointer<Function> funToUse, RuntimeProvider* rp)
{
    QVector<QPointF> allPoints;
    auto pointGatherer = [&allPoints](double x, double y, bool c)
    {
        allPoints.append({ x, y });
    };

    bool continuous = true;
    double plotStart = -1.0;
    double plotEnd = 1.0;
    bool counted = plot->counted;
    double stepValue = 0.01;
    int count = -1;
    bool useDefaultIntrval = true;

    rp->resolvePlotInterval(plot, sharedFromThis(), continuous, plotStart, plotEnd, counted, stepValue, count, useDefaultIntrval);

    std::tuple<bool, double, double, bool, double, int> currentCalculatedId {continuous, plotStart, plotEnd, counted, stepValue, count};

    if(currentCalculatedId != lastPrecalculatedIntervalData)
    {
        Executor<decltype(pointGatherer)> pgex(pointGatherer);
        lastPrecalculatedIntervalData = pgex.execute(plot,
                     sharedFromThis(),
                     funToUse,
                     [&rp](QSharedPointer<Plot> plot,
                           QSharedPointer<Assignment> assignment,
                           bool& continuous,
                           double& plotStart,
                           double& plotEnd,
                           bool& counted,
                           double& stepValue,
                           int& count,
                           bool useDefaultValues)
                    {
                         rp->resolvePlotInterval(plot, assignment, continuous, plotStart, plotEnd, counted, stepValue, count, useDefaultValues);
                    },
                    true,
                    [&rp, this](int l, int c, QString s) {rp->reportError(lineNumber, c, s); },
                    -1,
                    rp
        );

        if(allPoints.isEmpty())
        {
            rp->reportError(lineNumber, ERRORCODE(15), "Invalid data to plot: " + plot->plotTarget);
        }

        precalculatedPoints = allPoints;
        precalculatedSetForce = false;
    }
}

void Assignment::dealWithIndexedAssignmentToSomething(RuntimeProvider *rp, IndexedAccess* ia_m)
{
    QString at = rp->typeOfVariable(varName);

    // assigning to a point
    if(at == "point" || at == "point")
    {
        auto p = rp->getAssignment(ia_m->indexedVariable);
        if(p)
        {
            if(p->targetProperties == "points")
            {
                if(p->precalculatedPoints.isEmpty())
                {
                    // get the start/end of the "Stepped" object
                    IndexedAccess* ia = nullptr; Assignment* a = nullptr;
                    auto svp = p->startValueProvider();
                    double v = svp ? svp->Calculate(rp, ia, a) : -1.0;
                    auto evp = p->endValueProvider();
                    double e = evp ? evp->Calculate(rp, ia, a) : 1.0;
                    auto stepFun = p->step;
                    double stepv = DEFAULT_RANGE_STEP;

                    if(stepFun)
                    {
                        auto count = stepFun->Calculate(rp, ia, a);
                        if(p->counted)
                        {
                            if(count > 1)
                            {
                                stepv = (e - v) / (count - 1);
                            }
                            else
                            {
                                stepv = (e - v);
                            }
                        }
                    }


                    if(v < e && stepv < 0.0 || v > e && stepv > 0.0)
                    {
                        throw syntax_error_exception(ERRORCODE(0), "Erroneous looping conditions detected, check your range");
                    }
                    // and calculate the points
                    auto f = rp->getFunction(p->providedFunction());
                    if(!f)
                    {
                        throw syntax_error_exception(ERRORCODE(60), "No function for points assignment");
                    }
                    QVector<QPointF> allPoints;
                    auto pars = f->get_domain_variables();
                    if(pars.size() == 1)
                    {
                        do
                        {
                            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
                            f->SetVariable(pars[0].c_str(), v);
                            auto fv = f->Calculate(rp, ia, a);
                            allPoints.push_back({v, fv});
                            v += stepv;
                            if(stepv < 0.0 && v < e)
                            {
                                break;
                            }

                            if(stepv > 0.0 && v > e)
                            {
                                break;
                            }
                        }
                        while(true);
                    }
                    p->precalculatedPoints = allPoints;
                }

                if(ia_m->index < p->precalculatedPoints.size())
                {
                    double px = p->precalculatedPoints[ia_m->index].x();
                    double py = p->precalculatedPoints[ia_m->index].y();

                    QSharedPointer<PointDefinitionAssignment> assignmentData;
                    assignmentData.reset(new PointDefinitionAssignment(lineNumber, this->statement));

                    assignmentData->x = Function::temporaryFunction(QString::number(px, 'f', 6), this);
                    assignmentData->y = Function::temporaryFunction(QString::number(py, 'f', 6), this);
                    assignmentData->varName = varName + ":";
                    rp->addOrUpdateAssignment(assignmentData);
                }

            }
        }
    }
    else
    if(at == Types::TYPE_NUMBER)
    {
        // assigning to a number
        auto source = rp->getAssignment(ia_m->indexedVariable);
        if(source)
        {
            QString sourceType = rp->typeOfVariable(source->varName);
            if(sourceType != Types::TYPE_LIST)
            {
                throw syntax_error_exception(ERRORCODE(61), "Cannot assign a non numeric value (<b>%s</b>) to a numeric variable (<b>%s</b>)", source->varName.toStdString().c_str(), varName.toStdString().c_str());
            }

            auto arrayAssignment = rp->getAssignmentAs<ArrayAssignment>(source->varName);
            if(!arrayAssignment)
            {
                auto pointsOfAssignment = rp->getAssignmentAs<PointsOfObjectAssignment>(source->varName);
                if(!pointsOfAssignment)
                {
                    throw syntax_error_exception(ERRORCODE(62), "Cannot identify the object assigning from: <b>%s</b> in this context: <b>%s</b>", source->varName.toStdString().c_str(), statement.toStdString().c_str());
                }
                // find the point we are looking for

                //double v = pointsOfAssignment->precalculatedPoints[ia_m->index];
                //rp->variables()[varName] = v;
                return;
            }

            // this
            if(ia_m->index >= arrayAssignment->m_elements.size())
            {
                throw syntax_error_exception(ERRORCODE(55), "Index out of bounds for <b>%s</b> in this context: <b>%s</b>. Requested <b>%d</b>, available: <b>%d</b>",
                                             source->varName.toStdString().c_str(), statement.toStdString().c_str(), ia_m->index, arrayAssignment->m_elements.size());
            }

            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            double v = arrayAssignment->m_elements[ia_m->index]->Calculate(rp, ia, a);
            rp->variables()[varName] = v;
        }
        else
        {
            throw syntax_error_exception(ERRORCODE(64), "Cannot identify the assignment source (<b>%s</b>)", ia_m->indexedVariable.toStdString().c_str());
        }
    }
}

void Assignment::forceSetPrecalculatedPoints(const QVector<QPointF> &newPoints)
{
    precalculatedPoints = newPoints;
    precalculatedSetForce = true;
}
