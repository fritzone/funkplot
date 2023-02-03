#include "Parametric.h"
#include "RuntimeProvider.h"
#include "Function.h"
#include "util.h"

QVector<QSharedPointer<Statement> > Parametric::create(int ln, const QString &codeline, QStringList &codelines, StatementCallback cb, StatementReaderCallback srcb)
{
    QSharedPointer<Parametric> result;
    result.reset(new Parametric(ln, codeline));

    QString f_body = codeline.mid(Keywords::KW_PARAMETRIC.length());
    consumeSpace(f_body);
    f_body = f_body.mid(Keywords::KW_FUNCTION.length());
    consumeSpace(f_body);

    QString sp;
    result->funName = extract_proper_expression(f_body, sp, {'('}, {}, true).simplified();
    consumeSpace(f_body);
    if(f_body.isEmpty())
    {
        throw syntax_error_exception(ERRORCODE(69), "Invalid parametric function definition: %s. Missing <b>function</b> in declaration.", codeline.toLatin1().data());
    }
    if(f_body.at(0) != '(')
    {
        throw syntax_error_exception(ERRORCODE(69), "Invalid parametric function definition: %s. Missing parenthesis in declaration.", codeline.toLatin1().data());
    }
    f_body = f_body.mid(1);
    QString fun_parname = extract_proper_expression(f_body, sp, {')'}, {}, true);
    result->funParName = fun_parname.simplified();

    // read in the body
    bool done = false;

    while(!codelines.isEmpty() && !done)
    {
        QString at0 = codelines.at(0).simplified();
        codelines.pop_front();

        if(at0 == Keywords::KW_DONE || at0 == Keywords::KW_END )
        {
            break;
        }

        qDebug() << "pm try" << at0;

        if(! (at0.isEmpty() || at0[0] == '#'))
        {

            if(at0.startsWith("x"))
            {
                result->functions.first = result->extractFunction(at0);
            }
            else
            if(at0.startsWith("y"))
            {
                result->functions.second = result->extractFunction(at0);
            }
            else
            {
                throw syntax_error_exception(ERRORCODE(70), "Invalid parametric function definition: %s. Only the coordinates <b>x</b> and <b>y</b> are supported.", codeline.toLatin1().data());
            }

        }
    }

    RuntimeProvider::get()->addParametricFunction(result);

    return handleStatementCallback( vectorize(result), cb);
}

QSharedPointer<Function> Parametric::extractFunction(QString s)
{
    QString cn = s.at(0);
    // skip x or y
    s = s.mid(1);
    consumeSpace(s);

    if(!s.isEmpty() && s.at(0) != '=')
    {
        throw syntax_error_exception(ERRORCODE(71), "Invalid parametric function definition: %s. Need an equality sign here", s.toLatin1().data());
    }

    QString tempFun = funName + "_" + cn + "(" + funParName + ")" + s;
    QSharedPointer<Function> f { new Function(tempFun.toLocal8Bit().data(), this) };

    return f;

}

QPointF Parametric::calculate(double t)
{
    functions.first->SetVariable(funParName.toStdString(), t);
    functions.second->SetVariable(funParName.toStdString(), t);

    return QPointF{functions.first->Calculate(), functions.second->Calculate()};
}

QPointF Parametric::calculate()
{

    return QPointF{functions.first->Calculate(), functions.second->Calculate()};

}
