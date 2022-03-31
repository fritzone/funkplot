#include "RuntimeProvider.h"

const QString STR_FUNCTION = Keywords::KW_FUNCTION + " ";    // function f(x) = x * 2 + 5
const QString STR_PLOT = Keywords::KW_PLOT + " ";            // plot f over (-2, 2) [continuous]
const QString STR_OVER = Keywords::KW_OVER + " ";            // plot f over (-2, 2) [continuous]
const QString STR_LET = Keywords::KW_LET + " ";              // let something = points of f over (-2, 2) [continuous|step<cnt|0.01>]
const QString STR_OF  = Keywords::KW_OF + " ";               // let something = points of f over (-2, 2) [continuous|step<cnt|0.01>]
const QString STR_STEP = Keywords::KW_STEP + " ";            // plot f over (-2, 2) [continuous|step<cnt|0.01>]
const QString STR_COUNTS = Keywords::KW_COUNTS + " ";
const QString STR_ROTATE = Keywords::KW_ROTATE + " ";
const QString STR_FOREACH = Keywords::KW_FOREACH + " ";      // foreach p in something do ... done
const QString STR_SET = Keywords::KW_SET + " ";              // set color red
const QString STR_IN = Keywords::KW_IN + " ";                // foreach p in something do ... done


RuntimeProvider::RuntimeProvider(CB_ErrorReporter erp, CB_PointDrawer pd, CB_StatementTracker str, CB_PenSetter ps, CB_PlotDrawer pld) :
    m_vars(), m_errorReporter(erp), m_pointDrawer(pd), m_statementTracker(str), m_penSetter(ps), m_plotDrawer(pld)
{

}

int RuntimeProvider::defd(const std::string &s)
{
    if(m_vars.contains(QString::fromStdString(s)))
    {
        return 1;
    }

    return 0;
}

double RuntimeProvider::value(const std::string &s)
{
    if(defd(s))
    {
        return m_vars.value(QString::fromStdString(s));
    }

    return std::numeric_limits<double>::quiet_NaN();
}

void RuntimeProvider::setValue(const QString &s, double v)
{
    m_vars[s] = v;
}

void RuntimeProvider::setValue(const QString &s, QPointF v)
{
    m_points[s] = v;
}


QMap<QString, double> &RuntimeProvider::variables()
{
    return m_vars;
}

QSharedPointer<Function> RuntimeProvider::getFunction(const QString &name, QSharedPointer<Assignment>& assignment)
{
    QSharedPointer<Function> result;

    auto it = std::find_if(functions.begin(), functions.end(), [&name](QSharedPointer<FunctionDefinition> f) -> bool {
        return f->f->get_name() == name.toStdString();
    });

    if(it != functions.end())
    {

        result = ((*it)->f);
    }

    if(!result)
    {
        // second: or an assignment bound to a function
        for(const auto& adef : qAsConst(assignments))
        {
            if(adef->varName == name)
            {
                if(!adef->providedFunction().isEmpty())
                {
                    result = getFunction(adef->providedFunction(), assignment);
                    if(result)
                    {
                        assignment = adef;
                        break;
                    }
                }
            }
        }
    }

    return result;
}

void RuntimeProvider::reportError(const QString &err)
{
    if(m_errorReporter)
    {
        if(m_shouldReport)
        {
            m_errorReporter(err);
        }
    }
}

bool RuntimeProvider::resolveAsPoint(QSharedPointer<Plot> plot)
{
    for(const auto& adef : qAsConst(assignments))
    {
        if(plot->plotTarget == adef->varName)
        {
            auto fcp = adef->fullCoordProvider();
            if( std::get<0>(fcp) && std::get<1>(fcp) )
            {
                double x = std::get<0>(fcp)->Calculate(this);
                double y = std::get<1>(fcp)->Calculate(this);
                m_pointDrawer(x, y);
                return true;
            }
        }
    }
    return false;
}

void RuntimeProvider::reset()
{
    plots.clear();
    functions.clear();
    assignments.clear();
    m_setts.clear();
    statements.clear();
    m_vars.clear();

}

void RuntimeProvider::parse(QStringList codelines)
{
    while(!codelines.empty())
    {
        resolveCodeline(codelines, statements, nullptr);
    }

}

