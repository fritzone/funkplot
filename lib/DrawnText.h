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
    int stackIndex = 0;        // how many labels already exist at this world position
    QPointF labelDir;          // normalised world-space offset direction; (0,0) → default (8,-8)
    double labelDistance = 12.0; // pixel distance from the point
};

#endif // DRAWNTEXT_H
