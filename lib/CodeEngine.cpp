#include "Append.h"
#include "ArithmeticAssignmentToArrayElement.h"
#include "ArrayAssignment.h"
#include "ArrayIteratorLoopTarget.h"
#include "CodeEngine.h"
#include "Else.h"
#include "If.h"
#include "PointArrayAssignment.h"
#include "Keywords.h"
#include "Function.h"
#include "Done.h"
#include "LoopTarget.h"
#include "RangeIteratorLoopTarget.h"
#include "FunctionIteratorLoopTarget.h"
#include "Loop.h"
#include "Plot.h"
#include "Set.h"
#include "PointsOfObjectAssignment.h"
#include "ArithmeticAssignment.h"
#include "PointDefinitionAssignment.h"
#include "PointDefinitionAssignmentToOtherPoint.h"
#include "FunctionDefinition.h"
#include "Rotation.h"

#ifdef ENABLE_PYTHON
#include "PythonScript.h"
#endif

#include "RuntimeProvider.h"
#include "VariableDeclaration.h"
#include "constants.h"

CodeEngine::CodeEngine(QStringList codelines, RuntimeProvider *rp) : m_codelines(codelines), m_rp(rp)
{
}

bool CodeEngine::parse()
{
    while(!m_codelines.isEmpty() && m_codelines.last().isEmpty())
    {
        m_codelines.removeLast();
    }

    int lineNo = 0;
    while(!m_codelines.empty())
    {
        try
        {
            if(!m_codelines[0].simplified().startsWith("#"))
            {
                resolveCodeline(lineNo, m_codelines, m_rp->getStatements(), nullptr);
            }
            else
            {
                m_codelines.pop_front();
            }
        }
        catch(...)
        {
            return false;
        }
    }
    return true;
}

QSharedPointer<Statement> CodeEngine::resolveCodeline(int& ln, QStringList &codelines,
                                                      QVector<QSharedPointer<Statement> > &statements,
                                                      QSharedPointer<Statement> parentScope)
{
    QString codeline = codelines[0];
    codelines.pop_front();
    codeline = codeline.simplified();
    // small hacks to make parsing easier
    codeline.replace("(", " ( ");
    codeline.replace(")", " ) ");
    codeline.replace("list [", "list[");
    codeline = codeline.trimmed();

    ln ++;

    //    codeline = codeline.simplified();
    if(!codeline.isEmpty())
    {
        try
        {

            QSharedPointer<Statement> createdStatement;

            if(codeline.startsWith(Keywords::KW_VAR))
            {
                QVector<QSharedPointer<Statement>> stats = createVariableDeclaration(ln, codeline);
                for(auto& s : stats)
                {
                    statements.append(s);

                    s->parent = parentScope;
                    s->runtimeProvider = m_rp;
                }
            }
            else
            if(codeline.startsWith(Keywords::KW_APPEND))
            {
                statements.append(createdStatement = createListAppend(ln, codeline));
            }
            else
            if(codeline.startsWith(Keywords::KW_FUNCTION))
            {
                statements.append(createdStatement = createFunction(ln, codeline));
            }
            else
            if(codeline.startsWith(Keywords::KW_PLOT))
            {
                statements.append(createdStatement = createPlot(ln, codeline));
            }
            else
            if(codeline.startsWith(Keywords::KW_LET)) // variable assignment
            {
                statements.append(createdStatement = createAssignment(ln, codeline));
            }
            else
            if(codeline.startsWith(Keywords::KW_SET)) // setting the color, line width, rotation, etc ...
            {
                statements.append(createdStatement = createSet(ln, codeline));
            }
            else
            if(codeline.startsWith(Keywords::KW_FOR)) // looping over a set of points or something else
            {
                statements.append(createdStatement = createLoop(ln, codeline, codelines));
            }
            else
            if(codeline.startsWith(Keywords::KW_IF)) // checking a condition
            {
                statements.append(createdStatement = createIf(ln, codeline, codelines));
            }
            else
            if(codeline.startsWith(Keywords::KW_DONE)) // done processing a codeblock
            {
                statements.append(createdStatement = QSharedPointer<Done>(new Done(ln, codeline)));
            }
            else
            if(codeline.startsWith(Keywords::KW_ELSE)) // done with the if, here comes the else
            {
                statements.append(createdStatement = QSharedPointer<Else>(new Else(ln, codeline)));
            }
            else
            if(codeline.startsWith(Keywords::KW_ROTATE)) // rotating something
            {
                statements.append(createdStatement = createRotation(ln, codeline, codelines));
            }
            else

#ifdef ENABLE_PYTHON
            if(codeline.startsWith(ScriptingLangugages::PYTHON)) // running python code
            {
                statements.append(createdStatement = createPythonSriptlet(ln, codeline, codelines));
            }
            else
#endif
            if(!codeline.isEmpty())
            {
                m_rp->reportError(ln, ERRORCODE(35), "Invalid statement: " + codeline);
            }

            if(createdStatement)
            {
                createdStatement->parent = parentScope;
                createdStatement->runtimeProvider = m_rp;
            }

        }
        catch(syntax_error_exception& ex)
        {
            m_rp->reportError(ln, ex.error_code(), ex.what());
            throw;
        }
        catch(std::exception& ex)
        {
            m_rp->reportError(ln, ERRORCODE(41), ex.what());
            throw;
        }
    }

    if(!statements.isEmpty())
    {
        return statements.constLast();
    }

    return nullptr;

}

