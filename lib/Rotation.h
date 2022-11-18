#ifndef ROTATION_H
#define ROTATION_H

#include "Keywords.h"
#include "Statement.h"
#include <QPointF>

class Function;

/**
 * @brief The Rotation struct represents a rotation
 */
struct Rotation : public Statement
{
    explicit Rotation(int ln, QString a) : Statement(ln, a) {}

    bool execute(RuntimeProvider* rp) override;
    QString keyword() const override
    {
        return Keywords::KW_ROTATE;
    }

    QSharedPointer<Function> degree;
    QString what;
    QString unit = "radians";

    QPointF around {0.0, 0.0};

    QString aroundX = "0.0";
    QString aroundY = "0.0";


};


#endif // ROTATION_H
