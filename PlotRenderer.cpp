#include "PlotRenderer.h"
#include "qdebug.h"

#include <QPainter>
#include <QPainterPath>
#include <QRect>

PlotRenderer::PlotRenderer(QWidget* parent) : AbstractDrawer(parent)
{
    this->installEventFilter(this);
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
    painter.fillRect(rect(), "white");

    painter.scale(scale,scale);
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

        painter.drawPoint(p);
    }

}


void PlotRenderer::mousePressEvent(QMouseEvent *event)
{
    AbstractDrawer::mousePressEvent(event);
}

void PlotRenderer::mouseMoveEvent(QMouseEvent *event)
{
    AbstractDrawer::mouseMoveEvent(event);
    repaint();
}

void PlotRenderer::mouseReleaseEvent(QMouseEvent *event)
{
    AbstractDrawer::mouseReleaseEvent(event);
}

void PlotRenderer::wheelEvent(QWheelEvent *event)
{
    AbstractDrawer::wheelEvent(event);
    repaint();
}
