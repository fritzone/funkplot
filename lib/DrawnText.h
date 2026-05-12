#ifndef DRAWNTEXT_H
#define DRAWNTEXT_H

#include <QFont>
#include <QPen>
#include <QString>
#include <QPointF>

struct DrawnText
{
    QString text;
    QPen pen {Qt::black};
    QPointF point;
    QFont f;
    bool isCoordinateSystem = false;
};

#endif // DRAWNTEXT_H
