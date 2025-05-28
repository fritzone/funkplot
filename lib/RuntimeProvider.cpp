#include "RuntimeProvider.h"
#include "ArrayAssignment.h"
#include "Parametric.h"
#include "PointArrayAssignment.h"
#include "Plot.h"
#include "CodeEngine.h"
#include "FunctionDefinition.h"

#ifdef ENABLE_PYTHON
#include "PythonRunner.h"
#endif

#include "PointDefinitionAssignmentToOtherPoint.h"
#include "PointsOfObjectAssignment.h"
#include "constants.h"
#include "util.h"

#include <QString>

const QString STR_DEFAULT_FOR_STEP = QString::number(DEFAULT_FOR_STEP);
const QString DTR_DEFAULT_RANGE_STEP = QString::number(DEFAULT_RANGE_STEP, 'f', 2 );
RuntimeProvider* RuntimeProvider::m_instance = nullptr;

RuntimeProvider::RuntimeProvider(CB_ErrorReporter erp, CB_StringPrinter sp, CB_PointDrawer pd, CB_LineDrawer ld, CB_StatementTracker str, CB_PenSetter ps, CB_PlotDrawer pld) :
    m_numericVariables(), m_errorReporter(erp), m_pointDrawer(pd), m_lineDrawer(ld), m_statementTracker(str), m_penSetter(ps), m_plotDrawer(pld), m_stringPrinter(sp)
{
    m_instance = this;
    populateBuiltinFunctions();
}

RuntimeProvider::RuntimeProvider(std::tuple<CB_ErrorReporter, CB_StringPrinter, CB_PointDrawer, CB_LineDrawer, CB_StatementTracker, CB_PenSetter, CB_PlotDrawer> p) :
    RuntimeProvider(std::get<0>(p), std::get<1>(p), std::get<2>(p), std::get<3>(p), std::get<4>(p), std::get<5>(p), std::get<6>(p))
{
    m_instance = this;
    populateBuiltinFunctions();
}

RuntimeProvider *RuntimeProvider::get()
{
    return m_instance;
}

int RuntimeProvider::defd(const std::string &s, Assignment*& assg)
{
    if(m_numericVariables.contains(QString::fromStdString(s)))
    {
        return 1;
    }

    auto a = getAssignment(QString::fromStdString(s));
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
        return m_numericVariables.value(QString::fromStdString(s));
    }

    return std::numeric_limits<double>::quiet_NaN();
}

double RuntimeProvider::value(const std::string &obj, const std::string &attr)
{
    auto a = getAssignment(QString::fromStdString(obj));
    if(!a)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
    auto fcp = a->fullCoordProvider(this);
    if( std::get<0>(fcp) && std::get<1>(fcp) )
    {
        if(attr == X) return std::get<0>(fcp)->Calculate();
        if(attr == Y) return std::get<1>(fcp)->Calculate();
    }

    return std::numeric_limits<double>::quiet_NaN();
}

void RuntimeProvider::setValue(const QString &s, double v)
{
    m_numericVariables[s] = v;
}

void RuntimeProvider::setValue(const QString &s, QPointF v)
{
    m_points[s] = v;
}


QMap<QString, double> &RuntimeProvider::variables()
{
    return m_numericVariables;
}

QSharedPointer<Function> RuntimeProvider::getNameFunctionOrAssignment(const QString &name, QSharedPointer<Assignment>& assignment)
{
    QSharedPointer<Function> result;

    // zero: the builtin functions
    auto it_bui = std::find_if(supported_functions.begin(), supported_functions.end(), [&name](fun_desc_solve fds) -> bool {
        return fds.name == name.toStdString();
    });

    if(it_bui != supported_functions.end())
    {
        return Function::temporaryFunction( QString::fromStdString( (*it_bui).name + "($)"), nullptr);
    }

    // first, the user defined functions
    auto it = std::find_if(m_functions.begin(), m_functions.end(), [&name](QSharedPointer<FunctionDefinition> f) -> bool {
        return f->f->get_name() == name.toStdString();
    });

    if(it != m_functions.end())
    {
        result = ((*it)->f);
    }

    // if not found, then maybe an assignments' functions
    if(!result)
    {
        // second: or an assignment bound to a function
        for(const auto& adef : qAsConst(m_assignments))
        {
            if(adef->varName == name)
            {
                if(!adef->providedFunction().isEmpty()) // if this came from "points of function"
                {
                    result = getNameFunctionOrAssignment(adef->providedFunction(), assignment);
                    if(result)
                    {
                        assignment = adef;
                        break;
                    }

                    auto pf = getParametricFunction(adef->providedFunction());
                    if(pf)
                    {
                        assignment = adef;
                        break;
                    }
                }
                else // just a plian list of points
                {
                    assignment = adef;
                    break;
                }
            }
        }
    }

    return result;
}

