#ifndef ABSTRACT_DRAWER
#define ABSTRACT_DRAWER

#include <QWidget>

class AbstractDrawer : public QWidget
{
    Q_OBJECT

public:
    explicit AbstractDrawer(QWidget* parent = nullptr) : QWidget(parent) {}

    virtual void resizeEvent(QResizeEvent* event) = 0;
};

#endif // !ABSTRACT_DRAWER

