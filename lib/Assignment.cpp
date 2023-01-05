#include "Assignment.h"
#include "Keywords.h"
#include "Function.h"
#include "RuntimeProvider.h"
#include "Executor.h"
#include "ArrayAssignment.h"
#include "Plot.h"

#include "ArithmeticAssignmentToArrayElement.h"
#include "PointArrayAssignment.h"
#include "PointDefinitionAssignmentToOtherPoint.h"
#include "PointsOfObjectAssignment.h"
#include "constants.h"

#include <QVector>
#include <QSharedPointer>

Assignment::Assignment(int ln, const QString &s) : Statement(ln, s)
{
    qDebug() << "Create:" << s << "At:" << (void*)this;
}

QString Assignment::providedFunction()
{
    return "";
}

QSharedPointer<Function> Assignment::startValueProvider()
{
    return nullptr;
}

QSharedPointer<Function> Assignment::endValueProvider()
{
    return nullptr;
}

QString Assignment::keyword() const
{
    return Keywords::KW_LET;
}

std::tuple<QSharedPointer<Function>, QSharedPointer<Function> > Assignment::fullCoordProvider(RuntimeProvider *rp)
{
    return {nullptr, nullptr};
}

void Assignment::resolvePrecalculatedPointsForIndexedAccessWithFunction(QSharedPointer<Plot> plot, QSharedPointer<Function> funToUse, RuntimeProvider* rp)
{
    QVector<QPointF> allPoints;
    auto pointGatherer = [&allPoints](double x, double y, bool c)
    {
        allPoints.append({ x, y });
    };

    bool continuous = true;
    double plotStart = -1.0;
    double plotEnd = 1.0;
    bool counted = plot->counted;
    double stepValue = 0.01;
    int count = -1;
    bool useDefaultIntrval = true;

    rp->resolvePlotInterval(plot, sharedFromThis(), continuous, plotStart, plotEnd, counted, stepValue, count, useDefaultIntrval);

    std::tuple<bool, double, double, bool, double, int> currentCalculatedId {continuous, plotStart, plotEnd, counted, stepValue, count};

    if(currentCalculatedId != lastPrecalculatedIntervalData)
    {
        Executor<decltype(pointGatherer)> pgex(pointGatherer);
        lastPrecalculatedIntervalData = pgex.execute(plot,
                     sharedFromThis(),
                     funToUse,
                     [&rp](QSharedPointer<Plot> plot,
                           QSharedPointer<Assignment> assignment,
                           bool& continuous,
                           double& plotStart,
                           double& plotEnd,
                           bool& counted,
                           double& stepValue,
                           int& count,
                           bool useDefaultValues)
                    {
                         rp->resolvePlotInterval(plot, assignment, continuous, plotStart, plotEnd, counted, stepValue, count, useDefaultValues);
                    },
                    true,
                    [&rp, this](int l, int c, QString s) {rp->reportError(lineNumber, c, s); },
                    -1,
                    rp
        );

        if(allPoints.isEmpty())
        {
            rp->reportError(lineNumber, ERRORCODE(15), "Invalid data to plot: " + plot->plotTarget);
        }

        precalculatedPoints = allPoints;
        precalculatedSetForce = false;
    }
}

