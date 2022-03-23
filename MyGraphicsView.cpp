#include "MyGraphicsView.h"
#include <QMouseEvent>
#include <QDebug>

MyGraphicsView::MyGraphicsView(QWidget *parent) : QGraphicsView(parent) {this->installEventFilter(this);
}

void MyGraphicsView::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "mouse press" << event->x() << " " << event->y();
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "mouse move";

}
