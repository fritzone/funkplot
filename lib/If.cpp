#include "If.h"
#include "RuntimeProvider.h"

bool If::execute(RuntimeProvider *rp)
{
    int ln = 0;

    try
    {

        rp->setCurrentStatement(this->statement);
        IndexedAccess* ia = nullptr; Assignment* a = nullptr;
        auto v = expression->Calculate(rp, ia, a);
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
    catch(syntax_error_exception& ex)
    {
        rp->reportError(ln, ex.error_code(), ex.what());
    }
    catch(std::exception& ex)
    {
        rp->reportError(lineNumber, ERRORCODE(41), ex.what());
    }

    return true;
}