void Assignment::dealWithIndexedAssignmentToSomething(RuntimeProvider *rp, IndexedAccess* ia_m)
{
    QString at = rp->typeOfVariable(varName);

    // assigning to a point
    if(at == "point" || at == "point")
    {
        auto p = rp->getAssignment(ia_m->indexedVariable);
        if(p)
        {
            if(p->targetProperties == "points")
            {
                if(p->precalculatedPoints.isEmpty())
                {
                    // get the start/end of the "Stepped" object
                    IndexedAccess* ia = nullptr; Assignment* a = nullptr;
                    auto svp = p->startValueProvider();
                    double v = svp ? svp->Calculate(rp, ia, a) : -1.0;
                    auto evp = p->endValueProvider();
                    double e = evp ? evp->Calculate(rp, ia, a) : 1.0;
                    auto stepFun = p->step;
                    double stepv = DEFAULT_RANGE_STEP;

                    if(stepFun)
                    {
                        auto count = stepFun->Calculate(rp, ia, a);
                        if(p->counted)
                        {
                            if(count > 1)
                            {
                                stepv = (e - v) / (count - 1);
                            }
                            else
                            {
                                stepv = (e - v);
                            }
                        }
                    }


                    if(v < e && stepv < 0.0 || v > e && stepv > 0.0)
                    {
                        throw syntax_error_exception(ERRORCODE(0), "Erroneous looping conditions detected, check your range");
                    }
                    // and calculate the points
                    auto f = rp->getFunction(p->providedFunction());
                    if(!f)
                    {
                        throw syntax_error_exception(ERRORCODE(60), "No function for points assignment");
                    }
                    QVector<QPointF> allPoints;
                    auto pars = f->get_domain_variables();
                    if(pars.size() == 1)
                    {
                        do
                        {
                            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
                            f->SetVariable(pars[0].c_str(), v);
                            auto fv = f->Calculate(rp, ia, a);
                            allPoints.push_back({v, fv});
                            v += stepv;
                            if(stepv < 0.0 && v < e)
                            {
                                break;
                            }

                            if(stepv > 0.0 && v > e)
                            {
                                break;
                            }
                        }
                        while(true);
                    }
                    p->precalculatedPoints = allPoints;
                }

                if(ia_m->index < p->precalculatedPoints.size())
                {
                    double px = p->precalculatedPoints[ia_m->index].x();
                    double py = p->precalculatedPoints[ia_m->index].y();

                    QSharedPointer<PointDefinitionAssignment> assignmentData;
                    assignmentData.reset(new PointDefinitionAssignment(lineNumber, this->statement));

                    assignmentData->x = Function::temporaryFunction(QString::number(px, 'f', 6), this);
                    assignmentData->y = Function::temporaryFunction(QString::number(py, 'f', 6), this);
                    assignmentData->varName = varName + ":";
                    rp->addOrUpdateAssignment(assignmentData);
                }

            }
        }
    }
    else
    if(at == Types::TYPE_NUMBER)
    {
        // assigning to a number
        auto source = rp->getAssignment(ia_m->indexedVariable);
        if(source)
        {
            QString sourceType = rp->typeOfVariable(source->varName);
            if(sourceType != Types::TYPE_LIST)
            {
                throw syntax_error_exception(ERRORCODE(61), "Cannot assign a non numeric value (<b>%s</b>) to a numeric variable (<b>%s</b>)", source->varName.toStdString().c_str(), varName.toStdString().c_str());
            }

            auto arrayAssignment = rp->getAssignmentAs<ArrayAssignment>(source->varName);
            if(!arrayAssignment)
            {
                auto pointsOfAssignment = rp->getAssignmentAs<PointsOfObjectAssignment>(source->varName);
                if(!pointsOfAssignment)
                {
                    throw syntax_error_exception(ERRORCODE(62), "Cannot identify the object assigning from: <b>%s</b> in this context: <b>%s</b>", source->varName.toStdString().c_str(), statement.toStdString().c_str());
                }
                // find the point we are looking for

                //double v = pointsOfAssignment->precalculatedPoints[ia_m->index];
                //rp->variables()[varName] = v;
                return;
            }

            // this
            if(ia_m->index >= arrayAssignment->m_elements.size())
            {
                throw syntax_error_exception(ERRORCODE(55), "Index out of bounds for <b>%s</b> in this context: <b>%s</b>. Requested <b>%d</b>, available: <b>%d</b>",
                                             source->varName.toStdString().c_str(), statement.toStdString().c_str(), ia_m->index, arrayAssignment->m_elements.size());
            }

            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            double v = arrayAssignment->m_elements[ia_m->index]->Calculate(rp, ia, a);
            rp->variables()[varName] = v;
        }
        else
        {
            throw syntax_error_exception(ERRORCODE(64), "Cannot identify the assignment source (<b>%s</b>)", ia_m->indexedVariable.toStdString().c_str());
        }
    }
}