bool RuntimeProvider::get_shouldReport() const
{
    return m_shouldReport;
}

void RuntimeProvider::set_ShouldReport(bool newShouldReport)
{
    m_shouldReport = newShouldReport;
}


void RuntimeProvider::execute()
{
    try
    {
        for(const auto& stmt : qAsConst(statements))
        {
            setCurrentStatement(stmt->statement);
            stmt->execute(this);
        }
    }
    catch(std::exception& ex)
    {
        reportError(ex.what());

    }

}

void RuntimeProvider::setCurrentStatement(const QString &newCurrentStatement)
{
    m_statementTracker(newCurrentStatement);
}

void RuntimeProvider::setPen(int r, int g, int b, int a)
{
    m_penSetter(r, g, b, a);
}

void RuntimeProvider::drawPlot(QSharedPointer<Plot> plot)
{
    m_plotDrawer(plot);
}

void RuntimeProvider::resolvePlotInterval(QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment,
                                          bool& continuous, double& plotStart, double& plotEnd, bool& counted, double &stepValue, int& count)
{
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

    if(stepFun)
    {
        count = stepFun->Calculate(this);
        if(counted)
        {
            if(count > 1)
            {
                stepValue = (plotEnd - plotStart) / (count - 1);
            }
            else
            {
                stepValue = (plotEnd - plotStart);
            }
        }
    }
}

QSharedPointer<Statement> RuntimeProvider::resolveCodeline(QStringList& codelines, QVector<QSharedPointer<Statement>>& statements, QSharedPointer<Statement> parentScope)
{
    QString codeline = codelines[0];
    codelines.pop_front();
    codeline = codeline.simplified();

    try
    {

        QSharedPointer<Statement> createdStatement;

        if(codeline.startsWith(STR_FUNCTION))
        {
            statements.append(createdStatement = createFunction(codeline));
        }
        else
        if(codeline.startsWith(STR_PLOT))
        {
            statements.append(createdStatement = createPlot(codeline));
        }
        else
        if(codeline.startsWith(STR_LET)) // variable assignment
        {
            statements.append(createdStatement = resolveObjectAssignment(codeline));
        }
        else
        if(codeline.startsWith(STR_SET)) // setting the color, line width, rotation, etc ...
        {
            statements.append(createdStatement = createSett(codeline));
        }
        else
        if(codeline.startsWith(STR_FOREACH)) // looping over a set of points or something else
        {
            statements.append(createdStatement = createLoop(codeline, codelines));
        }
        else
        if(codeline.startsWith(Keywords::KW_DONE)) // looping over a set of points or something else
        {
            statements.append(createdStatement = QSharedPointer<Done>(new Done(codeline)));
        }
        else
        if(codeline.startsWith(Keywords::KW_ROTATE)) // rotating something
        {
            statements.append(createdStatement = createRotation(codeline, codelines));
        }
        else
        if(!codeline.isEmpty())
        {
            reportError("Invalid statement: " + codeline);
        }
        if(createdStatement)
        {
            createdStatement->parent = parentScope;
            createdStatement->runtimeProvider = this;
        }

    }
    catch(std::exception& ex)
    {
        reportError(ex.what());
        return nullptr;
    }
    if(!statements.isEmpty())
    {
        return statements.constLast();
    }

    return nullptr;
}



QSharedPointer<Statement> RuntimeProvider::createFunction(const QString &codeline)
{
    QString f_body = codeline.mid(STR_FUNCTION.length());
    Function* f = new Function(f_body.toLocal8Bit().data());
    QSharedPointer<FunctionDefinition> fd;
    fd.reset(new FunctionDefinition(codeline) );
    fd->f = QSharedPointer<Function>(f);
    functions.push_back(fd);

    return fd;
}

QSharedPointer<Statement> RuntimeProvider::createSett(const QString &codeline)
{
    QString s_body = codeline.mid(STR_SET.length());
    // set what?
    QString what;
    while(!s_body.isEmpty() && !s_body[0].isSpace())
    {
        what += s_body[0];
        s_body = s_body.mid(1);
    }
    if(what != "color")
    {
        throw syntax_error_exception("Syntax error: only colors can be set for now");
    }
    s_body = s_body.mid(1);
    // to what, can be: colorname, #RRGGBB or R,G,B (real numbers for this situation)
    QString to_what;
    while(!s_body.isEmpty())
    {
        to_what += s_body[0];
        s_body = s_body.mid(1);
    }
    QSharedPointer<Sett> sett(new Sett(codeline));
    sett->what = what;
    sett->value = to_what;
    return sett;
}

