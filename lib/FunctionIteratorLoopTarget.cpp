#include "FunctionIteratorLoopTarget.h"
#include "Loop.h"
#include "RuntimeProvider.h"
#include "constants.h"

#include <QPointF>

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

                auto tempFun = Function::temporaryFunction(lineFormula, this->theLoop.get());

                double xx = cx;
                while(xx <= x)
                {
                    tempFun->SetVariable("$", xx);
                    IndexedAccess* ia = nullptr; Assignment* a = nullptr;
                    double yy = tempFun->Calculate(rp, ia, a);
                    p = QPointF(xx, yy);
                    theLoop->updateLoopVariable( p );

                    QSharedPointer<Assignment> assignmentData = rp->getAssignments()[fakeVarPos];
                    QString spx = QString::number(p.x(), 'f', 10);
                    QString spy = QString::number(p.y(), 'f', 10);
                    dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->x = Function::temporaryFunction(spx, this->theLoop.get());
                    dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->y = Function::temporaryFunction(spy, this->theLoop.get());
                    lp();

                    xx += 0.01;
                }

                cx = x;
                cy = y;
                p = QPointF(cx, cy);
                theLoop->updateLoopVariable(p);

                QSharedPointer<Assignment> assignmentData = rp->getAssignments()[fakeVarPos];
                dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->x = Function::temporaryFunction(QString::number(p.x(), 'f', 10), this->theLoop.get());
                dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->y = Function::temporaryFunction(QString::number(p.y(), 'f', 10), this->theLoop.get());

                lp();
            }
        }
        else
        {
            p = QPointF(x, y);
            theLoop->updateLoopVariable( p );

            QSharedPointer<Assignment> assignmentData = rp->getAssignments()[fakeVarPos];
            dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->x = Function::temporaryFunction(QString::number(p.x(), 'f', 10), this->theLoop.get());
            dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->y = Function::temporaryFunction(QString::number(p.y(), 'f', 10), this->theLoop.get());

            lp();
        }
    };

    QSharedPointer<Assignment> assignment(nullptr);

    auto funToUse = rp->getNameFunctionOrAssignment(theLoop->loop_target->name, assignment);

    QSharedPointer<PointDefinitionAssignment> assignmentData;
    assignmentData.reset(new PointDefinitionAssignment(theLoop->lineNumber, theLoop->statement));

    // create a new fake point for the point

    assignmentData->x = Function::temporaryFunction("0.0", this->theLoop.get());
    assignmentData->y = Function::temporaryFunction("0.0", this->theLoop.get());
    assignmentData->varName = theLoop->loop_variable;
    rp->getAssignments().append(assignmentData);
    fakeVarPos = rp->getAssignments().size() - 1;

    double plotStart = -1.0;
    double plotEnd = 1.0;
    double step = DEFAULT_RANGE_STEP;
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
    rp->getAssignments().pop_back();
    return true;
}
