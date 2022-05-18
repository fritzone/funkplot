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
    textEdit->insertText(R"(var b point, ps array
function f(x) = sin(x)
let ps = points of f counts 10
plot ps
let b = ps[1]
set color red
plot b
set color aquamarine
plot ps[2]

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

void CodeEditorTabPage::highlightLine(int l, QString a)
{
    frmLineNrs->highlightLine(l, a);

    QTextCursor cursor(textEdit->document()->findBlockByLineNumber(l - 1)); // ln-1 because line number starts from 0
    textEdit->setTextCursor(cursor);
    textEdit->setFocus(Qt::OtherFocusReason);
}

void CodeEditorTabPage::appendText(QString s)
{
    textEdit->appendPlainText(s);
}

void CodeEditorTabPage::restoreTextEditState()
{
    textEdit->setTextCursor(cursor);
}

void CodeEditorTabPage::saveTextEditState()
{
    cursor = textEdit->textCursor();
}
