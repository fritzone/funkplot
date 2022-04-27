#ifndef STEPPED_H
#define STEPPED_H

#include <QSharedPointer>

class Function;

/**
 * @brief The Stepped struct is representing something tha can be stepped, such
 * as a loop or a plot. A stepped object can be either continuous (meaning, there will
 * be lines drawn from a previous point to the current one) or counted, meaning
 * there can be a maximum of N points in it.
 */
struct Stepped
{
    Stepped() noexcept;

    bool continuous = false;
    bool counted = false;

    QSharedPointer<Function> step;
    QSharedPointer<Function> start;
    QSharedPointer<Function> end;

};

#endif // STEPPED_H
