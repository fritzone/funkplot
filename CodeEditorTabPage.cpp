#include "CodeEditorTabPage.h"
#include "ui_CodeEditorTabPage.h"

CodeEditorTabPage::CodeEditorTabPage(QWidget *parent, RuntimeProvider* rp) :
    QWidget(parent),
    ui(new Ui::CodeEditorTabPage)
{
    ui->setupUi(this);

    frmLineNrs = new FrameForLineNumbers(ui->frmCodeditor);
    textEdit = new TextEditWithCodeCompletion(ui->frmCodeditor, rp);
    ui->horizontalLayout->addWidget(frmLineNrs);
    ui->horizontalLayout->addWidget(textEdit);
    textEdit->setLineNumberFrame(frmLineNrs);
    textEdit->setFocus();
}

CodeEditorTabPage::~CodeEditorTabPage()
{
    delete ui;
}
