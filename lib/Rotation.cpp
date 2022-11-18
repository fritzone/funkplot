#include "Rotation.h"

#include <QtMath>

#include "RuntimeProvider.h"

bool Rotation::execute(RuntimeProvider *rp)
{
    for(auto& adef : rp->getAssignments())
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

                    qDebug() << "Rotate:" << a << "degrees";

                    if(unit != "radians")
                    {
                        a = qDegreesToRadians(a);
                    }


                    auto rotfX = Function::temporaryFunction(aroundX, this);
                    auto rotfY = Function::temporaryFunction(aroundY, this);

                    double dRPx = rotfX->Calculate(rp, ia, as);
                    double dRPy = rotfY->Calculate(rp, ia, as);

                    auto p = rotatePoint(dRPx, dRPy, a, {x, y});

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
