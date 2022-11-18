#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    connect(ui->label, &ClickableLabel::clicked, this, [this]() {this->close();});
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
