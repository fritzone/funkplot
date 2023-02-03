#ifndef POLAR_H
#define POLAR_H

#include "Keywords.h"
#include "Statement.h"
#include "Stepped.h"
#include "StatementHandler.h"

class Plot;
/**
 * @brief The Polar struct represents a "Polar" statement.
 */
struct Polar : public Stepped, public Statement, public QEnableSharedFromThis<Polar>
{

    Polar() = default;

    /**
     * @brief Polar Creates a new Polar, with the given statement
     */
    explicit Polar(int ln, const QString& s) : Statement(ln, s) {}

    /**
     * @brief keyword returns "Polar"
     */
    QString keyword() const override
    {
        return kw();
    }

    static QString kw()
    {
        return Keywords::KW_POLAR;
    }

    static QVector<QSharedPointer<Statement>> create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback srcb);

    bool execute(RuntimeProvider* rp) override;

    QSharedPointer<Plot> target;

};

REGISTER_STATEMENTHANDLER(Polar)

#endif // POLAR_H
