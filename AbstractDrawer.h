#ifndef ABSTRACT_DRAWER
#define ABSTRACT_DRAWER

#include <QPen>
#include <QWidget>


struct DrawnLine
{
    QLineF line;
    QPen pen;
};

struct DrawnPoint
{
    QPointF point;
    QPen pen;
};


class AbstractDrawer
{

public:
    explicit AbstractDrawer(QWidget* parent = nullptr){}

    virtual void resizeEvent(QResizeEvent* event) = 0;

    virtual void drawCoordinateSystem();

    virtual int sceneX(double x) = 0;
    virtual int sceneY(double y) = 0;


    QPoint toScene(QPointF f);

    virtual void drawLine(const QLineF&, const QPen&) = 0;

    double coordStartX()
    {
        return -15.0;
    }

    double coordEndX()
    {
        return 15.0;
    }

    double coordStartY()
    {
        return -10.0;
    }

    double coordEndY()
    {
        return 10.0;
    }

    double zoomFactor()
    {
        return 50.0;
    }

    double rotationAngle()
    {
        return 0;
    }
    int get_sceneScrollX() const
    {
        return sceneScrollX;
    }

    int get_sceneScrollY() const
    {
        return sceneScrollY;
    }

    void reset();
    void addLine(QLineF l, QPen p);
    void addPoint(QPointF l, QPen p);
    virtual void redrawEverything() = 0;


protected:
    int sceneScrollX = 0;
    int sceneScrollY = 0;
    QVector<DrawnLine> drawnLines;
    QVector<DrawnPoint> drawnPoints;

};


#endif // !ABSTRACT_DRAWER