void RuntimeProvider::reportError(int errLine, int errorCode, const QString &err)
{
    if(m_errorReporter)
    {
        if(m_shouldReport)
        {
            m_errorReporter(errLine, errorCode, err);
        }
    }
}

void RuntimeProvider::printString(const QString &s)
{
    if(m_stringPrinter)
    {
        m_stringPrinter(s);
    }
}

bool RuntimeProvider::resolveAsPoint(QSharedPointer<Plot> plot)
{
    for(const auto& adef : qAsConst(m_assignments))
    {
        if(plot->plotTarget == adef->varName || plot->plotTarget + ":" == adef->varName)
        {
            auto fcp = adef->fullCoordProvider(this);
            auto x_provider = std::get<0>(fcp);
            auto y_provider = std::get<1>(fcp);
            if( x_provider && y_provider )
            {
                double x = std::get<0>(fcp)->Calculate();
                double y = std::get<1>(fcp)->Calculate();

                if(plot->polarPlot)
                {
                    // need to convert the polar coordinate (x, y) to ccartesizan ones, that can be plotted

                    double carX = y * cos( x );
                    double carY = y * sin( x );

                    x = carX;
                    y = carY;
                }

                m_pointDrawer(x, y);

                // qDebug() << "Plotting:" << QPointF(x,y);
                return true;
            }
        }
    }
    return false;
}

bool RuntimeProvider::resolveAsIndexedPlotDrawing(QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment)
{
    // try it as an indexed, hopefully that will work
    auto tf = Function::temporaryFunction(plot->plotTarget, assignment.get());
    IndexedAccess* ia = nullptr; Assignment* a = nullptr;
    auto v = tf->Calculate(this, ia, a);

    if(!v.has_value() && !ia)
    {
        reportError(plot->lineNumber,  ERRORCODE(15), "Invalid data to plot: " + plot->plotTarget);
    }
    else
    if(ia)
    {
        // let's see if this is an assignment which was indexed
        QSharedPointer<Function> funToUse = getNameFunctionOrAssignment(ia->indexedVariable, assignment);
        if(!funToUse && !assignment)
        {
            reportError(plot->lineNumber, ERRORCODE(15), "Invalid data to plot: " + plot->plotTarget);
            return false;
        }
        else
        {
            if(funToUse)
            {
                assignment->resolvePrecalculatedPointsForIndexedAccessWithFunction(plot, funToUse, this);
                // fetch the point with the given index
                auto& pcp = assignment->getPrecalculatedPoints();
                if(ia->index < pcp.size())
                {
                    double x = pcp[ia->index].x();
                    double y = pcp[ia->index].y();

                    if(plot->polarPlot)
                    {
                        // need to convert the polar coordinate (x, y) to ccartesizan ones, that can be plotted

                        double carX = y * cos( x );
                        double carY = y * sin( x );

                        x = carX;
                        y = carY;
                    }

                    m_pointDrawer(x, y);
                }
                else
                {
                    reportError(plot->lineNumber,  ERRORCODE(55), "Index out of bounds for " + plot->plotTarget + ". Found:" + QString::number(assignment->getPrecalculatedPoints().size() - 1) + " points, requested: " + QString::number(ia->index) );
                    return false;
                }
            }
            else
            {
                assignment->resolvePrecalculatedPointsForIndexedAccessWithList(plot, this);
                // fetch the point with the given index
                auto& pcp = assignment->getPrecalculatedPoints();

                if(ia->index < pcp.size())
                {
                    double x = pcp[ia->index].x();
                    double y = pcp[ia->index].y();

                    if(plot->polarPlot)
                    {
                        // need to convert the polar coordinate (x, y) to ccartesizan ones, that can be plotted

                        double carX = y * cos( x );
                        double carY = y * sin( x );

                        x = carX;
                        y = carY;
                    }

                    m_pointDrawer(x, y);
                }
                else
                {
                    reportError(plot->lineNumber,  ERRORCODE(55), "Index out of bounds for " + plot->plotTarget + ". Found:" + QString::number(assignment->getPrecalculatedPoints().size() - 1) + " points, requested: " + QString::number(ia->index) );
                    return false;
                }
            }
        }
    }
    else
    {
        reportError(plot->lineNumber,  ERRORCODE(15), "Invalid data to plot: " + plot->plotTarget);
        return false;
    }

    return true;
}

