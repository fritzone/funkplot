#ifndef DRAWINGFORM_H
#define DRAWINGFORM_H

#include "RuntimeProvider.h"
#include "ViewType.h"

#include <QPen>
#include <QWidget>


class MyGraphicsView;
class PlotRenderer;
class AbstractDrawer;
class QGraphicsScene;
class ProjectedRenderer;
class ImageDrawer;

namespace Ui {
class DrawingForm;
}

class DrawingForm : public QWidget
{
    Q_OBJECT

public:
    explicit DrawingForm(RuntimeProvider *rp, QWidget *parent = nullptr);
    ~DrawingForm();

    void setView(ViewType);
    QVector<QPointF> drawPlot(QSharedPointer<Plot> plot);
    void drawPoint(double x, double y);
    void setDrawingPen(int r, int g, int b, int a, int s);
    void setPixelSize(size_t s);
    void reset();
    void populateDrawerData(AbstractDrawer* target);
    QPixmap screenshot();

public slots:

    void on_rotationAngleChange(double);
    void on_zoomFactorChange(double);
    void on_gridChange(bool);
    void on_coordEndYChange(double);
    void on_coordStartYChange(double);
    void on_coordEndXChange(double);
    void on_coordStartXChange(double);

signals:

    void contentChanged();

private slots:
    void on_comboBox_currentTextChanged(const QString &arg1);

private:

    void createQtSceneGraphicsView();
    void createWidgetDrawing();
    void addLine(qreal x1, qreal y1, qreal x2, qreal y2);
    void addPoint(qreal x, qreal y);
    void setFlexibleStyle();
    void setFixedStyle();

#if 0
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    void create3DDrawing();
#endif
#endif

    template<class E>
    void genericPlotIterator(QSharedPointer<Plot> plot, E executor)
    {
        m_rp->genericPlotIterator(plot, executor);
    }

private:

    Ui::DrawingForm *ui;

    QPen m_drawingPen {Qt::black};          // the pen used for drawing, holds the color of it
    size_t m_pixelSize = 1;                 // the size of a pixel (pixels are small ellipses)
    RuntimeProvider* m_rp;                  // the runtime provider of this class
    AbstractDrawer* m_ar = nullptr;         // who actually does the drawing from the three below
    PlotRenderer* m_pr = nullptr;           // a drawer which does pixel by pixel drawing with a QPainter
    MyGraphicsView* graphicsView = nullptr; // a drawer which draws using Qt graphics view
    ProjectedRenderer *view = nullptr;      // a drawer which uses Qt3d notions
    ImageDrawer* m_imgDrawer = nullptr;     // drawing to an image
    QVector<AbstractDrawer*> m_drawers;     // all the registerd drawers
};

#endif // DRAWINGFORM_H
