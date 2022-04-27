#include "MyGraphicsView.h"
#include "util.h"

#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsTextItem>

MyGraphicsView::MyGraphicsView(QWidget *parent) : QGraphicsView(parent) 
{
    this->installEventFilter(this);

    setObjectName(QString::fromUtf8("graphicsView"));
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    initScene();
}

void MyGraphicsView::mousePressEvent(QMouseEvent *event)
{
    AbstractDrawer::mousePressEvent(event);
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    AbstractDrawer::mouseMoveEvent(event);

    emit redraw();

}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    AbstractDrawer::mouseReleaseEvent(event);
}

//void MyGraphicsView::wheelEvent(QWheelEvent *event)
//{
//    AbstractDrawer::wheelEvent(event);
//}

void MyGraphicsView::resizeEvent(QResizeEvent* event)
{
    scene()->setSceneRect(rect());
}

void MyGraphicsView::drawCoordinateSystem()
{
    auto sc = scene();
    sc->clear();

    AbstractDrawer::drawCoordinateSystem();

}

void MyGraphicsView::drawLine(const QLineF &l, const QPen &p)
{
    auto sc = scene();
    QPoint p1 = toScene(l.p1());
    QPoint p2 = toScene(l.p2());
    sc->addLine(QLine{p1, p2}, p);
    //addLine(l, p);
}

int MyGraphicsView::sceneX(double x)
{
    int zeroX = scene()->sceneRect().width() / 2;

    return zeroX + get_sceneScrollX() + x * zoomFactor();
}

int MyGraphicsView::sceneY(double y)
{
    int zeroY = scene()->sceneRect().height() / 2;

    return zeroY + get_sceneScrollY() - y * zoomFactor();
}


void MyGraphicsView::redrawEverything()
{
    drawCoordinateSystem();
    for(const auto&l : qAsConst(drawnLines))
    {
        QPoint p1 = toScene({l.line.x1(), l.line.y1()});
        QPoint p2 = toScene({l.line.x2(), l.line.y2()});
        scene()->addLine(QLine(p1, p2) , l.pen);
    }

    for(const auto&p : qAsConst(drawnPoints))
    {
        QPoint pd = toScene({p.point.x(), p.point.y()});
        scene()->addEllipse(pd.x(), pd.y(), 1.0, 1.0, p.pen);
    }

}

void MyGraphicsView::initScene()
{
    sc = new QGraphicsScene(rect());
    sc->setBackgroundBrush(Qt::white);

    setScene(sc);
    viewport()->setFocusProxy(0);
}
