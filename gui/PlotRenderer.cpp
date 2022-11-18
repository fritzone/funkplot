#include "PlotRenderer.h"

#include <QPainter>
#include <QPainterPath>
#include <QRect>
#include <QDebug>
#include <RuntimeProvider.h>

#include <cmath>

PlotRenderer::PlotRenderer(QWidget* parent) : QWidget(parent), ScreenDrawer()
{
    QSizePolicy sp;
    sp.setHorizontalPolicy(QSizePolicy::Expanding);
    sp.setVerticalPolicy(QSizePolicy::Expanding);

    setSizePolicy(sp);
    installEventFilter(this);
}

void PlotRenderer::drawCoordinateSystem()
{
    AbstractDrawer::drawCoordinateSystem();
    repaint();
}

void PlotRenderer::drawLine(const QLineF &l, const QPen &p)
{
    AbstractDrawer::addLine(l, p, 1);
    //m_drawnLines.append({l, p});
}

void PlotRenderer::redrawEverything()
{
    repaint();
}

void PlotRenderer::paintEvent(QPaintEvent* event)
{
    if(RuntimeProvider::get()->isRunning())
    {
        return;
    }

    QPainter painter(this);
    painter.fillRect(rect(), "white");

    painter.scale(m_scale,m_scale);
    QPen pen;
    QBrush brush;

    painter.setPen(pen);
    painter.setBrush(brush);

    for(const auto& dl : qAsConst(m_drawnLines))
    {
        QPen p(dl.pen);
        if(dl.size != dl.pen.width())
        {
            p.setWidth(dl.size);
        }
        painter.setPen(p);

        QPoint p1 = toScene(dl.line.p1());
        QPoint p2 = toScene(dl.line.p2());
        painter.drawLine(QLine{p1, p2});
    }

    for(const auto& dp : qAsConst(m_drawnPoints))
    {
        painter.setPen(dp.pen);

        QPoint p = toScene(dp.point);
        if(dp.size == 1)
        {
            painter.drawPoint(p);
        }
        else
        {
            QBrush b = dp.pen.brush();
            QColor c = b.color();
            c.setAlpha(dp.pen.color().alpha());
            painter.setBrush(c);

            QRect r = QRect{static_cast<int>(p.x() - dp.size), static_cast<int>(p.y() - dp.size),
                            static_cast<int>(2 * dp.size), static_cast<int>(2 * dp.size)};

            painter.drawEllipse(r);
        }
    }

}


void PlotRenderer::mousePressEvent(QMouseEvent *event)
{
    ScreenDrawer::mousePressEvent(event);
}

void PlotRenderer::mouseMoveEvent(QMouseEvent *event)
{
    ScreenDrawer::mouseMoveEvent(event);
    repaint();
}

void PlotRenderer::mouseReleaseEvent(QMouseEvent *event)
{
    ScreenDrawer::mouseReleaseEvent(event);
}

void PlotRenderer::wheelEvent(QWheelEvent *event)
{
    ScreenDrawer::wheelEvent(event);
    repaint();
}

QRect PlotRenderer::getClipRect()
{
    return rect();
}
