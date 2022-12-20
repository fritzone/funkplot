#ifndef IMAGEDRAWER_H
#define IMAGEDRAWER_H

#include <AbstractDrawer.h>

#include <QBuffer>
#include <QImage>

class ImageDrawer :  public QObject, public AbstractDrawer
{
    Q_OBJECT

public:
    ImageDrawer(int w = 640, int h = 480, QWidget* parent = nullptr);

    void drawCoordinateSystem() override;
    QRect getClipRect() override;
    void redrawEverything() override;
    void drawLine(const QLineF&, const QPen&) override;

    void save(const QString&);
    QByteArray saveToBuffer();

    QImage image() const;


public slots:

    void setRotationAngle(double newRotationAngle);
    void setZoomFactor(double newZoomFactor);
    void setShowGrid(bool v);
    void setCoordEndY(double newCoordEndY);
    void setCoordStartY(double newCoordStartY);
    void setCoordEndX(double newCoordEndX);
    void setCoordStartX(double newCoordStartX);


private:

    int m_w = 640;
    int m_h = 480;

    QSharedPointer<QImage> m_img;

    QPen m_drawingPen {Qt::black};          // the pen used for drawing, holds the color of it
    size_t m_pixelSize = 1;                 // the size of a pixel (pixels are small ellipses)

};

#endif // IMAGEDRAWER_H
