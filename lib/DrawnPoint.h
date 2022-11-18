#ifndef DRAWNPOINT_H
#define DRAWNPOINT_H

#include <QPen>
#include <QPointF>

struct DrawnPoint
{
    QPointF point;
    QPen pen;
    size_t size = 1;
};

#endif // DRAWNPOINT_H
