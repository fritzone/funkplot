#ifndef RUNTIMEPROVIDER_H
#define RUNTIMEPROVIDER_H

#include "CodeEngine.h"

#include <QMap>
#include <QString>
#include <QPointF>

#include <string>

class MainWindow;

class RuntimeProvider
{
public:


    using CB_ErrorReporter = std::function<void(QString)>;
    using CB_PointDrawer = std::function<void(double,double)>;
    using CB_StatementTracker = std::function<void(QString)>;
    using CB_PenSetter = std::function<void(int r, int g, int b, int a)>;
    using CB_PlotDrawer = std::function<void(QSharedPointer<Plot>)>;

    explicit RuntimeProvider(CB_ErrorReporter, CB_PointDrawer, CB_StatementTracker, CB_PenSetter, CB_PlotDrawer);

    int defd(const std::string& s);
    double value(const std::string& s);
    void setValue(const QString&s, double v);
    void setValue(const QString&s, QPointF v);

    QMap<QString, double>& variables();

    template<class E>
    void genericPlotIterator(QSharedPointer<Plot> plot, E executor)
    {
        QSharedPointer<Assignment> assignment(nullptr);
        bool continuous = true;

        // first test: see if this is a function we need to plot
        QSharedPointer<Function> funToUse = getFunction(plot->plotTarget, assignment);

        if(!funToUse)
        {
            if(!resolveAsPoint(plot))
            {
                reportError("Invalid data to plot: " + plot->plotTarget);
                return;
            }
        }

        double plotStart = -1.0;
        double plotEnd = 1.0;
        bool counted = plot->counted;
        QSharedPointer<Function> stepFun = nullptr;

        if(assignment)
        {
            if(!plot->start)
            {
                if(! assignment->startValueProvider())
                {
                    reportError("Invalid plotting interval for " + assignment->varName + ". There is no clear start value defined for it.");
                    return;
                }
                else
                {
                    plotStart = assignment->startValueProvider()->Calculate(this);
                }
            }
            else
            {
                plotStart = plot->start->Calculate(this);
            }

            if(!plot->end)
            {
                if(! assignment->endValueProvider())
                {
                    reportError("Invalid plotting interval. There is no clear end value defined for it.");
                    return;
                }
                else
                {
                    plotEnd = assignment->endValueProvider()->Calculate(this);
                }
            }
            else
            {
                plotEnd = plot->end->Calculate(this);
            }
            continuous = assignment->continuous || plot->continuous;
            counted |= assignment->counted;
            stepFun = assignment->step;

        }
        else
        {
            stepFun = plot->step;
            continuous = plot->continuous;
            if(plot->start)
            {
                plotStart = plot->start->Calculate(this);
            }

            if(plot->end)
            {
                plotEnd = plot->end->Calculate(this);
            }
        }

        auto pars = funToUse->get_domain_variables();
        if(pars.size() == 1)
        {
            double t = stepFun->Calculate(this);
            double stepValue = t;
            if(counted)
            {
                if(t > 1)
                {
                    stepValue = (plotEnd - plotStart) / (t - 1);
                }
                else
                {
                    stepValue = (plotEnd - plotStart);
                }
            }
            for(double x=plotStart; x<=plotEnd; x += stepValue)
            {

                funToUse->SetVariable(pars[0].c_str(), x);
                double y = funToUse->Calculate(this);

                executor(x, y, continuous);

            }

            if(!counted)
            {
                // the last points always goes to plotEnd
                funToUse->SetVariable(pars[0].c_str(), plotEnd);
                double y = funToUse->Calculate(this);

                executor(plotEnd, y, continuous);
            }
        }
        else
        {
            reportError("Invalid function to plot: " + plot->plotTarget + ". Multidomain functions are not supported yet");
            return;
        }


    }

    QSharedPointer<Function> getFunction(const QString &name, QSharedPointer<Assignment>& assignment);

    void reportError(const QString& err);
    bool resolveAsPoint(QSharedPointer<Plot> plot);
    void reset();
    void execute(QStringList codelines);
    void setCurrentStatement(const QString &newCurrentStatement);
    void setPen(int, int, int, int);
    void drawPlot(QSharedPointer<Plot> plot);

    QSharedPointer<Statement> resolveCodeline(QStringList& codelines, QVector<QSharedPointer<Statement>>& statements, QSharedPointer<Statement> parentScope);

    QSharedPointer<Statement> createPlot(const QString &codeline);
    QSharedPointer<Statement> resolveObjectAssignment(const QString &codeline);
    QSharedPointer<Statement> createFunction(const QString& codeline);
    QSharedPointer<Statement> createSett(const QString &codeline);
    QSharedPointer<Statement> createLoop(const QString &codeline, QStringList& codelines);
    void resolveOverKeyword(QString codeline, QSharedPointer<Stepped>);
    QSharedPointer<Assignment> providePointsOfDefinition(const QString &codeline, QString assignment_body, const QString &varName, const QString &targetProperties);
    QVector<QSharedPointer<Assignment> > &get_assignments();

private:

    QMap<QString, double> m_vars;
    QMap<QString, QPointF> m_points;
    QVector<QSharedPointer<Plot>> plots;
    QVector<QSharedPointer<FunctionDefinition>> functions;
    QVector<QSharedPointer<Assignment>> assignments;
    QVector<QSharedPointer<Sett>> m_setts;

    QVector<QSharedPointer<Statement>> statements;

    CB_ErrorReporter m_errorReporter;
    CB_PointDrawer m_pointDrawer;
    CB_StatementTracker m_statementTracker;
    CB_PenSetter m_penSetter;
    CB_PlotDrawer m_plotDrawer;
};

#endif // RUNTIMEPROVIDER_H
