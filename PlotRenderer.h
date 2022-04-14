#ifndef PLOT_RENDERER
#define PLOT_RENDERER

#include "AbstractDrawer.h"

#include <QWidget>
#include <QPaintEvent>

class PlotRenderer : public AbstractDrawer
{
    Q_OBJECT

public:

    explicit PlotRenderer(QWidget* parent = nullptr);
    void resizeEvent(QResizeEvent* event) override ;


protected:
    void paintEvent(QPaintEvent* event) override;

};

#endif

