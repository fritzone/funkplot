#ifndef PLOT_H
#define PLOT_H

#include "Keywords.h"
#include "Statement.h"
#include "Stepped.h"
#include "StatementHandler.h"

/**
 * @brief The Plot struct represents a "plot" statement.
 */
struct Plot : public Stepped, public Statement, public QEnableSharedFromThis<Plot>
{

    Plot() = default;

    /**
     * @brief Plot Creates a new plot, with the given statement
     */
    explicit Plot(int ln, const QString& s);

    /**
     * @brief keyword returns "plot"
     */
    QString keyword() const override;

    static QString kw()
    {
        return Keywords::KW_PLOT;
    }


    static QVector<QSharedPointer<Statement>> create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback srcb);

    bool execute(RuntimeProvider* rp) override;

public:

    QString plotTarget; // which is the target of this plot (a function, a point, a list of points)
    bool polarPlot = false;

};

REGISTER_STATEMENTHANDLER(Plot)

#endif // PLOT_H
