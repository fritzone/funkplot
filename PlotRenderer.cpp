#include "PlotRenderer.h"

#include <QPainterPath>
#include <QRect>

PlotRenderer::PlotRenderer(QWidget* parent) : AbstractDrawer(parent)
{

}

void PlotRenderer::resizeEvent(QResizeEvent* event)
{
}

void PlotRenderer::paintEvent(QPaintEvent* /* event */)
{
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
}