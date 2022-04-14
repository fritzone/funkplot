#include "PlotRenderer.h"
#include "qdebug.h"

#include <QPainter>
#include <QPainterPath>
#include <QRect>

PlotRenderer::PlotRenderer(QWidget* parent) : AbstractDrawer(parent)
{

}

void PlotRenderer::resizeEvent(QResizeEvent* event)
{
}

void PlotRenderer::drawCoordinateSystem()
{

}

QPoint PlotRenderer::toScene(QPointF)
{
    return {0, 0};
}

void PlotRenderer::paintEvent(QPaintEvent* /* event */)
{
    qWarning() << "AAAAAAAAAAAAAAAAAAAAAAaa";

    QPainter painter(this);
    QPen pen;
    QBrush brush;

    painter.setPen(pen);
    painter.setBrush(brush);

    static const QPoint points[4] = {
        QPoint(10, 80),
        QPoint(20, 10),
        QPoint(80, 30),
        QPoint(90, 70)
    };

    QRect rect(10, 20, 80, 60);

    QPainterPath path;
    path.moveTo(20, 80);
    path.lineTo(20, 30);
    path.cubicTo(80, 0, 50, 50, 80, 80);

    painter.drawLine(rect.bottomLeft(), rect.topRight());
    painter.drawPath(path);

}
