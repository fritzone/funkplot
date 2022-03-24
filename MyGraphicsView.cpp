#include "MyGraphicsView.h"
#include <QMouseEvent>
#include <QDebug>

MyGraphicsView::MyGraphicsView(QWidget *parent) : QGraphicsView(parent) {this->installEventFilter(this);
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
