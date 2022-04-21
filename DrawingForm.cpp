#include "DrawingForm.h"
#include "GraphicsViewZoom.h"
#include "MyGraphicsView.h"
#include "ui_DrawingForm.h"
#include "PlotRenderer.h"
#include "AbstractDrawer.h"

#include <QCamera>
#include <QEntity>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QMaterial>
#include <QOrbitCameraController>
#include <QPhongMaterial>
#include <QPropertyAnimation>
#include <QSphereMesh>
#include <QTorusMesh>
#include <Qt3DWindow>
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
#include <QtOpenGLWidgets/QOpenGLWidget>
#endif

#include <QSurfaceFormat>

#include "OrbitTransformController.h"

OrbitTransformController::OrbitTransformController(QObject *parent)
    : QObject(parent)
    , m_target(nullptr)
    , m_matrix()
    , m_radius(1.0f)
    , m_angle(0.0f)
{
}

void OrbitTransformController::setTarget(Qt3DCore::QTransform *target)
{
    if (m_target != target) {
        m_target = target;
        emit targetChanged();
    }
}

Qt3DCore::QTransform *OrbitTransformController::target() const
{
    return m_target;
}

void OrbitTransformController::setRadius(float radius)
{
    if (!qFuzzyCompare(radius, m_radius)) {
        m_radius = radius;
        updateMatrix();
        emit radiusChanged();
    }
}

float OrbitTransformController::radius() const
{
    return m_radius;
}

void OrbitTransformController::setAngle(float angle)
{
    if (!qFuzzyCompare(angle, m_angle)) {
        m_angle = angle;
        updateMatrix();
        emit angleChanged();
    }
}

float OrbitTransformController::angle() const
{
    return m_angle;
}

void OrbitTransformController::updateMatrix()
{
    m_matrix.setToIdentity();
    m_matrix.rotate(m_angle, QVector3D(0.0f, 1.0f, 0.0f));
    m_matrix.translate(m_radius, 0.0f, 0.0f);
    m_target->setMatrix(m_matrix);
}


Qt3DCore::QEntity *createScene()
{
    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;

    // Material
    Qt3DRender::QMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);

    // Torus
    Qt3DCore::QEntity *torusEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QTorusMesh *torusMesh = new Qt3DExtras::QTorusMesh;
    torusMesh->setRadius(5);
    torusMesh->setMinorRadius(1);
    torusMesh->setRings(100);
    torusMesh->setSlices(20);

    Qt3DCore::QTransform *torusTransform = new Qt3DCore::QTransform;
    torusTransform->setScale3D(QVector3D(1.5, 1, 0.5));
    torusTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 45.0f));

    torusEntity->addComponent(torusMesh);
    torusEntity->addComponent(torusTransform);
    torusEntity->addComponent(material);

    // Sphere
    Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
    sphereMesh->setRadius(3);
    sphereMesh->setGenerateTangents(true);

    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform;
    OrbitTransformController *controller = new OrbitTransformController(sphereTransform);
    controller->setTarget(sphereTransform);
    controller->setRadius(20.0f);

    QPropertyAnimation *sphereRotateTransformAnimation = new QPropertyAnimation(sphereTransform);
    sphereRotateTransformAnimation->setTargetObject(controller);
    sphereRotateTransformAnimation->setPropertyName("angle");
    sphereRotateTransformAnimation->setStartValue(QVariant::fromValue(0));
    sphereRotateTransformAnimation->setEndValue(QVariant::fromValue(360));
    sphereRotateTransformAnimation->setDuration(10000);
    sphereRotateTransformAnimation->setLoopCount(-1);
    sphereRotateTransformAnimation->start();

    sphereEntity->addComponent(sphereMesh);
    sphereEntity->addComponent(sphereTransform);
    sphereEntity->addComponent(material);

    return rootEntity;
}

DrawingForm::DrawingForm(RuntimeProvider *rp, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrawingForm),
    m_rp(rp)
{
    ui->setupUi(this);

    int window_type = 2; // 0 - qgraphicscene, 1 - qwidget, 2 - qt3d

    switch(window_type)
    {
    case 0:
    {
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
        break;
    }
    case 1:
    {
        m_pr = new PlotRenderer(this);

        ui->verticalLayout->addWidget(m_pr);

        m_ar = m_pr;
        break;
    }

    case 2:
    {
        // TODO: implement when on a fresh enough qt which supports this
        Qt3DExtras::Qt3DWindow *view = new Qt3DExtras::Qt3DWindow;

        Qt3DCore::QEntity *scene = createScene();

        // Camera
        Qt3DRender::QCamera *camera = view->camera();
        camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
        camera->setPosition(QVector3D(0, 0, 40.0f));
        camera->setViewCenter(QVector3D(0, 0, 0));

        // For camera controls
        Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(scene);
        camController->setLinearSpeed( 50.0f );
        camController->setLookSpeed( 180.0f );
        camController->setCamera(camera);

        view->setRootEntity(scene);

        ui->verticalLayout->addWidget(QWidget::createWindowContainer( view, this));

    }


    }

//    drawCoordinateSystem();
//    connect(this, &DrawingForm::contentChanged, this, [this]()
//            {
//                m_ar->redrawEverything();
//            }
//            );


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
//    QWidget::resizeEvent(event);

//    m_ar->resizeEvent(event);

//    QRect wrect = rect();
//    m_ar->redrawEverything();

}



void DrawingForm::drawPoint(double x, double y)
{
    QPoint p = m_ar->toScene({x, y});
    // sc->addEllipse(p.x(), p.y(), 1.0, 1.0, drawingPen);
    m_ar->addPoint({x, y}, drawingPen);

}
