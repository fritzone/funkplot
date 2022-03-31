#include "DrawingForm.h"
#include "GraphicsViewZoom.h"
#include "MyGraphicsView.h"
#include "ui_DrawingForm.h"

#include <QGraphicsScene>
#include <QGraphicsTextItem>

DrawingForm::DrawingForm(RuntimeProvider *rp, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrawingForm),
    m_rp(rp)
{
    ui->setupUi(this);

    // teh drawing one
    graphicsView = new MyGraphicsView(ui->frmDrawing);
    graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    Graphics_view_zoom* z = new Graphics_view_zoom(graphicsView);
    z->set_modifiers(Qt::NoModifier);

    sc = new QGraphicsScene(graphicsView->rect());
    graphicsView->setScene(sc);
    graphicsView->viewport()->setFocusProxy(0);
    sc->setBackgroundBrush(Qt::white);

    ui->verticalLayout->addWidget(graphicsView);

    drawCoordinateSystem();

    connect(graphicsView, &MyGraphicsView::redraw, this, [this]()
            {
                redrawEverything();
            }
            );


}

DrawingForm::~DrawingForm()
{
    delete ui;
}

void DrawingForm::drawCoordinateSystem()
{
    sc->clear();

    sc->addLine(QLineF(toScene( {0, 0} ), toScene( {coordEndX(), 0 })), QPen(Qt::blue));
    sc->addLine(QLineF(toScene( {0, 0} ), toScene( {coordStartX(), 0})), QPen(Qt::red));
    sc->addLine(QLineF(toScene( {0, 0} ), toScene( {0, coordEndY()})), QPen(Qt::red));
    sc->addLine(QLineF(toScene( {0, 0} ), toScene( {0, coordStartY()})), QPen(Qt::red));

    auto endPoint1 = toScene( {0, coordEndY()});

    // the points of the arrow will be on a circle, radius 0.05, with rotation angle +/- 15 degreees
    double r = 0.3;
    double firstAngdiffTop = 0.261799 + M_PI;
    double firstAngdiffBottom = 0.261799 + M_PI;
    QPointF firstArrowheadTop = toScene({r * sin(firstAngdiffTop), coordEndY() + r * cos(+ firstAngdiffTop)});
    QPointF firstArrowheadBottom = toScene({r * sin(- firstAngdiffBottom), coordEndY() + r * cos(- firstAngdiffBottom)});

    sc->addLine(QLineF(endPoint1, firstArrowheadTop), QPen(Qt::red));
    sc->addLine(QLineF(endPoint1, firstArrowheadBottom), QPen(Qt::red));

    auto endPoint2 = toScene( {coordEndX(), 0});

    double secondAngdiffTop = 0.261799 + 3 * M_PI / 2;
    double secondAngdiffBottom = 0.261799 + M_PI /2;

    QPointF secondArrowheadTop = toScene({coordEndX() + r * sin(secondAngdiffTop), r * cos(+ secondAngdiffTop)});
    QPointF secondArrowheadBottom = toScene({coordEndX() + r * sin(- secondAngdiffBottom), r * cos(- secondAngdiffBottom)});

    sc->addLine(QLineF(endPoint2, secondArrowheadTop), QPen(Qt::blue));
    sc->addLine(QLineF(endPoint2, secondArrowheadBottom), QPen(Qt::blue));

    // the small lines
    r = 0.1;
    for(double x = -1.0; x > coordStartX(); x -= 1.0)
    {
        double angdiff = M_PI/2;

        QPointF lineTop = toScene({x + r * sin(angdiff + M_PI / 2), + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = toScene({x + r * sin(angdiff + 3 * M_PI / 2), + r * cos(angdiff + 3 * M_PI / 2)});

        sc->addLine( {lineTop, lineBottom}, QPen(Qt::red));

        QGraphicsTextItem *text = sc->addText(QString::number(x, 'f', 0));
        text->setPos(lineBottom.x() - text->boundingRect().width() / 2, lineBottom.y() + 6);
    }

    for(double x = 1.0; x < coordEndX(); x += 1.0)
    {
        double angdiff = M_PI/2;

        QPointF lineTop = toScene({x + r * sin(angdiff + M_PI / 2), + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = toScene({x + r * sin(angdiff + 3 * M_PI / 2), + r * cos(angdiff + 3 * M_PI / 2)});

        sc->addLine( {lineTop, lineBottom}, QPen(Qt::blue));

        QGraphicsTextItem *text = sc->addText(QString::number(x, 'f', 0));
        text->setPos(lineBottom.x() - text->boundingRect().width() / 2, lineBottom.y() + 6);
    }

    for(double y = -1.0; y > coordStartY(); y -= 1.0)
    {
        double angdiff = M_PI;

        QPointF lineTop = toScene({r * sin(angdiff + M_PI / 2),y + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = toScene({r * sin(angdiff + 3 * M_PI / 2),y + r * cos(angdiff + 3 * M_PI / 2)});

        sc->addLine( {lineTop, lineBottom}, QPen(Qt::red));

        QGraphicsTextItem *text = sc->addText(QString::number(y, 'f', 0));
        text->setPos(lineTop.x() - text->boundingRect().width(), lineTop.y() - text->boundingRect().height() / 2);
    }

    for(double y = 1.0; y < coordEndY(); y += 1.0)
    {
        double angdiff = M_PI;

        QPointF lineTop = toScene({r * sin(angdiff + M_PI / 2),y + r * cos(angdiff + M_PI / 2)});
        QPointF lineBottom = toScene({r * sin(angdiff + 3 * M_PI / 2),y + r * cos(angdiff + 3 * M_PI / 2)});

        sc->addLine( {lineTop, lineBottom}, QPen(Qt::red));

        QGraphicsTextItem *text = sc->addText(QString::number(y, 'f', 0));
        text->setPos(lineTop.x() - text->boundingRect().width(), lineTop.y() - text->boundingRect().height() / 2);
    }
}

void DrawingForm::setDrawingPen(int r, int g, int b, int a)
{
    drawingPen = QPen(QColor(qRgba( r, g, b, a)));
}

int DrawingForm::sceneX(double x)
{
    int zeroX = sc->sceneRect().width() / 2;

    return zeroX + graphicsView->get_sceneScrollX() + x * zoomFactor();
}

int DrawingForm::sceneY(double y)
{
    int zeroY = sc->sceneRect().height() / 2;

    return zeroY + graphicsView->get_sceneScrollY() - y * zoomFactor();
}

QPoint DrawingForm::toScene(QPointF f)
{
    QPointF rotated = rotatePoint(0, 0, rotationAngle(), f);
    int scx = sceneX(rotated.x());
    int scy = sceneY(rotated.y());

    return {scx, scy};
}

double DrawingForm::coordStartX()
{
    return -15.0;
}

double DrawingForm::coordEndX()
{
    return 15.0;
}

double DrawingForm::coordStartY()
{
    return -10.0;
}

double DrawingForm::coordEndY()
{
    return 10.0;
}

double DrawingForm::zoomFactor()
{
    return 50.0;
}

double DrawingForm::rotationAngle()
{
    return 0;
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
                QPoint scp1 {toScene({cx, cy})};
                QPoint scp2 {toScene({x, y})};
                sc->addLine( QLine(scp1, scp2), drawingPen);
                drawnLines.append({{static_cast<qreal>(cx), static_cast<qreal>(cy), static_cast<qreal>(x), static_cast<qreal>(y)}, drawingPen});
                cx = x;
                cy = y;
            }
        }
        else
        {
            QPoint scp {toScene({x, y})};

            sc->addEllipse(scp.x(), scp.y(), 1.0, 1.0, drawingPen);
            drawnPoints.append({{x, y}, drawingPen});
        }
    };

    genericPlotIterator(plot, executor);

    return points;

}

void DrawingForm::reset()
{
    drawnLines.clear();
    drawnPoints.clear();

}

void DrawingForm::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    sc->setSceneRect(graphicsView->rect());

    QRect wrect = rect();
    redrawEverything();

}

void DrawingForm::redrawEverything()
{
    drawCoordinateSystem();
    for(const auto&l : qAsConst(drawnLines))
    {
        QPoint p1 = toScene({l.line.x1(), l.line.y1()});
        QPoint p2 = toScene({l.line.x2(), l.line.y2()});
        sc->addLine(QLine(p1, p2) , l.pen);
    }

    for(const auto&p : qAsConst(drawnPoints))
    {
        QPoint pd = toScene({p.point.x(), p.point.y()});
        sc->addEllipse(pd.x(), pd.y(), 1.0, 1.0, p.pen);
    }

}

void DrawingForm::drawPoint(double x, double y)
{
    QPoint p = toScene({x, y});
    sc->addEllipse(p.x(), p.y(), 1.0, 1.0, drawingPen);
    drawnPoints.append({{x, y}, drawingPen});

}
