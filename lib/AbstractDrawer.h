#ifndef ABSTRACT_DRAWER
#define ABSTRACT_DRAWER

#include <QPen>

#include "DrawnLine.h"
#include "DrawnPoint.h"

class AbstractExporter;

/**
 * @brief The AbstractDrawer class is a base class for the drawing components of the application
 */
class AbstractDrawer
{

public:
    explicit AbstractDrawer() = default;

    virtual void drawCoordinateSystem();

    virtual QRect getClipRect() = 0;
    virtual void redrawEverything() = 0;
    virtual void drawLine(const QLineF&, const QPen&) = 0;
    virtual double zoomFactor() const;
    //virtual void drawText(const QPoint&, const QString&, const QPen&) = 0;


    QPoint toScene(QPointF f);
    double rotationAngle();
    double coordStartX();
    double coordEndX();
    double coordStartY();
    double coordEndY();
    int getSceneScrollX() const;
    int getSceneScrollY() const;
    void reset();
    int sceneX(double x);
    int sceneY(double y);
    const std::vector<DrawnLine> &getDrawnLines() const;
    void setDrawnLines(const std::vector<DrawnLine> &newDrawnLines);
    const std::vector<DrawnPoint> &getDrawnPoints() const;
    void setDrawnPoints(const std::vector<DrawnPoint> &newDrawnPoints);
    void setRotationAngle(double newRotationAngle);
    void setZoomFactor(double newZoomFactor);
    void setShowGrid(bool v);
    void setCoordEndY(double newCoordEndY);
    void setCoordStartY(double newCoordStartY);
    void setCoordEndX(double newCoordEndX);
    void setCoordStartX(double newCoordStartX);
    void addLine(QLineF l, QPen p, int size);
    void addPoint(QPointF l, QPen p, size_t s);

protected:

    int m_sceneScrollX = 0;
    int m_sceneScrollY = 0;
    std::vector<DrawnLine> m_drawnLines;
    std::vector<DrawnPoint> m_drawnPoints;
    double m_scale = 1.0;
    double m_rotationAngle = 0.0;
    double m_zoomFactor = 50.0;
    double m_coordEndY = 10.0;
    double m_coordStartY = -10.0;
    double m_coordEndX = 15.0;
    double m_coordStartX = -15.0;
    bool m_drawGrid = true;
};


#endif // !ABSTRACT_DRAWER

