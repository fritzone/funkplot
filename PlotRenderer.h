#ifndef PLOT_RENDERER
#define PLOT_RENDERER

#include "AbstractDrawer.h"

#include <QWidget>
#include <QPaintEvent>

class PlotRenderer :public QWidget, public AbstractDrawer
{
    Q_OBJECT

public:

    explicit PlotRenderer(QWidget* parent = nullptr);
    void resizeEvent(QResizeEvent* event) override ;
    void drawCoordinateSystem() override;
    void drawLine(const QLineF&, const QPen&) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent * event) override;

    int sceneX(double x) override;

    int sceneY(double y) override;
    virtual void redrawEverything() override;

protected:
    void paintEvent(QPaintEvent* event) override;

    bool m_hasCoordinateSystem = false;

};

#endif

