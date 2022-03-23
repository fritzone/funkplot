#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QEvent>

class MyGraphicsView : public QGraphicsView
{

    Q_OBJECT
public:

    explicit MyGraphicsView(QWidget* parent);

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

};

#endif // MYGRAPHICSVIEW_H
