#ifndef RUNTIMEPROVIDER_H
#define RUNTIMEPROVIDER_H

#include <QObject>

#include "Set.h"
#include "Executor.h"

#include <QMap>
#include <QString>
#include <QPointF>
#include <QDebug>

#include <string>

class MainWindow;

/**
 * @brief The RuntimeProvider class is responsible for keeping track of the existing objects/variables
 * functions defined in the application.
 */
class RuntimeProvider : public QObject
{
    Q_OBJECT

public:

    using CB_ErrorReporter = std::function<void(int, int, QString)>;
    using CB_StringPrinter = std::function<void(QString)>;
    using CB_PointDrawer = std::function<void(double,double)>;
    using CB_StatementTracker = std::function<void(QString)>;
    using CB_PenSetter = std::function<void(int r, int g, int b, int a, int s)>;
    using CB_PlotDrawer = std::function<void(QSharedPointer<Plot>)>;

    explicit RuntimeProvider(CB_ErrorReporter, CB_StringPrinter, CB_PointDrawer, CB_StatementTracker, CB_PenSetter, CB_PlotDrawer);
    explicit RuntimeProvider(std::tuple<CB_ErrorReporter, CB_StringPrinter, CB_PointDrawer, CB_StatementTracker, CB_PenSetter, CB_PlotDrawer>);
    virtual ~RuntimeProvider() = default;

    static RuntimeProvider* get();

    template<class E>
    void genericPlotIterator(QSharedPointer<Plot> plot, const E executor)
    {
        debugVariables();

        QSharedPointer<Assignment> assignment(nullptr);
        // first test: see if this is a function we need to plot
        QSharedPointer<Function> funToUse = getNameFunctionOrAssignment(plot->plotTarget, assignment);
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
        if(assignment)
        {
            if(resolveAsPoint(plot))
            {
                return;
            }
            if(resolveAsPrecalculatedDrawing(plot, assignment))
            {
                return;
            }
        }
        if(funToUse)
        {
            Executor<E> ex(executor);
            ex.execute(plot, assignment, funToUse, [this](QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment,
                bool& continuous, double& plotStart, double& plotEnd, bool& counted, double& stepValue, int& count, bool useDefaultValues) {
                resolvePlotInterval(plot, assignment, continuous, plotStart, plotEnd, counted, stepValue, count, useDefaultValues);
            }, (assignment && assignment->startValueProvider()) ? false : true, m_errorReporter, m_codelinesSize - m_codelines.size(), this);
        }
        else
        {
            qCritical() << "Nothing works";
        }
    }

    void reportError(int errLine, int errorCode, const QString& err);
    void printString(const QString& s);
    bool resolveAsPoint(QSharedPointer<Plot> plot);
    bool resolveAsIndexedPlotDrawing(QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment);
    bool resolveAsPrecalculatedDrawing(QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment);
    void reset();
    void execute();
    void stopExecution();
    void setCurrentStatement(const QString &newCurrentStatement);
    void setPen(int, int, int, int);
    void setPixelSize(size_t);
    void setPalette(QString);
    void drawPlot(QSharedPointer<Plot> plot);
    void resolvePlotInterval(QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment, bool &continuous, double &plotStart, double &plotEnd, bool &counted, double &stepValue, int &count, bool useDefaultValues);
    void addFunction(QSharedPointer<FunctionDefinition> fd);
    void addVariable(const QString&name, const QString& type);
    void specifyVariableDomain(const QString&name, const QString& type);
    void setValue(const QString&s, double v);
    void setValue(const QString&s, QPointF v);
    void addOrUpdateAssignment(QSharedPointer<Assignment>);
    void setShouldReport(bool newShouldReport);
    void addOrUpdatePointDefinitionAssignment(int lineNumber, Statement *statement, double x, double y, const QString &varName);
    QSharedPointer<PointDefinitionAssignment> provideTemporaryPointDefinitionAssignment(int lineNumber, Statement *statement, double x, double y, const QString &varName);

    int defd(const std::string& s, Assignment *&assg);
    double value(const std::string& s);
    double value(const std::string& obj, const std::string& attr);
    QSharedPointer<Function> getNameFunctionOrAssignment(const QString &name, QSharedPointer<Assignment>& assignment);
    QMap<QString, double>& variables();
    QVector<QSharedPointer<Assignment> > &getAssignments();
    QSharedPointer<Assignment> getAssignment(const QString& n);
    QSharedPointer<Function> getFunction(const QString& n);
    std::vector<std::string> getBuiltinFunctions() const;
    std::vector<std::string> getFunctionNames() const;
    std::vector<std::string> getVariables() const;
    std::vector<std::string> getDeclaredVariables() const;
    std::vector<std::string> getAllVariables() const;
    QSharedPointer<CodeEngine> getCodeEngine() const;
    bool hasVariable(const QString& name) const;
    QString domainOfVariable(const QString&);
    QString valueOfVariable(const QString& vn);
    bool parse(const QStringList &codelines);
    bool shouldReport() const;
    double getIndexedVariableValue(const char* n, int index);
    bool isRunning() const;
    bool isNumericVariable(const QString& s) const;

    // will return the assignment as the given T type, if any
    template<class T> T* getAssignmentAs(const QString& n)
    {
        for(int fds_c = assignments.size() - 1; fds_c >= 0; fds_c--)
        {
            QSharedPointer<Assignment> fds = assignments[fds_c];
            if(fds.get()->varName == n || fds.get()->varName == n + ":")
            {
                T* t = dynamic_cast<T*>(fds.get());
                if(t)
                {
                    return t;
                }
            }
        }
        return nullptr;
    }

    /**
     * @brief typeOfVariable returns the type of the variable:
     *
     * n - for numeric
     * p - for points
     * l - for plot/array
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

    /**
     * @brief getCurrentPalette returns the name of the current palette
     * @return
     */
    const QString &getCurrentPalette() const;

    /**
     * @brief getStatements retusn all the statements of the code, as an rw object
     * @return
     */
    QVector<QSharedPointer<Statement>>& getStatements();

    /**
     * @brief getFunctions returns all the functions of the rp
     * @return
     */
    const QVector<QSharedPointer<FunctionDefinition>>& getFunctions() const;

    void debugVariables();
    void populateBuiltinFunctions();
    bool getShowCoordinates() const;
    void setShowCoordinates(bool newShowCoordinates);

signals:

    void rotationAngleChange(double);
    void zoomFactorChange(double);
    void gridChange(bool);
    void coordEndYChange(double);
    void coordStartYChange(double);
    void coordEndXChange(double);
    void coordStartXChange(double);

private:

    QMap<QString, double> m_numericVariables;
    QMap<QString, QPointF> m_points;
    QMap<QString, QStringList> m_arrays;

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
    CB_StringPrinter m_stringPrinter;

    bool m_shouldReport = true;

    // this contains all the variables (key) and their type (value)
    // the var section initializes this
    QMap<QString, QString> m_allVariables;

    // holds the types of list type variables (for example list of points)
    QMap<QString, QString> m_listDomains;

    QStringList m_codelines;
    int m_codelinesSize = -1;
    QString m_currentPalette;

    // the current color
    int m_r = 0, m_g = 0, m_b = 0, m_a = 255;

    // the current pixel size
    int m_ps = 1;

    QSharedPointer<CodeEngine> ce;

    static RuntimeProvider* m_instance;

    bool m_running = false;

    bool m_showCoordinates = true;
};

#endif // RUNTIMEPROVIDER_H
