#include "Append.h"
#include "ArrayAssignment.h"
#include "PointArrayAssignment.h"
#include "RuntimeProvider.h"
#include "Function.h"

bool Append::execute(RuntimeProvider* rp)
{
    if(domain == Domains::DOMAIN_NUMBERS)
    {
        auto arrayAssignment = rp->getAssignmentAs<ArrayAssignment>(varName);

        if(arrayAssignment)
        {
            for(int idx_v = 0; idx_v < numberValues.size(); idx_v ++)
            {
                arrayAssignment->m_elements.append(Function::temporaryFunction(numberValues[idx_v], this));
            }
        }
    }
    else
    if(domain == Domains::DOMAIN_POINTS)
    {
        auto pointArrayAssignment = rp->getAssignmentAs<PointArrayAssignment>(varName);

        if(pointArrayAssignment)
        {
            for(int idx_v = 0; idx_v < pointValues.size(); idx_v ++)
            {
                auto p = pointValues[idx_v];

                double x = std::get<0>(p)->Calculate();
                double y = std::get<1>(p)->Calculate();

                auto fx = Function::temporaryFunction(QString::number(x, 'f', 6), this);
                auto fy = Function::temporaryFunction(QString::number(y, 'f', 6), this);


                pointArrayAssignment->m_elements.append({fx, fy});
            }

            pointArrayAssignment->resolvePrecalculatedPointsForIndexedAccessWithList(nullptr, rp);
            pointArrayAssignment->setPrecalculatedSetForce(true);
        }
    }
    return true;
}

QVector<QSharedPointer<Statement> > Append::create(int ln, const QString &codeline, QStringList &codelines, StatementCallback cb, StatementReaderCallback)
{
    QVector<QSharedPointer<Statement>> resultList;

    QSharedPointer<Append> result;
    result.reset(new Append(ln, codeline));

    QString append_body = codeline.mid(Keywords::KW_APPEND.length());
    consumeSpace(append_body);

    if(!append_body.startsWith(Keywords::KW_TO))
    {
        throw funkplot::syntax_error_exception(ERRORCODE(59), "<b>append</b> statement does not contain the <b>to</b> keyword in: <b>%s</b>", codeline.toStdString().c_str());
    }

    append_body = append_body.mid(Keywords::KW_TO.length());
    consumeSpace(append_body);

    result->varName = getDelimitedId(append_body);

    if(!RuntimeProvider::get()->hasVariable(result->varName))
    {
        throw funkplot::syntax_error_exception(ERRORCODE(8), "Undeclared variable <b>%s</b> while parsing: <b>%s</b>", result->varName.toStdString().c_str(), codeline.toStdString().c_str());
    }

    result->domain = getDelimitedId(append_body)    ;
    if(!Domains::all().contains(result->domain))
    {
        throw funkplot::syntax_error_exception(ERRORCODE(63), "Unsupported domain <b>%s</b> while parsing: <b>%s</b>", result->domain.toStdString().c_str(), codeline.toStdString().c_str());
    }

    QString domainOfVar = RuntimeProvider::get()->domainOfVariable(result->varName);
    if(result->domain != domainOfVar)
    {
        throw funkplot::syntax_error_exception(ERRORCODE(8), "Incompatible domain types for append, found <b>%s</b> expected <b>%s</b> while parsing <b>%s</b>",
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
        append_body.remove(' ');
        append_body.replace("),(", ");(");

        QStringList arrayElements = append_body.split(";");
        for(QString e : arrayElements)
        {
            if(!e.startsWith("("))
            {
                throw funkplot::syntax_error_exception(ERRORCODE(35), "Invalid point array append: missing <b>(</b> from <b>%s</b>", codeline.toStdString().c_str());
            }
            e = e.mid(1);
            QString sx = getDelimitedId(e, {','});
            QString sy = getDelimitedId(e, {')'});

            auto t = std::make_tuple(Function::temporaryFunction(sx, result.get()), Function::temporaryFunction(sy, result.get()));
            result->pointValues.append(t);
        }

    }
    resultList.append(result);

    return handleStatementCallback(resultList, cb);
}
