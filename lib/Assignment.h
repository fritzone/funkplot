#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include "Statement.h"
#include "Stepped.h"
#include "StatementHandler.h"

#include <QSharedPointer>
#include <QVector>
#include <QPointF>

struct Plot;
struct IndexedAccess;
class Function;

/**
 * @brief The Assignment struct is a base class for all asignments that take place in the code
 */
struct Assignment : public Stepped, public Statement, public QEnableSharedFromThis<Assignment>
{

    Assignment() = default;
    explicit Assignment(int ln, const QString& s);

    /**
     * @brief providedFunction Whether this assignment was done to the internal points of a specific function
     *
     * Used in plotting a function whose points were assigned to a variable
     *
     * @return The name of the function
     */
    virtual QString providedFunction();

    /**
     * @brief startValueProvider The function which gives a start value to a plot, if any.
     *
     * Even a number can be a start value provider function
     *
     * @return The start value provider, if any
     */
    virtual QSharedPointer<Function> startValueProvider();

    /**
     * @brief endValueProvider The function which gives an end value to a plot, if any.
     *
     * Even a number can be an end value provider function
     *
     * @return The end value provider, if any
     */
    virtual QSharedPointer<Function> endValueProvider();

    /**
     * @brief keyword This should return "LET"
     * @return
     */
    QString keyword() const override;

    /**
     * @brief fullCoordProvider Whether this assignment gives a full set of coordinates that can be drawn.
     *
     * Such as a point assignment
     *
     * @param rp in whcih RuntimeProvider to look
     * @return The point coordiante providers, if any
     */
    virtual std::tuple<QSharedPointer<Function>, QSharedPointer<Function>> fullCoordProvider(RuntimeProvider* rp);

    /**
     * @brief resolvePrecalculatedPointsForIndexedAccessWithFunction resolves the precalculated points for this assignment
     * @param plot the plot to calculate the points, providing start/end/step
     * @param funToUse the function to plot
     * @param rp the runtime provider in which all this happens
     */
    void resolvePrecalculatedPointsForIndexedAccessWithFunction(QSharedPointer<Plot> plot, QSharedPointer<Function> funToUse, RuntimeProvider *rp);

    virtual void resolvePrecalculatedPointsForIndexedAccessWithList(QSharedPointer<Plot> plot, RuntimeProvider *rp) {}
    /**
     * @brief dealWithIndexedAssignmentTOPoint deals with indexed access assignment to a point
     * @param rp the runtime environment
     * @param ia_m the indexed access provider
     */
    void dealWithIndexedAssignmentToSomething(RuntimeProvider *rp, IndexedAccess *ia_m);

    /**
     * @brief toString returns the value of the variable as a string
     * @return
     */
    virtual QString toString() { return ""; }

    /**
     * @brief forceSetPrecalculatedPoints Forcefulyl overwrites the precalculated points
     * @param newPoints
     */
    void forceSetPrecalculatedPoints(const QVector<QPointF>& newPoints);

    /***********************************************************************************************/

    QString varName;                     // the name of the object we will refer to in later code (such as: plot assignedStuff)
    QString targetProperties;            // the name of the properties of the assigned objects, such as: points. If targetProperties is "arythmetic" then a new function is created and evaluated at run time
    bool precalculatedSetForce = false;
    std::tuple<bool, double, double, bool, double, int> lastPrecalculatedIntervalData {false, std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), false,std::numeric_limits<double>::quiet_NaN(), 0};

    static QString kw();


    static QVector<QSharedPointer<Statement>> create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback srcb);

    static QSharedPointer<Assignment> providePointsOfDefinition(int ln, const QString &codeline, QString assignment_body, const QString &varName, const QString &targetProperties);


public:
    void setPrecalculatedSetForce(bool newPrecalculatedSetForce);

    QVector<QPointF> &getPrecalculatedPoints();
    void setPrecalculatedPoints(const QVector<QPointF> &newPrecalculatedPoints);

private:
    QVector<QPointF> precalculatedPoints;// if any precalculation happens, this is the place

};

REGISTER_STATEMENTHANDLER(Assignment)

#endif // ASSIGNMENT_H
