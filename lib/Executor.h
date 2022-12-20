#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "Plot.h"
#include "Function.h"

#include <QDebug>

class RuntimeProvider;

template<class EX> class Executor
{
public:

//    using PLOT_RESOLVER = std::function<void(QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment,
//        bool& continuous, double& plotStart, double& plotEnd, bool& counted, double& stepValue, int& count, bool useDefaultValues)>;

    using LastPlotData = std::tuple<bool, double, double, bool, double, int>;

    explicit Executor(EX e) : executor(e) {}

    template<class PLOT_RESOLVER>
    LastPlotData execute(QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment, QSharedPointer<Function> funToUse, PLOT_RESOLVER pr, bool useDefaultIntrval, std::function<void(int, int, QString)> erp, int cdl, RuntimeProvider* rp)
    {
        bool continuous = true;
        double plotStart = -1.0;
        double plotEnd = 1.0;
        bool counted = plot->counted;
        double stepValue = 0.01;
        int count = -1;
        pr(plot, assignment, continuous, plotStart, plotEnd, counted, stepValue, count, useDefaultIntrval);

        qDebug() << "StepValue:" << stepValue << "PlotStart:" << plotStart << "PlotEnd:" << plotEnd << "Continuous:" << continuous;

        auto pars = funToUse->get_domain_variables();
        if(pars.size() == 1)
        {
            int pointsDrawn = 0;
            for(double x=plotStart; x<=plotEnd; x += stepValue)
            {

                funToUse->SetVariable(pars[0].c_str(), x);
                IndexedAccess* ia = nullptr; Assignment* a = nullptr;
                double y = funToUse->Calculate(rp, ia, a);

                qDebug() << "X=" << x << "Y=" << y;
                executor(x, y, continuous);

                pointsDrawn ++;

            }

            if(!counted || (counted &&  pointsDrawn < count ))
            {
                // the last points always goes to plotEnd
                funToUse->SetVariable(pars[0].c_str(), plotEnd);
                IndexedAccess* ia = nullptr; Assignment* a = nullptr;
                double y = funToUse->Calculate(rp, ia, a);

                executor(plotEnd, y, continuous);
            }
        }
        else
        {
            erp(cdl, ERRORCODE(15), "Invalid function to plot: " + plot->plotTarget + ". Multidomain functions are not supported yet");
        }

        return {continuous, plotStart, plotEnd, counted, stepValue, count};

    }
private:
    EX executor;
};

#endif // EXECUTOR_H
