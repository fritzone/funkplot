#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPen>
#include <QMap>
#include <QSet>
#include <QDebug>
#include <QLabel>

#include "Function.h"
#include "PaletteListForm.h"
#include "RuntimeProvider.h"
#include "CodeEngine.h"
#include "TextEditWithCodeCompletion.h"

#include <functional>

class QGraphicsScene;
class MyGraphicsView;
class Program;
class DrawingForm;

class CodeEditorTabPage;
namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(RuntimeProvider *rp, DrawingForm* df, QWidget *parent = nullptr);
    ~MainWindow();

    void reportError(int, QString err);
    void setCurrentStatement(const QString &newCurrentStatement);
    void runCurrentCode();

private slots:
    void on_actionPalettes_triggered();

private:

    std::tuple<int, QString> errorAndLineFromErrorText(const QString&);

private:


    Ui::MainWindow *ui;
    bool firstMessage = true;
    QLabel *label = nullptr;

    // in case we resize/zoom/scroll the window, these objects will be used to redraw the scene
    friend struct Set;
    friend struct Assignment;
    DrawingForm* m_df;/*
    RuntimeProvider *m_rp;
    QDockWidget *dock = nullptr;
    FrameForLineNumbers* frmLineNrs = nullptr;*/
    QSharedPointer<Program> m_currentProgram;
    QVector<QSharedPointer<Program>> m_programs;
    PaletteListForm* plf;


};

#endif // MAINWINDOW_H
