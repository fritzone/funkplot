#ifndef PLOT_H
#define PLOT_H

#include "Keywords.h"
#include "Statement.h"
#include "Stepped.h"

/**
 * @brief The Plot struct represents a "plot" statement.
 */
struct Plot : public Stepped, public Statement, public QEnableSharedFromThis<Plot>
{

    /**
     * @brief Plot Creates a new plot, with the given statement
     */
    explicit Plot(int ln, const QString& s);

    /**
     * @brief keyword returns "plot"
     */
    QString keyword() const override;

    bool execute(RuntimeProvider* rp) override;

    QString plotTarget; // which is the target of this plot (a function, a point, a list of points)

};

#endif // PLOT_H
