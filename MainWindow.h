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

class CodeEditorTabPage;
namespace Ui {
class MainWindow;
}

class Program
{

public:
    Program(QWidget *tabContainer, RuntimeProvider *rp);
    void setCurrentStatement(const QString &newCurrentStatement);
    void run();

    CodeEditorTabPage* m_tabPage = nullptr;
    QString currentStatement;
    RuntimeProvider *m_rp = nullptr;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(RuntimeProvider *rp, DrawingForm* df, QWidget *parent = nullptr);
    ~MainWindow();

    void reportError(QString err);
    void setCurrentStatement(const QString &newCurrentStatement);

    void runCurrentCode();

private:

    Ui::MainWindow *ui;

    // in case we resize/zoom/scroll the window, these objects will be used to redraw the scene
    friend struct Sett;
    friend struct Assignment;
    DrawingForm* m_df;/*
    RuntimeProvider *m_rp;
    QDockWidget *dock = nullptr;
    FrameForLineNumbers* frmLineNrs = nullptr;*/
    QSharedPointer<Program> m_currentProgram;
    QVector<QSharedPointer<Program>> m_programs;
};

#endif // MAINWINDOW_H