QSharedPointer<Statement> RuntimeProvider::createLoop(const QString &codeline, QStringList &codelines)
{
    QString l_decl = codeline.mid(STR_FOREACH.length());
    QVector<QSharedPointer<Statement>> loop_body;
    QSharedPointer<Loop> result = nullptr;

    // the loop variable
    QString loop_variable;
    while(!l_decl.isEmpty() && !l_decl[0].isSpace())
    {
        loop_variable += l_decl[0];
        l_decl = l_decl.mid(1);
    }
    // skip the space
    l_decl = l_decl.mid(1);

    // in keyword
    if(l_decl.startsWith(STR_IN))
    {
        // skip it
        l_decl = l_decl.mid(STR_IN.length());

        // see what are we looping through
        QString loop_over;
        while(!l_decl.isEmpty() && !l_decl[0].isSpace() && !(l_decl[0] == '(') && !(l_decl[0] == '['))
        {
            loop_over += l_decl[0];
            l_decl = l_decl.mid(1);
        }
        // skip the space
        consumeSpace(l_decl);

        // create the correct loop object
        result.reset(new Loop(codeline));
        result->loop_variable = loop_variable;
        if(loop_over == Keywords::KW_RANGE && (l_decl[0] == '(' || l_decl[0] == '['))
        {
            auto ritl = new RangeIteratorLoopTarget(result);
            result->loop_target.reset(ritl);
            l_decl = l_decl.mid(1);
            QString range_start = "";
            while(!l_decl.isEmpty() && !l_decl[0].isSpace() && !(l_decl[0] == ','))
            {
                range_start += l_decl[0];
                l_decl = l_decl.mid(1);
            }
            consumeSpace(l_decl);
            if(l_decl[0] == ',')
            {
                l_decl = l_decl.mid(1);
                consumeSpace(l_decl);
            }

            ritl->startFun = temporaryFunction(range_start);

            QString range_end = "";
            while(!l_decl.isEmpty() && !l_decl[0].isSpace() && !(l_decl[0] == ')' || l_decl[0] == ']'))
            {
                range_end += l_decl[0];
                l_decl = l_decl.mid(1);
            }

            ritl->endFun = temporaryFunction(range_end);

            if(l_decl[0] == ')')
            {
                l_decl = l_decl.mid(1);
            }
            consumeSpace(l_decl);
            // do we have a step
            if(l_decl.startsWith(Keywords::KW_STEP))
            {
                l_decl = l_decl.mid(STR_STEP.length());
                consumeSpace(l_decl);
                QString step;
                while(!l_decl.isEmpty() && !l_decl[0].isSpace())
                {
                    step += l_decl[0];
                    l_decl = l_decl.mid(1);
                }

                ritl->stepFun = temporaryFunction(step);

                consumeSpace(l_decl);
            }
            else
            {
                ritl->stepFun = temporaryFunction("0.01");
            }
        }
        else
        {
            result->loop_target.reset(new FunctionIteratorLoopTarget(result));
            result->loop_target->name = loop_over;
        }

        if(!l_decl.startsWith(Keywords::KW_DO))
        {
            throw syntax_error_exception("foreach does not contain the do keyword");
        }

        bool done = false;
        while(!codelines.isEmpty() && !done)
        {
            QSharedPointer<Statement> st = nullptr;

            try
            {
                st = resolveCodeline(codelines, result->body, result);
            }
            catch(...)
            {
                throw;
            }

            if(st)
            {
                done = st->keyword() == Keywords::KW_DONE;
            }
            else
            {
                throw syntax_error_exception("something is wrong with this expression: %s", codeline.toStdString().c_str());
            }
        }

        if(!done)
        {
            throw syntax_error_exception("foreach body does not end with done");
        }

    }
    else
    {
        throw syntax_error_exception("foreach does not contain the in keyword");
    }

    return result;

}

