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
    IndexedAccess* ia = nullptr; Assignment* a = nullptr;
    double v = startFun->Calculate(rp, ia, a);
    double e = endFun->Calculate(rp, ia, a);

    double stepv = stepFun->Calculate(rp, ia, a);

    if(v < e && stepv < 0.0 || v > e && stepv > 0.0)
    {
        throw syntax_error_exception(ERRORCODE(0), "Erroneous looping conditions detected, check your range");
    }

    do
    {
        theLoop->updateLoopVariable(v);
        lp();
        IndexedAccess* ia_l = nullptr; Assignment* a_l = nullptr;
        stepv = stepFun->Calculate(rp, ia_l, a_l);
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
