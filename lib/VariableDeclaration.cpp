#include "VariableDeclaration.h"
#include "Statement.h"
#include "util.h"
#include "RuntimeProvider.h"


QVector<QSharedPointer<Statement>> VariableDeclaration::create(int ln, const QString &codeline, QStringList &codelines, StatementCallback cb, StatementReaderCallback srcb)
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
            throw funkplot::syntax_error_exception(ERRORCODE(1), "Invalid variable declaration: <b>%s</b>", s.toStdString().c_str());
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
                throw funkplot::syntax_error_exception(ERRORCODE(2), "Invalid variable type <b>%s</b> in <b>%s", type.toStdString().c_str(), s.toStdString().c_str());
            }

            declI.removeLast();

            for(const auto& vn : qAsConst(declI))
            {
                if(vn == "e")
                {
                    throw funkplot::syntax_error_exception(ERRORCODE(2), "Invalid variable name <b>e</b>. <b>e</b> is reserved for the Euler constant <b>2.71</b>.");
                }

                QSharedPointer<VariableDeclaration> resultI;
                resultI.reset(new VariableDeclaration(ln, codeline));
                resultI->name = vn;
                resultI->type = type;

                result.append(resultI);

                RuntimeProvider::get()->addVariable(vn, type);

                if(type == Types::TYPE_POINT)
                {
                    RuntimeProvider::get()->addOrUpdatePointDefinitionAssignment(ln, resultI.get(), 0, 0, vn);
                }

            }
        }
        else
        {
            if(list_kw_index + 1 < declI.size())
            {
                if(declI[list_kw_index + 1] != Keywords::KW_OF )
                {
                    throw funkplot::syntax_error_exception(ERRORCODE(3), "Missing keyword <b>of</b> in <b>%s</b>",s.toStdString().c_str());
                }
                if(list_kw_index + 2 < declI.size())
                {
                    QString domain = declI[list_kw_index + 2];

                    if(Domains::all().indexOf(domain) == -1)
                    {
                        throw funkplot::syntax_error_exception(ERRORCODE(4), "Invalid list variable type <b>%s</b> in <b>%s", domain.toStdString().c_str(), s.toStdString().c_str());
                    }

                    for(int i =0; i<list_kw_index; i++)
                    {

                        QSharedPointer<VariableDeclaration> resultI;
                        resultI.reset(new VariableDeclaration(ln, codeline));
                        resultI->name = declI[i];
                        resultI->type = Types::TYPE_LIST;

                        result.append(resultI);

                        RuntimeProvider::get()->addVariable(declI[i], Types::TYPE_LIST);
                        RuntimeProvider::get()->specifyVariableDomain(declI[i], domain);
                    }
                }
                else
                {
                    throw funkplot::syntax_error_exception(ERRORCODE(5), "Untyped lists are not supported in <b>%s</b>",s.toStdString().c_str());
                }
            }
            else
            {
                throw funkplot::syntax_error_exception(ERRORCODE(6), "Untyped lists are not supported in <b>%s</b>",s.toStdString().c_str());
            }
        }
    }
    return handleStatementCallback(result, cb);
}
