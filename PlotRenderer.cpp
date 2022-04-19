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
    AbstractDrawer::drawCoordinateSystem();
    repaint();
}

void PlotRenderer::drawLine(const QLineF &l, const QPen &p)
{
    drawnLines.append({l, p});
}

int PlotRenderer::sceneX(double x)
{
    int zeroX = rect().width() / 2;

    return zeroX + get_sceneScrollX() + x * zoomFactor();
}

int PlotRenderer::sceneY(double y)
{
    int zeroY = rect().height() / 2;

    return zeroY + get_sceneScrollY() - y * zoomFactor();
}

void PlotRenderer::redrawEverything()
{
    repaint();
}

void PlotRenderer::paintEvent(QPaintEvent* /* event */)
{
    QPainter painter(this);
    QPen pen;
    QBrush brush;

    painter.setPen(pen);
    painter.setBrush(brush);

    for(const auto& dl : drawnLines)
    {
        painter.setPen(dl.pen);
        QPoint p1 = toScene(dl.line.p1());
        QPoint p2 = toScene(dl.line.p2());
        painter.drawLine(QLine{p1, p2});
    }

    for(const auto& dp : drawnPoints)
    {
        painter.setPen(dp.pen);

        QPoint p = toScene(dp.point);

        painter.drawEllipse(p.x() - 1, p.y() - 1, 2.0, 2.0);
    }

}
