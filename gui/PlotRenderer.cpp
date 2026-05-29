#include "PlotRenderer.h"

#include <QPainter>
#include <QPainterPath>
#include <QRect>
#include <QDebug>
#include <RuntimeProvider.h>

#include <cmath>

PlotRenderer::PlotRenderer(QWidget* parent) : QWidget(parent), ScreenDrawer()
{
    setFlexibleStyle();
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

QSize PlotRenderer::sizeHint() const
{
    qInfo() << QSize{getWidth(), getHeight()};

    return {getWidth(), getHeight()};
}

void PlotRenderer::setFixedStyle()
{
    setFlexibleStyle();

    QSizePolicy sp;
    sp.setHorizontalPolicy(QSizePolicy::Fixed);
    sp.setVerticalPolicy(QSizePolicy::Fixed);

    setSizePolicy(sp);
}

void PlotRenderer::setFlexibleStyle()
{
    QSizePolicy sp;
    sp.setHorizontalPolicy(QSizePolicy::Expanding);
    sp.setVerticalPolicy(QSizePolicy::Expanding);

    setSizePolicy(sp);
}

void PlotRenderer::paintEvent(QPaintEvent* event)
{
    if(RuntimeProvider::get()->isRunning())
    {
        return;
    }

    QPainter painter(this);
    painter.fillRect(rect(), "white");

    QPen pen;
    painter.setPen(pen);

    painter.drawRect(rect());

    QBrush brush;
    painter.setBrush(brush);

    qInfo() << "DrawnLinesSize:" << m_drawnLines.size();

    for(const auto& dl : m_drawnLines)
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

    for(const auto& dp : m_drawnPoints)
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

    for(const auto& dt : m_drawnText)
    {
        painter.setPen(dt.pen);
        painter.setFont(dt.f);

        QPoint p = toScene(dt.point);
        if (dt.isCoordinateSystem)
        {
            painter.drawText(p, dt.text);
        }
        else
        {
            if (dt.labelDir.x() != 0.0 || dt.labelDir.y() != 0.0)
            {
                // world-space direction → screen: flip y, keep x
                double sx = dt.labelDir.x();
                double sy = -dt.labelDir.y();
                double L = dt.labelDistance + dt.stackIndex * 14.0;
                p += QPoint(qRound(sx * L), qRound(sy * L));
            }
            else
            {
                p += QPoint(8, -8 - dt.stackIndex * 14);
            }
            drawLabelText(painter, p, dt.text);
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
