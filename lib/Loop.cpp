#include "Loop.h"
#include "RuntimeProvider.h"

bool Loop::execute(RuntimeProvider *rp)
{
    int ln = 0;
    auto looper = [&rp, &ln, this]()
    {
        try
        {
            for(const auto& stmt : qAsConst(body))
            {
                // qInfo() << "LOOP: Running:" << stmt->statement;
                if(!rp->isRunning())
                {
                    break;
                }

                ln = stmt->lineNumber;
                rp->setCurrentStatement(stmt->statement);
                stmt->execute(rp);
            }
        }
        catch(syntax_error_exception& ex)
        {
            rp->reportError(ln, ex.error_code(), ex.what());
        }
        catch(std::exception& ex)
        {
            rp->reportError(ln, ERRORCODE(41), ex.what());
        }
    };

    try
    {
        rp->setCurrentStatement(this->statement);
        loop_target->loop(looper, rp);
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

void Loop::updateLoopVariable(double v)
{
    runtimeProvider->setValue(loop_variable, v);
}

void Loop::updateLoopVariable(QPointF v)
{
    runtimeProvider->setValue(loop_variable, v);
}
