#include "CodeEditorTabPage.h"
#include "ui_CodeEditorTabPage.h"

CodeEditorTabPage::CodeEditorTabPage(QWidget *parent, RuntimeProvider* rp) :
    QWidget(parent),
    ui(new Ui::CodeEditorTabPage)
{
    ui->setupUi(this);

    m_frmLineNrs = new FrameForLineNumbers(ui->frmCodeditor);
    m_textEdit = new TextEditWithCodeCompletion(ui->frmCodeditor, rp);
    ui->horizontalLayout->addWidget(m_frmLineNrs);
    ui->horizontalLayout->addWidget(m_textEdit);
    m_textEdit->setLineNumberFrame(m_frmLineNrs);
    m_textEdit->setFocus();

    connect(m_textEdit, SIGNAL(pTextChanged()), this, SLOT(onTextChanged()));

    auto DUMMY3 = R"(
var a b number

let a = 4
let b = 5

if a == 4 and b == 5 do
  python do print (a, b)
else
   python do print (b)
done

)";

    m_textEdit->insertText(DUMMY3);
    m_textEdit->setInitialText(DUMMY3);
    m_textEdit->set_LastSavedText(DUMMY3);
    m_textEdit->setFocus();

    QTextCursor t = m_textEdit->textCursor();
    t.setPosition(0);
    m_textEdit->setTextCursor(t);
    m_textEdit->ensureCursorVisible();
    m_textEdit->updateLineNumbers();

}


CodeEditorTabPage::~CodeEditorTabPage()
{
    delete ui;
}

TextEditWithCodeCompletion *CodeEditorTabPage::getTextEdit() const
{

    return m_textEdit;
}

void CodeEditorTabPage::highlightLine(int l, const QString &a)
{
    m_frmLineNrs->highlightLine(l, a);

    if(l > 0)
    {
        QTextCursor cursor(m_textEdit->document()->findBlockByLineNumber(l - 1)); // ln-1 because line number starts from 0
        m_textEdit->setTextCursor(cursor);
        m_textEdit->setFocus(Qt::OtherFocusReason);
    }
    else
    {
        QTextCursor cursor(m_textEdit->document()->findBlockByLineNumber(0)); // ln-1 because line number starts from 0
        m_textEdit->setTextCursor(cursor);
        m_textEdit->setFocus(Qt::OtherFocusReason);
    }
}

void CodeEditorTabPage::appendText(const QString &s)
{
    auto c = m_textEdit->textCursor();
    auto c1 = c;
    c.setPosition(0);
    m_textEdit->setTextCursor(c);
    m_textEdit->insertText(s);
    m_textEdit->setTextCursor(c1);
}

void CodeEditorTabPage::restoreTextEditState()
{
    m_textEdit->setTextCursor(m_cursor);
}

void CodeEditorTabPage::saveTextEditState()
{
    m_cursor = m_textEdit->textCursor();
}

void CodeEditorTabPage::setText(const QStringList &qsl)
{
    auto s = qsl.join('\n');
    m_textEdit->setPlainText(s);
    m_textEdit->setInitialText(s);
}

void CodeEditorTabPage::onTextChanged()
{
    emit textChanged();
}
