#ifndef PROJECTEDRENDERER_H
#define PROJECTEDRENDERER_H

#include "AbstractDrawer.h"

#include <QEntity>
#include <Qt3DWindow>

class ProjectedRenderer :public Qt3DExtras::Qt3DWindow, public AbstractDrawer
{
    Q_OBJECT

public:

    explicit ProjectedRenderer(QWidget* parent = nullptr);
    void resizeEvent(QResizeEvent* event) override ;
    void drawCoordinateSystem() override;
    void drawLine(const QLineF&, const QPen&) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent * event) override;

    virtual void redrawEverything() override;

    double coordStartZ();
    double coordEndZ();

    Qt3DCore::QEntity *get_rootEntity() const;

protected:
    void paintEvent(QPaintEvent* event) override;

    bool m_hasCoordinateSystem = false;
    Qt3DCore::QEntity *rootEntity = nullptr;

private:

    void draw3DLine(const QVector3D& start, const QVector3D& end, const QColor& color);
    void resetCamera();
    void assignCameraController();

};

#endif // PROJECTEDRENDERER_H
