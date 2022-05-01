#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "CodeEngine.h"
#include "Function.h"
#include "RuntimeProvider.h"

template<class EX> class Executor
{
public:
    explicit Executor(EX e) : executor(e) {}

    void execute(QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment, QSharedPointer<Function> funToUse, RuntimeProvider* rp, bool useDefaultIntrval)
    {
        bool continuous = true;
        double plotStart = -1.0;
        double plotEnd = 1.0;
        bool counted = plot->counted;
        double stepValue = 0.01;
        int count = -1;
        rp->resolvePlotInterval(plot, assignment, continuous, plotStart, plotEnd, counted, stepValue, count, useDefaultIntrval);

        auto pars = funToUse->get_domain_variables();
        if(pars.size() == 1)
        {
            int pointsDrawn = 0;
            for(double x=plotStart; x<=plotEnd; x += stepValue)
            {

                funToUse->SetVariable(pars[0].c_str(), x);
                IndexedAccess* ia = nullptr;
                double y = funToUse->Calculate(rp, ia);

                executor(x, y, continuous);

                pointsDrawn ++;

            }

            if(!counted || (counted &&  pointsDrawn < count ))
            {
                // the last points always goes to plotEnd
                funToUse->SetVariable(pars[0].c_str(), plotEnd);
                IndexedAccess* ia = nullptr;
                double y = funToUse->Calculate(rp, ia);

                executor(plotEnd, y, continuous);
            }
        }
        else
        {
            rp->reportError("Invalid function to plot: " + plot->plotTarget + ". Multidomain functions are not supported yet");
            return;
        }
    }
private:
    EX executor;
};

#endif // EXECUTOR_H
