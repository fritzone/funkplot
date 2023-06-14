#include "DrawingForm.h"
#include "GraphicsViewZoom.h"
#include "MyGraphicsView.h"
#include "ui_DrawingForm.h"
#include "PlotRenderer.h"
#include "AbstractDrawer.h"
#include "Plot.h"

#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
#include <QtOpenGLWidgets/QOpenGLWidget>
#include "ProjectedRenderer.h"
#else
#include <QOpenGLWidget>
#endif

DrawingForm::DrawingForm(RuntimeProvider *rp, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrawingForm),
    m_rp(rp)
{
    ui->setupUi(this);

    connect(this, &DrawingForm::contentChanged, this, [this]() {
                m_ar->redrawEverything();
    });
    setFlexibleStyle();

    //createQtSceneGraphicsView();
    createWidgetDrawing();
#if 0
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    create3DDrawing();
#endif
#endif
    //m_imgDrawer = new ImageDrawer(640, 480);
    //m_drawers.append(m_imgDrawer);
}

DrawingForm::~DrawingForm()
{
    delete ui;
}

void DrawingForm::createQtSceneGraphicsView()
{
    graphicsView = new MyGraphicsView(ui->frmDrawing);
    QOpenGLWidget* gl = new QOpenGLWidget();
    QSurfaceFormat format;
    format.setSamples(4);
    gl->setFormat(format);
    graphicsView->setViewport(gl);
    ui->verticalLayout->addWidget(graphicsView);
    Graphics_view_zoom* z = new Graphics_view_zoom(graphicsView);
    z->set_modifiers(Qt::NoModifier);

    // local handler for QScene stuff
    connect(graphicsView, &MyGraphicsView::redraw, this, [this]() {
        graphicsView->redrawEverything();
    });
    graphicsView->drawCoordinateSystem();
    graphicsView->redrawEverything();
}

void DrawingForm::createWidgetDrawing()
{
    m_pr = new PlotRenderer(this);
    m_drawers.append(m_pr);

    ui->verticalLayout->addWidget(m_pr);
}

void DrawingForm::addLine(qreal x1, qreal y1, qreal x2, qreal y2)
{
    for(auto& d : m_drawers)
    {
        d->addLine({x1, y1, x2, y2}, m_drawingPen, m_pixelSize);
    }
}

void DrawingForm::addPoint(qreal x, qreal y)
{
    for(auto& d : m_drawers)
    {
        d->addPoint({x, y}, m_drawingPen, m_pixelSize);
    }
}

void DrawingForm::setFlexibleStyle()
{
    ui->topSpacer->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Ignored);
    ui->rightSpacer->changeSize(0, 0, QSizePolicy::Ignored, QSizePolicy::Minimum);
    ui->leftSpacer->changeSize(0, 0, QSizePolicy::Ignored, QSizePolicy::Minimum);
    ui->bottomSpacer->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Ignored);

    ui->gridLayout->invalidate();
}

