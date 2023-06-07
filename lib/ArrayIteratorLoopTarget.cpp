#include "ArrayAssignment.h"
#include "ArrayIteratorLoopTarget.h"
#include "Loop.h"
#include "PointArrayAssignment.h"
#include "PointsOfObjectAssignment.h"
#include "RuntimeProvider.h"
#include "Function.h"

ArrayIteratorLoopTarget::ArrayIteratorLoopTarget(QSharedPointer<Loop> l)
{
    theLoop = l;
}

bool ArrayIteratorLoopTarget::loop(LooperCallback lp, RuntimeProvider *rp)
{
    auto var = rp->getAssignment(name);
    if(!var)
    {
        return false;
    }

    auto arrayAssignment = rp->getAssignmentAs<ArrayAssignment>(name);

    if(arrayAssignment)
    {
        auto els = arrayAssignment->m_elements;
        for(const auto& el : els)
        {
            double v = el->Calculate();
            theLoop->updateLoopVariable(v);

            // update the variable too
            auto asv = rp->getAssignment(theLoop->loop_variable);
            if(asv)
            {
                // TODO: find out
            }

            lp();
        }
        return true;
    }

    {
    auto pointArrayAssignment = rp->getAssignmentAs<PointArrayAssignment>(name);
    if(pointArrayAssignment)
    {
        auto els = pointArrayAssignment->m_elements;
        for(const auto& el : els)
        {
            double x = std::get<0>(el)->Calculate();
            double y = std::get<1>(el)->Calculate();

            QPointF p{x, y};

            theLoop->updateLoopVariable(p);

            // here update the value of the loop_variable too
            auto assignmentData = rp->getAssignmentAs<PointDefinitionAssignment>(theLoop->loop_variable);
            if(assignmentData)
            {
                assignmentData->x = Function::temporaryFunction(QString::number(x, 'f', 6), this->theLoop.data());
                assignmentData->y = Function::temporaryFunction(QString::number(y, 'f', 6), this->theLoop.get());
            }

            lp();
        }
        return true;
    }
    }

    auto pointsOfObjectAssignment = rp->getAssignmentAs<PointsOfObjectAssignment>(name);
    if(pointsOfObjectAssignment)
    {
        pointsOfObjectAssignment->resolvePrecalculatedPointsForIndexedAccessWithList(nullptr, rp);

        auto els = pointsOfObjectAssignment->getPrecalculatedPoints();
        for(const auto& el : els)
        {
            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            double x = el.x();
            double y = el.y();

            QPointF p{x, y};

            theLoop->updateLoopVariable(p);

            // here update the value of the loop_variable too
            auto assignmentData = rp->getAssignmentAs<PointDefinitionAssignment>(theLoop->loop_variable);
            if(assignmentData)
            {
                assignmentData->x = Function::temporaryFunction(QString::number(x, 'f', 6), this->theLoop.data());
                assignmentData->y = Function::temporaryFunction(QString::number(y, 'f', 6), this->theLoop.get());
            }

            lp();
        }
        return true;
    }

    return false;

}