QVector<QSharedPointer<Statement>> CodeEngine::createVariableDeclaration(int ln, const QString &codeline)
{
    QVector<QSharedPointer<Statement>> result;

    // variable declarations are like: var somestuff sometype
    QString vd_body = codeline.mid(Keywords::KW_VAR.length());
    consumeSpace(vd_body);
#if QT_VERSION > QT_VERSION_CHECK(5, 15, 0)
    QStringList allDeclarations = vd_body.split(",", Qt::SkipEmptyParts);
#else
    QStringList allDeclarations = vd_body.split(QLatin1Char(','), QString::SkipEmptyParts);
#endif
    for(const auto &s : allDeclarations)
    {
#if QT_VERSION > QT_VERSION_CHECK(5, 15, 0)
        QStringList declI = s.split(" ", Qt::SkipEmptyParts);
#else
        QStringList declI = s.split(" ", QString::SkipEmptyParts);
#endif
        if(declI.size() < 2)
        {
            throw syntax_error_exception(ERRORCODE(1), "Invalid variable declaration: <b>%s</b>", s.toStdString().c_str());
        }

        // let's see if we have "list" in the elements
        bool has_list = false;
        int list_kw_index = -1;
        for(int i =0; i<declI.size(); i++)
        {
            auto d = declI[i];
            if(d == Types::TYPE_LIST)
            {
                has_list = true;
                list_kw_index = i;
                break;
            }
        }

        if(!has_list)
        {
            QString type = declI.last();
            if(!Types::all().contains(type))
            {
                throw syntax_error_exception(ERRORCODE(2), "Invalid variable type <b>%s</b> in <b>%s", type.toStdString().c_str(), s.toStdString().c_str());
            }

            declI.removeLast();

            for(const auto& vn : qAsConst(declI))
            {
                QSharedPointer<VariableDeclaration> resultI;
                resultI.reset(new VariableDeclaration(ln, codeline));
                resultI->name = vn;
                resultI->type = type;

                result.append(resultI);

                m_rp->addVariable(vn, type);

                if(type == Types::TYPE_POINT)
                {
                    m_rp->addOrUpdatePointDefinitionAssignment(ln, resultI.get(), 0, 0, vn);
                }

            }
        }
        else
        {
            if(list_kw_index + 1 < declI.size())
            {
                if(declI[list_kw_index + 1] != Keywords::KW_OF )
                {
                    throw syntax_error_exception(ERRORCODE(3), "Missing keyword <b>of</b> in <b>%s</b>",s.toStdString().c_str());
                }
                if(list_kw_index + 2 < declI.size())
                {
                    QString domain = declI[list_kw_index + 2];

                    if(Domains::all().indexOf(domain) == -1)
                    {
                        throw syntax_error_exception(ERRORCODE(4), "Invalid list variable type <b>%s</b> in <b>%s", domain.toStdString().c_str(), s.toStdString().c_str());
                    }

                    for(int i =0; i<list_kw_index; i++)
                    {

                        QSharedPointer<VariableDeclaration> resultI;
                        resultI.reset(new VariableDeclaration(ln, codeline));
                        resultI->name = declI[i];
                        resultI->type = Types::TYPE_LIST;

                        result.append(resultI);

                        m_rp->addVariable(declI[i], Types::TYPE_LIST);
                        m_rp->specifyVariableDomain(declI[i], domain);
                    }
                }
                else
                {
                    throw syntax_error_exception(ERRORCODE(5), "Untyped lists are not supported in <b>%s</b>",s.toStdString().c_str());
                }
            }
            else
            {
                throw syntax_error_exception(ERRORCODE(6), "Untyped lists are not supported in <b>%s</b>",s.toStdString().c_str());
            }
        }
    }

    return result;
}

