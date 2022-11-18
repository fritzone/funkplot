#ifndef SCREENDRAWER_H
#define SCREENDRAWER_H

#include "AbstractDrawer.h"

class QMouseEvent;
class QWheelEvent;

class ScreenDrawer : public AbstractDrawer
{

public:

    explicit ScreenDrawer() = default;

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent ( QWheelEvent * event );

    //virtual void resizeEvent(QResizeEvent* event) = 0;

private:

    bool m_dragging = false;
    int m_dragDownX = 0;
    int m_dragDownY = 0;
    int m_origScSx = 0;
    int m_origScSy = 0;

};

#endif // SCREENDRAWER_H
