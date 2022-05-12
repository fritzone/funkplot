#include "RuntimeProvider.h"
#include "constants.h"
#include "util.h"

const QString STR_FUNCTION = Keywords::KW_FUNCTION + " ";    // function f(x) = x * 2 + 5
const QString STR_PLOT = Keywords::KW_PLOT + " ";            // plot f over (-2, 2) [continuous]
const QString STR_OVER = Keywords::KW_OVER + " ";            // plot f over (-2, 2) [continuous]
const QString STR_LET = Keywords::KW_LET + " ";              // let something = points of f over (-2, 2) [continuous|step<cnt|0.01>]
const QString STR_OF  = Keywords::KW_OF + " ";               // let something = points of f over (-2, 2) [continuous|step<cnt|0.01>]
const QString STR_STEP = Keywords::KW_STEP + " ";            // plot f over (-2, 2) [continuous|step<cnt|0.01>]
const QString STR_COUNTS = Keywords::KW_COUNTS + " ";
const QString STR_ROTATE = Keywords::KW_ROTATE + " ";
const QString STR_SET = Keywords::KW_SET + " ";              // set color red
const QString STR_IN = Keywords::KW_IN + " ";                // for p in something do ... done
const QString STR_TO = Keywords::KW_TO + " ";                // for i = 0 to 256 step 1 do

const QString STR_DEFAULT_FOR_STEP = QString::number(DEFAULT_FOR_STEP);
const QString DTR_DEFAULT_RANGE_STEP = QString::number(DEFAULT_RANGE_STEP, 'f', 2 );


RuntimeProvider::RuntimeProvider(CB_ErrorReporter erp, CB_PointDrawer pd, CB_StatementTracker str, CB_PenSetter ps, CB_PlotDrawer pld) :
    m_vars(), m_errorReporter(erp), m_pointDrawer(pd), m_statementTracker(str), m_penSetter(ps), m_plotDrawer(pld)
{

}

int RuntimeProvider::defd(const std::string &s, Assignment*& assg)
{
    if(m_vars.contains(QString::fromStdString(s)))
    {
        return 1;
    }

    auto a = get_assignment(QString::fromStdString(s));
    if(a)
    {
        assg = a.get();
    }

    return 0;
}

double RuntimeProvider::value(const std::string &s)
{
    Assignment* assig = nullptr;
    if(defd(s, assig))
    {
        return m_vars.value(QString::fromStdString(s));
    }

    return std::numeric_limits<double>::quiet_NaN();
}

double RuntimeProvider::value(const std::string &obj, const std::string &attr)
{
    auto a = get_assignment(QString::fromStdString(obj));

    auto fcp = a->fullCoordProvider(this);
    if( std::get<0>(fcp) && std::get<1>(fcp) )
    {
        IndexedAccess* ia = nullptr; Assignment* a = nullptr;
        if(attr == "x") return std::get<0>(fcp)->Calculate(this, ia, a);
        if(attr == "y") return std::get<1>(fcp)->Calculate(this, ia, a);
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

    // zero: the builtin functions
    auto it_bui = std::find_if(supported_functions.begin(), supported_functions.end(), [&name](fun_desc_solve fds) -> bool {
        return fds.name == name.toStdString();
    });

    if(it_bui != supported_functions.end())
    {
        return temporaryFunction( QString::fromStdString( (*it_bui).name + "($)"), nullptr);
    }

    // first, the user defined functions
    auto it = std::find_if(functions.begin(), functions.end(), [&name](QSharedPointer<FunctionDefinition> f) -> bool {
        return f->f->get_name() == name.toStdString();
    });

    if(it != functions.end())
    {
        result = ((*it)->f);
    }

    // if not found, then maybe an assignments' functions
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
            m_errorReporter(m_codelinesSize - m_codelines.size(), err);
        }
    }
}

bool RuntimeProvider::resolveAsPoint(QSharedPointer<Plot> plot)
{
    for(const auto& adef : qAsConst(assignments))
    {
        if(plot->plotTarget == adef->varName || plot->plotTarget + ":" == adef->varName)
        {
            auto fcp = adef->fullCoordProvider(this);
            if( std::get<0>(fcp) && std::get<1>(fcp) )
            {
                IndexedAccess* ia = nullptr; Assignment* a = nullptr;
                double x = std::get<0>(fcp)->Calculate(this, ia, a);
                double y = std::get<1>(fcp)->Calculate(this, ia, a);
                m_pointDrawer(x, y);
                return true;
            }
        }
    }
    return false;
}