QSharedPointer<Statement> CodeEngine::createAssignment(int ln, const QString &codeline)
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

    if(!m_rp->hasVariable(varName) && !indexed)
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
            if(m_rp->typeOfVariable(varName) != Types::TYPE_POINT)
            {
                throw syntax_error_exception(ERRORCODE(10), "Conflicting type assignment: <b>point</b> assigned to a non point type variable: <b>%s (%s)</b>", varName.toStdString().c_str(), m_rp->typeOfVariable(varName).toStdString().c_str());
            }

            QString nextWord = getDelimitedId(assignment_body);
            if(nextWord != "at")
            {
                QSharedPointer<PointDefinitionAssignmentToOtherPoint> result;
                result.reset(new PointDefinitionAssignmentToOtherPoint(ln, codeline));
                result->otherPoint = nextWord;
                result->varName = varName;

                m_rp->addOrUpdateAssignment(result);

                return result;

                //                throw syntax_error_exception(ERRORCODE(11), "Invalid point assignment: %s (missing at keyword)", codeline.toStdString().c_str());
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
                m_rp->addOrUpdateAssignment(result);

                return result;
            }
        }
        if(targetProperties == Types::TYPE_LIST) // or maybe we create a list
        {
            if(m_rp->typeOfVariable(varName) != Types::TYPE_LIST)
            {
                throw syntax_error_exception(ERRORCODE(12), "Conflicting type assignment: <b>array</b> assigned to a non array type variable: <b>%s (%s)</b>", varName.toStdString().c_str(), m_rp->typeOfVariable(varName).toStdString().c_str());
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

            QString typeOfVarName = m_rp->domainOfVariable(varName);

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
                m_rp->addOrUpdateAssignment(result);
                return result;
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
                m_rp->addOrUpdateAssignment(result);
                return result;
            }
        }
        else
        {
            if(indexed && !realVarName.isEmpty())
            {
                varName = realVarName;
            }
            if(m_rp->typeOfVariable(varName) == Types::TYPE_POINT)
            {
                QSharedPointer<PointDefinitionAssignmentToOtherPoint> result;
                result.reset(new PointDefinitionAssignmentToOtherPoint(ln, codeline));
                result->otherPoint = targetProperties; // yeah, almost
                // if this was an indexed stuff ...
                if(delimiter == '[') result->otherPoint += "["+ assignment_body;
                result->varName = varName;

                m_rp->addOrUpdateAssignment(result);

                return result;
            }
            else
            {
                if(m_rp->typeOfVariable(varName) != Types::TYPE_NUMBER && !indexed)
                {
                    throw syntax_error_exception(ERRORCODE(14), "Invalid assignment: <b>%s</b> (<b>arithmetic expression</b> assigned to <b>%s</b>)", varName.toStdString().c_str(), m_rp->typeOfVariable(varName).toStdString().c_str());
                }
                if(!indexed)
                {
                    QSharedPointer<ArithmeticAssignment> result;
                    result.reset(new ArithmeticAssignment(ln, codeline));

                    QString expression = targetProperties;
                    expression += (delimiter == '[' ? "[" : "") + assignment_body;
                    result->arithmetic =  Function::temporaryFunction(expression, result.get());

                    result->varName = varName;
                    m_rp->addOrUpdateAssignment(result);

                    return result;
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
                    m_rp->addOrUpdateAssignment(result);
                    return result;
                }
            }
        }
    }

    // here we are sure we want the points of a plot, variable type should be list
    if(m_rp->typeOfVariable(varName) != Types::TYPE_LIST)
    {
        throw syntax_error_exception(ERRORCODE(12), "Invalid assignment: <b>%s</b> (<b>array</b> assigned to <b>%s</b>)", varName.toStdString().c_str(), m_rp->typeOfVariable(varName).toStdString().c_str());
    }
    QSharedPointer<Assignment> assignmentData = providePointsOfDefinition(ln, codeline, assignment_body, varName, targetProperties);
    m_rp->addOrUpdateAssignment(assignmentData);
    return assignmentData;
}

QSharedPointer<Statement> CodeEngine::createFunction(int ln, const QString &codeline)
{
    QString f_body = codeline.mid(Keywords::KW_FUNCTION.length());
    consumeSpace(f_body);

    QSharedPointer<FunctionDefinition> fd;
    fd.reset(new FunctionDefinition(ln, codeline) );
    Function* f = new Function(f_body.toLocal8Bit().data(), fd.get());

    fd->f = QSharedPointer<Function>(f);
    m_rp->addFunction(fd);

    return fd;
}

