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
        Point,
        Angle
    };

    Type drawType;
    QString targetVar;   // segment var (draw segment / draw angle between)
    QString targetVar2;  // second segment var (draw angle between s1 and s2)
    QSharedPointer<Function> x1, y1, x2, y2, x3, y3;
    QSharedPointer<Function> count;
    int ticks    = 0; // segment: N tick marks at midpoint (congruency class N)
    int arcCount = 1; // angle:   N concentric arcs       (congruency class N)
    QString angleLabel;  // "ABC" → label arm1=A, vertex=B, arm2=C; draws dots too
    QString pointLabel;  // optional label for "draw point ... as \"X\""
    QString drawColor;   // optional color override for angle drawing (empty = inherit)
    bool doubleLine = false; // draw angle arms/arcs with 2x current line width
};

REGISTER_STATEMENTHANDLER(Draw)

#endif // DRAW_H