bool RuntimeProvider::resolveAsIndexedPlotDrawing(QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment)
{
    // try it as an indexed, hopefully that will work
    auto tf = temporaryFunction(plot->plotTarget, assignment.get());
    IndexedAccess* ia = nullptr; Assignment* a = nullptr;
    double v = tf->Calculate(this, ia, a);

    if(v == std::numeric_limits<double>::quiet_NaN() && !ia)
    {
        reportError("Invalid data to plot: " + plot->plotTarget);
    }
    else
    if(ia)
    {
        // let's see if this is an assignment which was indexed
        QSharedPointer<Function> funToUse = getFunction(ia->indexedVariable, assignment);
        if(!funToUse && !assignment)
        {
            reportError("Invalid data to plot: " + plot->plotTarget);
            return false;
        }
        else
        {
            assignment->resolvePrecalculatedPointsForIndexedAccess(plot, funToUse, this);
            // fetch the point with the given index
            if(ia->index < assignment->precalculatedPoints.size())
            {
                double x = assignment->precalculatedPoints[ia->index].x();
                double y = assignment->precalculatedPoints[ia->index].y();

                m_pointDrawer(x, y);
            }
            else
            {
                reportError("Index out of bounds for " + plot->plotTarget + ". Found:" + QString::number(assignment->precalculatedPoints.size() - 1) + " points, requested: " + QString::number(ia->index) );
                return false;
            }
        }
    }
    else
    {
        reportError("Invalid data to plot: " + plot->plotTarget);
        return false;
    }

    return true;
}

void RuntimeProvider::reset()
{
    plots.clear();
    functions.clear();
    assignments.clear();
    m_setts.clear();
    statements.clear();
    m_vars.clear();
    m_allVariables.clear();
    m_penSetter(0, 0, 0, 1);
}

void RuntimeProvider::parse(const QStringList& codelines)
{
    m_codelines = codelines;

    while(m_codelines.last().isEmpty())
    {
        m_codelines.removeLast();
    }
    m_codelinesSize = m_codelines.size();

    while(!m_codelines.empty())
    {
        resolveCodeline(m_codelines, statements, nullptr);
    }

}

