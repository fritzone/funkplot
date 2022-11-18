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
    qreal oldScale = m_scale;
    qreal scf = (event->angleDelta().y() / 120.0);
    m_scale += scf / zoomFactor();
    if(m_scale <= 0.001)
    {
        m_scale = oldScale;
    }
}
