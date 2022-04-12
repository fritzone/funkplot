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
#include "CodeEditorTabPage.h"

#include <functional>

class QGraphicsScene;
class MyGraphicsView;


class DrawingForm;
namespace Ui {
class MainWindow;
}

struct Program
{
    // create a new program
    Program(QWidget* tabContainer, RuntimeProvider *rp);

    void setCurrentStatement(const QString &newCurrentStatement);
    void run();

    QString currentStatement;
    RuntimeProvider *m_rp;
    TextEditWithCodeCompletion* m_textEditor = nullptr;
    CodeEditorTabPage* m_tabPage;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(RuntimeProvider *rp, DrawingForm* df, QWidget *parent = nullptr);
    ~MainWindow();

    void reportError(QString err);
    void setCurrentStatement(const QString &newCurrentStatement);

private:

    void runCurrentCode();

private:
    Ui::MainWindow *ui;

    // the common drawing form
    DrawingForm* m_df;

    // the programs and their associated tab/texteditor pages
    QVector<QSharedPointer<Program>> m_programs;
    QSharedPointer<Program> m_currentProgram;
};

#endif // MAINWINDOW_H