bool RuntimeProvider::resolveAsPrecalculatedDrawing(QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment)
{
    bool first = true;
    double px = 0, py = 0; // previous x,y
    auto& pcp = assignment->getPrecalculatedPoints();
    if(plot->continuous && pcp.size() > 0)
    {
        px = pcp.at(0).x();
        py = pcp.at(0).y();
    }

    if(!assignment->precalculatedSetForce)
    {
        return false;
    }

    for(auto i = 0; i<pcp.size(); i++)
    {
        double x = pcp[i].x();
        double y = pcp[i].y();
        if(plot->polarPlot)
        {
            // need to convert the polar coordinate (x, y) to ccartesizan ones, that can be plotted

            double carX = y * cos( x );
            double carY = y * sin( x );

            x = carX;
            y = carY;
        }
        if(plot->continuous)
        {
            if(first)
            {
                px = x;
                py = y;
                first = false;
            }
            else
            {
                m_lineDrawer(px, py, x, y);
                px = x;
                py = y;
            }
        }
        else
        {
            m_pointDrawer(x, y);
        }
    }

    return true;
}

void RuntimeProvider::reset()
{
    m_plots.clear();
    m_functions.clear();
    m_parametricFunctions.clear();
    populateBuiltinFunctions();

    m_assignments.clear();
    m_setts.clear();
    m_statements.clear();
    m_numericVariables.clear();
    m_allVariables.clear();
    setPen(0, 0, 0, 255);
    m_penSetter(0, 0, 0, 255, 1);
    m_currentPalette = "";
    m_showCoordinates = true;
    m_ps = 1;
    m_r = 0; m_g = 0; m_b = 0; m_a = 255;

}

bool RuntimeProvider::parse(const QStringList& codelines)
{
    m_codelines = codelines;
    m_codelinesSize = m_codelines.size();
    m_codeEngine.reset(new CodeEngine(codelines, this));
    return m_codeEngine->parse();
}

void RuntimeProvider::addOrUpdateAssignment(QSharedPointer<Assignment> a)
{
    auto pa = getAssignment(a->varName);
    if(pa)
    {
        if(a.dynamicCast<PointDefinitionAssignment>())
        {
            auto pda = getAssignmentAs<PointDefinitionAssignment>(pa->varName);

            if(pda)
            {
                pda->x = a.dynamicCast<PointDefinitionAssignment>()->x;
                pda->y = a.dynamicCast<PointDefinitionAssignment>()->y;
            }

            auto pdatop = getAssignmentAs<PointDefinitionAssignmentToOtherPoint>(pa->varName);

            if(a.dynamicCast<PointDefinitionAssignmentToOtherPoint>())
            {
                if(pdatop)
                {
                    pdatop->otherPoint = a.dynamicCast<PointDefinitionAssignmentToOtherPoint>()->otherPoint;
                }

                else
                {
                    m_assignments.removeAll(pa); // we have a more specialized assignment to this point, remove the weaker
                    m_assignments.append(a);
                }
            }
        }
    }
    else
    {
        m_assignments.append(a);
    }
}

bool RuntimeProvider::shouldReport() const
{
    return m_shouldReport;
}

void RuntimeProvider::setShouldReport(bool newShouldReport)
{
    m_shouldReport = newShouldReport;
}

void RuntimeProvider::addOrUpdatePointDefinitionAssignment(int lineNumber, Statement* statement, double x, double y, const QString& varName)
{
    QSharedPointer<PointDefinitionAssignment> assignmentData;
    assignmentData.reset(new PointDefinitionAssignment(lineNumber, statement->statement));

    assignmentData->x = Function::temporaryFunction(QString::number(x, 'f', 6), statement);
    assignmentData->y = Function::temporaryFunction(QString::number(y, 'f', 6), statement);
    assignmentData->varName = varName;
    addOrUpdateAssignment(assignmentData);
}

