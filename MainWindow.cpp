#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Function.h"
#include "util.h"
#include "colors.h"
#include "MyGraphicsView.h"

#include <QGraphicsTextItem>
#include <QDebug>
#include <QSharedPointer>
#include <QPen>
#include <QDockWidget>

#include <functional>


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


    sc = new QGraphicsScene(graphicsView->rect());
    graphicsView->setScene(sc);

    QDockWidget *dock = new QDockWidget(this);
    dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dock->setWidget(ui->frmCodeBlock);

    addDockWidget(Qt::TopDockWidgetArea, dock);

    sc->setBackgroundBrush(Qt::white);
    drawCoordinateSystem();

    connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockWidgetTopLevelChanged(bool)));
    dock->setTitleBarWidget(ui->frame);

    QMenuBar *menuBar = new QMenuBar(dock);
    auto fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction("&New");


    ui->verticalLayout->addWidget(graphicsView);

    graphicsView->viewport()->setFocusProxy(0);

}

MainWindow::~MainWindow()
{
    delete ui;
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
    sc->addLine(sceneX(0), sceneY(0), sceneX(coordEndX()), sceneY(0), QPen(Qt::red));
    sc->addLine(sceneX(0), sceneY(0), sceneX(coordStartX()), sceneY(0), QPen(Qt::red));
    sc->addLine(sceneX(0), sceneY(0), sceneX(0), sceneY(coordEndY()), QPen(Qt::red));
    sc->addLine(sceneX(0), sceneY(0), sceneX(0), sceneY(coordStartY()), QPen(Qt::red));
    sc->addLine(sceneX(0), sceneY(coordEndY()), sceneX(0) - 6, sceneY(coordEndY()) + 6, QPen(Qt::red));
    sc->addLine(sceneX(0), sceneY(coordEndY()), sceneX(0) + 6, sceneY(coordEndY()) + 6, QPen(Qt::red));
    sc->addLine(sceneX(coordEndX()), sceneY(0), sceneX(coordEndX()) - 6, sceneY(0) - 6, QPen(Qt::red));
    sc->addLine(sceneX(coordEndX()), sceneY(0), sceneX(coordEndX()) - 6, sceneY(0) + 6, QPen(Qt::red));

    for(double x = -1.0; x > coordStartX(); x -= 1.0)
    {
        QPoint pointOnCoordLine{sceneX(x), sceneY(0)};
        sc->addLine(pointOnCoordLine.x(), pointOnCoordLine.y() - 3 ,pointOnCoordLine.x(), pointOnCoordLine.y() + 3 , QPen(Qt::red));

        QGraphicsTextItem *text = sc->addText(QString::number(x, 'f', 0));
        text->setPos(pointOnCoordLine.x() - text->boundingRect().width() / 2, pointOnCoordLine.y() + 6);
    }

    for(double x = 1.0; x < coordEndX(); x += 1.0)
    {
        QPoint pointOnCoordLine{sceneX(x), sceneY(0)};
        sc->addLine(pointOnCoordLine.x(), pointOnCoordLine.y() - 3 ,pointOnCoordLine.x(), pointOnCoordLine.y() + 3 , QPen(Qt::red));

        QGraphicsTextItem *text = sc->addText(QString::number(x, 'f', 0));
        text->setPos(pointOnCoordLine.x() - text->boundingRect().width() / 2, pointOnCoordLine.y() + 6);
    }

    for(double y = -1.0; y > coordStartY(); y -= 1.0)
    {
        QPoint pointOnCoordLine{sceneX(0), sceneY(y)};
        sc->addLine(pointOnCoordLine.x(), pointOnCoordLine.y() - 3 ,pointOnCoordLine.x(), pointOnCoordLine.y() + 3 , QPen(Qt::red));

        QGraphicsTextItem *text = sc->addText(QString::number(y, 'f', 0));
        text->setPos(pointOnCoordLine.x() + 6, pointOnCoordLine.y() - text->boundingRect().height() / 2);
    }

    for(double y = 1.0; y < coordEndY(); y += 1.0)
    {
        QPoint pointOnCoordLine{sceneX(0), sceneY(y)};
        sc->addLine(pointOnCoordLine.x(), pointOnCoordLine.y() - 3 ,pointOnCoordLine.x(), pointOnCoordLine.y() + 3 , QPen(Qt::red));

        QGraphicsTextItem *text = sc->addText(QString::number(y, 'f', 0));
        text->setPos(pointOnCoordLine.x() + 6, pointOnCoordLine.y() - text->boundingRect().height() / 2);
    }
}

void MainWindow::setDrawingPen(int r, int g, int b, int a)
{
    drawingPen = QPen(QColor(qRgba( r, g, b, a)));
}

int MainWindow::sceneX(double x)
{
    int zeroX = sc->sceneRect().width() / 2;

    return zeroX + sceneScrollX + x * zoomFactor();
}

int MainWindow::sceneY(double y)
{
    int zeroY = sc->sceneRect().height() / 2;

    return zeroY + sceneScrollY - y * zoomFactor();
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
            drawnPoints.append({x, y});
        }
    };

    genericPlotIterator(plot, executor);

    return points;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    sc->setSceneRect(graphicsView->rect());
    qDebug() << sc->sceneRect();
    QRect wrect = rect();
//    ui->splitter->setSizes({wrect.height() / 3, 2 * wrect.height() / 3} );
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
        sc->addEllipse(sceneX(p.x()), sceneY(p.y()), 1.0, 1.0);
    }

}

void MainWindow::drawPoint(double x, double y)
{

    sc->addEllipse(sceneX(x), sceneY(y), 1.0, 1.0, drawingPen);
    drawnPoints.append({x, y});

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
