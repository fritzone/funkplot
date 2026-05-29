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
#include "LineAssignment.h"
#include "SegmentAssignment.h"
#include "PointIntersectionAssignment.h"
#include "PointOnSegmentAssignment.h"
#include "AngleAssignment.h"
#include "ExtendedSegmentAssignment.h"
#include "ExtendedLineAssignment.h"
#include "PointReflectionAssignment.h"
#include "MeasurementAssignment.h"
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
    auto pointGatherer = [&allPoints](QSharedPointer<Plot> p, double x, double y, bool c, int s)
    {
        if(p->polarPlot)
        {
            double carX = y * cos( x );
            double carY = y * sin( x );

            x = carX;
            y = carY;
        }

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

        // let's see if we have a function or a parametric function
        if(funToUse)
        {
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
                        rp,
                        plot->plotTarget,
                        rp->getPixelSize()
                        );

        }
        else
        {
            auto pf = RuntimeProvider::get()->getParametricFunction(plot->plotTarget);
            if(pf)
            {
                lastPrecalculatedIntervalData = pgex.executeParametricFunction(plot,
                    sharedFromThis(),
                    pf,
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
                    rp,
                    plot->plotTarget,
                    rp->getPixelSize()
                    );

            }

        }

        if(allPoints.isEmpty())
        {
            rp->reportError(lineNumber, ERRORCODE(15), "Invalid data to plot: " + plot->plotTarget);
        }

        precalculatedPoints = allPoints;
        precalculatedSetForce = true;
    }
}