void Assignment::forceSetPrecalculatedPoints(const QVector<QPointF> &newPoints)
{
    precalculatedPoints = newPoints;
    precalculatedSetForce = true;
}

void Assignment::setPrecalculatedSetForce(bool newPrecalculatedSetForce)
{
    precalculatedSetForce = newPrecalculatedSetForce;
}

QString Assignment::kw()
{
    return Keywords::KW_LET;
}

QVector<QSharedPointer<Statement> > Assignment::create(int ln, const QString &codeline, QStringList &codelines, StatementCallback cb, StatementReaderCallback srcb)
{
    // let keyword
    QString assignment_body = codeline.mid(Keywords::KW_LET.length());
    consumeSpace(assignment_body);

    // variable
    QString varName = getDelimitedId(assignment_body);
    QString realVarName = ""; // if we do indexed assignment
    bool indexed = false;
    QString index;

    // let's see if this looks like an indexed assignment
    if(varName.indexOf("[") != -1)
    {
        if(varName.indexOf("]") == -1)
        {
            throw syntax_error_exception(ERRORCODE(7), "Invalid assignment: <b>%s</b> (something messy with the index for <b>%s</b>)", codeline.toStdString().c_str(), varName.toStdString().c_str());
        }

        realVarName = varName.left(varName.indexOf("["));
        indexed = true;
        index = varName.mid(varName.indexOf("[") + 1, varName.indexOf("]")  - varName.indexOf("[") - 1);
    }

    if(!RuntimeProvider::get()->hasVariable(varName) && !indexed)
    {
        throw syntax_error_exception(ERRORCODE(8), "Invalid assignment: <b>%s</b> (variable <b>%s</b> was not declared)", codeline.toStdString().c_str(), varName.toStdString().c_str());
    }

    // assignment operator
    if(!assignment_body.isEmpty() && assignment_body[0] != '=')
    {
        throw syntax_error_exception(ERRORCODE(9), "Invalid assignment: <b>%s</b> (missing assignment operator)", codeline.toStdString().c_str());
    }

    // skip "= "
    assignment_body = assignment_body.mid(1);
    consumeSpace(assignment_body);

    // what feature do we want to assign to
    char delimiter = 0;
    QString targetProperties = getDelimitedId(assignment_body, {' ', '[', '('}, delimiter);

    // if it's not points, for the moment it is an arythmetic calculation
    if(targetProperties != "points")
    {
        if(targetProperties == "point") // or just a simple point
        {
            if(RuntimeProvider::get()->typeOfVariable(varName) != Types::TYPE_POINT)
            {
                throw syntax_error_exception(ERRORCODE(10), "Conflicting type assignment: <b>point</b> assigned to a non point type variable: <b>%s (%s)</b>", varName.toStdString().c_str(), RuntimeProvider::get()->typeOfVariable(varName).toStdString().c_str());
            }

            QString nextWord = getDelimitedId(assignment_body);
            if(nextWord != "at")
            {
                QSharedPointer<PointDefinitionAssignmentToOtherPoint> result;
                result.reset(new PointDefinitionAssignmentToOtherPoint(ln, codeline));
                result->otherPoint = nextWord;
                result->varName = varName;

                RuntimeProvider::get()->addOrUpdateAssignment(result);

                return handleStatementCallback(vectorize(result), cb);
            }
            else
            {

                consumeSpace(assignment_body);

                if(!assignment_body.isEmpty() && assignment_body[0] == '(')
                {
                    // skip (
                    assignment_body = assignment_body.mid(1);
                }
                [[maybe_unused]] QString fnai;
                QString px = extract_proper_expression(assignment_body, fnai, {','});
                QString py = extract_proper_expression(assignment_body , fnai, {')'});

                QSharedPointer<PointDefinitionAssignmentToOtherPoint> result;
                result.reset(new PointDefinitionAssignmentToOtherPoint(ln, codeline));

                result->x = Function::temporaryFunction(px, result.get());
                result->y = Function::temporaryFunction(py, result.get());
                result->varName = varName;
                RuntimeProvider::get()->addOrUpdateAssignment(result);

                return handleStatementCallback(vectorize(result), cb);
            }
        }
        if(targetProperties == Types::TYPE_LIST) // or maybe we create a list
        {
            if(RuntimeProvider::get()->typeOfVariable(varName) != Types::TYPE_LIST)
            {
                throw syntax_error_exception(ERRORCODE(12), "Conflicting type assignment: <b>array</b> assigned to a non array type variable: <b>%s (%s)</b>", varName.toStdString().c_str(), RuntimeProvider::get()->typeOfVariable(varName).toStdString().c_str());
            }

            // skip the spaces after the Types::TYPE_LIST keyword
            consumeSpace(assignment_body);

            if(!assignment_body.isEmpty())
            {
                if(assignment_body.at(0) != '[' && delimiter != '[')
                {
                    throw syntax_error_exception(ERRORCODE(13), "Invalid array assignment: missing <b>[</b> from <b>%s</b>", codeline.toStdString().c_str());
                }
            }

            QString arrayBody = "";
            while(!assignment_body.isEmpty() && assignment_body.at(0) != ']')
            {
                arrayBody += assignment_body.at(0);
                assignment_body = assignment_body.mid(1);
            }

            QString typeOfVarName = RuntimeProvider::get()->domainOfVariable(varName);

            if(typeOfVarName == Domains::DOMAIN_NUMBERS)
            {
                QStringList arrayElements = arrayBody.split(",");
                QSharedPointer<ArrayAssignment> result;
                result.reset(new ArrayAssignment(ln, codeline));
                for(const QString& e : arrayElements)
                {
                    result->m_elements.append(Function::temporaryFunction(e, result.get()));
                }
                result->varName = varName;
                RuntimeProvider::get()->addOrUpdateAssignment(result);
                return handleStatementCallback(vectorize(result), cb);;
            }
            else
                if(typeOfVarName == Domains::DOMAIN_POINTS)
                {
                    arrayBody.remove(' ');
                    arrayBody.replace("),(", ");(");
                    QSharedPointer<PointArrayAssignment> result;
                    result.reset(new PointArrayAssignment(ln, codeline));
#if QT_VERSION > QT_VERSION_CHECK(5, 15, 0)
                    QStringList arrayElements = arrayBody.split(";", Qt::SkipEmptyParts);
#else
                    QStringList arrayElements = arrayBody.split(";", QString::SkipEmptyParts);
#endif
                    for(QString e : arrayElements)
                    {
                        if(!e.startsWith("("))
                        {
                            throw syntax_error_exception(ERRORCODE(35), "Invalid point array assignment: missing <b>(</b> from <b>%s</b>", codeline.toStdString().c_str());
                        }
                        e = e.mid(1);
                        QString sx = getDelimitedId(e, {','});
                        QString sy = getDelimitedId(e, {')'});

                        auto t = std::make_tuple(Function::temporaryFunction(sx, result.get()), Function::temporaryFunction(sy, result.get()));
                        result->m_elements.append(t);
                    }
                    result->varName = varName;
                    RuntimeProvider::get()->addOrUpdateAssignment(result);
                    return handleStatementCallback(vectorize(result), cb);;
                }
        }
        else
        {
            if(indexed && !realVarName.isEmpty())
            {
                varName = realVarName;
            }
            if(RuntimeProvider::get()->typeOfVariable(varName) == Types::TYPE_POINT)
            {
                QSharedPointer<PointDefinitionAssignmentToOtherPoint> result;
                result.reset(new PointDefinitionAssignmentToOtherPoint(ln, codeline));
                result->otherPoint = targetProperties; // yeah, almost
                // if this was an indexed stuff ...
                if(delimiter == '[') result->otherPoint += "["+ assignment_body;
                result->varName = varName;

                RuntimeProvider::get()->addOrUpdateAssignment(result);

                return handleStatementCallback(vectorize(result), cb);;
            }
            else
            {
                if(RuntimeProvider::get()->typeOfVariable(varName) != Types::TYPE_NUMBER && !indexed)
                {
                    throw syntax_error_exception(ERRORCODE(14), "Invalid assignment: <b>%s</b> (<b>arithmetic expression</b> assigned to <b>%s</b>)", varName.toStdString().c_str(), RuntimeProvider::get()->typeOfVariable(varName).toStdString().c_str());
                }
                if(!indexed)
                {
                    QSharedPointer<ArithmeticAssignment> result;
                    result.reset(new ArithmeticAssignment(ln, codeline));

                    QString expression = targetProperties;
                    expression += (delimiter == '[' ? "[" : "") + assignment_body;
                    result->arithmetic =  Function::temporaryFunction(expression, result.get());

                    result->varName = varName;
                    RuntimeProvider::get()->addOrUpdateAssignment(result);

                    return handleStatementCallback(vectorize(result), cb);;
                }
                else
                {
                    QSharedPointer<ArithmeticAssignmentToArrayElement> result;
                    result.reset(new ArithmeticAssignmentToArrayElement(ln, codeline));
                    QString expression = targetProperties;
                    expression += assignment_body;
                    result->arithmetic = Function::temporaryFunction(expression, result.get());
                    result->varName = varName;
                    result->index = Function::temporaryFunction(index, result.get());
                    RuntimeProvider::get()->addOrUpdateAssignment(result);
                    return handleStatementCallback(vectorize(result), cb);;
                }
            }
        }
    }

    // here we are sure we want the points of a plot, variable type should be list
    if(RuntimeProvider::get()->typeOfVariable(varName) != Types::TYPE_LIST)
    {
        throw syntax_error_exception(ERRORCODE(12), "Invalid assignment: <b>%s</b> (<b>array</b> assigned to <b>%s</b>)", varName.toStdString().c_str(), RuntimeProvider::get()->typeOfVariable(varName).toStdString().c_str());
    }
    QSharedPointer<Assignment> result = providePointsOfDefinition(ln, codeline, assignment_body, varName, targetProperties);
    RuntimeProvider::get()->addOrUpdateAssignment(result);
    return handleStatementCallback(vectorize(result), cb);
}

