#include "ImageDrawer.h"

#include <QPainter>
#include <QDebug>

ImageDrawer::ImageDrawer(int w, int h, QWidget *parent) : AbstractDrawer(),
    m_w(w), m_h(h)
{
    m_img.reset(new QImage(m_w, m_h, QImage::Format_RGB32));
}

void ImageDrawer::drawCoordinateSystem()
{
    AbstractDrawer::drawCoordinateSystem();
}

QRect ImageDrawer::getClipRect()
{
    return m_img->rect();
}

void ImageDrawer::redrawEverything()
{
    QPainter painter(m_img.get());
    painter.fillRect(m_img->rect(), "white");

    painter.scale(m_scale,m_scale);
    QPen pen;
    QBrush brush;

    painter.setPen(pen);
    painter.setBrush(brush);

    for(const auto& dl : qAsConst(m_drawnLines))
    {
        painter.setPen(dl.pen);
        qDebug() << dl.pen.width();
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
            painter.setBrush(dp.pen.brush());

            QRect r = QRect{static_cast<int>(p.x() - dp.size), static_cast<int>(p.y() - dp.size),
                            static_cast<int>(2 * dp.size), static_cast<int>(2 * dp.size)};

            painter.drawEllipse(r);
        }
    }

}

void ImageDrawer::drawLine(const QLineF &l, const QPen &p)
{
    m_drawnLines.push_back({l, p});
}

void ImageDrawer::save(const QString &s)
{
    m_img->save(s);
}

QByteArray ImageDrawer::saveToBuffer()
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    m_img->save(&buffer, "PNG");
    return buffer.buffer();
}

QImage ImageDrawer::image() const
{
    return *m_img;
}

void ImageDrawer::setRotationAngle(double newRotationAngle)
{
    AbstractDrawer::setRotationAngle(newRotationAngle);
}

void ImageDrawer::setZoomFactor(double newZoomFactor)
{
    AbstractDrawer::setZoomFactor(newZoomFactor);
}

void ImageDrawer::setShowGrid(bool v)
{
    AbstractDrawer::setShowGrid(v);
}

void ImageDrawer::setCoordEndY(double newCoordEndY)
{
    AbstractDrawer::setCoordEndY(newCoordEndY);
}

void ImageDrawer::setCoordStartY(double newCoordStartY)
{
    AbstractDrawer::setCoordStartY(newCoordStartY);
}

void ImageDrawer::setCoordEndX(double newCoordEndX)
{
    AbstractDrawer::setCoordEndX(newCoordEndX);
}

void ImageDrawer::setCoordStartX(double newCoordStartX)
{
    AbstractDrawer::setCoordStartX(newCoordStartX);
}