QSharedPointer<Statement> RuntimeProvider::createRotation(const QString &codeline, QStringList &codelines)
{
    QString r_decl = codeline.mid(STR_ROTATE.length());
    QString rotate_what = getDelimitedId(r_decl);
    QString with_keyword = getDelimitedId(r_decl);
    if(with_keyword != "with")
    {
        throw syntax_error_exception("rotate statement does not contain <b>with</b> keyword");
    }
    QString rotation_amount = getDelimitedId(r_decl);
    QString rotation_unit = getDelimitedId(r_decl);

    QSharedPointer<Rotation> result;
    result.reset(new Rotation(codeline));
    result->degree = temporaryFunction(rotation_amount);
    result->what = rotate_what;

    if(rotation_unit != "degrees" && rotation_unit != "radians" && !rotation_unit.isEmpty())
    {
        throw syntax_error_exception("Rotation unit must be either degree or radians (default)");
    }
    result->unit = rotation_unit;

    QString around_kw = getDelimitedId(r_decl);
    QString nextWord = getDelimitedId(r_decl);
    if(nextWord == "point")
    {
        nextWord = getDelimitedId(r_decl);
        if(nextWord != "at")
        {
            throw syntax_error_exception("Invalid reference: %s (missing at keyword)", codeline.toStdString().c_str());
        }

        if(!r_decl.isEmpty())
        {
            if(r_decl[0] == '(') // around a specific point
            {
                // skip paren
                r_decl = r_decl.mid(1);
                QString px = extract_proper_expression(r_decl, {','});
                QString py = extract_proper_expression(r_decl, {')'});

                result->aroundX = px;
                result->aroundY = py;
            }
        }
    }
    return result;
}


QSharedPointer<Statement> RuntimeProvider::createPlot(const QString& codeline)
{
    QSharedPointer<Plot> plotData;
    plotData.reset(new Plot(codeline));

    QString plot_body = codeline.mid(STR_PLOT.length());
    // function name
    QString funToPlot;
    while(!plot_body.isEmpty() && !plot_body[0].isSpace())
    {
        funToPlot += plot_body[0];
        plot_body = plot_body.mid(1);
    }
    plotData->plotTarget = funToPlot;

    plot_body = plot_body.mid(1);
    // over keyword
    if(plot_body.startsWith(Keywords::KW_OVER))
    {
        resolveOverKeyword(plot_body, plotData);
    }

    return plotData;
}

QSharedPointer<Assignment> RuntimeProvider::providePointsOfDefinition(const QString& codeline, QString assignment_body, const QString& varName, const QString& targetProperties)
{
    if(!assignment_body.startsWith(STR_OF))
    {
        throw syntax_error_exception("Invalid assignment: %s (missing of keyword)", codeline.toStdString().c_str());
    }

    // skipping the of keyword
    assignment_body = assignment_body.mid(STR_OF.length());
    consumeSpace(assignment_body);

    QSharedPointer<PointsOfObjectAssignment> assignmentData;
    assignmentData.reset(new PointsOfObjectAssignment(codeline));

    assignmentData->varName = varName;
    assignmentData->targetProperties = targetProperties;

    // what object do we want the points of
    assignmentData->ofWhat = getDelimitedId(assignment_body);
    QSharedPointer<Assignment> assignment;

    if(!getFunction(assignmentData->ofWhat, assignment))
    {
        throw syntax_error_exception("Invalid assignment: %s. No such function: %s", codeline.toStdString().c_str(), assignmentData->ofWhat.toStdString().c_str());
    }

    // over keyword
    if(assignment_body.startsWith(Keywords::KW_OVER))
    {
        resolveOverKeyword(assignment_body, assignmentData);
    }

    return assignmentData;
}

