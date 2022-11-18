#include "PointDefinitionAssignmentToOtherPoint.h"
#include "Function.h"
#include "IndexedAccess.h"
#include "RuntimeProvider.h"
#include "PointArrayAssignment.h"

std::tuple<QSharedPointer<Function>, QSharedPointer<Function> > PointDefinitionAssignmentToOtherPoint::fullCoordProvider(RuntimeProvider *rp)
{
    return PointDefinitionAssignment::fullCoordProvider(rp);
}

bool PointDefinitionAssignmentToOtherPoint::execute(RuntimeProvider *rp)
{
    QString otherPointsName = otherPoint;
    if(otherPointsName.indexOf("[") != -1)
    {
        if(otherPointsName.indexOf("]") != -1)
        {
            otherPointsName = otherPointsName.left(otherPointsName.indexOf("["));
        }
        else
        {
            throw syntax_error_exception(ERRORCODE(54), "Invalid assignment: <b>%s</b> (something messy with the index for <b>%s</b>)", statement.toStdString().c_str(), otherPointsName.toStdString().c_str());
        }
    }

    auto same = rp->getAssignmentAs<PointDefinitionAssignmentToOtherPoint>(otherPointsName);

    if(!same)
    {
        //auto ab = rp->get_assignment(otherPoint);
        auto a = rp->getAssignmentAs<PointDefinitionAssignment>(otherPointsName);

        // means this is not a point assignment
        if(!a)
        {
            // then maybe a points of something?
            auto b = rp->getAssignmentAs<PointsOfObjectAssignment>(otherPointsName);
            if(!b)
            {
                // then maybe it was created as an array?
                auto pas = rp->getAssignmentAs<PointArrayAssignment>(otherPointsName);
                if(pas)
                {
                    pas->resolvePrecalculatedPointsForIndexedAccessWithList(nullptr, rp);
                    resolveAssignmentFromIndexed(pas, rp);
                    return true;
                }

                // last resort
                if(!otherPoint.isEmpty())
                {
                    IndexedAccess* ia_m = nullptr; Assignment* a1 = nullptr;
                    auto tempFun = Function::temporaryFunction(otherPoint, this);
                    tempFun->Calculate(rp, ia_m, a1);
                    // if this is an indexed something ...
                    if(ia_m)
                    {
                        // same logic as in RuntimeProvider.h
                        dealWithIndexedAssignmentToSomething(rp, ia_m);
                        delete ia_m;
                        return true;
                    }
                }

                return false;
            }
            else
            {
                b->resolvePrecalculatedPointsForIndexedAccessWithList(nullptr, rp);
                resolveAssignmentFromIndexed(b, rp);

                rp->debugVariables();
                // qDebug() << "This:" << (void*)this;
                return true;
            }

            return false;
        }
        copyFrom(a);
        return true;
    }
    copyFrom(same);
    return true;
}
