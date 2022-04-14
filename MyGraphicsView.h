#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QEvent>

class MyGraphicsView : public QGraphicsView, p
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
