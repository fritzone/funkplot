#include "ProjectedRenderer.h"

#include <QAttribute>
#include <QBuffer>
#include <QCamera>
#include <QEntity>
#include <QGeometry>
#include <QGeometryRenderer>
#include <QOrbitCameraController>
#include <QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>

ProjectedRenderer::ProjectedRenderer(QWidget *parent)
{
    rootEntity = new Qt3DCore::QEntity;
    setRootEntity(rootEntity);

    resetCamera();
    assignCameraController();
}

void ProjectedRenderer::resizeEvent(QResizeEvent *event)
{

}

void ProjectedRenderer::drawCoordinateSystem()
{
    // draw the basic (x,y) axes
    AbstractDrawer::drawCoordinateSystem();

    // the Z axis
    draw3DLine(QVector3D{0, 0, 0}, QVector3D{0, 0, static_cast<float>(coordEndZ())}, Qt::green);
    draw3DLine(QVector3D{0, 0, 0}, QVector3D{0, 0, static_cast<float>(coordStartZ())}, Qt::black);

    // the points of the arrow will be on a circle, radius 0.05, with rotation angle +/- 15 degreees
    double r = 0.3;
    double firstAngdiffTop = 0.261799 + M_PI;
    double firstAngdiffBottom = 0.261799 + M_PI;
    QPointF firstArrowheadTop{r * sin(firstAngdiffTop), coordEndZ() + r * cos(+ firstAngdiffTop)};
    QPointF firstArrowheadBottom{r * sin(- firstAngdiffBottom), coordEndZ() + r * cos(- firstAngdiffBottom)};

    draw3DLine(QVector3D{0, 0, static_cast<float>(coordEndZ())}, QVector3D{0, static_cast<float>(firstArrowheadTop.x()), static_cast<float>(firstArrowheadTop.y())}, Qt::green);
    draw3DLine(QVector3D{0, 0, static_cast<float>(coordEndZ())}, QVector3D{0, static_cast<float>(firstArrowheadBottom.x()), static_cast<float>(firstArrowheadBottom.y())}, Qt::green);

    r = 0.1;
    // Z negative
    for(double z = -1.0; z > coordStartZ(); z -= 1.0)
    {
        double angdiff = M_PI/2;

        QPointF lineTop{z + r * sin(angdiff + M_PI / 2), + r * cos(angdiff + M_PI / 2)};
        QPointF lineBottom{z + r * sin(angdiff + 3 * M_PI / 2), + r * cos(angdiff + 3 * M_PI / 2)};

        draw3DLine(QVector3D{0, static_cast<float>(lineTop.y()), static_cast<float>(lineTop.x())},
                   QVector3D{0, static_cast<float>(lineBottom.y()), static_cast<float>(lineBottom.x())}, Qt::black);


        //        QGraphicsTextItem *text = sc->addText(QString::number(x, 'f', 0));
        //        text->setPos(lineBottom.x() - text->boundingRect().width() / 2, lineBottom.y() + 6);
    }

    // Z positive
    for(double z = 1.0; z < coordEndZ(); z += 1.0)
    {
        double angdiff = M_PI/2;

        QPointF lineTop{z + r * sin(angdiff + M_PI / 2), + r * cos(angdiff + M_PI / 2)};
        QPointF lineBottom{z + r * sin(angdiff + 3 * M_PI / 2), + r * cos(angdiff + 3 * M_PI / 2)};

        draw3DLine(QVector3D{0, static_cast<float>(lineTop.y()), static_cast<float>(lineTop.x())},
                   QVector3D{0, static_cast<float>(lineBottom.y()), static_cast<float>(lineBottom.x())}, Qt::green);


        //        QGraphicsTextItem *text = sc->addText(QString::number(x, 'f', 0));
        //        text->setPos(lineBottom.x() - text->boundingRect().width() / 2, lineBottom.y() + 6);
    }

}

void ProjectedRenderer::drawLine(const QLineF &l, const QPen &p)
{
    qDebug() << "Line: " << l;
    draw3DLine({static_cast<float>(l.p1().x()), static_cast<float>(l.p1().y()), 0.0}, {static_cast<float>(l.p2().x()), static_cast<float>(l.p2().y()), 0.0}, p.color());
}

void ProjectedRenderer::mousePressEvent(QMouseEvent *event)
{

}

void ProjectedRenderer::mouseMoveEvent(QMouseEvent *event)
{

}

void ProjectedRenderer::mouseReleaseEvent(QMouseEvent *event)
{

}

void ProjectedRenderer::wheelEvent(QWheelEvent *event)
{

}

void ProjectedRenderer::redrawEverything()
{

}
/*
void ProjectedRenderer::addLine(QLineF l, QPen p)
{
    this->drawLine(l, p);
}

void ProjectedRenderer::addPoint(QPointF l, QPen p, size_t s)
{
    //this->drawLine(QLineF{ QPointF{l.x() - 0.01, l.y() - 0.01},  QPointF{l.x() + 0.01, l.y() + 0.01}}, p);

    Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
    sphereMesh->setRadius(0.01 * s);
    auto *material = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setAmbient(p.color());

    Qt3DCore::QTransform *sphereLocationTransform = new Qt3DCore::QTransform;
    sphereLocationTransform->setTranslation(QVector3D{static_cast<float>(l.x()), static_cast<float>(l.y()), 0.0});

    sphereEntity->addComponent(sphereMesh);
    sphereEntity->addComponent(sphereLocationTransform);
    sphereEntity->addComponent(material);
}
*/
double ProjectedRenderer::coordStartZ()
{
    return -15.0;
}

double ProjectedRenderer::coordEndZ()
{
    return 15.0;
}

void ProjectedRenderer::paintEvent(QPaintEvent *event)
{

}

Qt3DCore::QEntity *ProjectedRenderer::get_rootEntity() const
{
    return rootEntity;
}

void ProjectedRenderer::draw3DLine(const QVector3D& start, const QVector3D& end, const QColor& color)
{
    auto *geometry = new Qt3DCore::QGeometry(rootEntity);

    // position vertices (start and end)
    QByteArray bufferBytes;
    bufferBytes.resize(3 * 2 * sizeof(float)); // start.x, start.y, start.end + end.x, end.y, end.z
    float *positions = reinterpret_cast<float*>(bufferBytes.data());
    *positions++ = start.x();
    *positions++ = start.y();
    *positions++ = start.z();
    *positions++ = end.x();
    *positions++ = end.y();
    *positions++ = end.z();

    auto *buf = new Qt3DCore::QBuffer(geometry);
    buf->setData(bufferBytes);

    auto *positionAttribute = new Qt3DCore::QAttribute(geometry);
    positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(2);
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize(2 * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());
    *indices++ = 0;
    *indices++ = 1;

    auto *indexBuffer = new Qt3DCore::QBuffer(geometry);
    indexBuffer->setData(indexBytes);

    auto *indexAttribute = new Qt3DCore::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DCore::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DCore::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(2);
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    auto *line = new Qt3DRender::QGeometryRenderer(rootEntity);
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    auto *material = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setAmbient(color);

    // entity
    auto *lineEntity = new Qt3DCore::QEntity(rootEntity);
    lineEntity->addComponent(line);
    lineEntity->addComponent(material);
}

void ProjectedRenderer::resetCamera()
{
    Qt3DRender::QCamera *camera = this->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 25.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));
}

void ProjectedRenderer::assignCameraController()
{
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(get_rootEntity());
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera());
}