void RuntimeProvider::addOrUpdateAssignment(QSharedPointer<Assignment> a)
{
    auto pa = get_assignment(a->varName);
    if(pa)
    {
        if(pa.dynamicCast<PointDefinitionAssignment>())
        {
            pa.dynamicCast<PointDefinitionAssignment>()->x = a.dynamicCast<PointDefinitionAssignment>()->x;
            pa.dynamicCast<PointDefinitionAssignment>()->y = a.dynamicCast<PointDefinitionAssignment>()->y;
        }
    }
    else
    {
        assignments.append(a);
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

QString RuntimeProvider::typeOfVariable(const char *n)
{
    // this is a numeric type variable, held in m_Vars
    if(m_vars.contains(n))
    {
        return "n";
    }

    // this is a point/array assiogned variable, held in the assignments
    for(const auto& adef : qAsConst(assignments))
    {
        if(adef->varName == n && !m_allVariables.contains(n))
        {
            return "p";
        }
    }

    // this variable was declared, but not assigned to yet
    if(m_allVariables.contains(n))
    {
        return m_allVariables[n];
    }

    return "x";
}

QString RuntimeProvider::typeOfVariable(const QString &s)
{
    return typeOfVariable(s.toLocal8Bit().data());
}

double RuntimeProvider::getIndexedVariableValue(const char *n, int index)
{
    return -1;
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
                                          bool& continuous, double& plotStart, double& plotEnd, bool& counted, double &stepValue, int& count, bool useDefaultValues)
{
    QSharedPointer<Function> stepFun = nullptr;

    if(assignment)
    {
        if(!plot->start)
        {
            if(! assignment->startValueProvider())
            {
                if(!useDefaultValues)
                {
                    reportError("Invalid plotting interval for <b>" + assignment->varName + "</b>. There is no clear start value defined for it.");
                    return;
                }
            }
            else
            {
                IndexedAccess* ia = nullptr; Assignment* a = nullptr;
                plotStart = assignment->startValueProvider()->Calculate(this, ia, a);
            }
        }
        else
        {
            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            plotStart = plot->start->Calculate(this, ia, a);
        }

        if(!plot->end)
        {
            if(! assignment->endValueProvider())
            {
                if(!useDefaultValues)
                {
                    reportError("Invalid plotting interval. There is no clear end value defined for it.");
                    return;
                }
            }
            else
            {
                IndexedAccess* ia = nullptr; Assignment* a = nullptr;
                plotEnd = assignment->endValueProvider()->Calculate(this, ia, a);
            }
        }
        else
        {
            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            plotEnd = plot->end->Calculate(this, ia, a);
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
            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            plotStart = plot->start->Calculate(this, ia, a);
        }

        if(plot->end)
        {
            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            plotEnd = plot->end->Calculate(this, ia, a);
        }
    }

    if(stepFun)
    {
        IndexedAccess* ia = nullptr; Assignment* a = nullptr;
        count = stepFun->Calculate(this, ia, a);
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
    codeline.replace("(", " ( ");
    codeline.replace(")", " ) ");
    codeline = codeline.trimmed();
//    codeline = codeline.simplified();
    if(!codeline.isEmpty())
    try
    {

        QSharedPointer<Statement> createdStatement;

        if(codeline.startsWith(Keywords::KW_VAR))
        {
            createVariableDeclaration(codeline);
        }
        else
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
            statements.append(createdStatement = createAssignment(codeline));
        }
        else
        if(codeline.startsWith(STR_SET)) // setting the color, line width, rotation, etc ...
        {
            statements.append(createdStatement = createSet(codeline));
        }
        else
        if(codeline.startsWith(Keywords::KW_FOR)) // looping over a set of points or something else
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

void RuntimeProvider::createVariableDeclaration(const QString &codeline)
{
    // variable declarations are like: var
    QString vd_body = codeline.mid(Keywords::KW_VAR.length());
    consumeSpace(vd_body);
    QStringList allDeclarations = vd_body.split(",", Qt::SkipEmptyParts);
    for(const auto &s : allDeclarations)
    {
        QStringList declI = s.split(" ", Qt::SkipEmptyParts);
        if(declI.size() < 2)
        {
            throw syntax_error_exception("Invalid variable declaration: <b>%s</b>", s.toStdString().c_str());
        }

        QString type = declI.last();

        if(!Types::all().contains(type))
        {
            throw syntax_error_exception("Invalid variable type <b>%s</b> in <b>%s", type, s);
        }

        declI.removeLast();

        for(const auto& vn : declI)
        {
            m_allVariables.insert(vn, type);
        }
    }
}



QSharedPointer<Statement> RuntimeProvider::createFunction(const QString &codeline)
{
    QString f_body = codeline.mid(STR_FUNCTION.length());
    QSharedPointer<FunctionDefinition> fd;
    fd.reset(new FunctionDefinition(codeline) );
    Function* f = new Function(f_body.toLocal8Bit().data(), fd.get());

    fd->f = QSharedPointer<Function>(f);
    functions.push_back(fd);

    return fd;
}

QSharedPointer<Statement> RuntimeProvider::createSet(const QString &codeline)
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
    QSharedPointer<Set> sett(new Set(codeline));
    sett->what = what;
    sett->value = to_what;
    return sett;
}

QSharedPointer<Statement> RuntimeProvider::createLoop(const QString &codeline, QStringList &codelines)
{
    QString l_decl = codeline.mid(Keywords::KW_FOR.length());
    consumeSpace(l_decl);
    QVector<QSharedPointer<Statement>> loop_body;

    // the loop object, regardless if its an in loop or assignment loop
    QSharedPointer<Loop> result = nullptr;
    result.reset(new Loop(codeline));

    // the loop variable
    QString loop_variable = extract_proper_expression(l_decl);

    if(m_allVariables.count(loop_variable) == 0)
    {
        throw syntax_error_exception("Undeclared variable %s cannot be used in for loop (%s)", loop_variable.toStdString().c_str(), codeline.toStdString().c_str());
    }

    result->loop_variable = loop_variable;

    consumeSpace(l_decl);

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

        if(loop_over == Keywords::KW_RANGE)
        {
            // see if it starts with parenthesis or not

            bool needs_closing_paren = false;
            if(!l_decl.isEmpty() && l_decl[0] == '(')
            {
                needs_closing_paren = true;
                // skip openin parenthesis
                l_decl = l_decl.mid(1);
                consumeSpace(l_decl);
            }

            auto ritl = new RangeIteratorLoopTarget(result);
            result->loop_target.reset(ritl);


            // get the range start functions
            QString range_start = extract_proper_expression(l_decl, {','});
            consumeSpace(l_decl);

            if(!l_decl.isEmpty() && l_decl[0] == ',')
            {
                l_decl = l_decl.mid(1);
                consumeSpace(l_decl);
            }

            ritl->startFun = temporaryFunction(range_start, result.get());

            // range end
            QString range_end = extract_proper_expression(l_decl, (needs_closing_paren ? QSet{QChar(')')} : QSet{QChar(' ')}) );
            ritl->endFun = temporaryFunction(range_end, result.get());

            if(!l_decl.isEmpty() && l_decl[0] == ')' && needs_closing_paren)
            {
                l_decl = l_decl.mid(1);
            }

            consumeSpace(l_decl);
            // do we have a step
            if(l_decl.startsWith(Keywords::KW_STEP))
            {
                l_decl = l_decl.mid(STR_STEP.length());
                consumeSpace(l_decl);

                QString step = extract_proper_expression(l_decl);
                ritl->stepFun = temporaryFunction(step, result.get());

                consumeSpace(l_decl);
            }
            else
            {
                ritl->stepFun = temporaryFunction(DEFAULT_RANGE_STEP, result.get());
            }
        }
        else
        {
            result->loop_target.reset(new FunctionIteratorLoopTarget(result));
            result->loop_target->name = loop_over;
        }



    }
    else
    if(l_decl.startsWith("=")) // the classical loop: for i = 0 to 256 step 1 do
    {
        // skip the equal sign
        l_decl = l_decl.mid(1);
        consumeSpace(l_decl);

        // the loop target of this is a range iterator, since this loop is a stepped range
        auto ritl = new RangeIteratorLoopTarget(result);
        result->loop_target.reset(ritl);

        // the start:
        QString range_start = extract_proper_expression(l_decl);
        ritl->startFun = temporaryFunction(range_start, result.get());

        if(!l_decl.startsWith(Keywords::KW_TO))
        {
            throw syntax_error_exception("for statement with direct assignment does not contain the to keyword");
        }

        l_decl = l_decl.mid(STR_TO.length());
        consumeSpace(l_decl);

        // range end
        QString range_end = extract_proper_expression(l_decl);
        ritl->endFun = temporaryFunction(range_end, result.get());

        consumeSpace(l_decl);
        // do we have a step
        if(l_decl.startsWith(Keywords::KW_STEP))
        {
            l_decl = l_decl.mid(STR_STEP.length());
            consumeSpace(l_decl);

            QString step = extract_proper_expression(l_decl);
            ritl->stepFun = temporaryFunction(step, result.get());

            consumeSpace(l_decl);
        }
        else
        {
            ritl->stepFun = temporaryFunction(DEFAULT_FOR_STEP, result.get());
        }


    }
    else
    {
        throw syntax_error_exception("invalid for statement");
    }

    if(!l_decl.startsWith(Keywords::KW_DO))
    {
        throw syntax_error_exception("for statement does not contain the do keyword");
    }

    bool done = false;
    while(!codelines.isEmpty() && !done)
    {
        QSharedPointer<Statement> st = nullptr;
        qDebug() << "try" << codelines.at(0);

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
        throw syntax_error_exception("for body does not end with done");
    }

    return result;

}

QSharedPointer<Statement> RuntimeProvider::createRotation(const QString &codeline, QStringList &codelines)
{
    QString r_decl = codeline.mid(STR_ROTATE.length());
    QString rotate_what = getDelimitedId(r_decl);

    if(m_allVariables.count(rotate_what) == 0)
    {
        throw syntax_error_exception("rotate statement tries to rotate undeclared variable");
    }

    QString with_keyword = getDelimitedId(r_decl);
    if(with_keyword != "with")
    {
        throw syntax_error_exception("rotate statement does not contain <b>with</b> keyword");
    }
    QString rotation_amount = extract_proper_expression(r_decl, {' '}, QSet<QString>{QString("degrees"), QString("radians"), "around"} );
    QString rotation_unit = getDelimitedId(r_decl);

    QSharedPointer<Rotation> result;
    result.reset(new Rotation(codeline));
    result->degree = temporaryFunction(rotation_amount, result.get());
    result->what = rotate_what;

    if(rotation_unit != "degrees" && rotation_unit != "radians" && !rotation_unit.isEmpty())
    {
        throw syntax_error_exception("Rotation unit must be either degree or radians (default)");
    }
    result->unit = rotation_unit;

    QString around_kw = getDelimitedId(r_decl);
    if(!around_kw.isEmpty() && around_kw != Keywords::KW_AROUND)
    {
        throw syntax_error_exception("Unknown keyword in rotation: " , around_kw);
    }
    QString nextWord = getDelimitedId(r_decl);
    if(nextWord == "point")
    {
        nextWord = getDelimitedId(r_decl);
        if(nextWord != "at")
        {
            throw syntax_error_exception("Invalid reference: %s (missing <b><pre>at</pre></b> keyword)", codeline.toStdString().c_str());
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
    QString funToPlot = extract_proper_expression(plot_body, {' '}, {Keywords::KW_OVER, Keywords::KW_CONTINUOUS}, true);

    // let's see if we have a function for this already
    QSharedPointer<Function> ff = get_function(funToPlot);

    if(!ff)
    {
        // or maybe an assignment
        auto a = get_assignment(funToPlot);
        if(!a)
        {
            // let's see if there is a variable defined with this name, which was not assigned yet. That's an error
            if(Types::all().contains(typeOfVariable(funToPlot)))
            {
                throw syntax_error_exception("Invalid plot: <b>%s</b> was declared, but not defined to hold a value", funToPlot.toStdString().c_str());
            }

            // let's see if we are plotting a single point of a series of plots: plot ps[2]

            IndexedAccess* ia_m = nullptr; Assignment* a = nullptr;
            auto tempFun = temporaryFunction(funToPlot, plotData.get());
            double v = tempFun->Calculate(this, ia_m, a);
            // if this is an indexed something ... let's hope down there someone will take care of it :)
            if(!ia_m)
            {

                QSharedPointer<FunctionDefinition> fd;
                fd.reset(new FunctionDefinition(codeline) );
                QString plgfn = "plot_fn_" +QString::number(functions.size()) + "(x) =";

                // make the funToPlot look acceptable by adding (x) to each builtin function if any
                QString funToPlotFinal = funToPlot.simplified();
                // firstly remove the space before each parenthesis if found
                funToPlotFinal.replace(" (", "(");
                // then each func(x) will be replaced with func to not to have (x)(x)
                for(const auto& f : supported_functions)
                {
                    if(f.standalone_plottable)
                    {
                        funToPlotFinal.replace(QString::fromStdString(f.name) + "(x)", QString::fromStdString(f.name));
                    }
                }

                for(const auto& f : supported_functions)
                {
                    if(f.standalone_plottable)
                    {
                        funToPlotFinal.replace(QString::fromStdString(f.name) + "(", QString::fromStdString(f.name) + "#");
                    }
                }

                // and the other way around to make it legal
                for(const auto& f : supported_functions)
                {
                    if(f.standalone_plottable)
                    {
                        funToPlotFinal.replace(QString::fromStdString(f.name), QString::fromStdString(f.name) + "(x)");
                    }
                }
                funToPlotFinal.replace("(x)#", "(");


                qDebug() << funToPlotFinal;

                Function* f = new Function(QString(plgfn + funToPlotFinal).toStdString().c_str(), plotData.get());

                fd->f = QSharedPointer<Function>(f);
                funToPlot = QString::fromStdString(fd->f->get_name());
                functions.push_back(fd);
            }
            else
            {
                delete ia_m;
            }
        }
        else
        {
            if(typeOfVariable(funToPlot) == "n")
            {
                throw syntax_error_exception("Invalid plot: <b>%s</b> was declared to be <b>numeric</b>, but it is impossible to plot a number", funToPlot.toStdString().c_str());
            }
        }
    }

    plotData->plotTarget = funToPlot;
    consumeSpace(plot_body);

    // over keyword
    if(plot_body.startsWith(Keywords::KW_OVER))
    {
        resolveOverKeyword(plot_body, plotData, plotData.get());
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

    QSharedPointer<PointsOfObjectAssignment> result;
    result.reset(new PointsOfObjectAssignment(codeline));

    result->varName = varName;
    result->targetProperties = targetProperties;

    // what object do we want the points of
    result->ofWhat = getDelimitedId(assignment_body);
    QSharedPointer<Assignment> assignment;

    if(!getFunction(result->ofWhat, assignment))
    {
        throw syntax_error_exception("Invalid assignment: %s. No such function: %s", codeline.toStdString().c_str(), result->ofWhat.toStdString().c_str());
    }

    // over keyword
    if(assignment_body.startsWith(Keywords::KW_OVER))
    {
        resolveOverKeyword(assignment_body, result, result.get());
    }

    if (assignment_body.startsWith(STR_COUNTS))
    {
        assignment_body = assignment_body.mid(STR_COUNTS.length());
        QString strPointCount = getDelimitedId(assignment_body);
        double stp = strPointCount.toDouble();
        result->step = temporaryFunction(strPointCount, result.get());
        result->counted = true;

        if(codeline == Keywords::KW_SEGMENTS)
        {
            result->step = temporaryFunction(strPointCount + " + 1", result.get());
            result->continuous = true;
        }
    }
    return result;
}

QSharedPointer<Statement> RuntimeProvider::createAssignment(const QString &codeline)
{
    // let keyword
    QString assignment_body = codeline.mid(STR_LET.length());
    consumeSpace(assignment_body);

    // variable
    QString varName = getDelimitedId(assignment_body);

    if(m_allVariables.count(varName) == 0)
    {
        throw syntax_error_exception("Invalid assignment: <b>%s</b> (variable <b>%s</b> was not declared)", codeline.toStdString().c_str(), varName.toStdString().c_str());
    }

    // assignment operator
    if(!assignment_body.isEmpty() && assignment_body[0] != '=')
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
            if(typeOfVariable(varName) != "p" && typeOfVariable(varName) != "point")
            {
                throw syntax_error_exception("Conflicting type assignment: <b>point</b> assigned to a non point type variable: <b>%s (%s)</b>", varName.toStdString().c_str(), typeOfVariable(varName).toStdString().c_str());
            }

            QString nextWord = getDelimitedId(assignment_body);
            if(nextWord != "at")
            {
                QSharedPointer<PointDefinitionAssignmentToOtherPoint> result;
                result.reset(new PointDefinitionAssignmentToOtherPoint(codeline));
                result->otherPoint = nextWord;
                result->varName = varName;

                assignments.append(result);

                return result;

//                throw syntax_error_exception("Invalid point assignment: %s (missing at keyword)", codeline.toStdString().c_str());
            }
            else
            {

                consumeSpace(assignment_body);

                if(!assignment_body.isEmpty() && assignment_body[0] == '(')
                {
                    // skip (
                    assignment_body = assignment_body.mid(1);
                }

                QString px = extract_proper_expression(assignment_body, {','});
                QString py = extract_proper_expression(assignment_body , {')'});

                QSharedPointer<PointDefinitionAssignment> result;
                result.reset(new PointDefinitionAssignment(codeline));

                result->x = temporaryFunction(px, result.get());
                result->y = temporaryFunction(py, result.get());
                result->varName = varName;
                assignments.append(result);

                return result;
            }
        }
        else
        {

            if(typeOfVariable(varName) == "p" || typeOfVariable(varName) == "point")
            {
                QSharedPointer<PointDefinitionAssignmentToOtherPoint> result;
                result.reset(new PointDefinitionAssignmentToOtherPoint(codeline));
                result->otherPoint = targetProperties; // yeah, stupid
                result->varName = varName;

                assignments.append(result);

                return result;
            }
            else
            {

                if(typeOfVariable(varName) != "n" && typeOfVariable(varName) != "numeric")
                {
                    throw syntax_error_exception("Invalid assignment: <b>%s</b> (<b>arythmetic expression</b> assigned to <b>%s</b>)", varName.toStdString().c_str(), typeOfVariable(varName).toStdString().c_str());
                }

                QSharedPointer<ArythmeticAssignment> result;
                result.reset(new ArythmeticAssignment(codeline));

                QString expression = targetProperties;
                expression += assignment_body;
                result->arythmetic = temporaryFunction(expression, result.get());
                assignments.append(result);

                result->varName = varName;
                assignments.append(result);

                return result;
            }

        }
    }

    // here we are sure we want the points of a plot, variable type should be list
    if(typeOfVariable(varName) != "l" && typeOfVariable(varName) != "array")
    {
        throw syntax_error_exception("Invalid assignment: <b>%s</b> (<b>array</b> assigned to <b>%s</b>)", varName.toStdString().c_str(), typeOfVariable(varName).toStdString().c_str());
    }
    QSharedPointer<Assignment> assignmentData = providePointsOfDefinition(codeline, assignment_body, varName, targetProperties);
    assignments.append(assignmentData);
    return assignmentData;
}


void RuntimeProvider::resolveOverKeyword(QString codeline, QSharedPointer<Stepped> stepped, Statement* s)
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

    // skipping , or anything else
    while(!codeline.isEmpty() && (codeline.at(0) == ' ' || codeline.at(0) == ','))
    {
        codeline = codeline.mid(1);
    }
    QString second_par = getDelimitedId(codeline, {' ', close_char}, delim);
    // skipping , or anything else
    while(!codeline.isEmpty() && (codeline.at(0) == ' ' || codeline.at(0) == close_char) )
    {
        codeline = codeline.mid(1);
    }
    if(codeline.startsWith(Keywords::KW_CONTINUOUS))
    {
        stepped->continuous = true;
        codeline = codeline.mid(Keywords::KW_CONTINUOUS.length());
        consumeSpace(codeline);
    };

    if (codeline.startsWith(STR_STEP))
    {
        codeline = codeline.mid(STR_STEP.length());
        stepped->step = temporaryFunction(codeline, s);
    }
    else
        if (codeline.startsWith(STR_COUNTS))
        {
            codeline = codeline.mid(STR_COUNTS.length());
            QString strPointCount = getDelimitedId(codeline);
            double stp = strPointCount.toDouble();
            stepped->step = temporaryFunction(strPointCount, s);
            stepped->counted = true;

            if(codeline == Keywords::KW_SEGMENTS)
            {
                stepped->step = temporaryFunction(strPointCount + " + 1", s);
                stepped->continuous = true;
            }
        }
        else
            if(!codeline.isEmpty())
            {
                throw syntax_error_exception("Invalid keyword:", codeline);
            }
    stepped->start = temporaryFunction(first_par, s);
    stepped->end = temporaryFunction(second_par, s);

}

QVector<QSharedPointer<Assignment> >& RuntimeProvider::get_assignments()
{
    return assignments;
}

QSharedPointer<Assignment> RuntimeProvider::get_assignment(const QString &n)
{
    for(auto& fds : this->assignments)
    {
        if(fds->varName == n)
        {
            return fds;
        }
    }

    return nullptr;
}

QSharedPointer<Function> RuntimeProvider::get_function(const QString &n)
{
    for(const auto& fds : this->functions)
    {
        if(fds.data()->f->get_name() == n.toStdString())
        {
            return fds.data()->f;
        }
    }

    return nullptr;

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
    std::vector<std::string> variables;
    for(const auto& fds : this->assignments)
    {
        variables.push_back(fds->varName.toStdString());
    }

    return variables;

}

std::vector<std::string> RuntimeProvider::get_declared_variables() const
{
    auto defd = get_variables();
    std::vector<std::string> variables;

    for(const auto& s : m_allVariables.keys())
    {
        if(std::find(defd.begin(), defd.end(), s.toStdString()) == defd.end())
        {
            variables.push_back(s.toStdString());
        }
    }

    return variables;
}