QSharedPointer<PointDefinitionAssignment> RuntimeProvider::provideTemporaryPointDefinitionAssignment(int lineNumber, Statement *statement, double x, double y, const QString &varName)
{
    QSharedPointer<PointDefinitionAssignment> assignmentData;
    assignmentData.reset(new PointDefinitionAssignment(lineNumber, statement->statement));

    assignmentData->x = Function::temporaryFunction(QString::number(x, 'f', 6), statement);
    assignmentData->y = Function::temporaryFunction(QString::number(y, 'f', 6), statement);
    assignmentData->varName = varName;
    return assignmentData;
}

QString RuntimeProvider::typeOfVariable(const char *n)
{
    // this is a numeric type variable, held in m_Vars
    if(m_numericVariables.contains(n))
    {
        return Types::TYPE_NUMBER;
    }

    // this is a point/array assiogned variable, held in the assignments
    for(const auto& adef : qAsConst(m_assignments))
    {
        if(adef->varName == n && !m_allVariables.contains(n))
        {
            return Types::TYPE_POINT;
        }
    }

    // this variable was declared, but not assigned to yet
    if(m_allVariables.contains(n))
    {
        return m_allVariables[n];
    }

    return Types::TYPE_UNKNOWN;
}

QString RuntimeProvider::typeOfVariable(const QString &s)
{
    return typeOfVariable(s.toLocal8Bit().data());
}

void RuntimeProvider::addFunction(QSharedPointer<FunctionDefinition> fd)
{
    m_functions.push_back(fd);
}

void RuntimeProvider::addParametricFunction(QSharedPointer<Parametric> pfd)
{
    m_parametricFunctions.append(pfd);
}

void RuntimeProvider::addVariable(const QString &name, const QString &type)
{
    m_allVariables.insert(name, type);
}

void RuntimeProvider::specifyVariableDomain(const QString &name, const QString &type)
{
    m_listDomains.insert(name, type);
}

double RuntimeProvider::getIndexedVariableValue(const char *n, int index)
{
    auto arrayAssignment = getAssignmentAs<ArrayAssignment>(n);
    if(arrayAssignment)
    {

        // this
        if(index >= arrayAssignment->m_elements.size())
        {
            throw syntax_error_exception(ERRORCODE(55), "Index out of bounds for <b>%s</b>. Requested <b>%s</b>, available: <b>%s</b>", n, index, arrayAssignment->m_elements.size());
        }

        return arrayAssignment->m_elements[index]->Calculate();
    }
    else
    {

        auto pointsOfObjectAssignment = getAssignmentAs<PointsOfObjectAssignment>(n);
        if(pointsOfObjectAssignment)
        {
            return std::numeric_limits<double>::quiet_NaN();
        }

        throw syntax_error_exception(ERRORCODE(34), "Cannot identify the object assigning from: <b>%s</b>", n);

    }
}

bool RuntimeProvider::isRunning() const
{
    return m_running;
}

bool RuntimeProvider::isNumericVariable(const QString &s) const
{
    return m_numericVariables.contains(s);
}

const QString &RuntimeProvider::getCurrentPalette() const
{
    return m_currentPalette;
}

QVector<QSharedPointer<Statement> > &RuntimeProvider::getStatements()
{
    return m_statements;
}

const QVector<QSharedPointer<FunctionDefinition> > &RuntimeProvider::getFunctions() const
{
    return m_functions;
}

void RuntimeProvider::debugVariables()
{
#ifdef DEBUG
    for(const auto& [n, v]:m_numericVariables.toStdMap())
    {
        qDebug() << "N=" << n << "V=" <<v;
    }

    // this is a point/array assiogned variable, held in the assignments
    for(const auto& adef : qAsConst(m_assignments))
    {
        qDebug() << adef->varName << "@" << (void*)adef.data() << "D:" << domainOfVariable(adef->varName) << " ST:" << adef->statement;
    }

    for(const auto& [n, v]:m_allVariables.toStdMap())
    {
        qDebug() << "A N=" << n << "V=" <<v;
    }
#endif
}

void RuntimeProvider::populateBuiltinFunctions()
{
    for(const auto& fds : supported_functions)
    {
        if(fds.standalone_plottable)
        {
            QSharedPointer<FunctionDefinition> fd;
            fd.reset(new FunctionDefinition(-1, "") );
            std::string fde = fds.name + "(x)=" + fds.name + "(x)";
            fd->f = QSharedPointer<Function>( new Function(fde.c_str(), nullptr));
            addFunction(fd);
        }
    }
}

