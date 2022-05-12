#ifndef RUNTIMEPROVIDER_H
#define RUNTIMEPROVIDER_H

#include "CodeEngine.h"
#include "Set.h"
#include "Executor.h"

#include <QMap>
#include <QString>
#include <QPointF>
#include <QDebug>

#include <string>

class MainWindow;

class RuntimeProvider
{
public:

    using CB_ErrorReporter = std::function<void(int, QString)>;
    using CB_PointDrawer = std::function<void(double,double)>;
    using CB_StatementTracker = std::function<void(QString)>;
    using CB_PenSetter = std::function<void(int r, int g, int b, int a)>;
    using CB_PlotDrawer = std::function<void(QSharedPointer<Plot>)>;

    explicit RuntimeProvider(CB_ErrorReporter, CB_PointDrawer, CB_StatementTracker, CB_PenSetter, CB_PlotDrawer);
    virtual ~RuntimeProvider() = default;

    int defd(const std::string& s, Assignment *&assg);
    double value(const std::string& s);
    double value(const std::string& obj, const std::string& attr);
    void setValue(const QString&s, double v);
    void setValue(const QString&s, QPointF v);

    QMap<QString, double>& variables();


    template<class E>
    void genericPlotIterator(QSharedPointer<Plot> plot, const E executor)
    {

        QSharedPointer<Assignment> assignment(nullptr);

        // first test: see if this is a function we need to plot
        QSharedPointer<Function> funToUse = getFunction(plot->plotTarget, assignment);

        if(!funToUse && !assignment)
        {
            if(!resolveAsPoint(plot))
            {
                if(!resolveAsIndexedPlotDrawing(plot, assignment))
                {
                    return;
                }
            }
            return;
        }

        Executor<E> ex(executor);
        ex.execute(plot, assignment, funToUse, this, (assignment && assignment->startValueProvider()) ? false : true);

    }

    QSharedPointer<Function> getFunction(const QString &name, QSharedPointer<Assignment>& assignment);

    void reportError(const QString& err);
    bool resolveAsPoint(QSharedPointer<Plot> plot);
    bool resolveAsIndexedPlotDrawing(QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment);
    void reset();
    void execute();
    void setCurrentStatement(const QString &newCurrentStatement);
    void setPen(int, int, int, int);
    void drawPlot(QSharedPointer<Plot> plot);
    void resolvePlotInterval(QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment,
                             bool &continuous, double &plotStart, double &plotEnd, bool &counted, double &stepValue, int &count, bool useDefaultValues);

    QSharedPointer<Statement> resolveCodeline(QStringList& codelines, QVector<QSharedPointer<Statement>>& statements, QSharedPointer<Statement> parentScope);
    void createVariableDeclaration(const QString& codeline);
    QSharedPointer<Statement> createPlot(const QString &codeline);
    QSharedPointer<Statement> createAssignment(const QString &codeline);
    QSharedPointer<Statement> createFunction(const QString& codeline);
    QSharedPointer<Statement> createSet(const QString &codeline);
    QSharedPointer<Statement> createLoop(const QString &codeline, QStringList& codelines);
    QSharedPointer<Statement> createRotation(const QString &codeline, QStringList& codelines);

    void resolveOverKeyword(QString codeline, QSharedPointer<Stepped>, Statement *s);
    QSharedPointer<Assignment> providePointsOfDefinition(const QString &codeline, QString assignment_body, const QString &varName, const QString &targetProperties);
    QVector<QSharedPointer<Assignment> > &get_assignments();
    QSharedPointer<Assignment> get_assignment(const QString& n);
    QSharedPointer<Function> get_function(const QString& n);
    std::vector<std::string> get_builtin_functions() const;
    std::vector<std::string> get_functions() const;
    std::vector<std::string> get_variables() const;
    std::vector<std::string> get_declared_variables() const;
    void parse(const QStringList &codelines);
    void addOrUpdateAssignment(QSharedPointer<Assignment>);
    bool get_shouldReport() const;
    void set_ShouldReport(bool newShouldReport);

    /**
     * @brief typeOfVariable returns the type of the variable:
     *
     * n - for numeric
     * p - for points
     * l - for plot
     * f - for function
     * x - for unknown
     *
     * If the variable was declared, but not defined (ie: no assignment to it) it will return its type from keywords.h
     *
     * @param n
     * @return
     */
    QString typeOfVariable(const char* n);
    QString typeOfVariable(const QString& s);

    double getIndexedVariableValue(const char* n, int index);

private:

    QMap<QString, double> m_vars;
    QMap<QString, QPointF> m_points;
    QVector<QSharedPointer<Plot>> plots;
    QVector<QSharedPointer<FunctionDefinition>> functions;
    QVector<QSharedPointer<Assignment>> assignments;
    QVector<QSharedPointer<Set>> m_setts;

    QVector<QSharedPointer<Statement>> statements;

    CB_ErrorReporter m_errorReporter;
    CB_PointDrawer m_pointDrawer;
    CB_StatementTracker m_statementTracker;
    CB_PenSetter m_penSetter;
    CB_PlotDrawer m_plotDrawer;

    bool m_shouldReport = true;

    // this contains all the variables (key) and their type (value)
    // the var section initializes this
    QMap<QString, QString> m_allVariables;

    QStringList m_codelines;
    int m_codelinesSize = -1;
};

#endif // RUNTIMEPROVIDER_H
