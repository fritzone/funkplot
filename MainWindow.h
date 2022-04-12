#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPen>
#include <QMap>
#include <QSet>
#include <QDebug>

#include "Function.h"
#include "RuntimeProvider.h"
#include "CodeEngine.h"
#include "TextEditWithCodeCompletion.h"

#include <functional>

class QGraphicsScene;
class MyGraphicsView;


class DrawingForm;
namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(RuntimeProvider *rp, DrawingForm* df, QWidget *parent = nullptr);
    ~MainWindow();

    void reportError(QString err);
    void setCurrentStatement(const QString &newCurrentStatement);

private slots:
    void on_toolButton_clicked();

private:

    Ui::MainWindow *ui;

    // in case we resize/zoom/scroll the window, these objects will be used to redraw the scene
    friend struct Sett;
    friend struct Assignment;
    QString currentStatement;
    DrawingForm* m_df;
    RuntimeProvider *m_rp;
    QDockWidget *dock = nullptr;
    TextEditWithCodeCompletion *textEdit = nullptr;
    FrameForLineNumbers* frmLineNrs = nullptr;
};

#endif // MAINWINDOW_H
