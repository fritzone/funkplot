#include "MyGraphicsView.h"
#include "util.h"

#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsTextItem>

MyGraphicsView::MyGraphicsView(QWidget *parent) : QGraphicsView(parent) 
{
    this->installEventFilter(this);
}

void MyGraphicsView::mousePressEvent(QMouseEvent *event)
{
    drag_down_x = event->x();
    drag_down_y = event->y();
    origScSx = sceneScrollX;
    origScSy = sceneScrollY;
    dragging = true;
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if(dragging)
    {
        sceneScrollX = origScSx + (event->x() - drag_down_x);
        sceneScrollY = origScSy + (event->y() - drag_down_y);

        emit redraw();
    }
}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    dragging = false;
}

int MyGraphicsView::get_sceneScrollX() const
{
    return sceneScrollX;
}

int MyGraphicsView::get_sceneScrollY() const
{
    return sceneScrollY;
}

void MyGraphicsView::resizeEvent(QResizeEvent* event)
{
    scene()->setSceneRect(rect());
}

void MyGraphicsView::drawCoordinateSystem()
{
    auto sc = scene();
    sc->clear();

    sc->addLine(QLineF(toScene( {0, 0} ), toScene( {coordEndX(), 0 })), QPen(Qt::blue));
    sc->addLine(QLineF(toScene( {0, 0} ), toScene( {coordStartX(), 0})), QPen(Qt::red));
    sc->addLine(QLineF(toScene( {0, 0} ), toScene( {0, coordEndY()})), QPen(Qt::red));
    sc->addLine(QLineF(toScene( {0, 0} ), toScene( {0, coordStartY()})), QPen(Qt::red));

    auto endPoint1 = toScene( {0, coordEndY()});

    // the points of the arrow will be on a circle, radius 0.05, with rotation angle +/- 15 degreees
    double r = 0.3;
    double firstAngdiffTop = 0.261799 + M_PI;
    double firstAngdiffBottom = 0.261799 + M_PI;
    QPointF firstArrowheadTop = toScene({r * sin(firstAngdiffTop), coordEndY() + r * cos(+ firstAngdiffTop)});
    QPointF firstArrowheadBottom = toScene({r * sin(- firstAngdiffBottom), coordEndY() + r * cos(- firstAngdiffBottom)});

    sc->addLine(QLineF(endPoint1, firstArrowheadTop), QPen(Qt::red));
    sc->addLine(QLineF(endPoint1, firstArrowheadBottom), QPen(Qt::red));

    auto endPoint2 = toScene( {coordEndX(), 0});

    double secondAngdiffTop = 0.261799 + 3 * M_PI / 2;
    double secondAngdiffBottom = 0.261799 + M_PI /2;

    QPointF secondArrowheadTop = toScene({coordEndX() + r * sin(secondAngdiffTop), r * cos(+ secondAngdiffTop)});
    QPointF secondArrowheadBottom = toScene({coordEndX() + r * sin(- secondAngdiffBottom), r * cos(- secondAngdiffBottom)});

    sc->addLine(QLineF(endPoint2, secondArrowheadTop), QPen(Qt::blue));
    sc->addLine(QLineF(endPoint2, secondArrowheadBottom), QPen(Qt::blue));

    // the small lines
    r = 0.1;
    for(double x = -1.0; x > coordStartX(); x -= 1.0)
    {
        double angdiff = M_PI/2;

        QPointF lineTop = toScene({x + r * sin(angdiff + M_PI / 2), + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = toScene({x + r * sin(angdiff + 3 * M_PI / 2), + r * cos(angdiff + 3 * M_PI / 2)});

        sc->addLine( {lineTop, lineBottom}, QPen(Qt::red));

        QGraphicsTextItem *text = sc->addText(QString::number(x, 'f', 0));
        text->setPos(lineBottom.x() - text->boundingRect().width() / 2, lineBottom.y() + 6);
    }

    for(double x = 1.0; x < coordEndX(); x += 1.0)
    {
        double angdiff = M_PI/2;

        QPointF lineTop = toScene({x + r * sin(angdiff + M_PI / 2), + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = toScene({x + r * sin(angdiff + 3 * M_PI / 2), + r * cos(angdiff + 3 * M_PI / 2)});

        sc->addLine( {lineTop, lineBottom}, QPen(Qt::blue));

        QGraphicsTextItem *text = sc->addText(QString::number(x, 'f', 0));
        text->setPos(lineBottom.x() - text->boundingRect().width() / 2, lineBottom.y() + 6);
    }

    for(double y = -1.0; y > coordStartY(); y -= 1.0)
    {
        double angdiff = M_PI;

        QPointF lineTop = toScene({r * sin(angdiff + M_PI / 2),y + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = toScene({r * sin(angdiff + 3 * M_PI / 2),y + r * cos(angdiff + 3 * M_PI / 2)});

        sc->addLine( {lineTop, lineBottom}, QPen(Qt::red));

        QGraphicsTextItem *text = sc->addText(QString::number(y, 'f', 0));
        text->setPos(lineTop.x() - text->boundingRect().width(), lineTop.y() - text->boundingRect().height() / 2);
    }

    for(double y = 1.0; y < coordEndY(); y += 1.0)
    {
        double angdiff = M_PI;

        QPointF lineTop = toScene({r * sin(angdiff + M_PI / 2),y + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = toScene({r * sin(angdiff + 3 * M_PI / 2),y + r * cos(angdiff + 3 * M_PI / 2)});

        sc->addLine( {lineTop, lineBottom}, QPen(Qt::red));

        QGraphicsTextItem *text = sc->addText(QString::number(y, 'f', 0));
        text->setPos(lineTop.x() - text->boundingRect().width(), lineTop.y() - text->boundingRect().height() / 2);
    }
}

QPoint MyGraphicsView::toScene(QPointF f)
{
    QPointF rotated = rotatePoint(0, 0, rotationAngle(), f);
    int scx = sceneX(rotated.x());
    int scy = sceneY(rotated.y());

    return {scx, scy};
}
