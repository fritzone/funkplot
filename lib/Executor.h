#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "util.h"
#include "Function.h"
#include "Parametric.h"

#include <QDebug>
#include <functional>

class RuntimeProvider;
class Assignment;
class Plot;

using LastPlotData = std::tuple<bool, double, double, bool, double, int>;


template<class EX> class Executor
{
public:

//    using PLOT_RESOLVER = std::function<void(QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment,
//        bool& continuous, double& plotStart, double& plotEnd, bool& counted, double& stepValue, int& count, bool useDefaultValues)>;


    explicit Executor(EX e) : executor(e) {}

    template<class PLOT_RESOLVER>
    LastPlotData execute(QSharedPointer<Plot> plot,
                         QSharedPointer<Assignment> assignment,
                         QSharedPointer<Function> funToUse,
                         PLOT_RESOLVER pr,
                         bool useDefaultIntrval,
                         std::function<void(int, int, QString)> erp,
                         int cdl,
                         RuntimeProvider* rp,
                         QString plotTarget)
    {
        bool continuous = true;
        double plotStart = -1.0;
        double plotEnd = 1.0;
        bool counted = true;
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
                double y = funToUse->Calculate();

                if(!std::isnan(y))
                {
                    qDebug() << "X=" << x << "Y=" << y;
                    executor(plot, x, y, continuous);
                }
                pointsDrawn ++;

            }

            if(!counted || pointsDrawn < count )
            {
                // the last points always goes to plotEnd
                funToUse->SetVariable(pars[0].c_str(), plotEnd);
                double y = funToUse->Calculate();

                if(!std::isnan(y))
                {
                    executor(plot, plotEnd, y, continuous);
                }
            }
        }
        else
        {
            erp(cdl, ERRORCODE(15), "Invalid function to plot: " + plotTarget + ". Multidomain functions are not supported yet");
        }

        return {continuous, plotStart, plotEnd, counted, stepValue, count};
    }

    template<class PLOT_RESOLVER>
    LastPlotData executeParametricFunction(QSharedPointer<Plot> plot,
                         QSharedPointer<Assignment> assignment,
                         QSharedPointer<Parametric> funToUse,
                         PLOT_RESOLVER pr,
                         bool useDefaultIntrval,
                         std::function<void(int, int, QString)> erp,
                         int cdl,
                         RuntimeProvider* rp,
                         QString plotTarget)
    {
        bool continuous = true;
        double plotStart = -1.0;
        double plotEnd = 1.0;
        bool counted = true;
        double stepValue = 0.01;
        int count = -1;
        pr(plot, assignment, continuous, plotStart, plotEnd, counted, stepValue, count, useDefaultIntrval);

        qDebug() << "Parametric: StepValue:" << stepValue << "PlotStart:" << plotStart << "PlotEnd:" << plotEnd << "Continuous:" << continuous;

        int pointsDrawn = 0;
        for(double t=plotStart; t<=plotEnd; t += stepValue)
        {
            // first value, get the x
            funToUse->functions.first->SetVariable(funToUse->funParName.toStdString(), t);
            double x = funToUse->functions.first->Calculate();

            // next, the y
            funToUse->functions.second->SetVariable(funToUse->funParName.toStdString(), t);
            double y = funToUse->functions.second->Calculate();

            qDebug() << "t=" << t << "X=" << x << "Y=" << y;
            if(!std::isnan(y) && ! std::isnan(x)) executor(plot, x, y, continuous);

            pointsDrawn ++;

        }

        // last point
        if(!counted || pointsDrawn < count )
        {
            // the last points always goes to plotEnd
            // first value, get the x
            funToUse->functions.first->SetVariable(funToUse->funParName.toStdString(), plotEnd);
            double x = funToUse->functions.first->Calculate();

            // next, the y
            funToUse->functions.second->SetVariable(funToUse->funParName.toStdString(), plotEnd);
            double y = funToUse->functions.second->Calculate();

            qDebug() << "plotEnd:" << plotEnd << "X=" << x << "Y=" << y;
            if(!std::isnan(y) && ! std::isnan(x))
            {
                executor(plot, x, y, continuous);
            }


        }

        return {continuous, plotStart, plotEnd, counted, stepValue, count};
    }

private:

    EX executor;
};

#endif // EXECUTOR_H
