#include "CodeEngine.h"

#include "RuntimeProvider.h"
#include "colors.h"
#include "qmath.h"
#include <QDebug>

bool ArythmeticAssignment::execute(RuntimeProvider *rp)
{
    if(arythmetic)
    {
        double v = arythmetic->Calculate(rp);
        rp->variables()[varName] = v;
    }

    return true;
}

bool FunctionDefinition::execute(RuntimeProvider *mw)
{
    return true;
}

bool Sett::execute(RuntimeProvider *rp)
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
                    a = temporaryFunction(values[3].simplified())->Calculate(rp);
                }
                    [[fallthrough]];

                case 3: // R,G,B
                {
                    qreal r = temporaryFunction(values[0].simplified())->Calculate(rp);
                    qreal g = temporaryFunction(values[1].simplified())->Calculate(rp);
                    qreal b = temporaryFunction(values[2].simplified())->Calculate(rp);

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


                        a = temporaryFunction(values[1].simplified())->Calculate(rp);;
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

                            a = temporaryFunction(values[1].simplified())->Calculate(rp);
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
    double v = startFun->Calculate(rp);
    double e = endFun->Calculate(rp);

    double stepv = stepFun->Calculate(rp);

    if(v < e && stepv < 0.0 || v > e && stepv > 0.0)
    {
        throw syntax_error_exception("Infinite loop detected, check your range");
    }

    do
    {
        theLoop->updateLoopVariable(v);
        lp();
        stepv = stepFun->Calculate(rp);
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
                    double yy = tempFun->Calculate(rp);
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
            plotStart = assignment->startValueProvider()->Calculate(rp);
        }
        if( assignment->endValueProvider())
        {
            plotEnd = assignment->endValueProvider()->Calculate(rp);
        }

        continuous = assignment->continuous;
        step = assignment->step->Calculate(rp);
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
            double y = funToUse->Calculate(rp);

            points_of_loop_exec(x, y, continuous);
            pointsDrawn ++;

        }
        qDebug() << "drawn:" << pointsDrawn << "points";

        if(!counted || (counted && pointsDrawn == count - 1))
        {
            // the last points always goes to plotEnd
            funToUse->SetVariable(pars[0].c_str(), plotEnd);
            double y = funToUse->Calculate(rp);

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
        if(what == adef->varName)
        {
            // first implemented rotation: a point
            if(adef.dynamicCast<PointDefinitionAssignment>())
            {
                adef.dynamicCast<PointDefinitionAssignment>()->rotated = false;

                auto fcp = adef->fullCoordProvider();
                if( std::get<0>(fcp) && std::get<1>(fcp) )
                {

                    double x = std::get<0>(fcp)->Calculate(rp);
                    double y = std::get<1>(fcp)->Calculate(rp);

                    double a = degree->Calculate(rp);
                    if(unit != "radians")
                    {
                        a = qDegreesToRadians(a);
                    }

                    auto rotfX = temporaryFunction(aroundX);
                    auto rotfY = temporaryFunction(aroundY);

                    auto p = rotatePoint(rotfX->Calculate(rp), rotfY->Calculate(rp), a, {x, y});

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
