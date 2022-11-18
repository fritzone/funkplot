#ifndef IMAGEDRAWER_H
#define IMAGEDRAWER_H

#include <AbstractDrawer.h>

#include <QImage>

class ImageDrawer : public AbstractDrawer
{
public:
    ImageDrawer(int w = 640, int h = 480, QWidget* parent = nullptr);

    void drawCoordinateSystem() override;
    QRect getClipRect() override;
    void redrawEverything() override;
    void drawLine(const QLineF&, const QPen&) override;
    double zoomFactor() const override;

    void save(const QString&);
    void setZoom(double z);

    QImage image() const;

private:

    int m_w = 640;
    int m_h = 480;
    double m_zoom = 10.0;

    QSharedPointer<QImage> m_img;

    QPen m_drawingPen {Qt::black};          // the pen used for drawing, holds the color of it
    size_t m_pixelSize = 1;                 // the size of a pixel (pixels are small ellipses)

};

#endif // IMAGEDRAWER_H