QSharedPointer<Statement> CodeEngine::createListInsert(int ln, const QString &codeline)
{
// TODO!
    return nullptr;
}

QSharedPointer<Statement> CodeEngine::createListAppend(int ln, const QString &codeline)
{
    QSharedPointer<Append> result;
    result.reset(new Append(ln, codeline));

    QString append_body = codeline.mid(Keywords::KW_APPEND.length());
    consumeSpace(append_body);

    if(!append_body.startsWith(Keywords::KW_TO))
    {
        throw syntax_error_exception(ERRORCODE(59), "<b>append</b> statement does not contain the <b>to</b> keyword in: <b>%s</b>", codeline.toStdString().c_str());
    }

    append_body = append_body.mid(Keywords::KW_TO.length());
    consumeSpace(append_body);

    result->varName = getDelimitedId(append_body);

    if(!m_rp->hasVariable(result->varName))
    {
        throw syntax_error_exception(ERRORCODE(8), "Undeclared variable <b>%s</b> while parsing: <b>%s</b>", result->varName.toStdString().c_str(), codeline.toStdString().c_str());
    }

    result->domain = getDelimitedId(append_body)    ;
    if(!Domains::all().contains(result->domain))
    {
        throw syntax_error_exception(ERRORCODE(63), "Unsupported domain <b>%s</b> while parsing: <b>%s</b>", result->domain.toStdString().c_str(), codeline.toStdString().c_str());
    }

    QString domainOfVar = m_rp->domainOfVariable(result->varName);
    if(result->domain != domainOfVar)
    {
        throw syntax_error_exception(ERRORCODE(8), "Incompatible domain types for append, found <b>%s</b> expected <b>%s</b> while parsing <b>%s</b>",
                                     result->domain.toStdString().c_str(), domainOfVar.toStdString().c_str(), codeline.toStdString().c_str());
    }

    // append body has the list of numbers
    if(result->domain == Domains::DOMAIN_NUMBERS)
    {
#if QT_VERSION > QT_VERSION_CHECK(5, 15, 0)
        result->numberValues = append_body.split(',', Qt::SkipEmptyParts);
#else
        result->numberValues = append_body.split(',', QString::SkipEmptyParts);
#endif
    }
    else
    {
        // TODO: This is more or less the same as line 352 at assignment
        append_body.remove(' ');
        append_body.replace("),(", ");(");

        QStringList arrayElements = append_body.split(";");
        for(QString e : arrayElements)
        {
            if(!e.startsWith("("))
            {
                throw syntax_error_exception(ERRORCODE(35), "Invalid point array append: missing <b>(</b> from <b>%s</b>", codeline.toStdString().c_str());
            }
            e = e.mid(1);
            QString sx = getDelimitedId(e, {','});
            QString sy = getDelimitedId(e, {')'});

            auto t = std::make_tuple(Function::temporaryFunction(sx, result.get()), Function::temporaryFunction(sy, result.get()));
            result->pointValues.append(t);
        }

    }
    return result;
}

