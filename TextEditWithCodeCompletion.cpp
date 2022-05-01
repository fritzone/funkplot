#include "TextEditWithCodeCompletion.h"
#include "gui_colors.h"
#include "Highlighter.h"
#include "FrameForLineNumbers.h"
#include "RuntimeProvider.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>
#include <QEvent>
#include <QScrollBar>

int TextEditWithCodeCompletion::TablePositionInText::colorCounter = -1;

static bool skippableSinceWhitespace(QChar c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

TextEditWithCodeCompletion::TextEditWithCodeCompletion(QWidget* p, RuntimeProvider *rp) : QPlainTextEdit(p),
    m_lst(0), m_timer(), m_currentBgColor(Qt::white), m_highlighter(0),
    m_frameForLineNumbers(0), m_disabledRows(), m_rp(rp)
{
    m_lst = new ListWidgetForCodeCompletion(this);
    m_lst->setSelectionMode(QAbstractItemView::SingleSelection);
    m_lst->hide();
    m_timer.setInterval(300);
    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    QObject::connect(m_lst, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onListItemDoubleClicked(QModelIndex)));
    QObject::connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onVScroll(int)));

    QFont font;
    font.setFamily(QString::fromUtf8("Consolas"));
    font.setPixelSize(16);
    setFont(font);

    QPalette plette2 = palette(); // define pallete for textEdit..
    plette2.setColor(QPalette::Base, Qt::white); // set color "Red" for textedit base
    plette2.setColor(QPalette::Text, Qt::black); // set text color which is selected from color pallete
    setPalette(plette2); // change textedit palette

    setLineWrapMode(QPlainTextEdit::NoWrap);
    setFrameShape(QPlainTextEdit::NoFrame);
    setFrameShadow(QPlainTextEdit::Plain);

    m_highlighter.reset(new Highlighter(document(), rp));

    setPlainText("let i = 0\nfunction f(x) = sin(x)\nlet ps = points of f counts 256\nfor i = 0 to 256 step 1 do\nlet cp = ps[i]\nplot cp\ndone");
//    setPlainText("function f(x) = sin(x)\nplot f over(-10, 10)");

}

void TextEditWithCodeCompletion::onTimer()
{
#if 0
    if(!m_lst->isVisible())
    {
        QPoint p = cursorRect().bottomRight();
        m_lst->move(p);
        m_lst->show();
        m_timer.stop();
        populateCodeCompletionListbox();
    }
#endif
}

void TextEditWithCodeCompletion::onListItemDoubleClicked(QModelIndex idx)
{
    QListWidgetItem* item = m_lst->itemFromIndex(idx);
    QString g = item->text();
    m_lst->hide();
    resetBackgrounds();
    insertText(g);
}

void TextEditWithCodeCompletion::insertFunctionParantheses()
{
    insertText("()");
    QTextCursor t = textCursor();
    t.setPosition(t.position() - 1);
    setTextCursor(t);

}

void TextEditWithCodeCompletion::updateLineNumbers()
{
    if(m_frameForLineNumbers)
    {
        QTextDocument *pDoc = document();
        m_frameForLineNumbers->beginLineNumbers();

        int i = 1;
        for(QTextBlock block=pDoc->begin(); block!= pDoc->end(); block = block.next())
        {
            QRectF f = blockBoundingGeometry(block);
            QPointF p = contentOffset();
            m_frameForLineNumbers->addLineNumber(i, f.top() + p.y() + 2, m_disabledRows.indexOf(i) != -1);
            i++;
        }

        m_frameForLineNumbers->endLineNumbers();
    }
}

void TextEditWithCodeCompletion::onVScroll(int)
{
    updateLineNumbers();
}

