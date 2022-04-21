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
//    void wheelEvent ( QWheelEvent * event ) override;

    void resizeEvent(QResizeEvent* event) override;

    void drawCoordinateSystem() override;
    void drawLine(const QLineF&, const QPen&) override;

    int sceneX(double x) override;

    int sceneY(double y) override;
    void redrawEverything() override;

    void initScene();

signals:

    void redraw();

private:

    QGraphicsScene* sc = nullptr;
};

#endif // MYGRAPHICSVIEW_H