QSharedPointer<Statement> CodeEngine::createPlot(int ln, const QString &codeline)
{
    QSharedPointer<Plot> plotData;
    plotData.reset(new Plot(ln, codeline));

    QString plot_body = codeline.mid(Keywords::KW_PLOT.length());
    consumeSpace(plot_body);

    // function name
    QString fnai;
    QString funToPlot = extract_proper_expression(plot_body, fnai, {' '}, {Keywords::KW_COUNTS, Keywords::KW_OVER, Keywords::KW_CONTINUOUS, Keywords::KW_FOR}, true);

    // let's see if we have a function for this already
    QSharedPointer<Function> ff = m_rp->getFunction(funToPlot);

    if(!ff)
    {
        // or maybe an assignment
        auto a = m_rp->getAssignment(funToPlot);
        if(!a)
        {
            QString tov = m_rp->typeOfVariable(funToPlot);
            // let's see if there is a variable defined with this name, which was not assigned yet. That's an error
            if(Types::all().contains(tov) && tov != Types::TYPE_UNKNOWN)
            {
                // is this a direct indexed plot?
                if(funToPlot.indexOf('[') == -1)
                {
                    throw syntax_error_exception(ERRORCODE(49), "Invalid plot: <b>%s</b> was declared, but not defined to hold a value", funToPlot.toStdString().c_str());
                }
            }

            // let's see if we are plotting a single point of a series of plots: plot ps[2]

            IndexedAccess* ia_m = nullptr; Assignment* a = nullptr;
            auto tempFun = Function::temporaryFunction(funToPlot, plotData.get());
            if(fnai == Keywords::KW_FOR)
            {
                QString tmpCodeline = codeline;
                tmpCodeline = tmpCodeline.mid(tmpCodeline.indexOf(fnai) + Keywords::KW_FOR.length());
                consumeSpace(tmpCodeline);
                QString funVar = getDelimitedId(tmpCodeline);
                tempFun.get()->add_variable(funVar.toStdString().c_str()); // mostly the plots happen with "x"

                // making it consume the rest of the codeline too
                plot_body = tmpCodeline;
            }

            tempFun->Calculate(m_rp, ia_m, a);
            // if this is an indexed something ... let's hope down there someone will take care of it :)
            if(!ia_m)
            {

                QSharedPointer<FunctionDefinition> fd;
                fd.reset(new FunctionDefinition(ln, codeline) );
                QString plgfn = "plot_fn_" +QString::number(m_rp->getFunctions().size()) + "(x) =";

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
                m_rp->addFunction(fd);
            }
            else
            {
                delete ia_m;
            }
        }
        else
        {
            if(m_rp->typeOfVariable(funToPlot) == Types::TYPE_NUMBER)
            {
                throw syntax_error_exception(ERRORCODE(50), "Invalid plot: <b>%s</b> was declared to be <b>numeric</b>, but it is impossible to plot a number", funToPlot.toStdString().c_str());
            }
        }
    }

    plotData->plotTarget = funToPlot;
    consumeSpace(plot_body);

    // do we have a continuous keyword and nothing else?
    if(codeline.startsWith(Keywords::KW_CONTINUOUS))
    {
        plotData->continuous = true;
        plot_body = plot_body.mid(Keywords::KW_CONTINUOUS.length());
        consumeSpace(plot_body);
    };

    // over keyword
    if(plot_body.startsWith(Keywords::KW_OVER))
    {
        resolveOverKeyword(plot_body, plotData, plotData.get());
    }
    else
        if(plot_body.startsWith(Keywords::KW_COUNTS))
        {
            resolveCountsKeyword(plot_body, plotData, plotData.get());
        }

    return plotData;
}