void TextEditWithCodeCompletion::keyPressEvent(QKeyEvent *e)
{
    QStringList codelines = toPlainText().split("\n");
    m_rp->set_ShouldReport(false);
    m_rp->parse(codelines);
    m_rp->set_ShouldReport(true);
    m_highlighter.reset(new Highlighter(document(), m_rp));

    Qt::KeyboardModifiers m = e->modifiers();

    if(m_disabledRows.indexOf(textCursor().blockNumber() + 1) != -1)
    {
        bool process = false;

        if(e->key() == Qt::Key_Home || e->key() == Qt::Key_End
                || e->key() == Qt::Key_Left || e->key() == Qt::Key_Up
                || e->key() == Qt::Key_Right || e->key() == Qt::Key_Down
                || e->key() == Qt::Key_PageUp || e->key() == Qt::Key_PageDown
                || e->key() == Qt::Key_Insert)
        {
            process = true;
        }

        if(!process)
        {
            return;
        }
    }

    m_timer.stop();

    if(e->key() == Qt::Key_Period)
    {
        m_timer.start();
    }

    if(e->key() == Qt::Key_Comma)
    {
        m_timer.start();
    }

    if(m.testFlag(Qt::ControlModifier))
    {
        int t = e->key();
#if 0
        if(e->key() == Qt::Key_Space)
        {
            QPoint p = cursorRect().bottomRight();
            m_lst->move(p);
            m_lst->show();
            populateCodeCompletionListbox();
            updateLineNumbers();
            return;
        }
#endif
        if(t == Qt::Key_Enter ||t == Qt::Key_Return) // Ctrl + Enter is supposed to run the query if we have a connection
        {
            // TODO: Run the plotter
        }
    }

    if(e->key() == Qt::Key_Escape)
    {
        if(m_lst->isVisible())
        {
            m_lst->hide();
            resetBackgrounds();
        }
    }

    if(m_lst->isVisible())
    {
        QPlainTextEdit::keyPressEvent(e);
        populateCodeCompletionListbox();
        updateLineNumbers();
        return;
    }

    QPlainTextEdit::keyPressEvent(e);
    updateLineNumbers();
}


void TextEditWithCodeCompletion::resetBackgrounds()
{
    for(int i=0; i<m_lastTablePositions.size(); i++)
    {
        QTextCursor origCursor = textCursor();
        QTextCursor copyCursor = origCursor;

        copyCursor.setPosition(m_lastTablePositions.at(i).b);
        for(int j = m_lastTablePositions.at(i).b; j!= m_lastTablePositions.at(i).e+1; j++) copyCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        QTextCharFormat fmt = copyCursor.charFormat();
        fmt.setBackground(QBrush(Qt::white));
        copyCursor.setCharFormat(fmt);
        setTextCursor(origCursor);
    }
    m_lastTablePositions.clear();
    setFocus();

}

void TextEditWithCodeCompletion::insertText(const QString & text)
{
    QString g = toPlainText();
    int cp = textCursor().position() - 1;
    QString wordBeforeCursor = "";
    // and find the word
    while(cp > -1 && g.at(cp) != ' ' && g.at(cp) != '.' && g.at(cp) != ',')
    {
        wordBeforeCursor.prepend(g.at(cp--));
    }
    // now match wordBeforeCursor to text and see from which position we need to include the "text"
    int i = 0;
    while(i <text.length() && i<wordBeforeCursor.length() && wordBeforeCursor.at(i) == text.at(i)) i++;
    insertPlainText(text.mid(i));
}

void TextEditWithCodeCompletion::populateCodeCompletionListbox()
{
    m_lst->clear();

    int cp = textCursor().position() - 1;

    // 1. find the word which was before the cursor
    QString g = toPlainText();
    QString wordBeforeCursor = "";
    QString keywordBeforeCursor = "";
    // remove all the spaces that are between the cursor and the last word to left
    while(cp > 0 && g.at(cp) == ' ')
    {
        cp--;
    }

    // and find the word
    while(cp > -1 && g.at(cp) != ' ' && g.at(cp) != '=' && g.at(cp) != ')' && g.at(cp) != '(' && g.at(cp) != ',')
    {
        wordBeforeCursor.prepend(g.at(cp--));
    }

    // 2. Find the last keyword which was before the cursor
    bool foundKeyword = false;


}



QColor TextEditWithCodeCompletion::TablePositionInText::nextColor()
{
    // TODO: This is a little bit naive approach... Make a way to create a "shade" of various colors and put those in the list It'll look better
    static QVector<QColor> colors;
    colors.clear();
    colors << color1 << color2 << color3<< color4<< color5<< color6<< color7<< color8<< color9<< color10<< color11<< color12<< color13<< color14<< color15;

    return colors[++ colorCounter % colors.size()];
}
