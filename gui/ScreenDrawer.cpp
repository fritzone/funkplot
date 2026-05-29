#include "ScreenDrawer.h"

#include <QMouseEvent>
#include <QWheelEvent>


void ScreenDrawer::mousePressEvent(QMouseEvent *event)
{
    m_dragDownX = event->x();
    m_dragDownY = event->y();
    m_origScSx = m_sceneScrollX;
    m_origScSy = m_sceneScrollY;
    m_dragging = true;
}

void ScreenDrawer::mouseMoveEvent(QMouseEvent *event)
{
    if(m_dragging)
    {
        m_sceneScrollX = m_origScSx + (event->x() - m_dragDownX);
        m_sceneScrollY = m_origScSy + (event->y() - m_dragDownY);
    }
}

void ScreenDrawer::mouseReleaseEvent(QMouseEvent *event)
{
    m_dragging = false;
}

void ScreenDrawer::wheelEvent ( QWheelEvent * event )
{
    qreal scf = event->angleDelta().y() / 120.0;
    double newZoom = m_zoomFactor * (1.0 + scf * 0.02);
    if (newZoom > 0.1)
        m_zoomFactor = newZoom;
}