void Assignment::dealWithIndexedAssignmentToSomething(RuntimeProvider *rp, IndexedAccess* ia_m)
{
    QString at = rp->typeOfVariable(varName);

    // assigning to a point
    if(at.toLower() == "point")
    {
        auto p = rp->getAssignment(ia_m->indexedVariable);
        if(p)
        {
            if(p->targetProperties == "points")
            {
                if(p->precalculatedPoints.isEmpty())
                {
                    // get the start/end of the "Stepped" object
                    auto svp = p->startValueProvider();
                    double v = svp ? svp->Calculate() : -1.0;
                    auto evp = p->endValueProvider();
                    double e = evp ? evp->Calculate() : 1.0;
                    auto stepFun = p->step;
                    double stepv = DEFAULT_RANGE_STEP;

                    if(stepFun)
                    {
                        auto count = stepFun->Calculate();
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
                        throw funkplot::syntax_error_exception(ERRORCODE(0), "Erroneous looping conditions detected, check your range");
                    }
                    // and calculate the points
                    auto f = rp->getFunction(p->providedFunction());
                    if(!f)
                    {
                        throw funkplot::syntax_error_exception(ERRORCODE(60), "No function for points assignment");
                    }
                    QVector<QPointF> allPoints;
                    auto pars = f->get_domain_variables();
                    if(pars.size() == 1)
                    {
                        do
                        {
                            f->SetVariable(pars[0].c_str(), v);
                            auto fv = f->Calculate();
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
                throw funkplot::syntax_error_exception(ERRORCODE(61), "Cannot assign a non numeric value (<b>%s</b>) to a numeric variable (<b>%s</b>)", source->varName.toStdString().c_str(), varName.toStdString().c_str());
            }

            auto arrayAssignment = rp->getAssignmentAs<ArrayAssignment>(source->varName);
            if(!arrayAssignment)
            {
                auto pointsOfAssignment = rp->getAssignmentAs<PointsOfObjectAssignment>(source->varName);
                if(!pointsOfAssignment)
                {
                    throw funkplot::syntax_error_exception(ERRORCODE(62), "Cannot identify the object assigning from: <b>%s</b> in this context: <b>%s</b>", source->varName.toStdString().c_str(), statement.toStdString().c_str());
                }
                // find the point we are looking for

                //double v = pointsOfAssignment->precalculatedPoints[ia_m->index];
                //rp->variables()[varName] = v;
                return;
            }

            // this
            if(ia_m->index >= arrayAssignment->m_elements.size())
            {
                throw funkplot::syntax_error_exception(ERRORCODE(55), "Index out of bounds for <b>%s</b> in this context: <b>%s</b>. Requested <b>%d</b>, available: <b>%d</b>",
                                             source->varName.toStdString().c_str(), statement.toStdString().c_str(), ia_m->index, arrayAssignment->m_elements.size());
            }

            double v = arrayAssignment->m_elements[ia_m->index]->Calculate();
            rp->variables()[varName] = v;
        }
        else
        {
            throw funkplot::syntax_error_exception(ERRORCODE(64), "Cannot identify the assignment source (<b>%s</b>)", ia_m->indexedVariable.toStdString().c_str());
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

QVector<QPointF>& Assignment::getPrecalculatedPoints()
{
    return precalculatedPoints;
}

void Assignment::setPrecalculatedPoints(const QVector<QPointF> &newPrecalculatedPoints)
{
    precalculatedPoints = newPrecalculatedPoints;
}

QString Assignment::kw()
{
    return Keywords::KW_LET;
}

// Parses the angle after the "at" keyword into an ExtendedSegmentAssignment.
// Accepts: a named angle variable, or a numeric expression with optional degrees/radians unit.
static void parseAngle(QString& body, ExtendedSegmentAssignment* a)
{
    consumeSpace(body);
    QString token = getDelimitedId(body);
    // consume optional literal keyword "angle" (e.g. "at angle 45" → "at 45")
    if (token.toLower() == "angle")
        token = getDelimitedId(body);
    if (RuntimeProvider::get()->typeOfVariable(token) == Types::TYPE_ANGLE)
    {
        a->angleVarName   = token;
        a->angleInDegrees = false;  // angle variables are stored in radians
    }
    else
    {
        // literal expression; peek at the next token for optional unit
        a->angleLiteral   = Function::temporaryFunction(token, a);
        consumeSpace(body);
        QString unit = getDelimitedId(body);
        a->angleInDegrees = (unit != Keywords::KW_RADIANS);
        // if unit wasn't degrees/radians put it back (it belongs to the next clause)
        if (unit != Keywords::KW_DEGREES && unit != Keywords::KW_RADIANS && !unit.isEmpty())
            body = unit + (body.isEmpty() ? "" : " " + body);
    }
}

static void parseAsLabel(QString& body, Assignment* a)
{
    consumeSpace(body);
    if (!body.startsWith(Keywords::KW_AS + " ") && !body.startsWith(Keywords::KW_AS + "\""))
        return;
    body = body.mid(Keywords::KW_AS.length());
    consumeSpace(body);
    if (body.isEmpty() || body[0] != '"')
        return;
    body = body.mid(1);
    int closeQuote = body.indexOf('"');
    if (closeQuote < 0)
        return;
    a->label = body.left(closeQuote);
    body = body.mid(closeQuote + 1);
}

// Split a combined segment label like "P1Q2" into two endpoint names ("P1","Q2").
// Each name is [A-Za-z][0-9]*.
static bool splitTwoLabels(const QString& s, QString& l1, QString& l2)
{
    if (s.isEmpty() || !s[0].isLetter()) return false;
    int i = 1;
    while (i < s.length() && s[i].isDigit()) ++i;
    if (i >= s.length() || !s[i].isLetter()) return false;
    l1 = s.left(i);
    l2 = s.mid(i);
    return true;
}

static void parseSegmentEndpointLabels(QString& body, SegmentAssignment* sa)
{
    consumeSpace(body);
    if (!body.startsWith(Keywords::KW_AS + " ") && !body.startsWith(Keywords::KW_AS + "\""))
        return;
    body = body.mid(Keywords::KW_AS.length());
    consumeSpace(body);
    if (body.isEmpty() || body[0] != '"') return;
    body = body.mid(1);
    int closeQuote = body.indexOf('"');
    if (closeQuote < 0) return;
    QString combined = body.left(closeQuote);
    body = body.mid(closeQuote + 1);
    QString l1, l2;
    if (splitTwoLabels(combined, l1, l2)) {
        sa->ep1Label = l1;
        sa->ep2Label = l2;
    }
}

static void parseEndpoint(QString& body, QSharedPointer<Function>& x, QSharedPointer<Function>& y, Statement* s, QString* pointVarName = nullptr)
{
    consumeSpace(body);
    if (body.startsWith("(")) {
        body = body.mid(1);
        QString fnai;
        QString sx = extract_proper_expression(body, fnai, {','});
        QString sy = extract_proper_expression(body, fnai, {')'});
        x = Function::temporaryFunction(sx, s);
        y = Function::temporaryFunction(sy, s);
    } else {
        // Point names are single identifiers; getDelimitedId stops at the first space
        // without being confused by stop-words appearing later in the string.
        QString pointExpr = getDelimitedId(body);
        if (pointVarName) *pointVarName = pointExpr;
        x = Function::temporaryFunction(pointExpr + ".x", s);
        y = Function::temporaryFunction(pointExpr + ".y", s);
    }
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
            throw funkplot::syntax_error_exception(ERRORCODE(7), "Invalid assignment: <b>%s</b> (something messy with the index for <b>%s</b>)", codeline.toStdString().c_str(), varName.toStdString().c_str());
        }

        realVarName = varName.left(varName.indexOf("["));
        indexed = true;
        index = varName.mid(varName.indexOf("[") + 1, varName.indexOf("]")  - varName.indexOf("[") - 1);
    }

    if(!RuntimeProvider::get()->hasVariable(varName) && !indexed)
    {
        throw funkplot::syntax_error_exception(ERRORCODE(8), "Invalid assignment: <b>%s</b> (variable <b>%s</b> was not declared)", codeline.toStdString().c_str(), varName.toStdString().c_str());
    }

    // assignment operator
    if(!assignment_body.isEmpty() && assignment_body[0] != '=')
    {
        throw funkplot::syntax_error_exception(ERRORCODE(9), "Invalid assignment: <b>%s</b> (missing assignment operator)", codeline.toStdString().c_str());
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
        if(targetProperties == Keywords::KW_MIDPOINT)
        {
            if(RuntimeProvider::get()->typeOfVariable(varName) != Types::TYPE_POINT)
            {
                throw funkplot::syntax_error_exception(ERRORCODE(10), "Conflicting type assignment: <b>midpoint</b> assigned to a non point type variable: <b>%s (%s)</b>", varName.toStdString().c_str(), RuntimeProvider::get()->typeOfVariable(varName).toStdString().c_str());
            }
            consumeSpace(assignment_body);
            QString ofWord = getDelimitedId(assignment_body);
            if (ofWord != Keywords::KW_OF)
                throw funkplot::syntax_error_exception(ERRORCODE(76), "Missing keyword <b>of</b> in midpoint assignment: %s", codeline.toStdString().c_str());
            QString segName = getDelimitedId(assignment_body);

            QSharedPointer<PointOnSegmentAssignment> result(new PointOnSegmentAssignment(ln, codeline));
            result->segmentName = segName;
            result->varName = varName;
            result->mode = PointOnSegmentMode::Midpoint;

            parseAsLabel(assignment_body, result.get());
            RuntimeProvider::get()->addOrUpdateAssignment(result);
            return handleStatementCallback(vectorize(result), cb);
        }
        else
        if(targetProperties == "point") // or just a simple point
        {
            if(RuntimeProvider::get()->typeOfVariable(varName) != Types::TYPE_POINT)
            {
                throw funkplot::syntax_error_exception(ERRORCODE(10), "Conflicting type assignment: <b>point</b> assigned to a non point type variable: <b>%s (%s)</b>", varName.toStdString().c_str(), RuntimeProvider::get()->typeOfVariable(varName).toStdString().c_str());
            }

            QString nextWord = getDelimitedId(assignment_body);
            if(nextWord == Keywords::KW_ON)
            {
                // point on [segment] <segname|AB> at <spec>
                consumeSpace(assignment_body);
                QString segmentName = getDelimitedId(assignment_body);
                // Consume optional "segment" keyword so both forms work:
                //   point on AB at ...
                //   point on segment AB at ...
                if (segmentName == Keywords::KW_SEGMENT) {
                    consumeSpace(assignment_body);
                    segmentName = getDelimitedId(assignment_body);
                }
                consumeSpace(assignment_body);
                QString atOrNearest = getDelimitedId(assignment_body);
                consumeSpace(assignment_body);

                QSharedPointer<PointOnSegmentAssignment> result(new PointOnSegmentAssignment(ln, codeline));
                result->segmentName = segmentName;
                result->varName = varName;

                if (atOrNearest == Keywords::KW_NEAREST)
                {
                    // nearest to <point>
                    QString toWord = getDelimitedId(assignment_body);
                    if (toWord != Keywords::KW_TO)
                        throw funkplot::syntax_error_exception(ERRORCODE(72), "Missing keyword <b>to</b> in point-on-segment nearest assignment: %s", codeline.toStdString().c_str());
                    result->nearestToPoint = getDelimitedId(assignment_body);
                    result->mode = PointOnSegmentMode::Nearest;
                }
                else if (atOrNearest == Keywords::KW_AT)
                {
                    QString specToken = getDelimitedId(assignment_body);
                    consumeSpace(assignment_body);

                    if (specToken == Keywords::KW_DISTANCE)
                    {
                        // at distance <expr> from <ref>
                        QString fnai;
                        QString distExpr = extract_proper_expression(assignment_body, fnai, {' '}, {Keywords::KW_FROM}, false);
                        QString fromWord = getDelimitedId(assignment_body);
                        if (fromWord != Keywords::KW_FROM)
                            throw funkplot::syntax_error_exception(ERRORCODE(73), "Missing keyword <b>from</b> in point-on-segment distance assignment: %s", codeline.toStdString().c_str());
                        result->fromRef = getDelimitedId(assignment_body);
                        result->param = Function::temporaryFunction(distExpr, result.get());
                        result->mode = PointOnSegmentMode::Distance;
                    }
                    else if (specToken == "t")
                    {
                        // at t <expr>
                        QString fnai;
                        QString tExpr = extract_proper_expression(assignment_body, fnai, {' '}, {Keywords::KW_AS}, false);
                        result->param = Function::temporaryFunction(tExpr, result.get());
                        result->mode = PointOnSegmentMode::ParametricT;
                    }
                    else if (specToken == "x")
                    {
                        // at x <expr>
                        QString fnai;
                        QString xExpr = extract_proper_expression(assignment_body, fnai, {' '}, {Keywords::KW_AS}, false);
                        result->param = Function::temporaryFunction(xExpr, result.get());
                        result->mode = PointOnSegmentMode::AtX;
                    }
                    else if (specToken == "y")
                    {
                        // at y <expr>
                        QString fnai;
                        QString yExpr = extract_proper_expression(assignment_body, fnai, {' '}, {Keywords::KW_AS}, false);
                        result->param = Function::temporaryFunction(yExpr, result.get());
                        result->mode = PointOnSegmentMode::AtY;
                    }
                    else
                    {
                        // at <expr>% from <ref>
                        // specToken already consumed the first space-delimited word; it may contain '%'
                        // Reconstruct the full percentage string: specToken is e.g. "25%"
                        QString percentStr = specToken;
                        if (!percentStr.endsWith('%'))
                            throw funkplot::syntax_error_exception(ERRORCODE(74), "Expected percentage (e.g. <b>25%%</b>) in point-on-segment assignment: %s", codeline.toStdString().c_str());
                        percentStr.chop(1);  // remove '%'
                        QString fromWord = getDelimitedId(assignment_body);
                        if (fromWord != Keywords::KW_FROM)
                            throw funkplot::syntax_error_exception(ERRORCODE(73), "Missing keyword <b>from</b> in point-on-segment percentage assignment: %s", codeline.toStdString().c_str());
                        result->fromRef = getDelimitedId(assignment_body);
                        result->param = Function::temporaryFunction(percentStr, result.get());
                        result->mode = PointOnSegmentMode::Percentage;
                    }
                }
                else
                {
                    throw funkplot::syntax_error_exception(ERRORCODE(75), "Expected <b>at</b> or <b>nearest</b> in point-on-segment assignment: %s", codeline.toStdString().c_str());
                }

                parseAsLabel(assignment_body, result.get());
                RuntimeProvider::get()->addOrUpdateAssignment(result);
                return handleStatementCallback(vectorize(result), cb);
            }
            else if(nextWord != "at")
            {
                // Peek at the next word to detect "point A reflected across ..."
                consumeSpace(assignment_body);
                QString peekWord = getDelimitedId(assignment_body);
                if(peekWord == Keywords::KW_REFLECTED)
                {
                    QString acrossKw = getDelimitedId(assignment_body);
                    if(acrossKw != Keywords::KW_ACROSS)
                        throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>across</b> after <b>reflected</b>: %s", codeline.toStdString().c_str());
                    // Consume optional "line" / "segment" type keyword before the name
                    QString refTok = getDelimitedId(assignment_body);
                    if(refTok == Keywords::KW_LINE || refTok == Keywords::KW_SEGMENT)
                        refTok = getDelimitedId(assignment_body);
                    QSharedPointer<PointReflectionAssignment> result(new PointReflectionAssignment(ln, codeline));
                    result->sourceName = nextWord;
                    result->refName    = refTok;
                    result->varName    = varName;
                    parseAsLabel(assignment_body, result.get());
                    RuntimeProvider::get()->addOrUpdateAssignment(result);
                    return handleStatementCallback(vectorize(result), cb);
                }
                // Restore the peeked word if not "reflected"
                if(!peekWord.isEmpty())
                    assignment_body = peekWord + (assignment_body.isEmpty() ? "" : " " + assignment_body);

                QSharedPointer<PointDefinitionAssignmentToOtherPoint> result;
                result.reset(new PointDefinitionAssignmentToOtherPoint(ln, codeline));
                result->otherPoint = nextWord;
                result->varName = varName;

                parseAsLabel(assignment_body, result.get());
                RuntimeProvider::get()->addOrUpdateAssignment(result);

                return handleStatementCallback(vectorize(result), cb);
            }
            else
            {
                consumeSpace(assignment_body);
                if (assignment_body.startsWith(Keywords::KW_INTERSECTION)) {
                    assignment_body = assignment_body.mid(Keywords::KW_INTERSECTION.length());
                    consumeSpace(assignment_body);
                    if (!assignment_body.startsWith(Keywords::KW_OF)) {
                         throw funkplot::syntax_error_exception(ERRORCODE(51), "Invalid assignment: <b>%s</b> (missing of keyword)", codeline.toStdString().c_str());
                    }
                    assignment_body = assignment_body.mid(Keywords::KW_OF.length());
                    consumeSpace(assignment_body);
                    
                    QString obj1 = getDelimitedId(assignment_body);
                    QString andWord = getDelimitedId(assignment_body);
                    if (andWord != Keywords::KW_AND) {
                        throw funkplot::syntax_error_exception(ERRORCODE(53), "Missing keyword <b>and</b> in intersection assignment: %s", codeline.toStdString().c_str());
                    }
                    QString obj2 = getDelimitedId(assignment_body);
                    
                    QSharedPointer<PointIntersectionAssignment> result(new PointIntersectionAssignment(ln, codeline));
                    result->obj1 = obj1;
                    result->obj2 = obj2;
                    result->varName = varName;
                    parseAsLabel(assignment_body, result.get());
                    RuntimeProvider::get()->addOrUpdateAssignment(result);
                    return handleStatementCallback(vectorize(result), cb);
                }

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
                parseAsLabel(assignment_body, result.get());
                RuntimeProvider::get()->addOrUpdateAssignment(result);

                return handleStatementCallback(vectorize(result), cb);
            }
        }
        if(targetProperties == Keywords::KW_LINE || targetProperties == Keywords::KW_SEGMENT)
        {
            QString type = RuntimeProvider::get()->typeOfVariable(varName);
            if(targetProperties == Keywords::KW_LINE && type != Types::TYPE_LINE)
            {
                throw funkplot::syntax_error_exception(ERRORCODE(10), "Conflicting type assignment: <b>line</b> assigned to a non line type variable: <b>%s (%s)</b>", varName.toStdString().c_str(), type.toStdString().c_str());
            }
            if(targetProperties == Keywords::KW_SEGMENT && type != Types::TYPE_SEGMENT)
            {
                throw funkplot::syntax_error_exception(ERRORCODE(10), "Conflicting type assignment: <b>segment</b> assigned to a non segment type variable: <b>%s (%s)</b>", varName.toStdString().c_str(), type.toStdString().c_str());
            }

            QString nextWord = getDelimitedId(assignment_body);
            if(targetProperties == Keywords::KW_LINE && nextWord == Keywords::KW_THROUGH)
            {
                // Parse the first point, then decide based on the connector word
                QSharedPointer<ExtendedLineAssignment> ext(new ExtendedLineAssignment(ln, codeline));
                parseEndpoint(assignment_body, ext->x1, ext->y1, ext.get(), &ext->refPointName);
                QString connector = getDelimitedId(assignment_body);
                if(connector == Keywords::KW_AND)
                {
                    // Classic two-point line: line through P and Q
                    QSharedPointer<LineAssignment> result(new LineAssignment(ln, codeline));
                    result->x1 = ext->x1; result->y1 = ext->y1;
                    parseEndpoint(assignment_body, result->x2, result->y2, result.get());
                    result->varName = varName;
                    RuntimeProvider::get()->addOrUpdateAssignment(result);
                    return handleStatementCallback(vectorize(result), cb);
                }
                else if(connector == Keywords::KW_PARALLEL || connector == Keywords::KW_PERPENDICULAR)
                {
                    // line through P parallel/perpendicular to <ref>
                    QString toKw = getDelimitedId(assignment_body);
                    if(toKw != Keywords::KW_TO)
                        throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>to</b> after parallel/perpendicular: %s", codeline.toStdString().c_str());
                    ext->refName = getDelimitedId(assignment_body);
                    ext->form = (connector == Keywords::KW_PARALLEL) ? ExtendedLineForm::ThroughParallel : ExtendedLineForm::ThroughPerpendicular;
                    ext->varName = varName;
                    RuntimeProvider::get()->addOrUpdateAssignment(ext);
                    return handleStatementCallback(vectorize(ext), cb);
                }
                else
                {
                    throw funkplot::syntax_error_exception(ERRORCODE(53), "Expected <b>and</b>, <b>parallel</b>, or <b>perpendicular</b> after point in line definition: %s", codeline.toStdString().c_str());
                }
            }
            if(targetProperties == Keywords::KW_SEGMENT && nextWord == Keywords::KW_FROM)
            {
                // Use ExtendedSegmentAssignment as a temporary owner for the parsed functions;
                // if it turns out to be the classic "from p to q" form we repackage it.
                QSharedPointer<ExtendedSegmentAssignment> ext(new ExtendedSegmentAssignment(ln, codeline));
                parseEndpoint(assignment_body, ext->refPx, ext->refPy, ext.get(), &ext->refPointName);

                QString kw2 = getDelimitedId(assignment_body);
                if(kw2 == Keywords::KW_TO)
                {
                    // Classic form: segment from p1 to p2
                    QSharedPointer<SegmentAssignment> result(new SegmentAssignment(ln, codeline));
                    result->x1 = ext->refPx;  result->y1 = ext->refPy;  result->p1Name = ext->refPointName;
                    parseEndpoint(assignment_body, result->x2, result->y2, result.get(), &result->p2Name);
                    result->varName = varName;
                    parseSegmentEndpointLabels(assignment_body, result.get());
                    RuntimeProvider::get()->addOrUpdateAssignment(result);
                    return handleStatementCallback(vectorize(result), cb);
                }
                else if(kw2 == Keywords::KW_LENGTH)
                {
                    // Extended form: segment from p length L at <angle>
                    ext->form     = ExtendedSegmentForm::FromPointLengthAngle;
                    ext->lengthFn = Function::temporaryFunction(getDelimitedId(assignment_body), ext.get());
                    QString atKw  = getDelimitedId(assignment_body);
                    if(atKw != Keywords::KW_AT)
                        throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>at</b> after length in segment definition: %s", codeline.toStdString().c_str());
                    parseAngle(assignment_body, ext.get());
                    ext->varName = varName;
                    parseSegmentEndpointLabels(assignment_body, ext.get());
                    RuntimeProvider::get()->addOrUpdateAssignment(ext);
                    return handleStatementCallback(vectorize(ext), cb);
                }
                else if(kw2 == Keywords::KW_PERPENDICULAR)
                {
                    // segment from P perpendicular to S  (foot of altitude from P onto S)
                    QString toKw = getDelimitedId(assignment_body);
                    if(toKw != Keywords::KW_TO)
                        throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>to</b> after <b>perpendicular</b> in segment definition: %s", codeline.toStdString().c_str());
                    QString segName = getDelimitedId(assignment_body);
                    ext->form = ExtendedSegmentForm::PerpendicularFromPoint;
                    ext->refSegmentName = segName;
                    ext->varName = varName;
                    parseSegmentEndpointLabels(assignment_body, ext.get());
                    RuntimeProvider::get()->addOrUpdateAssignment(ext);
                    return handleStatementCallback(vectorize(ext), cb);
                }
                else if(kw2 == Keywords::KW_THROUGH)
                {
                    // segment from A through B length L  (from A, direction A→B, length L)
                    ext->throughPointName = getDelimitedId(assignment_body);
                    QString lenKw = getDelimitedId(assignment_body);
                    if(lenKw != Keywords::KW_LENGTH)
                        throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>length</b> after through-point in segment definition: %s", codeline.toStdString().c_str());
                    ext->lengthFn = Function::temporaryFunction(getDelimitedId(assignment_body), ext.get());
                    ext->form = ExtendedSegmentForm::FromThroughLength;
                    ext->varName = varName;
                    parseSegmentEndpointLabels(assignment_body, ext.get());
                    RuntimeProvider::get()->addOrUpdateAssignment(ext);
                    return handleStatementCallback(vectorize(ext), cb);
                }
                else
                {
                    throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>to</b>, <b>length</b>, <b>perpendicular to</b>, or <b>through</b> in segment definition: %s", codeline.toStdString().c_str());
                }
            }
            if(targetProperties == Keywords::KW_SEGMENT && nextWord == Keywords::KW_CENTERED)
            {
                // segment centered at p length L at <angle>
                QString atKw = getDelimitedId(assignment_body);
                if(atKw != Keywords::KW_AT)
                    throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>at</b> after <b>centered</b>: %s", codeline.toStdString().c_str());

                QSharedPointer<ExtendedSegmentAssignment> result(new ExtendedSegmentAssignment(ln, codeline));
                result->form = ExtendedSegmentForm::CenteredLengthAngle;
                parseEndpoint(assignment_body, result->refPx, result->refPy, result.get(), &result->refPointName);

                QString lenKw = getDelimitedId(assignment_body);
                if(lenKw != Keywords::KW_LENGTH)
                    throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>length</b> after center point: %s", codeline.toStdString().c_str());
                result->lengthFn = Function::temporaryFunction(getDelimitedId(assignment_body), result.get());

                QString atKw2 = getDelimitedId(assignment_body);
                if(atKw2 != Keywords::KW_AT)
                    throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>at</b> after length: %s", codeline.toStdString().c_str());
                parseAngle(assignment_body, result.get());

                result->varName = varName;
                parseSegmentEndpointLabels(assignment_body, result.get());
                RuntimeProvider::get()->addOrUpdateAssignment(result);
                return handleStatementCallback(vectorize(result), cb);
            }
            if(targetProperties == Keywords::KW_SEGMENT && nextWord == Keywords::KW_THROUGH)
            {
                // segment through p parallel/perpendicular to s length L
                // parseEndpoint cannot be used here because "to" appears later in the line
                // ("parallel to s1"), which makes extract_proper_expression read past the point
                // name. Use getDelimitedId directly — the point is always a single identifier
                // or a coordinate literal in this context.
                QSharedPointer<ExtendedSegmentAssignment> result(new ExtendedSegmentAssignment(ln, codeline));
                consumeSpace(assignment_body);
                if(assignment_body.startsWith("("))
                {
                    assignment_body = assignment_body.mid(1);
                    QString fnai;
                    QString sx = extract_proper_expression(assignment_body, fnai, {','});
                    QString sy = extract_proper_expression(assignment_body, fnai, {')'});
                    result->refPx = Function::temporaryFunction(sx, result.get());
                    result->refPy = Function::temporaryFunction(sy, result.get());
                }
                else
                {
                    result->refPointName = getDelimitedId(assignment_body);
                    result->refPx = Function::temporaryFunction(result->refPointName + ".x", result.get());
                    result->refPy = Function::temporaryFunction(result->refPointName + ".y", result.get());
                }

                QString dirKw = getDelimitedId(assignment_body);
                if(dirKw == Keywords::KW_PARALLEL)
                    result->form = ExtendedSegmentForm::ThroughParallel;
                else if(dirKw == Keywords::KW_PERPENDICULAR)
                    result->form = ExtendedSegmentForm::ThroughPerpendicular;
                else
                    throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>parallel</b> or <b>perpendicular</b> after point: %s", codeline.toStdString().c_str());

                QString toKw = getDelimitedId(assignment_body);
                if(toKw != Keywords::KW_TO)
                    throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>to</b> after parallel/perpendicular: %s", codeline.toStdString().c_str());
                result->refSegmentName = getDelimitedId(assignment_body);

                QString lenKw = getDelimitedId(assignment_body);
                if(lenKw != Keywords::KW_LENGTH)
                    throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>length</b> after segment name: %s", codeline.toStdString().c_str());
                result->lengthFn = Function::temporaryFunction(getDelimitedId(assignment_body), result.get());

                result->varName = varName;
                parseSegmentEndpointLabels(assignment_body, result.get());
                RuntimeProvider::get()->addOrUpdateAssignment(result);
                return handleStatementCallback(vectorize(result), cb);
            }
            if(targetProperties == Keywords::KW_SEGMENT && nextWord == Keywords::KW_CONGRUENT)
            {
                // segment congruent to <src> from <P> at angle <X>
                // segment congruent to <src> from <P> parallel to <ref>
                // segment congruent to <src> from <P> perpendicular to <ref>
                // segment congruent to <src> centered at <P> at angle <X>
                // segment congruent to <src> centered at <P> parallel to <ref>
                // segment congruent to <src> centered at <P> perpendicular to <ref>

                QString toKw = getDelimitedId(assignment_body);
                if(toKw != Keywords::KW_TO)
                    throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>to</b> after <b>congruent</b>: %s", codeline.toStdString().c_str());

                QString srcSeg = getDelimitedId(assignment_body);
                QString posKw  = getDelimitedId(assignment_body); // "from" or "centered"

                QSharedPointer<ExtendedSegmentAssignment> result(new ExtendedSegmentAssignment(ln, codeline));
                result->lengthFromSegmentName = srcSeg;

                if(posKw == Keywords::KW_FROM)
                {
                    parseEndpoint(assignment_body, result->refPx, result->refPy, result.get(), &result->refPointName);
                    QString dirKw = getDelimitedId(assignment_body);
                    if(dirKw == Keywords::KW_AT)
                    {
                        // "at angle <X>"
                        parseAngle(assignment_body, result.get());
                        result->form = ExtendedSegmentForm::FromPointLengthAngle;
                    }
                    else if(dirKw == Keywords::KW_PARALLEL)
                    {
                        QString toKw2 = getDelimitedId(assignment_body); // "to"
                        result->refSegmentName = getDelimitedId(assignment_body);
                        result->form = ExtendedSegmentForm::CongruentFromParallel;
                    }
                    else if(dirKw == Keywords::KW_PERPENDICULAR)
                    {
                        QString toKw2 = getDelimitedId(assignment_body); // "to"
                        result->refSegmentName = getDelimitedId(assignment_body);
                        result->form = ExtendedSegmentForm::CongruentFromPerpendicular;
                    }
                    else
                        throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>at angle</b>, <b>parallel to</b>, or <b>perpendicular to</b> after point: %s", codeline.toStdString().c_str());
                }
                else if(posKw == Keywords::KW_CENTERED)
                {
                    QString atKw = getDelimitedId(assignment_body); // "at"
                    if(atKw != Keywords::KW_AT)
                        throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>at</b> after <b>centered</b>: %s", codeline.toStdString().c_str());
                    parseEndpoint(assignment_body, result->refPx, result->refPy, result.get(), &result->refPointName);
                    QString dirKw = getDelimitedId(assignment_body);
                    if(dirKw == Keywords::KW_AT)
                    {
                        parseAngle(assignment_body, result.get());
                        result->form = ExtendedSegmentForm::CenteredLengthAngle;
                    }
                    else if(dirKw == Keywords::KW_PARALLEL)
                    {
                        QString toKw2 = getDelimitedId(assignment_body); // "to"
                        result->refSegmentName = getDelimitedId(assignment_body);
                        result->form = ExtendedSegmentForm::ThroughParallel;
                    }
                    else if(dirKw == Keywords::KW_PERPENDICULAR)
                    {
                        QString toKw2 = getDelimitedId(assignment_body); // "to"
                        result->refSegmentName = getDelimitedId(assignment_body);
                        result->form = ExtendedSegmentForm::ThroughPerpendicular;
                    }
                    else
                        throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>at angle</b>, <b>parallel to</b>, or <b>perpendicular to</b> after center point: %s", codeline.toStdString().c_str());
                }
                else
                    throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>from</b> or <b>centered at</b> after congruent source segment: %s", codeline.toStdString().c_str());

                result->varName = varName;
                parseSegmentEndpointLabels(assignment_body, result.get());
                RuntimeProvider::get()->addOrUpdateAssignment(result);
                return handleStatementCallback(vectorize(result), cb);
            }
        }
        // Measurement: distance from A to B  |  length of segment AB
        if(targetProperties == Keywords::KW_DISTANCE || targetProperties == Keywords::KW_LENGTH)
        {
            QSharedPointer<MeasurementAssignment> result(new MeasurementAssignment(ln, codeline));
            result->varName = varName;
            if(targetProperties == Keywords::KW_DISTANCE)
            {
                // distance from <p1> to <p2>
                QString fromKw = getDelimitedId(assignment_body);
                if(fromKw != Keywords::KW_FROM)
                    throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>from</b> after <b>distance</b>: %s", codeline.toStdString().c_str());
                result->p1Name = getDelimitedId(assignment_body);
                QString toKw = getDelimitedId(assignment_body);
                if(toKw != Keywords::KW_TO)
                    throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>to</b> in distance expression: %s", codeline.toStdString().c_str());
                result->p2Name = getDelimitedId(assignment_body);
                result->form = MeasurementForm::DistanceFromTo;
            }
            else
            {
                // length of [segment] <name>
                QString ofKw = getDelimitedId(assignment_body);
                if(ofKw != Keywords::KW_OF)
                    throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>of</b> after <b>length</b>: %s", codeline.toStdString().c_str());
                QString nextTok = getDelimitedId(assignment_body);
                if(nextTok == Keywords::KW_SEGMENT)
                    nextTok = getDelimitedId(assignment_body);
                result->segName = nextTok;
                result->form = MeasurementForm::LengthOfSegment;
            }
            RuntimeProvider::get()->addOrUpdateAssignment(result);
            return handleStatementCallback(vectorize(result), cb);
        }
        // Perpendicular bisector: perpendicular bisector of <segment>
        if(targetProperties == Keywords::KW_PERPENDICULAR)
        {
            QString nextW = getDelimitedId(assignment_body);
            if(nextW == Keywords::KW_BISECTOR)
            {
                if(RuntimeProvider::get()->typeOfVariable(varName) != Types::TYPE_LINE)
                    throw funkplot::syntax_error_exception(ERRORCODE(10), "Conflicting type: <b>perpendicular bisector</b> must be assigned to a <b>line</b> variable: %s", codeline.toStdString().c_str());
                QString ofKw = getDelimitedId(assignment_body);
                if(ofKw != Keywords::KW_OF)
                    throw funkplot::syntax_error_exception(ERRORCODE(54), "Expected <b>of</b> after <b>perpendicular bisector</b>: %s", codeline.toStdString().c_str());
                QString segName = getDelimitedId(assignment_body);
                QSharedPointer<ExtendedLineAssignment> result(new ExtendedLineAssignment(ln, codeline));
                result->form = ExtendedLineForm::PerpendicularBisector;
                result->refName = segName;
                result->varName = varName;
                RuntimeProvider::get()->addOrUpdateAssignment(result);
                return handleStatementCallback(vectorize(result), cb);
            }
            // not a bisector — fall through to arithmetic
            assignment_body = nextW + (assignment_body.isEmpty() ? "" : " " + assignment_body);
        }
        // Angle assignment: three forms:
        //   let alpha = angle of s           (angle segment makes with x-axis)
        //   let alpha = angle p1 vertex p3   (interior angle at vertex)
        //   let alpha = <expr> [degrees|radians]  (literal, default degrees)
        {
            bool isAngleVar = RuntimeProvider::get()->typeOfVariable(varName) == Types::TYPE_ANGLE;
            bool startsWithAngleKw = (targetProperties == Types::TYPE_ANGLE);

            if(startsWithAngleKw || isAngleVar)
            {
                // Angle bisector: let l = angle bisector A B C  (l must be a line variable)
                if(startsWithAngleKw && RuntimeProvider::get()->typeOfVariable(varName) == Types::TYPE_LINE)
                {
                    QString nextW = getDelimitedId(assignment_body);
                    if(nextW == Keywords::KW_BISECTOR)
                    {
                        QString p1n   = getDelimitedId(assignment_body);
                        QString vn    = getDelimitedId(assignment_body);
                        QString p3n   = getDelimitedId(assignment_body);
                        QSharedPointer<ExtendedLineAssignment> result(new ExtendedLineAssignment(ln, codeline));
                        result->form       = ExtendedLineForm::AngleBisector;
                        result->p1Name     = p1n;
                        result->vertexName = vn;
                        result->p3Name     = p3n;
                        result->varName    = varName;
                        RuntimeProvider::get()->addOrUpdateAssignment(result);
                        return handleStatementCallback(vectorize(result), cb);
                    }
                    // Not a bisector; put back the token and fall through to the throw below
                    assignment_body = nextW + (assignment_body.isEmpty() ? "" : " " + assignment_body);
                }
                if(!isAngleVar)
                {
                    throw funkplot::syntax_error_exception(ERRORCODE(10),
                        "Conflicting type assignment: <b>angle</b> assigned to a non angle type variable: <b>%s (%s)</b>",
                        varName.toStdString().c_str(),
                        RuntimeProvider::get()->typeOfVariable(varName).toStdString().c_str());
                }

                QSharedPointer<AngleAssignment> result(new AngleAssignment(ln, codeline));
                result->varName = varName;

                if(startsWithAngleKw)
                {
                    // "angle of segname"  or  "angle p1 vertex p3"
                    QString nextWord = getDelimitedId(assignment_body);
                    if(nextWord == Keywords::KW_OF)
                    {
                        result->source = AngleSource::OfSegment;
                        result->segmentName = getDelimitedId(assignment_body);
                    }
                    else
                    {
                        result->source = AngleSource::ThreePoints;
                        result->p1Name     = nextWord;
                        result->vertexName = getDelimitedId(assignment_body);
                        result->p3Name     = getDelimitedId(assignment_body);
                    }
                }
                else
                {
                    // "<expr> [degrees|radians]"  — targetProperties is already the expression
                    // (it was the first space-delimited token). The optional unit keyword is
                    // the very next token; anything after that (comments etc.) is ignored.
                    result->source = AngleSource::Literal;
                    result->expr = Function::temporaryFunction(targetProperties, result.get());
                    consumeSpace(assignment_body);
                    QString unitWord = getDelimitedId(assignment_body);
                    result->inDegrees = (unitWord != Keywords::KW_RADIANS);
                }

                RuntimeProvider::get()->addOrUpdateAssignment(result);
                return handleStatementCallback(vectorize(result), cb);
            }
        }

        if(targetProperties == Types::TYPE_LIST) // or maybe we create a list
        {
            if(RuntimeProvider::get()->typeOfVariable(varName) != Types::TYPE_LIST)
            {
                throw funkplot::syntax_error_exception(ERRORCODE(12), "Conflicting type assignment: <b>array</b> assigned to a non array type variable: <b>%s (%s)</b>", varName.toStdString().c_str(), RuntimeProvider::get()->typeOfVariable(varName).toStdString().c_str());
            }

            // skip the spaces after the Types::TYPE_LIST keyword
            consumeSpace(assignment_body);

            if(!assignment_body.isEmpty())
            {
                if(assignment_body.at(0) != '[' && delimiter != '[')
                {
                    throw funkplot::syntax_error_exception(ERRORCODE(13), "Invalid array assignment: missing <b>[</b> from <b>%s</b>", codeline.toStdString().c_str());
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
                            throw funkplot::syntax_error_exception(ERRORCODE(35), "Invalid point array assignment: missing <b>(</b> from <b>%s</b>", codeline.toStdString().c_str());
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
                if(delimiter == '[')
                {
                    result->otherPoint += "["+ assignment_body;
                }
                else
                {
                    result->otherPoint += assignment_body;
                }
                result->varName = varName;

                RuntimeProvider::get()->addOrUpdateAssignment(result);

                return handleStatementCallback(vectorize(result), cb);;
            }
            else
            {
                if(RuntimeProvider::get()->typeOfVariable(varName) != Types::TYPE_NUMBER && !indexed)
                {
                    throw funkplot::syntax_error_exception(ERRORCODE(14), "Invalid assignment: <b>%s</b> (<b>arithmetic expression</b> assigned to <b>%s</b>)", varName.toStdString().c_str(), RuntimeProvider::get()->typeOfVariable(varName).toStdString().c_str());
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
        throw funkplot::syntax_error_exception(ERRORCODE(12), "Invalid assignment: <b>%s</b> (<b>array</b> assigned to <b>%s</b>)", varName.toStdString().c_str(), RuntimeProvider::get()->typeOfVariable(varName).toStdString().c_str());
    }
    QSharedPointer<Assignment> result = providePointsOfDefinition(ln, codeline, assignment_body, varName, targetProperties);
    RuntimeProvider::get()->addOrUpdateAssignment(result);
    return handleStatementCallback(vectorize(result), cb);
    }

    QSharedPointer<Assignment> Assignment::providePointsOfDefinition(int ln, const QString &codeline, QString assignment_body, const QString &varName, const QString &targetProperties)
    {
    if(!assignment_body.startsWith(Keywords::KW_OF))
    {
        throw funkplot::syntax_error_exception(ERRORCODE(51), "Invalid assignment: <b>%s</b> (missing of keyword)", codeline.toStdString().c_str());
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

    if (result->ofWhat == Keywords::KW_LINE || result->ofWhat == Keywords::KW_SEGMENT) {
        bool isLine = result->ofWhat == Keywords::KW_LINE;
        consumeSpace(assignment_body);
        if (assignment_body.startsWith(Keywords::KW_FROM)) {
            assignment_body = assignment_body.mid(Keywords::KW_FROM.length());
            consumeSpace(assignment_body);
        }
        
        parseEndpoint(assignment_body, result->x1, result->y1, result.get());

        consumeSpace(assignment_body);
        if (assignment_body.startsWith(Keywords::KW_TO)) {
            assignment_body = assignment_body.mid(Keywords::KW_TO.length());
            consumeSpace(assignment_body);
        }
        
        parseEndpoint(assignment_body, result->x2, result->y2, result.get());

    } else {
        QSharedPointer<Assignment> assignment;

        if(!RuntimeProvider::get()->getNameFunctionOrAssignment(result->ofWhat, assignment) && !RuntimeProvider::get()->getParametricFunction(result->ofWhat))
        {
            throw funkplot::syntax_error_exception(ERRORCODE(52), "Invalid assignment: %s. No such function: %s", codeline.toStdString().c_str(), result->ofWhat.toStdString().c_str());
        }
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
