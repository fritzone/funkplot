#include "CodeEngine.h"
#include "StatementHandler.h"
#include "RuntimeProvider.h"

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

    if(!codeline.isEmpty())
    {
        try
        {
            bool accepted = false;
            for(const auto& h : HandlerStore::instance().handlerClasses)
            {
                if(h->accepts(codeline))
                {
                    h->execute(ln, codeline, codelines,
                        [this, &statements, parentScope](QSharedPointer<Statement> s) -> bool
                        {
                            statements.append(s);

                            s->parent = parentScope;
                            s->runtimeProvider = m_rp;
                            return true;
                        },
                        [this](int& ln, QStringList &codelines,
                               QVector<QSharedPointer<Statement> > &statements,
                               QSharedPointer<Statement> parentScope) -> QSharedPointer<Statement>
                        {
                            return this->resolveCodeline(ln, codelines, statements, parentScope);
                        }
                    );

                    accepted = true;
                }
            }

            if(!codeline.isEmpty() && !accepted)
            {
                m_rp->reportError(ln, ERRORCODE(35), "Invalid statement: " + codeline);
            }

        }
        catch(funkplot::syntax_error_exception& ex)
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
