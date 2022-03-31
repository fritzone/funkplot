#include "DrawingForm.h"
#include "MainWindow.h"
#include "RuntimeProvider.h"
#include <DockWidget.h>
#include <QApplication>
#include <KDDockWidgets/src/MainWindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    KDDockWidgets::MainWindow mainWindow(QStringLiteral("MyMainWindow"), KDDockWidgets::MainWindowOption_HasCentralWidget);
    mainWindow.setWindowTitle("FunPlotter");
    mainWindow.resize(1200, 1200);
    mainWindow.show();

    DrawingForm* df;
    MainWindow* mainWidget;

    RuntimeProvider* rp = new RuntimeProvider (
        [&df, &mainWidget](QString e) { mainWidget->reportError(e); },
        [&df, &mainWidget](double x, double y) {df->drawPoint(x ,y); },
        [&df, &mainWidget](QString s) { mainWidget->setCurrentStatement(s); },
        [&df, &mainWidget](int r, int g, int b, int a) { df->setDrawingPen(r, g, b, a); },
        [&df, &mainWidget](QSharedPointer<Plot> p) { df->drawPlot(p); }
        );

    df = new DrawingForm(rp);

    auto mainDock = new KDDockWidgets::DockWidget(QStringLiteral("Code"));
    mainWidget = new MainWindow(rp, df);
    mainDock->setWidget(mainWidget);

    auto drawingDock = new KDDockWidgets::DockWidget(QStringLiteral("Graphics"));
    drawingDock->setWidget(df);
    drawingDock->setOptions(KDDockWidgets::DockWidgetBase::Option_NotClosable);
    mainDock->setOptions(KDDockWidgets::DockWidgetBase::Option_NotClosable);

    mainWindow.addDockWidget(mainDock, KDDockWidgets::Location_OnTop);
    mainWindow.addDockWidget(drawingDock, KDDockWidgets::Location_None);
    mainWindow.setPersistentCentralWidget(drawingDock);

    return a.exec();
}
