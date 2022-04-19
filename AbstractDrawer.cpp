#include "AbstractDrawer.h"
#include "util.h"
#include <QPen>
#include <cmath>

static QPointF asItIs(QPointF p)
{
    return p;
}

void AbstractDrawer::drawCoordinateSystem()
{
    drawLine(QLineF(asItIs( {0, 0} ), asItIs( {coordEndX(), 0 })), QPen(Qt::blue));
    drawLine(QLineF(asItIs( {0, 0} ), asItIs( {coordStartX(), 0})), QPen(Qt::red));
    drawLine(QLineF(asItIs( {0, 0} ), asItIs( {0, coordEndY()})), QPen(Qt::red));
    drawLine(QLineF(asItIs( {0, 0} ), asItIs( {0, coordStartY()})), QPen(Qt::red));

    auto endPoint1 = asItIs( {0, coordEndY()});

    // the points of the arrow will be on a circle, radius 0.05, with rotation angle +/- 15 degreees
    double r = 0.3;
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
    for(double x = -1.0; x > coordStartX(); x -= 1.0)
    {
        double angdiff = M_PI/2;

        QPointF lineTop = asItIs({x + r * sin(angdiff + M_PI / 2), + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = asItIs({x + r * sin(angdiff + 3 * M_PI / 2), + r * cos(angdiff + 3 * M_PI / 2)});

        drawLine( {lineTop, lineBottom}, QPen(Qt::red));

//        QGraphicsTextItem *text = sc->addText(QString::number(x, 'f', 0));
//        text->setPos(lineBottom.x() - text->boundingRect().width() / 2, lineBottom.y() + 6);
    }

    for(double x = 1.0; x < coordEndX(); x += 1.0)
    {
        double angdiff = M_PI/2;

        QPointF lineTop = asItIs({x + r * sin(angdiff + M_PI / 2), + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = asItIs({x + r * sin(angdiff + 3 * M_PI / 2), + r * cos(angdiff + 3 * M_PI / 2)});

        drawLine( {lineTop, lineBottom}, QPen(Qt::blue));

//        QGraphicsTextItem *text = sc->addText(QString::number(x, 'f', 0));
//        text->setPos(lineBottom.x() - text->boundingRect().width() / 2, lineBottom.y() + 6);
    }

    for(double y = -1.0; y > coordStartY(); y -= 1.0)
    {
        double angdiff = M_PI;

        QPointF lineTop = asItIs({r * sin(angdiff + M_PI / 2),y + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = asItIs({r * sin(angdiff + 3 * M_PI / 2),y + r * cos(angdiff + 3 * M_PI / 2)});

        drawLine( {lineTop, lineBottom}, QPen(Qt::red));

//        QGraphicsTextItem *text = sc->addText(QString::number(y, 'f', 0));
//        text->setPos(lineTop.x() - text->boundingRect().width(), lineTop.y() - text->boundingRect().height() / 2);
    }

    for(double y = 1.0; y < coordEndY(); y += 1.0)
    {
        double angdiff = M_PI;

        QPointF lineTop = asItIs({r * sin(angdiff + M_PI / 2),y + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = asItIs({r * sin(angdiff + 3 * M_PI / 2),y + r * cos(angdiff + 3 * M_PI / 2)});

        drawLine( {lineTop, lineBottom}, QPen(Qt::red));

//        QGraphicsTextItem *text = sc->addText(QString::number(y, 'f', 0));
//        text->setPos(lineTop.x() - text->boundingRect().width(), lineTop.y() - text->boundingRect().height() / 2);
    }

}

QPoint AbstractDrawer::toScene(QPointF f)
{
    QPointF rotated = rotatePoint(0, 0, rotationAngle(), f);
    int scx = sceneX(rotated.x());
    int scy = sceneY(rotated.y());

    return {scx, scy};
}

void AbstractDrawer::reset()
{
    drawnLines.clear();
    drawnPoints.clear();

}

void AbstractDrawer::addLine(QLineF l, QPen p)
{
    drawnLines.append({l, p});
}

void AbstractDrawer::addPoint(QPointF l, QPen p)
{
    drawnPoints.append({l, p});
}
