#ifndef DRAWNLINE_H
#define DRAWNLINE_H

#include <QLineF>
#include <QPen>

struct DrawnLine
{
    QLineF line;
    QPen pen {Qt::black};
    int size = 1;
};

#endif // DRAWNLINE_H
