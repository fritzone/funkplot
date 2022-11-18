#ifndef FUNCTIONITERATORLOOPTARGET_H
#define FUNCTIONITERATORLOOPTARGET_H

#include "LoopTarget.h"

/**
 * @brief The FunctionIteratorLoopTarget struct represents a loop target
 * which is the iteration of a points of a plot/function
 */
struct FunctionIteratorLoopTarget : public LoopTarget
{
    explicit FunctionIteratorLoopTarget(QSharedPointer<Loop>);
    FunctionIteratorLoopTarget() = delete;

    bool loop(LooperCallback, RuntimeProvider*) override;

};

#endif // FUNCTIONITERATORLOOPTARGET_H
