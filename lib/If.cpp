#include "If.h"
#include "RuntimeProvider.h"

bool If::execute(RuntimeProvider *rp)
{
    int ln = 0;

    try
    {

        rp->setCurrentStatement(this->statement);
        auto v = expression->Calculate();
        if(v != 0.0)
        {
            for(const auto& stmt : qAsConst(ifBody))
            {
                qInfo() << "IF: Running:" << stmt->statement;
                if(!rp->isRunning())
                {
                    break;
                }

                ln = stmt->lineNumber;
                rp->setCurrentStatement(stmt->statement);
                stmt->execute(rp);
            }
        }
        else
        {
            if(!elseBody.isEmpty())
            {
                for(const auto& stmt : qAsConst(elseBody))
                {
                    qInfo() << "IF/ELSE: Running:" << stmt->statement;
                    if(!rp->isRunning())
                    {
                        break;
                    }

                    ln = stmt->lineNumber;
                    rp->setCurrentStatement(stmt->statement);
                    stmt->execute(rp);
                }
            }
        }
    }
    catch(funkplot::syntax_error_exception& ex)
    {
        rp->reportError(ln, ex.error_code(), ex.what());
    }
    catch(std::exception& ex)
    {
        rp->reportError(lineNumber, ERRORCODE(41), ex.what());
    }

    return true;
}

QVector<QSharedPointer<Statement> > If::create(int ln, const QString &codeline, QStringList &codelines, StatementCallback cb, StatementReaderCallback srcb)
{
    QString if_decl = codeline.mid(Keywords::KW_IF.length());
    consumeSpace(if_decl);

    QSharedPointer<If> result = nullptr;
    result.reset(new If(ln, codeline));

    auto rest = if_decl.split(" ");
    QString if_condition;

    if(rest.isEmpty() || rest.at(rest.length() -1) != Keywords::KW_DO)
    {
        throw funkplot::syntax_error_exception(777, "No do keyword in if statement");
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
                st = srcb(ln, codelines, result->ifBody, result);
            }
            catch(funkplot::syntax_error_exception& a)
            {
                RuntimeProvider::get()->reportError(ln, a.error_code(), a.what());
                throw;
            }

            if(st)
            {
                lastKeyword = st->keyword();
                done = st->keyword() == Keywords::KW_DONE || st->keyword() == Keywords::KW_ELSE;
            }
            else
            {
                throw funkplot::syntax_error_exception(ERRORCODE(42), "something is wrong with this expression: %s", codeline.toStdString().c_str());
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
                    st = srcb(ln, codelines, result->elseBody, result);
                }
                catch(funkplot::syntax_error_exception& a)
                {
                    RuntimeProvider::get()->reportError(ln, a.error_code(), a.what());
                    throw;
                }

                if(st)
                {
                    lastKeyword = st->keyword();
                    done = st->keyword() == Keywords::KW_DONE;
                }
                else
                {
                    throw funkplot::syntax_error_exception(ERRORCODE(42), "something is wrong with this expression: %s", codeline.toStdString().c_str());
                }
            }
        }
    }

    if(!done)
    {
        throw funkplot::syntax_error_exception(ERRORCODE(43), "for body does not end with done");
    }

    return handleStatementCallback(vectorize(result), cb);

}
