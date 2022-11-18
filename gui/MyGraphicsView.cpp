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

    QSizePolicy sp;
    sp.setHorizontalPolicy(QSizePolicy::Expanding);
    sp.setVerticalPolicy(QSizePolicy::Expanding);

    initScene();
}

void MyGraphicsView::mousePressEvent(QMouseEvent *event)
{
    ScreenDrawer::mousePressEvent(event);
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    ScreenDrawer::mouseMoveEvent(event);

    emit redraw();

}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    ScreenDrawer::mouseReleaseEvent(event);
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

void MyGraphicsView::redrawEverything()
{
    drawCoordinateSystem();
    for(const auto&l : qAsConst(m_drawnLines))
    {
        QPoint p1 = toScene({l.line.x1(), l.line.y1()});
        QPoint p2 = toScene({l.line.x2(), l.line.y2()});
        scene()->addLine(QLine(p1, p2) , l.pen);
    }

    for(const auto&p : qAsConst(m_drawnPoints))
    {
        QPoint pd = toScene({p.point.x(), p.point.y()});
        scene()->addEllipse(pd.x(), pd.y(), 1.0, 1.0, p.pen);
    }

}

QRect MyGraphicsView::getClipRect()
{
    return scene()->sceneRect().toRect();
}

void MyGraphicsView::initScene()
{
    sc = new QGraphicsScene(rect());
    sc->setBackgroundBrush(Qt::white);

    setScene(sc);
    viewport()->setFocusProxy(0);
}
