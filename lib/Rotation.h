#ifndef ROTATION_H
#define ROTATION_H

#include "Keywords.h"
#include "Statement.h"
#include "StatementHandler.h"
#include <QPointF>

class Function;

/**
 * @brief The Rotation struct represents a rotation
 */
struct Rotation : public Statement
{
    Rotation() = default;
    explicit Rotation(int ln, QString a) : Statement(ln, a) {}

    bool execute(RuntimeProvider* rp) override;
    QString keyword() const override
    {
        return Keywords::KW_ROTATE;
    }

    static QString kw()
    {
        return Keywords::KW_ROTATE;
    }

    static QVector<QSharedPointer<Statement>> create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback srcb);

    QSharedPointer<Function> degree;
    QString what;
    QString unit = "radians";

    QPointF around {0.0, 0.0};

    QString aroundX = "0.0";
    QString aroundY = "0.0";


};

REGISTER_STATEMENTHANDLER(Rotation)

#endif // ROTATION_H