bool RuntimeProvider::getShowCoordinates() const
{
    return m_showCoordinates;
}

void RuntimeProvider::setShowCoordinates(bool newShowCoordinates)
{
    m_showCoordinates = newShowCoordinates;
}

void RuntimeProvider::setupConnections(QObject *o)
{
    QObject::connect(this, SIGNAL(rotationAngleChange(double)), o, SLOT(on_rotationAngleChange(double)));
    QObject::connect(this, SIGNAL(zoomFactorChange(double)), o, SLOT(on_zoomFactorChange(double)));
    QObject::connect(this, SIGNAL(gridChange(bool)), o, SLOT(on_gridChange(bool)));
    QObject::connect(this, SIGNAL(coordEndYChange(double)), o, SLOT(on_coordEndYChange(double)));
    QObject::connect(this, SIGNAL(coordStartYChange(double)), o, SLOT(on_coordStartYChange(double)));
    QObject::connect(this, SIGNAL(coordEndXChange(double)), o, SLOT(on_coordEndXChange(double)));
    QObject::connect(this, SIGNAL(coordStartXChange(double)), o, SLOT(on_coordStartXChange(double)));
}

void RuntimeProvider::execute()
{
    m_running = true;
    int lineNo = 0;
    try
    {
        for(const auto& stmt : qAsConst(m_statements))
        {
            if(!m_running)
            {
                break;
            }
            lineNo = stmt->lineNumber;
            setCurrentStatement(stmt->statement);
            stmt->execute(this);
        }
    }
    catch(syntax_error_exception& ex)
    {
        reportError(lineNo, ex.error_code(), ex.what());
    }
    catch(std::exception& ex)
    {
        reportError(lineNo, ERRORCODE(41), ex.what());
    }
    m_running = false;
}

void RuntimeProvider::stopExecution()
{
    m_running = false;
#ifdef ENABLE_PYTHON
    python_quit(nullptr);
#endif

}

void RuntimeProvider::setCurrentStatement(const QString &newCurrentStatement)
{
    m_statementTracker(newCurrentStatement);
}

void RuntimeProvider::setPen(int r, int g, int b, int a)
{
    m_a = a;
    m_b = b;
    m_g = g;
    m_r = r;
    m_penSetter(r, g, b, a, m_ps);
}

void RuntimeProvider::setPixelSize(size_t s)
{
    m_ps = s;
    m_penSetter(m_r, m_g, m_b, m_a, m_ps);
}

void RuntimeProvider::setPalette(QString p)
{
    m_currentPalette = p;
}

void RuntimeProvider::drawPlot(QSharedPointer<Plot> plot)
{
    m_plotDrawer(plot);
}

void RuntimeProvider::resolvePlotInterval(QSharedPointer<Plot> plot, QSharedPointer<Assignment> assignment,
                                          bool& continuous, double& plotStart, double& plotEnd, bool& counted, double &stepValue, int& count, bool useDefaultValues)
{
    QSharedPointer<Function> stepFun = nullptr;

    counted &= plot->counted;

    if(assignment)
    {
        if(!plot->start)
        {
            if(! assignment->startValueProvider())
            {
                if(!useDefaultValues)
                {
                    reportError(assignment->lineNumber, ERRORCODE(25), "Invalid plotting interval for <b>" + assignment->varName + "</b>. There is no clear start value defined for it.");
                    return;
                }
            }
            else
            {
                plotStart = assignment->startValueProvider()->Calculate();
            }
        }
        else
        {
            plotStart = plot->start->Calculate();
        }

        if(!plot->end)
        {
            if(! assignment->endValueProvider())
            {
                if(!useDefaultValues)
                {
                    reportError(assignment->lineNumber, ERRORCODE(25), "Invalid plotting interval. There is no clear end value defined for it.");
                    return;
                }
            }
            else
            {
                plotEnd = assignment->endValueProvider()->Calculate();
            }
        }
        else
        {
            plotEnd = plot->end->Calculate();
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
            plotStart = plot->start->Calculate();
        }

        if(plot->end)
        {
            plotEnd = plot->end->Calculate();
        }
    }

    if(stepFun)
    {
        count = stepFun->Calculate();
        if(counted)
        {
            if(count > 1)
            {
                stepValue = (plotEnd - plotStart) / (count - 1);
                qDebug() << "StepValue:" << stepValue;
            }
            else
            {
                stepValue = (plotEnd - plotStart);
                qDebug() << "StepValue:" << stepValue;
            }
        }
        else
        {
            stepValue = stepFun->Calculate();
            qDebug() << "StepValue:" << stepValue;
        }
    }


}

