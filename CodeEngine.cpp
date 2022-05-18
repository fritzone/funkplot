#include "CodeEngine.h"
#include "RuntimeProvider.h"
#include "colors.h"
#include "constants.h"
#include "qmath.h"
#include <QDebug>
#include <QFile>

void Assignment::dealWithIndexedAssignmentTOPoint(RuntimeProvider *rp, IndexedAccess* ia_m)
{
    QString at = rp->typeOfVariable(varName);
    qDebug() << at;

    // assigning to a point
    if(at == "p" || at == "point")
    {
        auto p = rp->get_assignment(ia_m->indexedVariable);
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
                        IndexedAccess* ia = nullptr; Assignment* a = nullptr;
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
                    assignmentData.reset(new PointDefinitionAssignment(this->statement));

                    assignmentData->x = temporaryFunction(QString::number(px, 'f', 6), this);
                    assignmentData->y = temporaryFunction(QString::number(py, 'f', 6), this);
                    assignmentData->varName = varName + ":";
                    rp->addOrUpdateAssignment(assignmentData);
                }

            }
        }
    }
    else
    {
        // TODO: not a point
    }
}

bool ArythmeticAssignment::execute(RuntimeProvider *rp)
{
    if(arythmetic)
    {
        try
        {
            IndexedAccess* ia_m = nullptr; Assignment* a = nullptr;
            double v = arythmetic->Calculate(rp, ia_m, a);

            // if this is an indexed something ...
            if(ia_m)
            {
                // same logic as in RuntimeProvider.h
                dealWithIndexedAssignmentTOPoint(rp, ia_m);
            }
            else
            if(a) // an assignment?
            {
                QString at = rp->typeOfVariable(a->varName.toLocal8Bit().data());
                qDebug() << at;

                // assigning to a point
                if(at == "p")
                {
                    // rewriting the current assignment

                    auto fcp = a->fullCoordProvider(rp);
                    if( std::get<0>(fcp) && std::get<1>(fcp) )
                    {
                        IndexedAccess* ia = nullptr; Assignment* a = nullptr;
                        double x = std::get<0>(fcp)->Calculate(rp, ia, a);
                        double y = std::get<1>(fcp)->Calculate(rp, ia, a);

                        QSharedPointer<PointDefinitionAssignment> assignmentData;
                        assignmentData.reset(new PointDefinitionAssignment(this->statement));

                        assignmentData->x = temporaryFunction(QString::number(x, 'f', 6), this);
                        assignmentData->y = temporaryFunction(QString::number(y, 'f', 6), this);
                        assignmentData->varName = varName + ":";
                        rp->addOrUpdateAssignment(assignmentData);
                    }
                }
                else
                {
                    // TODO: not a point
                }
            }
            else
            {
                rp->variables()[varName] = v;
            }
        }
        catch(...)
        {
            QString vt = rp->typeOfVariable(varName.toStdString().c_str());
            if(vt == "p" || vt == "point")
            {
                throw syntax_error_exception("Cannot use the point type variable <b>%s</b> in this context: <b>%s</b>",  varName.toStdString().c_str(), statement.toStdString().c_str());
            }
            throw;
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
        else // a color from the current palette?
        if(value.startsWith("palette"))
        {
            QString local_value = value.mid(7);

            // skip space
            while(!local_value.isEmpty() && local_value.at(0).isSpace()) local_value = local_value.mid(1);
            // skip [

            bool needs_pq = false;
            if(!local_value.isEmpty() && local_value.at(0) == '[')
            {
                needs_pq = true;
                local_value = local_value.mid(1);
            }

            QString palIdx = "";
            while(!local_value.isEmpty())
            {
                if(local_value.at(0) != ']')
                {
                    palIdx += local_value.at(0);
                }
                else
                {
                    break;
                }
                local_value = local_value.mid(1);
            }

            if(!local_value.isEmpty()&& local_value.at(0) == ']' && !needs_pq || local_value.isEmpty() && needs_pq )
            {
                throw syntax_error_exception("Wrong palette indexing: <b>%s</b>", s.c_str());
            }

            if(!local_value.isEmpty()) local_value = local_value.mid(1);


            IndexedAccess* ia = nullptr; Assignment* as = nullptr;
            int pidx = temporaryFunction(palIdx.simplified(), this)->Calculate(rp, ia, as),
                cidx = 0;

            QString p = rp->get_currentPalette();
            QFile f(":/palettes/upals/" + p + ".map");
            if(f.open(QIODevice::Text | QIODevice::ReadOnly))
            {
                QTextStream stream(&f);
                while (!stream.atEnd())
                {
                    QString line = stream.readLine();
                    if(cidx == pidx)
                    {
                        QStringList rgbs = line.split(" ", Qt::SkipEmptyParts);
                        rp->setPen(rgbs[0].toInt(), rgbs[1].toInt(), rgbs[2].toInt(), 255);
                        break;
                    }
                    cidx ++;
                }
            }
            return true;

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
                IndexedAccess* ia = nullptr; Assignment* as = nullptr;
                a = temporaryFunction(values[3].simplified(), this)->Calculate(rp, ia, as);
            }
            [[fallthrough]];

            case 3: // R,G,B
            {
                IndexedAccess* ia = nullptr; Assignment* as = nullptr;
                qreal r = temporaryFunction(values[0].simplified(), this)->Calculate(rp, ia, as);
                qreal g = temporaryFunction(values[1].simplified(), this)->Calculate(rp, ia, as);
                qreal b = temporaryFunction(values[2].simplified(), this)->Calculate(rp, ia, as);

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

                    IndexedAccess* ia = nullptr; Assignment* as = nullptr;
                    a = temporaryFunction(values[1].simplified(), this)->Calculate(rp, ia, as);
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
                        IndexedAccess* ia = nullptr; Assignment* as = nullptr;
                        a = temporaryFunction(values[1].simplified(), this)->Calculate(rp, ia, as);
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
    else
    if(what == "palette")
    {
        rp->setPalette(value);
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
    IndexedAccess* ia = nullptr; Assignment* a = nullptr;
    double v = startFun->Calculate(rp, ia, a);
    double e = endFun->Calculate(rp, ia, a);

    double stepv = stepFun->Calculate(rp, ia, a);

    if(v < e && stepv < 0.0 || v > e && stepv > 0.0)
    {
        throw syntax_error_exception("Infinite loop detected, check your range");
    }

    do
    {
        theLoop->updateLoopVariable(v);
        lp();
        IndexedAccess* ia = nullptr; Assignment* a = nullptr;
        stepv = stepFun->Calculate(rp, ia, a);
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

std::tuple<QSharedPointer<Function>, QSharedPointer<Function> > PointDefinitionAssignment::fullCoordProvider(RuntimeProvider *rp)
{
    if(rotated)
    {
        QString spx = QString::number(rotated_x, 'f', 10);
        QString spy = QString::number(rotated_y, 'f', 10);
        return {temporaryFunction(spx, this), temporaryFunction(spy, this)};
    }
    else
    {
        return {x, y};
    }
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

                auto tempFun = temporaryFunction(lineFormula, this->theLoop.get());

                double xx = cx;
                while(xx <= x)
                {
                    tempFun->SetVariable("$", xx);
                    IndexedAccess* ia = nullptr; Assignment* a = nullptr;
                    double yy = tempFun->Calculate(rp, ia, a);
                    p = QPointF(xx, yy);
                    theLoop->updateLoopVariable( p );

                    QSharedPointer<Assignment> assignmentData = rp->get_assignments()[fakeVarPos];
                    QString spx = QString::number(p.x(), 'f', 10);
                    QString spy = QString::number(p.y(), 'f', 10);
                    dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->x = temporaryFunction(spx, this->theLoop.get());
                    dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->y = temporaryFunction(spy, this->theLoop.get());
                    lp();

                    xx += 0.01;
                }

                cx = x;
                cy = y;
                p = QPointF(cx, cy);
                theLoop->updateLoopVariable(p);

                QSharedPointer<Assignment> assignmentData = rp->get_assignments()[fakeVarPos];
                dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->x = temporaryFunction(QString::number(p.x(), 'f', 10), this->theLoop.get());
                dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->y = temporaryFunction(QString::number(p.y(), 'f', 10), this->theLoop.get());

                lp();
            }
        }
        else
        {
            p = QPointF(x, y);
            theLoop->updateLoopVariable( p );

            QSharedPointer<Assignment> assignmentData = rp->get_assignments()[fakeVarPos];
            dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->x = temporaryFunction(QString::number(p.x(), 'f', 10), this->theLoop.get());
            dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->y = temporaryFunction(QString::number(p.y(), 'f', 10), this->theLoop.get());

            lp();
        }
    };

    QSharedPointer<Assignment> assignment(nullptr);

    auto funToUse = rp->getFunction(theLoop->loop_target->name, assignment);

    QSharedPointer<PointDefinitionAssignment> assignmentData;
    assignmentData.reset(new PointDefinitionAssignment(theLoop->statement));

    // create a new fake point for the point

    assignmentData->x = temporaryFunction("0.0", this->theLoop.get());
    assignmentData->y = temporaryFunction("0.0", this->theLoop.get());
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
            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            plotStart = assignment->startValueProvider()->Calculate(rp, ia, a);
        }
        if( assignment->endValueProvider())
        {
            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            plotEnd = assignment->endValueProvider()->Calculate(rp, ia, a);
        }

        continuous = assignment->continuous;
        IndexedAccess* ia = nullptr; Assignment* a = nullptr;
        step = assignment->step->Calculate(rp, ia, a);
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
            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            double y = funToUse->Calculate(rp, ia, a);

            points_of_loop_exec(x, y, continuous);
            pointsDrawn ++;

        }
        qDebug() << "drawn:" << pointsDrawn << "points";

        if(!counted || (counted && pointsDrawn == count - 1))
        {
            // the last points always goes to plotEnd
            funToUse->SetVariable(pars[0].c_str(), plotEnd);
            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            double y = funToUse->Calculate(rp, ia, a);

            points_of_loop_exec(plotEnd, y, continuous);
        }
    }

    // remove the last point, since it was fake
    rp->get_assignments().pop_back();
    return true;
}

