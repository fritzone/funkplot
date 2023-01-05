#include "DrawingForm.h"
#include "HelpWindow.h"
#include "MainWindow.h"
#include "RuntimeProvider.h"
#include "StatementHandler.h"

#include <KDDockWidgets/src/MainWindow.h>
#include <DockWidget.h>

#include <QScreen>
#include <QSplashScreen>
#include <QAction>
#include <QApplication>
#include <QSettings>
#include <QStyleFactory>
#include <QWebEngineUrlScheme>

QScreen* getActiveScreen(QWidget* pWidget)
{
    QScreen* pActive = nullptr;

    while (pWidget)
    {
        auto w = pWidget->windowHandle();
        if (w != nullptr)
        {
            pActive = w->screen();
            break;
        }
        else
            pWidget = pWidget->parentWidget();
    }

    return pActive;
}

int main(int argc, char *argv[])
{
#ifndef _WIN32
    setlocale(LC_ALL|~LC_NUMERIC, "C");
#endif

    QWebEngineUrlScheme scheme("qthelp");
    QWebEngineUrlScheme::registerScheme(scheme);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    qSetMessagePattern("\033[32m%{time h:mm:ss.zzz}%{if-category}\033[32m %{category}:%{endif}\033[36m[%{threadid}]\033[32m %{if-debug}\033[32m%{backtrace depth=1}%{endif}%{if-warning}\033[31m%{backtrace depth=13}%{endif}%{if-critical}\033[31m%{backtrace depth=5}%{endif}%{if-fatal}\033[31m%{backtrace depth=5}%{endif}%{if-info}\033[33m%{backtrace depth=1}%{endif}\033[0m %{message}");

    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationName(UNA_FROG);
    QCoreApplication::setOrganizationDomain("funkplot.sh");
    QCoreApplication::setApplicationName("funkplot");
    QApplication a(argc, argv);
    QPixmap pixmap(":/img/img/about.png");
    QSplashScreen *splash = new QSplashScreen(pixmap, Qt::WindowStaysOnTopHint);
    splash->show();
    a.processEvents();
    splash->repaint();
    QCoreApplication::processEvents();

    KDDockWidgets::MainWindow mainWindow(QStringLiteral("fũnkplot"), KDDockWidgets::MainWindowOption_HasCentralWidget);
    mainWindow.setWindowTitle("fũnkplot");
    mainWindow.show();

#if 0
    QSettings settings;
    QRect r = settings.value("geometry").toRect();

    mainWindow.resize(r.size());
    mainWindow.move(settings.value("position").toPoint());
#endif

    qInfo() << "Qt:" << QT_VERSION_STR;

    QRect r = getActiveScreen(&mainWindow)->availableGeometry();
    mainWindow.resize(r.size());
    mainWindow.move(r.topLeft());

    DrawingForm* df = nullptr;
    MainWindow* mainWidget = nullptr;
    HelpWindow* hw = nullptr;

    RuntimeProvider* rp = new RuntimeProvider (
        [&df, &mainWidget](int l, int c, QString e) { mainWidget->reportError(l, c, e); },
        [&df, &mainWidget](QString s) { mainWidget->printString(s); },
        [&df, &mainWidget](double x, double y) {df->drawPoint(x ,y); },
        [&df, &mainWidget](QString s) { mainWidget->setCurrentStatement(s); },
        [&df, &mainWidget](int r, int g, int b, int a, int s) { df->setDrawingPen(r, g, b, a, s); },
        [&df, &mainWidget](QSharedPointer<Plot> p) { df->drawPlot(p); }
        );

    registerClasses();

    df = new DrawingForm(rp);
    QObject::connect(rp, SIGNAL(rotationAngleChange(double)), df, SLOT(on_rotationAngleChange(double)));
    QObject::connect(rp, SIGNAL(zoomFactorChange(double)), df, SLOT(on_zoomFactorChange(double)));
    QObject::connect(rp, SIGNAL(gridChange(bool)), df, SLOT(on_gridChange(bool)));
    QObject::connect(rp, SIGNAL(coordEndYChange(double)), df, SLOT(on_coordEndYChange(double)));
    QObject::connect(rp, SIGNAL(coordStartYChange(double)), df, SLOT(on_coordStartYChange(double)));
    QObject::connect(rp, SIGNAL(coordEndXChange(double)), df, SLOT(on_coordEndXChange(double)));
    QObject::connect(rp, SIGNAL(coordStartXChange(double)), df, SLOT(on_coordStartXChange(double)));

    hw = new HelpWindow();

    auto mainDock = new KDDockWidgets::DockWidget(QStringLiteral("Code"));
    mainWidget = new MainWindow(rp, df);
    mainDock->setWidget(mainWidget);
    mainDock->setOptions(KDDockWidgets::DockWidgetBase::Option_NotClosable);
    mainDock->setTitle("fũnkplot - Code");

    auto drawingDock = new KDDockWidgets::DockWidget(QStringLiteral("Graphics"));
    drawingDock->setWidget(df);
    drawingDock->setOptions(KDDockWidgets::DockWidgetBase::Option_NotClosable);

    auto helpDock = new KDDockWidgets::DockWidget(QStringLiteral("Help"));
    helpDock->setWidget(hw);

    mainWindow.addDockWidget(mainDock, KDDockWidgets::Location_None);
    mainWindow.setPersistentCentralWidget(mainDock);

    mainWindow.addDockWidget(helpDock, KDDockWidgets::Location_None);
    mainWindow.addDockWidget(drawingDock, KDDockWidgets::Location_OnRight);
    helpDock->hide();

    mainWidget->set_HelpWindow(helpDock);
    mainWindow.update();

    mainWindow.setWindowIcon(QIcon(":/icons/icons/fx.png"));

    QApplication::setStyle(QStyleFactory::create("Fusion"));
    splash->finish(&mainWindow);

    return a.exec();
}
