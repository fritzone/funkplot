#include "PointDefinitionAssignment.h"
#include "Function.h"
#include "RuntimeProvider.h"

class RuntimeProvider;

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
        return {Function::temporaryFunction(spx, this), Function::temporaryFunction(spy, this)};
    }
    else
    {
        return {x, y};
    }
}

QString PointDefinitionAssignment::toString()
{
    auto fcp = fullCoordProvider(runtimeProvider);
    auto x_provider = std::get<0>(fcp);
    auto y_provider = std::get<1>(fcp);
    if( x_provider && y_provider )
    {
        IndexedAccess* ia = nullptr; Assignment* a = nullptr;
        double x = x_provider->Calculate(runtimeProvider, ia, a);
        double y = y_provider->Calculate(runtimeProvider, ia, a);

        QString r = "Point(" + QString::number(x, 'f', 6) + ", " + QString::number(y, 'f', 6) + ")";
        return r;

    }
    else
    {
        return "Point(0.0, 0.0)";
    }
}

void PointDefinitionAssignment::rotate(std::tuple<double, double> rp, double angle)
{
    rotated = false;

    auto fcp = fullCoordProvider(RuntimeProvider::get());
    if( std::get<0>(fcp) && std::get<1>(fcp) )
    {
        IndexedAccess* ia = nullptr; Assignment* as = nullptr;
        double x = std::get<0>(fcp)->Calculate(RuntimeProvider::get(), ia, as);
        double y = std::get<1>(fcp)->Calculate(RuntimeProvider::get(), ia, as);

        auto p = rotatePoint(rp, angle, {x, y});

        rotated_x = p.x();
        rotated_y = p.y();
        rotated = true;
    }
}