void CodeEngine::resolveOverKeyword(QString codeline, QSharedPointer<Stepped> stepped, Statement *s)
{
    codeline = codeline.mid(Keywords::KW_OVER.length());
    consumeSpace(codeline);
    char close_char = 0;
    if(!codeline.startsWith("(") || !codeline.startsWith("["))
    {
        if (!codeline.isEmpty())
        {
            close_char = codeline[0].toLatin1() == '(' ? ')' : ']';
            codeline = codeline.mid(1);
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

    if (codeline.startsWith(Keywords::KW_STEP))
    {
        codeline = codeline.mid(Keywords::KW_STEP.length());
        consumeSpace(codeline);
        stepped->stepValue = codeline;

        stepped->step = Function::temporaryFunction(codeline, s);
    }
    else
        if (codeline.startsWith(Keywords::KW_COUNTS))
        {
            resolveCountsKeyword(codeline, stepped, s);
        }
        else
            if(!codeline.isEmpty())
            {
                throw syntax_error_exception(ERRORCODE(53), "Invalid keyword: <b>%s</b>", codeline.toStdString().c_str());
            }
    stepped->start = Function::temporaryFunction(first_par, s);
    stepped->end = Function::temporaryFunction(second_par, s);

}

void CodeEngine::resolveCountsKeyword(QString codeline, QSharedPointer<Stepped> stepped, Statement *s)
{
    codeline = codeline.mid(Keywords::KW_COUNTS.length());
    consumeSpace(codeline);
    QString strPointCount = getDelimitedId(codeline);
    stepped->stepValue = strPointCount;

    stepped->step = Function::temporaryFunction(strPointCount, s);
    stepped->counted = true;

    if(codeline == Keywords::KW_SEGMENTS)
    {
        stepped->step = Function::temporaryFunction(strPointCount + " + 1", s);
        stepped->continuous = true;
    }
}

#ifdef ENABLE_PYTHON
QSharedPointer<Statement> CodeEngine::createPythonSriptlet(int ln, const QString &codeline, QStringList &codelines)
{
    QSharedPointer<PythonScript> pythonScript;
    pythonScript.reset(new PythonScript(ln, codeline));

    QString python_body = codeline.mid(ScriptingLangugages::PYTHON.length());
    consumeSpace(python_body);

    QString scriptKeyword = getDelimitedId(python_body);
    if(scriptKeyword != Keywords::KW_SCRIPT)
    {
        throw syntax_error_exception(ERRORCODE(67), "Missing <b>script</b> keyword from python scriptlet in <b>%s</b>", codeline.toStdString().c_str());
    }
    QString typeKeyword = getDelimitedId(python_body);
    if(typeKeyword == Keywords::KW_BEGIN)
    {
        bool done = false;

        while(!codelines.isEmpty() && !done)
        {
            QString cl0 = codelines.at(0);

            if(cl0.simplified() == "end")
            {
                done = true;
            }
            else
            {
                pythonScript->pythonCodeLines.append(cl0);
            }

            codelines.pop_front();
        }
    }

    return pythonScript;
}
#endif

QSharedPointer<Assignment> CodeEngine::providePointsOfDefinition(int ln, const QString &codeline, QString assignment_body, const QString &varName, const QString &targetProperties)
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

    if(!m_rp->getNameFunctionOrAssignment(result->ofWhat, assignment))
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

QSharedPointer<Statement> CodeEngine::createSet(int ln, const QString &codeline)
{
    QString s_body = codeline.mid(Keywords::KW_SET.length());
    consumeSpace(s_body);

    // set what?
    QString what;
    while(!s_body.isEmpty() && !s_body[0].isSpace())
    {
        what += s_body[0];
        s_body = s_body.mid(1);
    }
    if(SetTargets::all().indexOf(what) == -1)
    {
        throw syntax_error_exception(ERRORCODE(36), "Syntax error: unsupported <b>set</b> target: <b>%s</b> in <b>%s</b>", what.toStdString().c_str(), codeline.toStdString().c_str());
    }
    s_body = s_body.mid(1);
    // to what, can be: colorname, #RRGGBB or R,G,B (real numbers for this situation)
    QString to_what;
    while(!s_body.isEmpty())
    {
        to_what += s_body[0];
        s_body = s_body.mid(1);
    }
    QSharedPointer<Set> sett(new Set(ln, codeline));
    sett->what = what;
    sett->value = to_what;
    return sett;
}

QSharedPointer<Statement> CodeEngine::createLoop(int ln, const QString &codeline, QStringList &codelines)
{
    QString l_decl = codeline.mid(Keywords::KW_FOR.length());
    consumeSpace(l_decl);
//    QVector<QSharedPointer<Statement>> loop_body;

    // the loop object, regardless if its an in loop or assignment loop
    QSharedPointer<Loop> result = nullptr;
    result.reset(new Loop(ln, codeline));

    // the loop variable
    [[maybe_unused]] QString fnai;
    QString loop_variable = extract_proper_expression(l_decl, fnai);

    if(!m_rp->hasVariable(loop_variable))
    {
        throw syntax_error_exception(ERRORCODE(37), "Undeclared variable <b>%s</b> cannot be used in for loop (%s)", loop_variable.toStdString().c_str(), codeline.toStdString().c_str());
    }

    result->loop_variable = loop_variable;

    consumeSpace(l_decl);

    // in keyword
    if(l_decl.startsWith(Keywords::KW_IN))
    {
        // skip it
        l_decl = l_decl.mid(Keywords::KW_IN.length());
        consumeSpace(l_decl);

        // see what are we looping through
        QString loop_over = extract_proper_expression(l_decl, fnai);

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
            QString range_start = extract_proper_expression(l_decl, fnai, {','});
            consumeSpace(l_decl);

            if(!l_decl.isEmpty() && l_decl[0] == ',')
            {
                l_decl = l_decl.mid(1);
                consumeSpace(l_decl);
            }

            ritl->startFun = Function::temporaryFunction(range_start, result.get());

            // range end
            QString range_end = extract_proper_expression(l_decl, fnai, (needs_closing_paren ? QSet<QChar>{QChar(')')} : QSet<QChar>{QChar(' ')}) );
            ritl->endFun = Function::temporaryFunction(range_end, result.get());

            if(!l_decl.isEmpty() && l_decl[0] == ')' && needs_closing_paren)
            {
                l_decl = l_decl.mid(1);
            }

            consumeSpace(l_decl);
            // do we have a step
            if(l_decl.startsWith(Keywords::KW_STEP))
            {
                l_decl = l_decl.mid(Keywords::KW_STEP.length());
                consumeSpace(l_decl);

                QString step = extract_proper_expression(l_decl, fnai);
                ritl->stepFun = Function::temporaryFunction(step, result.get());

                consumeSpace(l_decl);
            }
            else
            {
                ritl->stepFun = Function::temporaryFunction(DEFAULT_RANGE_STEP, result.get());
            }
        }
        else
        if(auto a = m_rp->getAssignment(loop_over)) // are we looping thorugh an array?
        {
            QString vt = m_rp->typeOfVariable(a->varName);
            if(vt == Types::TYPE_LIST)
            {
                result->loop_target.reset(new ArrayIteratorLoopTarget(result));
                result->loop_target->name = loop_over;

                QString domainOfVar = m_rp->domainOfVariable(a->varName);
                if(domainOfVar == Domains::DOMAIN_POINTS)
                {
                    // here add a temporary assignment for the loop_variable to point to the whatever it can
                    QSharedPointer<PointDefinitionAssignment> temp;
                    temp.reset(new PointDefinitionAssignment(ln, codeline));

                    temp->varName = loop_variable;
                    m_rp->addOrUpdateAssignment(temp);
                }
                else
                {
                    // here add a temporary assignment for the loop_variable to point to the whatever it can
                    QSharedPointer<ArithmeticAssignment> temp;
                    temp.reset(new ArithmeticAssignment(ln, codeline));

                    temp->varName = loop_variable;
                    m_rp->addOrUpdateAssignment(temp);
                }
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
        [[maybe_unused]] QString fnai;

        // skip the equal sign
        l_decl = l_decl.mid(1);
        consumeSpace(l_decl);

        // the loop target of this is a range iterator, since this loop is a stepped range
        auto ritl = new RangeIteratorLoopTarget(result);
        result->loop_target.reset(ritl);

        // the start:
        QString range_start = extract_proper_expression(l_decl, fnai);
        ritl->startFun = Function::temporaryFunction(range_start, result.get());

        if(!l_decl.startsWith(Keywords::KW_TO))
        {
            throw syntax_error_exception(ERRORCODE(38), "for statement with direct assignment does not contain the to keyword");
        }

        // create an assignment
        QSharedPointer<ArithmeticAssignment> temp;
        temp.reset(new ArithmeticAssignment(ln, codeline));
        temp->arithmetic = ritl->startFun;

        temp->varName = loop_variable;
        m_rp->addOrUpdateAssignment(temp);

        l_decl = l_decl.mid(Keywords::KW_TO.length());
        consumeSpace(l_decl);

        // range end
        QString range_end = extract_proper_expression(l_decl, fnai);
        ritl->endFun = Function::temporaryFunction(range_end, result.get());

        consumeSpace(l_decl);
        // do we have a step
        if(l_decl.startsWith(Keywords::KW_STEP))
        {
            l_decl = l_decl.mid(Keywords::KW_STEP.length());
            consumeSpace(l_decl);

            QString step = extract_proper_expression(l_decl, fnai);
            ritl->stepFun = Function::temporaryFunction(step, result.get());

            consumeSpace(l_decl);
        }
        else
        {
            ritl->stepFun = Function::temporaryFunction(DEFAULT_FOR_STEP, result.get());
        }
    }
    else
    {
        throw syntax_error_exception(ERRORCODE(39), "invalid for statement");
    }

    if(!l_decl.startsWith(Keywords::KW_DO))
    {
        throw syntax_error_exception(ERRORCODE(40), "for statement does not contain the do keyword");
    }

    bool done = false;
    while(!codelines.isEmpty() && !done)
    {
        QSharedPointer<Statement> st = nullptr;
        QString at0 = codelines.at(0).simplified();
        qDebug() << "try" << at0;

        if(at0.isEmpty() || at0[0] == '#')
        {
            codelines.pop_front();
        }
        else
        {
            try
            {
                st = resolveCodeline(ln, codelines, result->body, result);
            }
            catch(syntax_error_exception& a)
            {
                m_rp->reportError(ln, a.error_code(), a.what());
                throw;
            }

            if(st)
            {
                done = st->keyword() == Keywords::KW_DONE;
            }
            else
            {
                throw syntax_error_exception(ERRORCODE(42), "something is wrong with this expression: %s", codeline.toStdString().c_str());
            }
        }
    }

    if(!done)
    {
        throw syntax_error_exception(ERRORCODE(43), "for body does not end with done");
    }

    return result;

}

QSharedPointer<Statement> CodeEngine::createIf(int ln, const QString &codeline, QStringList &codelines)
{
    QString if_decl = codeline.mid(Keywords::KW_IF.length());
    consumeSpace(if_decl);
    //QVector<QSharedPointer<Statement>> if_body;

    QSharedPointer<If> result = nullptr;
    result.reset(new If(ln, codeline));

    auto rest = if_decl.split(" ");
    QString if_condition;

    if(rest.isEmpty() || rest.at(rest.length() -1) != Keywords::KW_DO)
    {
        throw syntax_error_exception(777, "No do keyword in if statement");
    }
    rest.removeAt(rest.length() - 1);

    if_condition = rest.join(" ");
    result->expression = Function::temporaryFunction(if_condition, result.get());

    consumeSpace(if_decl);
    bool done = false;
    QString lastKeyword = "";

    while(!codelines.isEmpty() && !done)
    {
        QSharedPointer<Statement> st = nullptr;
        QString at0 = codelines.at(0).simplified();

        if(at0.isEmpty() || at0[0] == '#')
        {
            codelines.pop_front();
        }
        else
        {
            try
            {
                st = resolveCodeline(ln, codelines, result->ifBody, result);
            }
            catch(syntax_error_exception& a)
            {
                m_rp->reportError(ln, a.error_code(), a.what());
                throw;
            }

            if(st)
            {
                lastKeyword = st->keyword();
                done = st->keyword() == Keywords::KW_DONE || st->keyword() == Keywords::KW_ELSE;
            }
            else
            {
                throw syntax_error_exception(ERRORCODE(42), "something is wrong with this expression: %s", codeline.toStdString().c_str());
            }
        }
    }

    if(lastKeyword == Keywords::KW_ELSE)
    {
        done = false;
        while(!codelines.isEmpty() && !done)
        {
            QSharedPointer<Statement> st = nullptr;
            QString at0 = codelines.at(0).simplified();

            if(at0.isEmpty() || at0[0] == '#')
            {
                codelines.pop_front();
            }
            else
            {
                try
                {
                    st = resolveCodeline(ln, codelines, result->elseBody, result);
                }
                catch(syntax_error_exception& a)
                {
                    m_rp->reportError(ln, a.error_code(), a.what());
                    throw;
                }

                if(st)
                {
                    lastKeyword = st->keyword();
                    done = st->keyword() == Keywords::KW_DONE;
                }
                else
                {
                    throw syntax_error_exception(ERRORCODE(42), "something is wrong with this expression: %s", codeline.toStdString().c_str());
                }
            }
        }
    }

    if(!done)
    {
        throw syntax_error_exception(ERRORCODE(43), "for body does not end with done");
    }

    return result;
}

QSharedPointer<Statement> CodeEngine::createRotation(int ln, const QString &codeline, QStringList &codelines)
{
    QString r_decl = codeline.mid(Keywords::KW_ROTATE.length());
    consumeSpace(r_decl);

    QString rotate_what = getDelimitedId(r_decl);

    if(!m_rp->hasVariable(rotate_what))
    {
        throw syntax_error_exception(ERRORCODE(44), "rotate statement tries to rotate undeclared variable");
    }

    QString with_keyword = getDelimitedId(r_decl);
    if(with_keyword != "with")
    {
        throw syntax_error_exception(ERRORCODE(45), "rotate statement does not contain <b>with</b> keyword");
    }

    [[maybe_unused]] QString fnai;

    QString rotation_amount = extract_proper_expression(r_decl, fnai, {' '}, QSet<QString>{QString("degrees"), QString("radians"), "around"} );
    QString rotation_unit = getDelimitedId(r_decl);

    QSharedPointer<Rotation> result;
    result.reset(new Rotation(ln, codeline));
    result->degree = Function::temporaryFunction(rotation_amount, result.get());
    result->what = rotate_what;

    if(rotation_unit != "degrees" && rotation_unit != "radians" && !rotation_unit.isEmpty())
    {
        throw syntax_error_exception(ERRORCODE(46), "Rotation unit must be either degree or radians (default)");
    }
    result->unit = rotation_unit;

    QString around_kw = getDelimitedId(r_decl);
    if(!around_kw.isEmpty() && around_kw != Keywords::KW_AROUND)
    {
        throw syntax_error_exception(ERRORCODE(47), "Unknown keyword in rotation: <b>%s</b>" , around_kw.toStdString().c_str());
    }
    QString nextWord = getDelimitedId(r_decl);
    if(nextWord == "point")
    {
        nextWord = getDelimitedId(r_decl);
        if(nextWord != "at")
        {
            throw syntax_error_exception(ERRORCODE(48), "Invalid reference: <b>%s</b> (missing <b><pre>at</pre></b> keyword)", codeline.toStdString().c_str());
        }

        if(!r_decl.isEmpty())
        {
            if(r_decl[0] == '(') // around a specific point
            {
                // skip paren
                r_decl = r_decl.mid(1);
                QString px = extract_proper_expression(r_decl, fnai, {','});
                QString py = extract_proper_expression(r_decl, fnai, {')'});

                result->aroundX = px;
                result->aroundY = py;
            }
        }
    }
    return result;
}
