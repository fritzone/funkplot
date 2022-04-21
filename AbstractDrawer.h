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
    virtual void redrawEverything() = 0;
    virtual void drawLine(const QLineF&, const QPen&) = 0;
    //virtual void drawText(const QPoint&, const QString&, const QPen&) = 0;

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);


    void wheelEvent ( QWheelEvent * event );

    QPoint toScene(QPointF f);
    double coordStartX();
    double coordEndX();
    double coordStartY();
    double coordEndY();
    double zoomFactor();
    double rotationAngle();
    int get_sceneScrollX() const;
    int get_sceneScrollY() const;
    void reset();
    void addLine(QLineF l, QPen p);
    void addPoint(QPointF l, QPen p);


protected:
    int sceneScrollX = 0;
    int sceneScrollY = 0;
    QVector<DrawnLine> drawnLines;
    QVector<DrawnPoint> drawnPoints;

    bool dragging = false;
    int drag_down_x = 0;
    int drag_down_y = 0;

    int origScSx = 0;
    int origScSy = 0;

    qreal scale = 1.0;
};


#endif // !ABSTRACT_DRAWER

