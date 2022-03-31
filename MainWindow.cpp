#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Function.h"
#include "util.h"
#include "colors.h"
#include "MyGraphicsView.h"
#include "GraphicsViewZoom.h"
#include "Highlighter.h"
#include "FrameForLineNumbers.h"

#include <QDebug>
#include <QSharedPointer>
#include <QPen>
#include <QDockWidget>
#include <QResizeEvent>
#include <QColorDialog>
#include <QLabel>

#include <functional>

#include "DrawingForm.h"

MainWindow::MainWindow(RuntimeProvider *rp, DrawingForm* df, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_df(df), m_rp(rp)
{

    ui->setupUi(this);

    connect(ui->toolBar->addAction(QIcon(":/img/img/video-play-3-32.ico"), "Run"), &QAction::triggered, this, [this]() {on_toolButton_clicked();});

    frmLineNrs = new FrameForLineNumbers(ui->frmCodeditor);
    textEdit = new TextEditWithCodeCompletion(ui->frmCodeditor, m_rp);
    ui->horizontalLayout->addWidget(frmLineNrs);
    ui->horizontalLayout->addWidget(textEdit);
    textEdit->setLineNumberFrame(frmLineNrs);
    textEdit->setFocus();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reportError(QString err)
{
    static bool firstMessage = true;
    static QLabel *label = nullptr;
    if(firstMessage)
    {
        label = new QLabel;
        statusBar()->addPermanentWidget(label, 1);
        firstMessage = false;
    }
    label->setText(err);
    qWarning() << err;
}


void MainWindow::on_toolButton_clicked()
{
    m_rp->reset();
    m_df->reset();
    m_df->drawCoordinateSystem();
    QStringList codelines = textEdit->toPlainText().split("\n");
    m_rp->parse(codelines);
    m_rp->execute();
}

QVector<QPointF> MainWindow::drawPlot(QSharedPointer<Plot> plot)
{
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
}


void MainWindow::setCurrentStatement(const QString &newCurrentStatement)
{
    currentStatement = newCurrentStatement;
}