void DrawingForm::setFixedStyle()
{
    ui->topSpacer->changeSize(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->rightSpacer->changeSize(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    ui->leftSpacer->changeSize(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
    ui->bottomSpacer->changeSize(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    ui->gridLayout->invalidate();
}

#if 0
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
void DrawingForm::create3DDrawing()
{
    view = new ProjectedRenderer;
    m_drawers.append(view);
    ui->verticalLayout->addWidget(QWidget::createWindowContainer( view, this));
}
#endif
#endif

void DrawingForm::setView(ViewType vt)
{
    switch(vt)
    {
    case ViewType::VIEW_2D:
    {
        if(!graphicsView)
        {
            // 0 - QGraphicsScene
            createQtSceneGraphicsView();
        }

        m_ar = graphicsView;
        if(m_pr) m_pr->hide();
#if 0
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
        if(view) view->hide();
#endif
#endif
        graphicsView->show();
        graphicsView->adjustSize();
        break;
    }
    case ViewType::VIEW_2D_QWIDGET:
        if(!m_pr)
        {
            createWidgetDrawing();
        }

#if 0
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
        if(view) view->hide();
#endif
#endif
        if(graphicsView) graphicsView->hide();

        m_ar = m_pr;
        m_pr->show();
        m_pr->adjustSize();
        break;
#if 0
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    case ViewType::VIEW_3D:

        if(!view)
        {
            create3DDrawing();
        }

        m_ar = view;
        if(m_pr) m_pr->hide();
        if(graphicsView) graphicsView->hide();
        view->show();

        break;
#endif
#endif
    }

    if(m_pr)
    {
        m_pr->drawCoordinateSystem();
        m_pr->redrawEverything();
    }
#if 0
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    view->drawCoordinateSystem();
    view->redrawEverything();
#endif
#endif
}

void DrawingForm::setDrawingPen(int r, int g, int b, int a, int s)
{
    m_drawingPen = QPen(QColor(r, g, b, a));
    m_pixelSize = ::abs(s);
}

void DrawingForm::setPixelSize(size_t s)
{
    m_pixelSize = s;
}

QVector<QPointF> DrawingForm::drawPlot(QSharedPointer<Plot> plot)
{
    double cx, cy;
    bool first = true;

    QVector<QPointF> points;

    auto executor = [this, &cx, &cy, &first, &points](QSharedPointer<Plot> plot, double x, double y, bool continuous)
    {
        if(plot->polarPlot)
        {
            // need to convert the polar coordinate (x, y) to ccartesizan ones, that can be plotted

            double carX = y * cos( x );
            double carY = y * sin( x );

            x = carX;
            y = carY;
        }

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
                //if(std::abs(cx - x) < 0.1 && std::abs(cy - y))
                {
                    addLine(static_cast<qreal>(cx), static_cast<qreal>(cy), static_cast<qreal>(x), static_cast<qreal>(y));
                }
                cx = x;
                cy = y;
            }
        }
        else
        {
            addPoint(x, y);
        }
    };

    genericPlotIterator(plot, executor);

    /*if(m_imgDrawer)
    {
        m_imgDrawer->redrawEverything();
    }*/

    return points;

}

void DrawingForm::reset()
{
    m_drawingPen = {Qt::black};
    m_pixelSize = 1;

    for(auto& d : m_drawers)
    {
        d->reset();
        d->drawCoordinateSystem();
    }
#if 0
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    view->reset();
    view->drawCoordinateSystem();
#endif
#endif
}

void DrawingForm::populateDrawerData(AbstractDrawer *target)
{
    target->setDrawnLines(m_pr->getDrawnLines());
    target->setDrawnPoints(m_pr->getDrawnPoints());
}

QPixmap DrawingForm::screenshot()
{
    return m_pr->grab();
}

void DrawingForm::drawPoint(double x, double y)
{
    addPoint(x, y);
}

void DrawingForm::drawLine(double x1, double y1, double x2, double y2)
{
    addLine(x1, y1, x2, y2);
}

void DrawingForm::on_rotationAngleChange(double v)
{

    for(auto& d : m_drawers)
    {
        d->setRotationAngle(v);
    }
}
void DrawingForm::on_zoomFactorChange(double v)
{
    for(auto& d : m_drawers)
    {
        d->setZoomFactor(v);
    }
}

void DrawingForm::on_gridChange(bool v)
{
    for(auto& d : m_drawers)
    {
        d->setShowGrid(v);
    }
}
void DrawingForm::on_coordEndYChange(double v)
{
    for(auto& d : m_drawers)
    {
        d->setCoordEndY(v);
    }
}
void DrawingForm::on_coordStartYChange(double v)
{
    for(auto& d : m_drawers)
    {
        d->setCoordStartY(v);
    }
}
void DrawingForm::on_coordEndXChange(double v)
{
    for(auto& d : m_drawers)
    {
        d->setCoordEndX(v);
    }
}
void DrawingForm::on_coordStartXChange(double v)
{
    for(auto& d : m_drawers)
    {
        d->setCoordStartX(v);
    }
}

void DrawingForm::on_comboBox_currentTextChanged(const QString &arg1)
{
    QString line = arg1;
    QRegularExpression re("(\\d+) x (\\d+)");
    QRegularExpressionMatch match;

    match = re.match(line);
    QRegularExpressionMatchIterator i = re.globalMatch(line);
    if(line != "Auto")
    {
        int w = match.captured(1).toInt();
        int h = match.captured(2).toInt();

        for(auto& d : m_drawers)
        {
            d->setWidth(w);
            d->setHeight(h);

            PlotRenderer* pr = dynamic_cast<PlotRenderer*>(d);
            if(pr)
            {
                pr->setFixedStyle();
                setFixedStyle();
                pr->redrawEverything();
            }
        }
    }
    else
    {
        for(auto& d : m_drawers)
        {
            d->setWidth(-1);
            d->setHeight(-1);

            PlotRenderer* pr = dynamic_cast<PlotRenderer*>(d);
            if(pr)
            {
                pr->setFlexibleStyle();
                setFlexibleStyle();
                pr->redrawEverything();
            }
        }
    }
}

