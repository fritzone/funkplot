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

    //textEdit->insertText("let i = 0\nfunction f(x) = sin(x)\nlet ps = points of f counts 256\nfor i = 0 to 256 step 1 do\nlet cp = ps[i]\nplot cp\ndone\na\nb\nc\nd\ne");
    textEdit->insertText(R"(plot sin(x) + 1 over (-5, 5)
)");
}

CodeEditorTabPage::~CodeEditorTabPage()
{
    delete ui;
}

TextEditWithCodeCompletion *CodeEditorTabPage::get_textEdit() const
{
    return textEdit;
}
