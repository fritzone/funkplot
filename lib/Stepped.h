#ifndef STEPPED_H
#define STEPPED_H

#include <QSharedPointer>

class Function;
class Statement;

/**
 * @brief The Stepped struct is representing something tha can be stepped, such
 * as a loop or a plot. A stepped object can be either continuous (meaning, there will
 * be lines drawn from a previous point to the current one) or counted, meaning
 * there can be a maximum of N points in it.
 */
struct Stepped
{
    Stepped() noexcept;
    virtual ~Stepped() = default;

    void init();

    bool continuous = false;
    bool counted = false;

    QSharedPointer<Function> step; // used as specifying the delta between two consecutive plots distances.
    QSharedPointer<Function> start;
    QSharedPointer<Function> end;

    QString stepValue; // the step value as string

    static void resolveOverKeyword(QString codeline, QSharedPointer<Stepped> stepped, Statement *s);

    static void resolveCountsKeyword(QString codeline, QSharedPointer<Stepped> stepped, Statement *s);

};

#endif // STEPPED_H
