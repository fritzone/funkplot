#include "RangeIteratorLoopTarget.h"
#include "Function.h"
#include "IndexedAccess.h"
#include "Assignment.h"
#include "Loop.h"
#include "util.h"

RangeIteratorLoopTarget::RangeIteratorLoopTarget(QSharedPointer<Loop> l)
{
    theLoop = l;
}

bool RangeIteratorLoopTarget::loop(LooperCallback lp, RuntimeProvider* rp)
{
    double v = startFun->Calculate();
    double e = endFun->Calculate();

    double stepv = stepFun->Calculate();

    if(v < e && stepv < 0.0 || v > e && stepv > 0.0)
    {
        throw funkplot::syntax_error_exception(ERRORCODE(0), "Erroneous looping conditions detected, check your range");
    }

    do
    {
        theLoop->updateLoopVariable(v);
        lp();
        stepv = stepFun->Calculate();
        v += stepv;

        if(stepv < 0.0 && v < e)
        {
            break;
        }

        if(stepv > 0.0 && v > e)
        {
            break;
        }


    }
    while(true);
    return true;
}