QSharedPointer<Statement> RuntimeProvider::resolveObjectAssignment(const QString &codeline)
{


    // let keyword
    QString assignment_body = codeline.mid(STR_LET.length());
    consumeSpace(assignment_body);

    // variable
    QString varName = getDelimitedId(assignment_body);

    // assignment operator
    if(assignment_body[0] != '=')
    {
        throw syntax_error_exception("Invalid assignment: %s (missing assignment operator)", codeline.toStdString().c_str());
    }

    // skip "= "
    assignment_body = assignment_body.mid(1);
    consumeSpace(assignment_body);

    // what feature do we want to assign to
    QString targetProperties = getDelimitedId(assignment_body);

    // if it's not points, for the moment it is an arythmetic calculation
    if(targetProperties != "points")
    {
        if(targetProperties == "point") // or just a simple point
        {
            QString nextWord = getDelimitedId(assignment_body);
            if(nextWord != "at")
            {
                throw syntax_error_exception("Invalid point assignment: %s (missing at keyword)", codeline.toStdString().c_str());
            }
            QString px = getDelimitedId(assignment_body, {','});
            QString py = getDelimitedId(assignment_body);

            QSharedPointer<PointDefinitionAssignment> assignmentData;
            assignmentData.reset(new PointDefinitionAssignment(codeline));

            assignmentData->x = temporaryFunction(px);
            assignmentData->y = temporaryFunction(py);
            assignmentData->varName = varName;
            assignments.append(assignmentData);

            return assignmentData;
        }
        else
        {
            QSharedPointer<ArythmeticAssignment> assignmentData;
            assignmentData.reset(new ArythmeticAssignment(codeline));

            QString expression = targetProperties;
            expression += assignment_body;
            assignmentData->arythmetic = temporaryFunction(expression);
            assignments.append(assignmentData);

            assignmentData->varName = varName;
            assignments.append(assignmentData);

            return assignmentData;

        }
    }

    // here we are sure we want the points of a plot

    // of keyword
    QSharedPointer<Assignment> assignmentData = providePointsOfDefinition(codeline, assignment_body, varName, targetProperties);
    assignments.append(assignmentData);
    return assignmentData;
}


void RuntimeProvider::resolveOverKeyword(QString codeline, QSharedPointer<Stepped> stepped)
{
    codeline = codeline.mid(Keywords::KW_OVER.length());
    consumeSpace(codeline);
    bool interval = false;
    char close_char = 0;
    if(!codeline.startsWith("(") || !codeline.startsWith("["))
    {
        if (!codeline.isEmpty())
        {
            close_char = codeline[0].toLatin1() == '(' ? ')' : ']';
            codeline = codeline.mid(1);
            interval = true;
        }
    }

    if (close_char == 0)
    {
        return;
    }

    // first parameter
    char delim;
    QString first_par = getDelimitedId(codeline, {',', ' '}, delim);
    QString second_par = getDelimitedId(codeline, {' ', close_char}, delim);

    if(codeline.startsWith(Keywords::KW_CONTINUOUS))
    {
        stepped->continuous = true;
        codeline = codeline.mid(Keywords::KW_CONTINUOUS.length());
        consumeSpace(codeline);
    };

    if (codeline.startsWith(STR_STEP))
    {
        codeline = codeline.mid(STR_STEP.length());
        stepped->step = temporaryFunction(codeline);
    }
    else
        if (codeline.startsWith(STR_COUNTS))
        {
            codeline = codeline.mid(STR_COUNTS.length());
            QString strPointCount = getDelimitedId(codeline);
            double stp = strPointCount.toDouble();
            stepped->step = temporaryFunction(strPointCount);
            stepped->counted = true;

            if(codeline == Keywords::KW_SEGMENTS)
            {
                stepped->step = temporaryFunction(strPointCount + " + 1");
                stepped->continuous = true;
            }
        }
        else
            if(!codeline.isEmpty())
            {
                throw syntax_error_exception("Invalid keyword:", codeline);
            }
    stepped->start = temporaryFunction(first_par);
    stepped->end = temporaryFunction(second_par);

}

QVector<QSharedPointer<Assignment> >& RuntimeProvider::get_assignments()
{
    return assignments;
}

std::vector<std::string> RuntimeProvider::get_builtin_functions() const
{
    std::vector<std::string> functions;

    for(const auto& fds : supported_functions)
    {
        functions.push_back(fds.name);
    }

    return functions;
}

std::vector<std::string> RuntimeProvider::get_functions() const
{
    std::vector<std::string> functions;

    for(const auto& fds : this->functions)
    {
        functions.push_back(fds.data()->f->get_name());
    }

    return functions;
}

std::vector<std::string> RuntimeProvider::get_variables() const
{
    std::vector<std::string> functions;

    for(const auto& fds : this->assignments)
    {
        functions.push_back(fds->varName.toStdString());
    }

    return functions;

}
