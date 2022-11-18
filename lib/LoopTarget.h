#ifndef LOOPTARGET_H
#define LOOPTARGET_H

#include "Stepped.h"

#include <functional>

struct Loop;
class RuntimeProvider;

/**
 * @brief The LoopTarget struct represents the part of a for loop which is
 * responsible for tracking the target of the loop, such as the points of a plot
 * or a ranged loop
 */
struct LoopTarget : public Stepped
{

    LoopTarget() = default;
    virtual ~LoopTarget() = default;

    using LooperCallback = std::function<void()>;

    /**
     * @brief loop This should be overridden in concrete classes.
     *
     * @param lpc is usually a lambda in the code which takes care of the "body"
     * of the loop
     *
     * @return true if the looping succeeded, false otherwise
     */
    virtual bool loop(LooperCallback lpc, RuntimeProvider*) = 0;

    QSharedPointer<Loop> theLoop;   // to which loop this target belongs to
    QString name;                   // the name of the "variable" of the loop
};


#endif // LOOPTARGET_H
