#include "AbstractDrawer.h"
#include "RuntimeProvider.h"

#include "util.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include <QMouseEvent>
#include <QPen>
#include <cmath>

static QPointF asItIs(QPointF p)
{
    return p;
}

void AbstractDrawer::drawCoordinateSystem()
{
    if(!RuntimeProvider::get()->getShowCoordinates())
    {
        return;
    }

    double r = 0.1;
    // now draw the grid, horizontal lines
    if(m_drawGrid)
    {
        {
            double y = 1.0;
            while( y < coordEndY())
            {
                double angdiff = M_PI;

                QPointF lineTop = asItIs({coordStartX() + r * sin(angdiff + M_PI / 2),y + r * cos(angdiff + M_PI / 2)});
                QPointF lineBottom = asItIs({coordEndX() + r * sin(angdiff + 3 * M_PI / 2),y + r * cos(angdiff + 3 * M_PI / 2)});

                drawLine( {lineTop, lineBottom}, QPen(Qt::black, 0.5, Qt::DotLine));
                y += 1.0;
            }
        }

        {
            double y = -1.0;
            while( y > coordStartY())
            {
                double angdiff = M_PI;

                QPointF lineTop = asItIs({coordStartX() + r * sin(angdiff + M_PI / 2),y + r * cos(angdiff + M_PI / 2)});
                QPointF lineBottom = asItIs({coordEndX() + r * sin(angdiff + 3 * M_PI / 2),y + r * cos(angdiff + 3 * M_PI / 2)});

                drawLine( {lineTop, lineBottom}, QPen(Qt::black, 0.5, Qt::DotLine));
                y -= 1.0;
            }
        }


        {
            double x = 1.0;
            while( x > coordStartX())
            {
                double angdiff = M_PI/2;

                QPointF lineTop = asItIs({x + r * sin(angdiff + M_PI / 2), coordStartY() + r * cos(angdiff + M_PI / 2)});
                QPointF lineBottom = asItIs({x + r * sin(angdiff + 3 * M_PI / 2), coordEndY() + r * cos(angdiff + 3 * M_PI / 2)});

                drawLine( {lineTop, lineBottom}, QPen(Qt::black, 0.5, Qt::DotLine));
                x -= 1.0;
            }
        }

        {
            double x = 1.0;
            while( x < coordEndX())
            {
                double angdiff = M_PI/2;

                QPointF lineTop = asItIs({x + r * sin(angdiff + M_PI / 2), coordStartY() + r * cos(angdiff + M_PI / 2)});
                QPointF lineBottom = asItIs({x + r * sin(angdiff + 3 * M_PI / 2), coordEndY() + r * cos(angdiff + 3 * M_PI / 2)});

                drawLine( {lineTop, lineBottom}, QPen(Qt::black, 0.5, Qt::DotLine));
                x += 1.0;
            }
        }
    }

    drawLine(QLineF(asItIs( {0, 0} ), asItIs( {coordEndX(), 0 })), QPen(Qt::blue));
    drawLine(QLineF(asItIs( {0, 0} ), asItIs( {coordStartX(), 0})), QPen(Qt::black));
    drawLine(QLineF(asItIs( {0, 0} ), asItIs( {0, coordEndY()})), QPen(Qt::red));
    drawLine(QLineF(asItIs( {0, 0} ), asItIs( {0, coordStartY()})), QPen(Qt::black));

    auto endPoint1 = asItIs( {0, coordEndY()});

    // the points of the arrow will be on a circle, radius 0.05, with rotation angle +/- 15 degreees
    r = 0.3;
    double firstAngdiffTop = 0.261799 + M_PI;
    double firstAngdiffBottom = 0.261799 + M_PI;
    QPointF firstArrowheadTop = asItIs({r * sin(firstAngdiffTop), coordEndY() + r * cos(+ firstAngdiffTop)});
    QPointF firstArrowheadBottom = asItIs({r * sin(- firstAngdiffBottom), coordEndY() + r * cos(- firstAngdiffBottom)});

    drawLine(QLineF(endPoint1, firstArrowheadTop), QPen(Qt::red));
    drawLine(QLineF(endPoint1, firstArrowheadBottom), QPen(Qt::red));

    auto endPoint2 = asItIs( {coordEndX(), 0});

    double secondAngdiffTop = 0.261799 + 3 * M_PI / 2;
    double secondAngdiffBottom = 0.261799 + M_PI /2;

    QPointF secondArrowheadTop = asItIs({coordEndX() + r * sin(secondAngdiffTop), r * cos(+ secondAngdiffTop)});
    QPointF secondArrowheadBottom = asItIs({coordEndX() + r * sin(- secondAngdiffBottom), r * cos(- secondAngdiffBottom)});

    drawLine(QLineF(endPoint2, secondArrowheadTop), QPen(Qt::blue));
    drawLine(QLineF(endPoint2, secondArrowheadBottom), QPen(Qt::blue));

    // the small lines
    r = 0.1;
    {
    double x = -1.0;
    while(x > coordStartX())
    {
        double angdiff = M_PI/2;

        QPointF lineTop = asItIs({x + r * sin(angdiff + M_PI / 2), + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = asItIs({x + r * sin(angdiff + 3 * M_PI / 2), + r * cos(angdiff + 3 * M_PI / 2)});

        drawLine( {lineTop, lineBottom}, QPen(Qt::black));
        if(m_drawNumbers) drawText(lineBottom, QString::number(x, 'f', 0), QPen(Qt::black));
//        QGraphicsTextItem *text = sc->addText(QString::number(x, 'f', 0));
//        text->setPos(lineBottom.x() - text->boundingRect().width() / 2, lineBottom.y() + 6);
        x -= 1.0;
    }
    }

    {
    double x = 1.0;
    while( x < coordEndX())
    {
        double angdiff = M_PI/2;

        QPointF lineTop = asItIs({x + r * sin(angdiff + M_PI / 2), + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = asItIs({x + r * sin(angdiff + 3 * M_PI / 2), + r * cos(angdiff + 3 * M_PI / 2)});

        drawLine( {lineTop, lineBottom}, QPen(Qt::blue));
        if(m_drawNumbers) drawText(lineBottom, QString::number(x, 'f', 0), QPen(Qt::black));
//        QGraphicsTextItem *text = sc->addText(QString::number(x, 'f', 0));
//        text->setPos(lineBottom.x() - text->boundingRect().width() / 2, lineBottom.y() + 6);
        x += 1.0;
    }
    }

    {
    double y = -1.0;
    while(y > coordStartY())
    {
        double angdiff = M_PI;

        QPointF lineTop = asItIs({r * sin(angdiff + M_PI / 2),y + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = asItIs({r * sin(angdiff + 3 * M_PI / 2),y + r * cos(angdiff + 3 * M_PI / 2)});

        drawLine( {lineTop, lineBottom}, QPen(Qt::black));
        if(m_drawNumbers) drawText(lineBottom, QString::number(y, 'f', 0), QPen(Qt::black));
//        QGraphicsTextItem *text = sc->addText(QString::number(y, 'f', 0));
//        text->setPos(lineTop.x() - text->boundingRect().width(), lineTop.y() - text->boundingRect().height() / 2);
        y -= 1.0;
    }
    }

    {
    double y = 1.0;
    while( y < coordEndY())
    {
        double angdiff = M_PI;

        QPointF lineTop = asItIs({r * sin(angdiff + M_PI / 2),y + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = asItIs({r * sin(angdiff + 3 * M_PI / 2),y + r * cos(angdiff + 3 * M_PI / 2)});

        drawLine( {lineTop, lineBottom}, QPen(Qt::red));
        if(m_drawNumbers) drawText(lineBottom, QString::number(y, 'f', 0), QPen(Qt::black));

//        QGraphicsTextItem *text = sc->addText(QString::number(y, 'f', 0));
//        text->setPos(lineTop.x() - text->boundingRect().width(), lineTop.y() - text->boundingRect().height() / 2);
        y += 1.0;
    }
    }
}

int AbstractDrawer::sceneX(double x)
{
    double zeroX = getClipRect().width() / 2.0;
    double res = zeroX + static_cast<double>( getSceneScrollX() ) + x * zoomFactor();
    int resI = static_cast<int>(round(res));
//    qDebug() << "x=" << x << "to:" << res << "I:" << resI;

    return resI;
}

int AbstractDrawer::sceneY(double y)
{
    double zeroY = getClipRect().height() / 2;
    double res = zeroY +static_cast<double>( getSceneScrollY() ) - y * zoomFactor();
    int resI = static_cast<int>(round(res));

//    qDebug() << "y=" << y << "to:" << res << "I:"<< resI;

    return resI;
}


QPoint AbstractDrawer::toScene(QPointF f)
{
    QPointF rotated = rotatePoint(0, 0, rotationAngle(), f);
    int scx = sceneX(rotated.x());
    int scy = sceneY(rotated.y());

    return {scx, scy};
}

double AbstractDrawer::coordStartX()
{
    return m_coordStartX;
}

double AbstractDrawer::coordEndX()
{
    return m_coordEndX;
}

double AbstractDrawer::coordStartY()
{
    return m_coordStartY;
}

double AbstractDrawer::coordEndY()
{
    return m_coordEndY;
}

double AbstractDrawer::zoomFactor() const
{
    return m_zoomFactor;
}

void AbstractDrawer::drawText(const QPointF &at, const QString& s, const QPen& p)
{
    m_drawnText.push_back({s, p, at});
}

double AbstractDrawer::rotationAngle()
{
    return m_rotationAngle;
}

int AbstractDrawer::getSceneScrollX() const
{
    return m_sceneScrollX;
}

int AbstractDrawer::getSceneScrollY() const
{
    return m_sceneScrollY;
}

void AbstractDrawer::reset()
{
    m_drawnLines.clear();
    m_drawnPoints.clear();

    m_rotationAngle = 0.0;
    m_zoomFactor = 50.0;
    m_coordEndY = 10.0;
    m_coordStartY = -10.0;
    m_coordEndX = 15.0;
    m_coordStartX = -15.0;
    m_drawGrid = true;

}

const std::vector<DrawnLine> &AbstractDrawer::getDrawnLines() const
{
    return m_drawnLines;
}

void AbstractDrawer::setDrawnLines(const std::vector<DrawnLine> &newDrawnLines)
{
    m_drawnLines = newDrawnLines;
}

const std::vector<DrawnPoint> &AbstractDrawer::getDrawnPoints() const
{
    return m_drawnPoints;
}

void AbstractDrawer::setDrawnPoints(const std::vector<DrawnPoint> &newDrawnPoints)
{
    m_drawnPoints = newDrawnPoints;
}

void AbstractDrawer::setRotationAngle(double newRotationAngle)
{
    m_rotationAngle = newRotationAngle;
}

void AbstractDrawer::setZoomFactor(double newZoomFactor)
{
    m_zoomFactor = newZoomFactor;
}

void AbstractDrawer::setShowGrid(bool v)
{
    m_drawGrid = v;
}

void AbstractDrawer::setCoordEndY(double newCoordEndY)
{
    m_coordEndY = newCoordEndY;
}

void AbstractDrawer::setCoordStartY(double newCoordStartY)
{
    m_coordStartY = newCoordStartY;
}

void AbstractDrawer::setCoordEndX(double newCoordEndX)
{
    m_coordEndX = newCoordEndX;
}

void AbstractDrawer::setCoordStartX(double newCoordStartX)
{
    m_coordStartX = newCoordStartX;
}

void AbstractDrawer::addLine(QLineF l, QPen p, int size)
{
    m_drawnLines.push_back({l, p, size});
}

void AbstractDrawer::addPoint(QPointF l, QPen p, size_t s)
{
    m_drawnPoints.push_back({l, p, s});
}

int AbstractDrawer::getWidth() const
{
    return m_width;
}

int AbstractDrawer::getHeight() const
{
    return m_height;
}

void AbstractDrawer::setWidth(int newWidth)
{
    m_width = newWidth;qInfo()<< newWidth;
}

void AbstractDrawer::setHeight(int newHeight)
{
    m_height = newHeight;
}

