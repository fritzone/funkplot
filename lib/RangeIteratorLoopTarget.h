#ifndef RANGEITERATORLOOPTARGET_H
#define RANGEITERATORLOOPTARGET_H

#include "LoopTarget.h"

#include <QSharedPointer>

struct Loop;

/**
 * @brief The RangeIteratorLoopTarget struct represents a loop target which
 * is range based, such as for loops created like: for i = 0 to 2456 step 2 do
 */
struct RangeIteratorLoopTarget : public LoopTarget
{
    explicit RangeIteratorLoopTarget(QSharedPointer<Loop>);
    RangeIteratorLoopTarget() = delete;

    bool loop(LooperCallback lp, RuntimeProvider*) override;

    QSharedPointer<Function> startFun;
    QSharedPointer<Function> endFun;
    QSharedPointer<Function> stepFun;
};

#endif // RANGEITERATORLOOPTARGET_H
