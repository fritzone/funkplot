#include "ExportImageDialog.h"
#include "ui_ExportImageDialog.h"

#include "DrawingForm.h"
//#include "ImageDrawer.h"

#include <QGraphicsPixmapItem>

ExportImageDialog::ExportImageDialog(DrawingForm *df, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportImageDialog), m_df(df)
{
    ui->setupUi(this);
    ui->frmWH->hide();

    /*imgDrawer = new ImageDrawer(640, 480);
    m_df->populateDrawerData(imgDrawer);
    imgDrawer->redrawEverything();*/

    item = new QGraphicsPixmapItem ( m_df->screenshot() );
    scene = new QGraphicsScene;
    scene->setSceneRect(item->boundingRect());
    scene->addItem(item);

    ui->graphicsView->setScene(scene);

    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

ExportImageDialog::~ExportImageDialog()
{
    delete ui;
}

int ExportImageDialog::width() const
{
    return m_width;
}

int ExportImageDialog::height() const
{
    return m_height;
}

void ExportImageDialog::setDf(DrawingForm *newDf)
{
    m_df = newDf;
}

void ExportImageDialog::on_comboBox_currentTextChanged(const QString &s)
{
    if(s == "Manual")
    {
        ui->frmWH->show();
    }
    else
    {
        ui->frmWH->hide();
    }
}
