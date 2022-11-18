#include "ArrayAssignment.h"
#include "ArrayIteratorLoopTarget.h"
#include "Loop.h"
#include "PointArrayAssignment.h"
#include "RuntimeProvider.h"

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
            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            double v = el->Calculate(rp, ia, a);
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
            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            double x = std::get<0>(el)->Calculate(rp, ia, a);
            double y = std::get<1>(el)->Calculate(rp, ia, a);

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

        auto els = pointsOfObjectAssignment->precalculatedPoints;
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
