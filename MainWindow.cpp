#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Function.h"
#include "util.h"
#include "colors.h"
#include "MyGraphicsView.h"
#include "GraphicsViewZoom.h"

#include <QGraphicsTextItem>
#include <QDebug>
#include <QSharedPointer>
#include <QPen>
#include <QDockWidget>
#include <QResizeEvent>

#include <functional>


void MainWindow::createMenubar()
{
    menuBar = new QMenuBar(dock);
    QMenu *fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction("&New");
    connect(fileMenu->addAction("E&xit"), &QAction::triggered, this, []() {QApplication::exit();});

    bar = new QMenuBar(dock);

    // right aligned help
    QMenu *helpMenu = bar->addMenu(tr("&Help"));
    helpMenu->addAction("&About");

    menuBar->setCornerWidget(bar);

}




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    rp(
        [this](QString e) { reportError(e); },
        [this](double x, double y) {drawPoint(x ,y); },
        [this](QString s) { setCurrentStatement(s); },
        [this](int r, int g, int b, int a) { setDrawingPen(r, g, b, a); },
        [this](QSharedPointer<Plot> p) { drawPlot(p); }
      )
{

    ui->setupUi(this);
    graphicsView = new MyGraphicsView(ui->frmDrawing);
    graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    Graphics_view_zoom* z = new Graphics_view_zoom(graphicsView);
    z->set_modifiers(Qt::NoModifier);

    sc = new QGraphicsScene(graphicsView->rect());
    graphicsView->setScene(sc);

    dock = new QDockWidget(this);
    dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dock->setWidget(ui->frmCodeBlock);

    addDockWidget(Qt::TopDockWidgetArea, dock);

    sc->setBackgroundBrush(Qt::white);
    drawCoordinateSystem();

    connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockWidgetTopLevelChanged(bool)));
    dock->setTitleBarWidget(ui->frame);

    // standard menu
    createMenubar();

    ui->verticalLayout->addWidget(graphicsView);

    graphicsView->viewport()->setFocusProxy(0);

    connect(graphicsView, &MyGraphicsView::redraw, this, [this]()
            {
                redrawEverything();
            }
    );
    dock->installEventFilter(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Resize && obj == dock)
    {
        QResizeEvent *resizeEvent = dynamic_cast<QResizeEvent*>(event);
        qDebug("Dock Resized (New Size) - Width: %d Height: %d",
               resizeEvent->size().width(),
               resizeEvent->size().height());

        qDebug() << menuBar->geometry();

        menuBar->setGeometry(QRect{0, 0, resizeEvent->size().width(), 23});
        menuBar->repaint(QRect{0, 0, resizeEvent->size().width(), 23});
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void MainWindow::reportError(QString err)
{
    ui->errorText->setText(err + " <b>[" + currentStatement + "]</b>");
    qWarning() << err;
}


void MainWindow::on_toolButton_clicked()
{
    rp.reset();
    drawnLines.clear();
    drawnPoints.clear();
    drawCoordinateSystem();
    QStringList codelines = ui->textEdit->toPlainText().split("\n");

    rp.execute(codelines);
}

void MainWindow::dockWidgetTopLevelChanged(bool)
{
    QDockWidget* dw = static_cast<QDockWidget*>(QObject::sender());
    if (dw->isFloating())
    {
        dw->setWindowFlags(Qt::CustomizeWindowHint |
                           Qt::Window | Qt::WindowMinimizeButtonHint |
                           Qt::WindowMaximizeButtonHint |
                           Qt::WindowCloseButtonHint);
        dw->show();
    }
}

void MainWindow::drawCoordinateSystem()
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

void MainWindow::setDrawingPen(int r, int g, int b, int a)
{
    drawingPen = QPen(QColor(qRgba( r, g, b, a)));
}

int MainWindow::sceneX(double x)
{
    int zeroX = sc->sceneRect().width() / 2;

    return zeroX + graphicsView->get_sceneScrollX() + x * zoomFactor();
}

int MainWindow::sceneY(double y)
{
    int zeroY = sc->sceneRect().height() / 2;

    return zeroY + graphicsView->get_sceneScrollY() - y * zoomFactor();
}

QPointF rotatePoint(float cx, float cy, float angle, QPointF p)
{
    float s = sin(angle);
    float c = cos(angle);

    // translate point back to origin:
    p.setX(p.x() - cx);
    p.setY(p.y() - cy);

    // rotate point
    float xnew = p.x() * c - p.y() * s;
    float ynew = p.x() * s + p.y() * c;

    // translate point back:
    p.setX( xnew + cx );
    p.setY( ynew + cy );
    return p;
}

QPoint MainWindow::toScene(QPointF f)
{
    QPointF rotated = rotatePoint(0, 0, rotationAngle(), f);
    int scx = sceneX(rotated.x());
    int scy = sceneY(rotated.y());

    return {scx, scy};
}


double MainWindow::coordStartX()
{
    return -15.0;
}

double MainWindow::coordEndX()
{
    return 15.0;
}

double MainWindow::coordStartY()
{
    return -10.0;
}

double MainWindow::coordEndY()
{
    return 10.0;
}

double MainWindow::zoomFactor()
{
    return 50.0;
}

double MainWindow::rotationAngle()
{
    return 0.523599;
}

QVector<QPointF> MainWindow::drawPlot(QSharedPointer<Plot> plot)
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
                sc->addLine( sceneX(cx), sceneY(cy), sceneX(x), sceneY(y), drawingPen);
                drawnLines.append({{cx, cy, x, y}, drawingPen});
                cx = x;
                cy = y;
            }
        }
        else
        {
            sc->addEllipse(sceneX(x), sceneY(y), 1.0, 1.0, drawingPen);
            drawnPoints.append({{x, y}, drawingPen});
        }
    };

    genericPlotIterator(plot, executor);

    return points;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    sc->setSceneRect(graphicsView->rect());

    QRect wrect = rect();
    redrawEverything();
}

void MainWindow::redrawEverything()
{
    drawCoordinateSystem();
    for(const auto&l : qAsConst(drawnLines))
    {
        sc->addLine(sceneX(l.line.x1()), sceneY(l.line.y1()), sceneX(l.line.x2()), sceneY(l.line.y2()), l.pen);
    }

    for(const auto&p : qAsConst(drawnPoints))
    {

        sc->addEllipse(sceneX(p.point.x()), sceneY(p.point.y()), 1.0, 1.0, p.pen);
    }

}

void MainWindow::drawPoint(double x, double y)
{

    sc->addEllipse(sceneX(x), sceneY(y), 1.0, 1.0, drawingPen);
    drawnPoints.append({{x, y}, drawingPen});

}

void MainWindow::setCurrentStatement(const QString &newCurrentStatement)
{
    currentStatement = newCurrentStatement;
}


void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    sc->setSceneRect(graphicsView->rect());
    qDebug() << sc->sceneRect() << pos << index;
    redrawEverything();
}
