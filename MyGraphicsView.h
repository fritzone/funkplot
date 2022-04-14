#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include "AbstractDrawer.h"

#include <QObject>
#include <QGraphicsView>
#include <QEvent>

class MyGraphicsView : public QGraphicsView, public AbstractDrawer
{

    Q_OBJECT
public:

    explicit MyGraphicsView(QWidget* parent);

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    int get_sceneScrollX() const;

    int get_sceneScrollY() const;

    void resizeEvent(QResizeEvent* event) override;

    void drawCoordinateSystem() override;

    int sceneX(double x)
    {
        int zeroX = scene()->sceneRect().width() / 2;

        return zeroX + get_sceneScrollX() + x * zoomFactor();
    }

    int sceneY(double y)
    {
        int zeroY = scene()->sceneRect().height() / 2;

        return zeroY + get_sceneScrollY() - y * zoomFactor();
    }

    QPoint toScene(QPointF f) override;

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


signals:

    void redraw();

private:

    int sceneScrollX = 0;
    int sceneScrollY = 0;

    int origScSx = 0;
    int origScSy = 0;

    bool dragging = false;
    int drag_down_x = 0;
    int drag_down_y = 0;
};

#endif // MYGRAPHICSVIEW_H
