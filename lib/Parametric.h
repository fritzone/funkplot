#ifndef PARAMETRIC_H
#define PARAMETRIC_H

#include "Statement.h"
#include "Keywords.h"
#include "StatementHandler.h"
#include "Function.h"

class RuntimeProvider;
class Function;

/**
 * @brief The Parametric struct represents the Parametric keyword
 */
struct Parametric : public Statement
{
    Parametric() = default;

    explicit Parametric(int ln, const QString& s) : Statement(ln, s) {}

    QString keyword() const override
    {
        return kw();
    }

    static QString kw()
    {
        return Keywords::KW_PARAMETRIC;
    }

    static QVector<QSharedPointer<Statement>> create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback srcb);

    QSharedPointer<Function> extractFunction(QString s);

    QPointF calculate(double t);
    QPointF calculate();

public:

    QPair<QSharedPointer<Function>, QSharedPointer<Function>> functions;
    QString funName;
    QString funParName;


};

REGISTER_STATEMENTHANDLER(Parametric)

#endif // PARAMETRIC_H
