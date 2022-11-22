#ifndef DRAWNTEXT_H
#define DRAWNTEXT_H

#include <QFont>
#include <QPen>
#include <QString>

struct DrawnText
{
    QString text;
    QPen pen {Qt::black};
    QPointF point;
    QFont f;
};

#endif // DRAWNTEXT_H
