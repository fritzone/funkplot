#ifndef ARRAYITERATORLOOPTARGET_H
#define ARRAYITERATORLOOPTARGET_H

#include "LoopTarget.h"

/**
 * @brief The FunctionIteratorLoopTarget struct represents a loop target
 * which is the iteration of a points of a plot/function
 */
struct ArrayIteratorLoopTarget : public LoopTarget
{
    explicit ArrayIteratorLoopTarget(QSharedPointer<Loop>);
    ArrayIteratorLoopTarget() = delete;

    bool loop(LooperCallback, RuntimeProvider*) override;

};

#endif // ARRAYITERATORLOOPTARGET_H