QVector<QSharedPointer<Assignment> >& RuntimeProvider::getAssignments()
{
    return m_assignments;
}

QSharedPointer<Assignment> RuntimeProvider::getAssignment(const QString &n)
{
    for(int fds_c = m_assignments.size() - 1; fds_c >= 0; fds_c--)
    {
        QSharedPointer<Assignment> fds = m_assignments[fds_c];
        if(fds.get()->varName == n)
        {
            return fds;
        }
    }

    return nullptr;
}

QSharedPointer<Parametric> RuntimeProvider::getParametricFunction(const QString &n)
{
    for(const auto& pf : m_parametricFunctions)
    {
        if(pf->funName == n)
        {
            return pf;
        }
    }

    return nullptr;
}

QSharedPointer<Function> RuntimeProvider::getFunction(const QString &n)
{
    for(const auto& fds : qAsConst(this->m_functions))
    {
        if(fds && fds.data()->f && fds.data()->f->get_name() == n.toStdString())
        {
            return fds.data()->f;
        }
    }

    return nullptr;

}

std::vector<std::string> RuntimeProvider::getBuiltinFunctions() const
{
    std::vector<std::string> function_names;

    for(const auto& fds : supported_functions)
    {
        function_names.push_back(fds.name);
    }

    return function_names;
}

std::vector<std::string> RuntimeProvider::getFunctionNames() const
{
    std::vector<std::string> function_names;

    for(const auto& fds : this->m_functions)
    {
        function_names.push_back(fds.data()->f->get_name());
    }

    return function_names;
}

std::vector<std::string> RuntimeProvider::getVariables() const
{
    std::vector<std::string> variables;
    for(const auto& fds : this->m_assignments)
    {
        variables.push_back(fds->varName.toStdString());
    }

    return variables;

}

std::vector<std::string> RuntimeProvider::getDeclaredVariables() const
{
    auto defd = getVariables();
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

std::vector<std::string> RuntimeProvider::getAllVariables() const
{
    std::vector<std::string> variables;

    for(const auto& s : m_allVariables.keys())
    {
        variables.push_back(s.toStdString());
    }

    return variables;
}

QSharedPointer<CodeEngine> RuntimeProvider::getCodeEngine() const
{
    return m_codeEngine;
}

bool RuntimeProvider::hasVariable(const QString &name) const
{
    return m_allVariables.count(name) != 0;
}

QString RuntimeProvider::domainOfVariable(const QString & vn)
{
    if(m_listDomains.contains(vn))
    {
        return m_listDomains[vn];
    }

    return Types::TYPE_UNKNOWN;
}

QString RuntimeProvider::valueOfVariable(const QString &vn)
{
    for(const auto& s : m_allVariables.keys())
    {
        if(s == vn)
        {
            if(typeOfVariable(vn) == Types::TYPE_NUMBER)
            {
                QString r = QString::number(value(vn.toStdString()), 'f', 6);
                return r;
            }

            if(typeOfVariable(vn) == Types::TYPE_POINT)
            {
                auto adef = getAssignmentAs<PointDefinitionAssignment>(vn);
                if(adef)
                {
                    return adef->toString();
                }
            }

            if(typeOfVariable(vn) == Types::TYPE_LIST)
            {
                QString varDomain = domainOfVariable(vn);
                if(varDomain == Domains::DOMAIN_NUMBERS)
                {
                    auto aa = getAssignmentAs<ArrayAssignment>(vn);
                    if(aa)
                    {
                        return aa->toString();
                    }
                }
                else
                if(varDomain == Domains::DOMAIN_POINTS)
                {
                    auto paa = getAssignmentAs<PointArrayAssignment>(vn);
                    if(paa)
                    {
                        return paa->toString();
                    }
                    else
                    {
                    auto poa = getAssignmentAs<PointsOfObjectAssignment>(vn);
                    if(poa)
                    {
                        return poa->toString();
                    }
                    }
                }
            }
        }
    }

    return "";
}
