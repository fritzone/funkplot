#include "CodeEngine.h"

#include "RuntimeProvider.h"
#include "colors.h"
#include "constants.h"
#include "qmath.h"
#include <QDebug>

bool ArythmeticAssignment::execute(RuntimeProvider *rp)
{
    if(arythmetic)
    {
        IndexedAccess* ia_m = nullptr;
        double v = arythmetic->Calculate(rp, ia_m);

        // if this is an indexed something ...
        if(ia_m)
        {
            // same logic as in RuntimeProvider.h
            char at = rp->typeOfVariable(ia_m->indexedVariable.toLocal8Bit().data());
            qDebug() << at;

            // assigning to a point
            if(at == 'p')
            {
                auto p = rp->get_assignment(ia_m->indexedVariable);
                if(p)
                {
                    if(p->targetProperties == "points")
                    {
                        if(p->precalculatedPoints.isEmpty())
                        {
                            // get the start/end of the "Stepped" object
                            IndexedAccess* ia = nullptr;
                            auto svp = p->startValueProvider();
                            double v = svp ? svp->Calculate(rp, ia) : -1.0;
                            auto evp = p->endValueProvider();
                            double e = evp ? evp->Calculate(rp, ia) : 1.0;
                            auto stepFun = p->step;
                            double stepv = DEFAULT_RANGE_STEP;

                            if(stepFun)
                            {
                                IndexedAccess* ia = nullptr;
                                auto count = stepFun->Calculate(rp, ia);
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
                                throw syntax_error_exception("Infinite loop detected, check your range");
                            }
                            // and calculate the points
                            auto f = rp->get_function(p->providedFunction());
                            if(!f)
                            {
                                throw syntax_error_exception("No function for points assignment");
                            }
                            QVector<QPointF> allPoints;
                            auto pars = f->get_domain_variables();
                            if(pars.size() == 1)

                                do
                                {


                                    IndexedAccess* ia = nullptr;
                                    f->SetVariable(pars[0].c_str(), v);


                                    auto fv = f->Calculate(rp, ia);
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

                            p->precalculatedPoints = allPoints;
                        }

                        if(ia_m->index < p->precalculatedPoints.size())
                        {
                            double px = p->precalculatedPoints[ia_m->index].x();
                            double py = p->precalculatedPoints[ia_m->index].y();

                            QSharedPointer<PointDefinitionAssignment> assignmentData;
                            assignmentData.reset(new PointDefinitionAssignment(this->statement));

                            assignmentData->x = temporaryFunction(QString::number(px, 'f', 6));
                            assignmentData->y = temporaryFunction(QString::number(py, 'f', 6));
                            assignmentData->varName = varName + ":";
                            rp->addOrUpdateAssignment(assignmentData);
                        }

                    }
                }
            }
            else
            {

            }
        }
        else
        {
            rp->variables()[varName] = v;
        }
    }

    return true;
}

bool FunctionDefinition::execute(RuntimeProvider *mw)
{
    return true;
}

bool Set::execute(RuntimeProvider *rp)
{
    if(what == "color")
    {
        std::string s = value.toStdString();

        if(Colors::colormap.count(s))
        {
            auto cui = Colors::colormap.at(s);

            rp->setPen(cui.r, cui.g, cui.b, 255);
        }
        else // is this an RGB color?
            if(value.startsWith("#"))
            {
                auto rgb = Colors::decodeHexRgb(value.toLocal8Bit().data());

                rp->setPen(std::get<0>(rgb), std::get<1>(rgb), std::get<2>(rgb), std::get<3>(rgb));
            }
            else // what remains is a list of RGB values
            {
                QStringList values = value.split(",");
                qreal a = 1.0;
                switch(values.length())
                {
                case 4: // R,G,B,A
                {
                    IndexedAccess* ia = nullptr;
                    a = temporaryFunction(values[3].simplified())->Calculate(rp, ia);
                }
                    [[fallthrough]];

                case 3: // R,G,B
                {
                    IndexedAccess* ia = nullptr;
                    qreal r = temporaryFunction(values[0].simplified())->Calculate(rp, ia);
                    qreal g = temporaryFunction(values[1].simplified())->Calculate(rp, ia);
                    qreal b = temporaryFunction(values[2].simplified())->Calculate(rp, ia);

                    qreal finalR = r;
                    qreal finalG = g;
                    qreal finalB = b;
                    qreal finalA = a;

                    if(r <= 1.0)
                    {
                        finalR = 255.0 * r;
                    }
                    if(g <= 1.0)
                    {
                        finalG = 255.0 * g;
                    }
                    if(b <= 1.0)
                    {
                        finalB = 255.0 * b;
                    }
                    if(a <= 1.0)
                    {
                        finalA = 255.0 * a;
                    }

                    rp->setPen(static_cast<int>(finalR), static_cast<int>(finalG),
                               static_cast<int>(finalB), static_cast<int>(finalA));
                    break;
                }
                case 2: // colorname,A
                {
                    if(Colors::colormap.count( values[0].simplified().toStdString() ))
                    {
                        auto cui = Colors::colormap.at(s);

                        IndexedAccess* ia = nullptr;
                        a = temporaryFunction(values[1].simplified())->Calculate(rp, ia);
                        qreal finalA = a;

                        if(a <= 1.0)
                        {
                            finalA = 255.0 * a;
                        }



                        rp->setPen(cui.r, cui.g, cui.g, static_cast<int>(finalA));
                    }
                    else // is this an RGB color?
                        if(value.startsWith("#"))
                        {
                            auto rgb = Colors::decodeHexRgb(value.toLocal8Bit().data());
                            IndexedAccess* ia = nullptr;
                            a = temporaryFunction(values[1].simplified())->Calculate(rp, ia);
                            qreal finalA = a;

                            if(a <= 1.0)
                            {
                                finalA = 255.0 * a;
                            }
                            else
                            {
                                finalA = std::get<3>(rgb);
                            }

                            rp->setPen(std::get<0>(rgb), std::get<1>(rgb), std::get<2>(rgb), static_cast<int>(finalA));
                        }
                }
                default:
                {
                    throw syntax_error_exception("Invalid color to set:", value);
                }
                }
            }
    }

    return true;
}

bool Loop::execute(RuntimeProvider *rp)
{

    auto looper = [&rp, this]()
    {
        try
        {
            for(const auto& stmt : qAsConst(body))
            {
                rp->setCurrentStatement(stmt->statement);
                stmt->execute(rp);
            }
        }
        catch(std::exception& ex)
        {
            rp->reportError(ex.what());

        }
    };

    try
    {
        rp->setCurrentStatement(this->statement);
        loop_target->loop(looper, rp);
    }
    catch(std::exception& ex)
    {
        rp->reportError(ex.what());
    }

    return true;
}

void Loop::updateLoopVariable(double v)
{
    runtimeProvider->setValue(loop_variable, v);
}

void Loop::updateLoopVariable(QPointF v)
{
    runtimeProvider->setValue(loop_variable, v);
}


RangeIteratorLoopTarget::RangeIteratorLoopTarget(QSharedPointer<Loop> l)
{
    theLoop = l;
}

bool RangeIteratorLoopTarget::loop(LooperCallback lp, RuntimeProvider* rp)
{
    IndexedAccess* ia = nullptr;
    double v = startFun->Calculate(rp, ia);
    double e = endFun->Calculate(rp, ia);

    double stepv = stepFun->Calculate(rp, ia);

    if(v < e && stepv < 0.0 || v > e && stepv > 0.0)
    {
        throw syntax_error_exception("Infinite loop detected, check your range");
    }

    do
    {
        theLoop->updateLoopVariable(v);
        lp();
        IndexedAccess* ia = nullptr;
        stepv = stepFun->Calculate(rp, ia);
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
    return true;
}

bool PointsOfObjectAssignment::execute(RuntimeProvider *rp)
{
    return true;
}

bool PointDefinitionAssignment::execute(RuntimeProvider *rp)
{
    return true;
}

FunctionIteratorLoopTarget::FunctionIteratorLoopTarget(QSharedPointer<Loop> l)
{
    theLoop = l;
}

bool FunctionIteratorLoopTarget::loop(LooperCallback lp, RuntimeProvider * rp)
{
    int fakeVarPos = -1;
    double cx, cy;
    bool first = true;

    auto points_of_loop_exec = [this, &cx, &cy, &first, &lp, &fakeVarPos, &rp](double x, double y, bool continuous)
    {

        QPointF p;
        if(continuous)
        {
            if(first)
            {
                p = QPointF(cx, cy);
                theLoop->updateLoopVariable( p );
                cx = x;
                cy = y;
                first = false;
            }
            else
            {
                // simulate a line from (cx, cy) to (x,y)
                // Formula: f($) = (y - cy) / (x - cx) * ($ - cx) + cy;

                double m = (y - cy) / (x - cx);
                QString lineFormula = "(" + QString::number(m, 'f', 10) + ") * ( $ ) - (" + QString::number(m, 'f', 10)  + ") * (" + QString::number(x, 'f', 10) + " ) + (" + QString::number(y, 'f', 10) + ")";

                auto tempFun = temporaryFunction(lineFormula);

                double xx = cx;
                while(xx <= x)
                {
                    tempFun->SetVariable("$", xx);
                    IndexedAccess* ia = nullptr;
                    double yy = tempFun->Calculate(rp, ia);
                    p = QPointF(xx, yy);
                    theLoop->updateLoopVariable( p );

                    QSharedPointer<Assignment> assignmentData = rp->get_assignments()[fakeVarPos];
                    QString spx = QString::number(p.x(), 'f', 10);
                    QString spy = QString::number(p.y(), 'f', 10);
                    dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->x = temporaryFunction(spx);
                    dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->y = temporaryFunction(spy);
                    lp();

                    xx += 0.01;
                }

                cx = x;
                cy = y;
                p = QPointF(cx, cy);
                theLoop->updateLoopVariable(p);

                QSharedPointer<Assignment> assignmentData = rp->get_assignments()[fakeVarPos];
                dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->x = temporaryFunction(QString::number(p.x(), 'f', 10));
                dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->y = temporaryFunction(QString::number(p.y(), 'f', 10));

                lp();
            }
        }
        else
        {
            p = QPointF(x, y);
            theLoop->updateLoopVariable( p );

            QSharedPointer<Assignment> assignmentData = rp->get_assignments()[fakeVarPos];
            dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->x = temporaryFunction(QString::number(p.x(), 'f', 10));
            dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->y = temporaryFunction(QString::number(p.y(), 'f', 10));

            lp();
        }
    };

    QSharedPointer<Assignment> assignment(nullptr);

    auto funToUse = rp->getFunction(theLoop->loop_target->name, assignment);

    QSharedPointer<PointDefinitionAssignment> assignmentData;
    assignmentData.reset(new PointDefinitionAssignment(theLoop->statement));

    // create a new fake point for the point

    assignmentData->x = temporaryFunction("0.0");
    assignmentData->y = temporaryFunction("0.0");
    assignmentData->varName = theLoop->loop_variable;
    rp->get_assignments().append(assignmentData);
    fakeVarPos = rp->get_assignments().size() - 1;

    double plotStart = -1.0;
    double plotEnd = 1.0;
    double step = 0.1;
    int count = -1;
    bool counted = false;

    if(assignment)
    {
        if(assignment->startValueProvider())
        {
            IndexedAccess* ia = nullptr;
            plotStart = assignment->startValueProvider()->Calculate(rp, ia);
        }
        if( assignment->endValueProvider())
        {
            IndexedAccess* ia = nullptr;
            plotEnd = assignment->endValueProvider()->Calculate(rp, ia);
        }

        continuous = assignment->continuous;
        IndexedAccess* ia = nullptr;
        step = assignment->step->Calculate(rp, ia);
        count = step;
        if(assignment->counted)
        {
            qDebug() << "counted step:" << step;
            counted = true;
            if(step > 1)
            {
                step = (plotEnd - plotStart) / (step - 1);
            }
            else
            {
                step = (plotEnd - plotStart);
            }
        }
    }


    auto pars = funToUse->get_domain_variables();
    if(pars.size() == 1)
    {


        int pointsDrawn = 0;

        for(double x=plotStart; x<=plotEnd; x+= step)
        {

            funToUse->SetVariable(pars[0].c_str(), x);
            IndexedAccess* ia = nullptr;
            double y = funToUse->Calculate(rp, ia);

            points_of_loop_exec(x, y, continuous);
            pointsDrawn ++;

        }
        qDebug() << "drawn:" << pointsDrawn << "points";

        if(!counted || (counted && pointsDrawn == count - 1))
        {
            // the last points always goes to plotEnd
            funToUse->SetVariable(pars[0].c_str(), plotEnd);
            IndexedAccess* ia = nullptr;
            double y = funToUse->Calculate(rp, ia);

            points_of_loop_exec(plotEnd, y, continuous);
        }
    }

    // remove the last point, since it was fake
    rp->get_assignments().pop_back();
    return true;
}

Stepped::Stepped() noexcept : step(temporaryFunction("0.1"))
{

}

bool Plot::execute(RuntimeProvider *rp)
{
    rp->drawPlot(this->sharedFromThis());
    return true;
}

bool Rotation::execute(RuntimeProvider *rp)
{
    for(auto& adef : rp->get_assignments())
    {
        if(what == adef->varName || what + ":" == adef->varName)
        {
            // first implemented rotation: a point
            if(adef.dynamicCast<PointDefinitionAssignment>())
            {
                adef.dynamicCast<PointDefinitionAssignment>()->rotated = false;

                auto fcp = adef->fullCoordProvider();
                if( std::get<0>(fcp) && std::get<1>(fcp) )
                {
                    IndexedAccess* ia = nullptr;
                    double x = std::get<0>(fcp)->Calculate(rp, ia);
                    double y = std::get<1>(fcp)->Calculate(rp, ia);

                    double a = degree->Calculate(rp, ia);
                    if(unit != "radians")
                    {
                        a = qDegreesToRadians(a);
                    }

                    auto rotfX = temporaryFunction(aroundX);
                    auto rotfY = temporaryFunction(aroundY);

                    auto p = rotatePoint(rotfX->Calculate(rp, ia), rotfY->Calculate(rp, ia), a, {x, y});

                    adef.dynamicCast<PointDefinitionAssignment>()->rotated_x = p.x();
                    adef.dynamicCast<PointDefinitionAssignment>()->rotated_y = p.y();
                    adef.dynamicCast<PointDefinitionAssignment>()->rotated = true;

                    return true;
                }
            }
        }
    }
    return false;
}

void Assignment::resolvePrecalculatedPointsForIndexedAccess(QSharedPointer<Plot> plot, QSharedPointer<Function> funToUse, RuntimeProvider* rp)
{
    if(precalculatedPoints.isEmpty())
    {
        QVector<QPointF> allPoints;
        auto pointGatherer = [&allPoints](double x, double y, bool c)
        {
            allPoints.append({x, y});
        };

        Executor<decltype(pointGatherer)> pgex(pointGatherer);
        pgex.execute(plot, sharedFromThis(), funToUse, rp, true);

        if(allPoints.isEmpty())
        {
            rp->reportError("Invalid data to plot: " + plot->plotTarget);
        }

        precalculatedPoints = allPoints;
    }
}
