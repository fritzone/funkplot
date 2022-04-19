#include "DrawingForm.h"
#include "GraphicsViewZoom.h"
#include "MyGraphicsView.h"
#include "ui_DrawingForm.h"
#include "PlotRenderer.h"
#include "AbstractDrawer.h"

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
#include <QtOpenGLWidgets/QOpenGLWidget>
#endif

#include <QSurfaceFormat>

DrawingForm::DrawingForm(RuntimeProvider *rp, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrawingForm),
    m_rp(rp)
{
    ui->setupUi(this);

    if (true)
    {
        // the drawing one, using QGraphicScene
        graphicsView = new MyGraphicsView(ui->frmDrawing);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        graphicsView->initScene();

#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
        QOpenGLWidget* gl = new QOpenGLWidget();
        QSurfaceFormat format;
        format.setSamples(4);
        gl->setFormat(format);
        graphicsView->setViewport(gl);
#endif

        ui->verticalLayout->addWidget(graphicsView);
        Graphics_view_zoom* z = new Graphics_view_zoom(graphicsView);
        z->set_modifiers(Qt::NoModifier);
        m_ar = graphicsView;


        connect(graphicsView, &MyGraphicsView::redraw, this, [this]()
        {
             graphicsView->redrawEverything();
        }
        );

    }
    else
    {
        m_pr = new PlotRenderer(this);

        ui->verticalLayout->addWidget(m_pr);

        m_ar = m_pr;

    }

    drawCoordinateSystem();


}

DrawingForm::~DrawingForm()
{
    delete ui;
}

void DrawingForm::drawCoordinateSystem()
{
    m_ar->drawCoordinateSystem();
}


void DrawingForm::setDrawingPen(int r, int g, int b, int a)
{
    drawingPen = QPen(QColor(qRgba( r, g, b, a)));
}


QVector<QPointF> DrawingForm::drawPlot(QSharedPointer<Plot> plot)
{
    double cx, cy;
    bool first = true;

    QVector<QPointF> points;

    auto executor = [this, &cx, &cy, &first, &points](double x, double y, bool continuous)
    {
        points.append({x, y});
        if(continuous)
        {
            if(first)
            {
                cx = x;
                cy = y;
                first = false;
            }
            else
            {
                QPoint scp1 {m_ar->toScene({cx, cy})};
                QPoint scp2 {m_ar->toScene({x, y})};
                //sc->addLine( QLine(scp1, scp2), drawingPen);
                m_ar->addLine({static_cast<qreal>(cx), static_cast<qreal>(cy), static_cast<qreal>(x), static_cast<qreal>(y)}, drawingPen);
                cx = x;
                cy = y;
            }
        }
        else
        {
            QPoint scp {m_ar->toScene({x, y})};

            // sc->addEllipse(scp.x(), scp.y(), 1.0, 1.0, drawingPen);
            m_ar->addPoint({x, y}, drawingPen);
        }
    };

    genericPlotIterator(plot, executor);

    return points;

}

void DrawingForm::reset()
{
    m_ar->reset();

}

void DrawingForm::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    m_ar->resizeEvent(event);

    QRect wrect = rect();
    m_ar->redrawEverything();

}



void DrawingForm::drawPoint(double x, double y)
{
    QPoint p = m_ar->toScene({x, y});
    // sc->addEllipse(p.x(), p.y(), 1.0, 1.0, drawingPen);
    m_ar->addPoint({x, y}, drawingPen);

}
