#ifndef ABSTRACT_DRAWER
#define ABSTRACT_DRAWER

#include <QWidget>

class AbstractDrawer
{

public:
    explicit AbstractDrawer(QWidget* parent = nullptr){}

    virtual void resizeEvent(QResizeEvent* event) = 0;

    virtual void drawCoordinateSystem() = 0;

    virtual QPoint toScene(QPointF) = 0;
};

#endif // !ABSTRACT_DRAWER

