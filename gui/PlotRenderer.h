#ifndef PLOT_RENDERER
#define PLOT_RENDERER

#include "ScreenDrawer.h"

#include <QWidget>
#include <QPaintEvent>

class PlotRenderer :public QWidget, public ScreenDrawer
{
    Q_OBJECT

public:

    explicit PlotRenderer(QWidget* parent = nullptr);

    void drawCoordinateSystem() override;
    void drawLine(const QLineF&, const QPen&) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent * event) override;
    QRect getClipRect() override;
    void redrawEverything() override;

    QSize sizeHint() const override;

    void setFixedStyle();
    void setFlexibleStyle();

protected:
    void paintEvent(QPaintEvent* event) override;

private:

    bool m_hasCoordinateSystem = false;

};

#endif