QSharedPointer<Assignment> Assignment::providePointsOfDefinition(int ln, const QString &codeline, QString assignment_body, const QString &varName, const QString &targetProperties)
{
    if(!assignment_body.startsWith(Keywords::KW_OF))
    {
        throw syntax_error_exception(ERRORCODE(51), "Invalid assignment: <b>%s</b> (missing of keyword)", codeline.toStdString().c_str());
    }

    // skipping the of keyword
    assignment_body = assignment_body.mid(Keywords::KW_OF.length());
    consumeSpace(assignment_body);

    QSharedPointer<PointsOfObjectAssignment> result;
    result.reset(new PointsOfObjectAssignment(ln, codeline));

    result->varName = varName;
    result->targetProperties = targetProperties;

    // what object do we want the points of
    result->ofWhat = getDelimitedId(assignment_body);
    QSharedPointer<Assignment> assignment;

    if(!RuntimeProvider::get()->getNameFunctionOrAssignment(result->ofWhat, assignment))
    {
        throw syntax_error_exception(ERRORCODE(52), "Invalid assignment: %s. No such function: %s", codeline.toStdString().c_str(), result->ofWhat.toStdString().c_str());
    }

    // over keyword
    if(assignment_body.startsWith(Keywords::KW_OVER))
    {
        resolveOverKeyword(assignment_body, result, result.get());
    }

    if (assignment_body.startsWith(Keywords::KW_COUNTS))
    {
        assignment_body = assignment_body.mid(Keywords::KW_COUNTS.length());
        consumeSpace(assignment_body);

        QString strPointCount = getDelimitedId(assignment_body);
        result->stepValue = strPointCount;

        result->step = Function::temporaryFunction(strPointCount, result.get());
        result->counted = true;

        if(codeline == Keywords::KW_SEGMENTS)
        {
            result->step = Function::temporaryFunction(strPointCount + " + 1", result.get());
            result->continuous = true;
        }
    }
    return result;
}