Stepped::Stepped() noexcept : step(temporaryFunction("0.1", nullptr))
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

                auto fcp = adef->fullCoordProvider(rp);
                if( std::get<0>(fcp) && std::get<1>(fcp) )
                {
                    IndexedAccess* ia = nullptr; Assignment* as = nullptr;
                    double x = std::get<0>(fcp)->Calculate(rp, ia, as);
                    double y = std::get<1>(fcp)->Calculate(rp, ia, as);

                    double a = degree->Calculate(rp, ia, as);
                    if(unit != "radians")
                    {
                        a = qDegreesToRadians(a);
                    }

                    auto rotfX = temporaryFunction(aroundX, this);
                    auto rotfY = temporaryFunction(aroundY, this);

                    auto p = rotatePoint(rotfX->Calculate(rp, ia, as), rotfY->Calculate(rp, ia, as), a, {x, y});

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

bool PointDefinitionAssignmentToOtherPoint::execute(RuntimeProvider *rp)
{
    auto ab = rp->get_assignment(otherPoint);
    auto a = ab.dynamicCast<PointDefinitionAssignment>();

    // means this is not a point assignment
    if(!a)
    {

        IndexedAccess* ia_m = nullptr; Assignment* a = nullptr;
        auto tempFun = temporaryFunction(otherPoint, this);
        double v = tempFun->Calculate(rp, ia_m, a);
        // if this is an indexed something ...
        if(ia_m)
        {
            // same logic as in RuntimeProvider.h
            dealWithIndexedAssignmentTOPoint(rp, ia_m);
            delete ia_m;
            return true;
        }

        return false;
    }

    rotated = a->rotated;
    x = a->x;
    y = a->y;
    rotated_x = a->rotated_x;
    rotated_y = a->rotated_y;
    return true;
}
