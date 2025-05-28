#include "pyembed.h"

#include "PythonScript.h"
#include "PythonRunner.h"
#include "RuntimeProvider.h"

#include <map>

bool PythonScript::execute(RuntimeProvider *rp)
{
    auto all_vars = rp->getAllVariables();
    std::map<std::string, std::string> vmap;

    for(const auto& v : all_vars)
    {
        QString vn = QString::fromStdString(v);
        QString vv = rp->valueOfVariable(vn);
        vmap.insert(std::make_pair(v, vv.toStdString()));
    }

    // lets normalize the python code, ie. remove the leading spaces to have valid python syntax
    int minSpace = std::numeric_limits<int>::max();
    QStringList pythonFinalCodelines;

    for(const auto& s : qAsConst(pythonCodeLines))
    {
        int i = 0;
        if(!s.simplified().isEmpty())
        {
            while(i < s.length() && s[i].isSpace() )
            {
                i ++;
            }
            if(minSpace > i)
            {
                minSpace = i;
            }
        }
    }

    // now from each pythonCodeLines remove minSpace characters from the beginning

    for(const auto& s : qAsConst(pythonCodeLines))
    {
        QString t = s.mid(minSpace);
        pythonFinalCodelines.append(t);
    }


    QString allCodelines = pythonFinalCodelines.join("\n");
    qInfo() << allCodelines;

    python_runner pyRunner;

    std::map<std::string, std::string> script_set_vars;
    std::string pres = pyRunner.run(vmap, allCodelines.toStdString(), script_set_vars);

    rp->printString(pres.c_str());

    for(const auto& [k, v] : script_set_vars )
    {
        qDebug() << "K=" << QString::fromStdString(k) << "V=" << QString::fromStdString(v);

        // simple numeric values
        QString key = QString::fromStdString(k);
        if(rp->hasVariable(key) && rp->isNumericVariable(key) )
        {
            rp->setValue(key, fromString(v));
        }
        else
        {
            auto a = rp->getAssignment(key);
            if(a)
            {
                QString at = rp->typeOfVariable(a->varName.toLocal8Bit().data());
                qDebug() << "VarType:" << at;

                // assigning to a point
                if(at == Types::TYPE_POINT)
                {
                    // rewriting the current assignment with the values that came in from the X:xxxx,Y:yyy from python
                    double x = fromString(fromPythonDict(v, X));
                    double y = fromString(fromPythonDict(v, Y));

                    auto tas = rp->provideTemporaryPointDefinitionAssignment(lineNumber, this, x, y, a->varName);
                    auto pa = rp->getAssignmentAs<PointDefinitionAssignment>(a->varName);

                    QString stas = tas->toString();
                    QString spa = pa->toString();

                    // stas contains the numbers themselves as coming from python
                    // spa is the calculated values in here
                    // only add a new one if there is a difference between, because it might overwrite the previous value
                    if(stas != spa)
                    {
                        rp->addOrUpdatePointDefinitionAssignment(lineNumber, this, x, y, a->varName);
                    }
                }
                else
                if(at == Types::TYPE_LIST)
                {
                    QString domain = rp->domainOfVariable(a->varName);
                    if(domain == Domains::DOMAIN_POINTS)
                    {
                        auto els = QString::fromStdString(v).split(",");
                        QVector<QPointF> newPoints;
                        for(const auto& e : els)
                        {
                            double x = fromString(fromPythonDict(e.toStdString(), X));
                            double y = fromString(fromPythonDict(e.toStdString(), Y));

                            newPoints.append(QPointF{x, y});
                        }
                        a->forceSetPrecalculatedPoints(newPoints);
                    }
                    else
                    {

                    }
                }
            }
        }
    }

    return true;
}

QVector<QSharedPointer<Statement> > PythonScript::create(int ln, const QString &codeline, QStringList &codelines, StatementCallback cb, StatementReaderCallback srcb)
{
    QSharedPointer<PythonScript> pythonScript;
    pythonScript.reset(new PythonScript(ln, codeline));

    QString python_body = codeline.mid(ScriptingLangugages::PYTHON.length());
    consumeSpace(python_body);

    QString scriptKeyword = getDelimitedId(python_body);
    if(scriptKeyword != Keywords::KW_SCRIPT && scriptKeyword != Keywords::KW_DO)
    {
        throw funkplot::syntax_error_exception(ERRORCODE(67), "Missing <b>script</b> or <b>do</b> keyword from python scriptlet in <b>%s</b>", codeline.toStdString().c_str());
    }

    if(scriptKeyword == Keywords::KW_SCRIPT )
    {
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
        else
        {
            throw funkplot::syntax_error_exception(ERRORCODE(66), "Missing <b>begin</b> keyword from python scriptlet in <b>%s</b>", codeline.toStdString().c_str());
        }
    }
    else
    {
        pythonScript->pythonCodeLines.append(python_body);

    }

    return handleStatementCallback( vectorize(pythonScript), cb);
}
