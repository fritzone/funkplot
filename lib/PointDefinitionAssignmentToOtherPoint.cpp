#include "PointDefinitionAssignmentToOtherPoint.h"
#include "Function.h"
#include "IndexedAccess.h"
#include "Parametric.h"
#include "RuntimeProvider.h"
#include "PointArrayAssignment.h"
#include "PointsOfObjectAssignment.h"

std::tuple<QSharedPointer<Function>, QSharedPointer<Function> > PointDefinitionAssignmentToOtherPoint::fullCoordProvider(RuntimeProvider *rp)
{
    return PointDefinitionAssignment::fullCoordProvider(rp);
}

bool PointDefinitionAssignmentToOtherPoint::execute(RuntimeProvider *rp)
{
    QString otherPointsName = otherPoint;

    // indexed
    if(otherPointsName.indexOf("[") != -1)
    {
        if(otherPointsName.indexOf("]") != -1)
        {
            otherPointsName = otherPointsName.left(otherPointsName.indexOf("["));
        }
        else
        {
            throw funkplot::syntax_error_exception(ERRORCODE(54), "Invalid assignment: <b>%s</b> (something messy with the index for <b>%s</b>)", statement.toStdString().c_str(), otherPointsName.toStdString().c_str());
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
                    auto v1 = tempFun->Calculate(rp, ia_m, a1);
                    // if this is an indexed something ...
                    if(ia_m)
                    {
                        // same logic as in RuntimeProvider.h
                        dealWithIndexedAssignmentToSomething(rp, ia_m);
                        delete ia_m;
                        return true;
                    }

                    // last resort: this is a point to which we have assigned a function expression
                    // for example: let p = 3 * f(1) + f(9)
                    // where f is a parametric function.
                    // calculate that expression and feed it back to the point
                    auto v2 = tempFun->Calculate(rp, ia_m, a1, 2);

                    if(v1.has_value() && v2.has_value())
                    {
                        QPointF p{ v1.value(), v2.value() };

                        x = Function::temporaryFunction(QString::number(p.x(), 'f', 6), this);
                        y = Function::temporaryFunction(QString::number(p.y(), 'f', 6), this);
                        rotated = false;
                        rotated_x = p.x();
                        rotated_y = p.y();

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

void PointDefinitionAssignmentToOtherPoint::resolveAssignmentFromIndexed(Assignment *pas, RuntimeProvider *rp)
{
    QString indexStr = otherPoint.mid(otherPoint.indexOf("[") + 1, otherPoint.indexOf("]") - otherPoint.indexOf("[") - 1);
    auto tempFun = Function::temporaryFunction(indexStr, this);
    double idx = tempFun->Calculate();
    auto pcp = pas->getPrecalculatedPoints();
    int pps = pcp.size();
    if(idx < pps)
    {
        QPointF p = pcp[idx];
        x = Function::temporaryFunction(QString::number(p.x(), 'f', 6), this);
        y = Function::temporaryFunction(QString::number(p.y(), 'f', 6), this);
        rotated = false;
        rotated_x = p.x();
        rotated_y = p.y();
    }
    else
    {

        std::stringstream ss;
        ss <<"Index out of bounds for <b>" << pas->varName.toStdString().c_str() << "</b> in this context: <b>" <<  statement.toStdString().c_str() <<
            "</b>. Requested <b>" << idx << "</b>, available: <b>" << pps - 1 << "</b> (Arrays start from index 0, not 1)";
        throw funkplot::syntax_error_exception(ERRORCODE(55), ss.str().c_str());
    }
}
