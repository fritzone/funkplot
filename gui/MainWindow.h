#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "PaletteListForm.h"
#include "RuntimeProvider.h"
#include "TabToolbar/Builder.h"
#include "ViewType.h"

#include <QMainWindow>
#include <QPen>
#include <QMap>
#include <QSet>
#include <QDebug>
#include <QLabel>
#include <QFutureWatcher>

#include <functional>

class QGraphicsScene;
class MyGraphicsView;
class Program;
class DrawingForm;
class Builtin;
class CodeEditorTabPage;

namespace KDDockWidgets { class DockWidget; }
namespace Ui {
class MainWindow;
}

namespace
{
static const QString UNA_FROG = "The Unauthorized Frog";
static const QString GEODRAW = "fũnkplot";
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

    explicit MainWindow(RuntimeProvider *rp, DrawingForm* df, QWidget *parent = nullptr);
    ~MainWindow();

    void reportError(int, int c, QString err);
    void printString(QString s);
    void setCurrentStatement(const QString &newCurrentStatement);
    void stopRunningCurrentCode();
    void setView(ViewType v);
    void set_HelpWindow(KDDockWidgets::DockWidget *newHelpWindow);
    void updateCurrentTabTextToReflectSavestate();

private slots:
    void on_actionFunctions_triggered();

private slots:
    void on_actionAbout_triggered();
    void on_actionExport_triggered();
    void on_tabWidget_tabCloseRequested(int index);
    void on_actionOpen_triggered();
    void on_actionSaveAs_triggered();
    void on_actionSave_triggered();
    void on_tabWidget_currentChanged(int index);
    void on_actionNew_triggered();
    void on_actionSyntax_triggered();
    void on_actionHelp_triggered();
    void on_actionPalettes_triggered();

private slots:
    void runCurrentCode();
    void runningFinished();
    void onErrorReceived(int,int,QString);
    void onStringReceived(QString);
    void onFunctionsMenuEntryTriggered();

signals:

    void runningDone();
    void onError(int,int,QString);
    void onString(QString);

private:

    std::tuple<int, QString> errorAndLineFromErrorText(const QString&);
    void createDefaultProgram(const QString &prgName = "noname 1");
    void closeEvent(QCloseEvent* event) override;
    void showEvent( QShowEvent* event ) override;
    void buildFunctionDatabase();

private:

    Ui::MainWindow *ui;
    bool m_firstMessage = true;
    QLabel *m_errorLabel = nullptr;
    DrawingForm* m_df;
    Program* m_currentProgram;
    QVector<Program*> m_programs;
    PaletteListForm* m_plf;
    KDDockWidgets::DockWidget* m_helpWindow;
    QFutureWatcher<bool> m_runningWatcher;
    QFuture<bool> m_runningFuture;
    RuntimeProvider* m_rp;
    QVector<std::tuple<int, int, QString>> m_sessionErrors;
    tt::Builder m_ttb {this};
    QMenu* m_functionsMenu = new QMenu(this);
    QMap<QString, QMenu*> m_classMenus;
    QMap<QString, QMenu*> m_categoryMenus;
    QMap<QString, QSharedPointer<Builtin>> m_builtins;
    bool m_showExitMsgBox = true;
};

#endif // MAINWINDOW_H
