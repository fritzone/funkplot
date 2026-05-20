#ifndef DRAW_H
#define DRAW_H

#include "Keywords.h"
#include "Statement.h"
#include "StatementHandler.h"
#include "Function.h"

/**
 * @brief The Draw struct represents a "draw" statement.
 */
struct Draw : public Statement, public QEnableSharedFromThis<Draw>
{
    Draw() = default;

    /**
     * @brief Draw Creates a new draw statement
     */
    explicit Draw(int ln, const QString& s);

    /**
     * @brief keyword returns "draw"
     */
    QString keyword() const override;

    static QString kw()
    {
        return Keywords::KW_DRAW;
    }

    static QVector<QSharedPointer<Statement>> create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback srcb);

    bool execute(RuntimeProvider* rp) override;

public:
    enum Type {
        Line,
        Segment,
        Point
    };

    Type drawType;
    QString targetVar;
    QSharedPointer<Function> x1, y1, x2, y2;
    QSharedPointer<Function> count;
};

REGISTER_STATEMENTHANDLER(Draw)

#endif // DRAW_H
