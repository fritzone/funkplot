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
#include <colors.h>
#include <QLabel>
#include <QHBoxLayout>

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
}

void TextEditWithCodeCompletion::onTimer()
{
    if(!m_lst->isVisible())
    {
        QPoint p = cursorRect().bottomRight();
        m_lst->move(p);
        m_lst->show();
        m_lst->raise();
        m_timer.stop();
        populateCodeCompletionListbox();
    }
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

void TextEditWithCodeCompletion::setLineNumberFrame(FrameForLineNumbers *f)
{
    m_frameForLineNumbers = f;
}

void TextEditWithCodeCompletion::updateLineNumbers()
{
    if(m_frameForLineNumbers)
    {
        QTextDocument *pDoc = document();
        m_frameForLineNumbers->beginLineNumbers();

        int i = 1;
        QPointF p = contentOffset();
        int prev_y = std::numeric_limits<int>::min();

        for(QTextBlock block = pDoc->begin(); block!= pDoc->end(); block = block.next())
        {

            QRectF f = blockBoundingGeometry(block);
            int y = f.top() + p.y() + 2;
            // qDebug() << "block "<< i << "=" << block.text() << "f=" << f << " p=" << p << "prev_y:" << prev_y << "y: " << y;

            if(prev_y >= y)
            {
                y = prev_y + f.height();
            }

            m_frameForLineNumbers->addLineNumber(i, y, m_disabledRows.indexOf(i) != -1);
            prev_y = y;
            i++;
        }

        m_frameForLineNumbers->endLineNumbers();
    }
}

void TextEditWithCodeCompletion::disableRow(int row)
{
    m_disabledRows.append(row);
}

void TextEditWithCodeCompletion::enableRow(int row)
{
    m_disabledRows.remove(m_disabledRows.indexOf(row));
}

void TextEditWithCodeCompletion::onVScroll(int)
{
    updateLineNumbers();
}

void TextEditWithCodeCompletion::onTextChanged()
{

}

void TextEditWithCodeCompletion::resetHighlighter()
{
    if(m_rp->isRunning())
    {
        return;
    }

    QStringList codelines = toPlainText().split("\n");
    m_rp->setShouldReport(false);
    m_rp->reset();

    m_rp->parse(codelines);
    m_rp->setShouldReport(true);
    m_highlighter.reset(new Highlighter(document(), m_rp));
    updateLineNumbers();
    // resetting the runtime provider ...
}

void TextEditWithCodeCompletion::keyPressEvent(QKeyEvent *e)
{

    if(m_frameForLineNumbers)
    {
        m_frameForLineNumbers->highlightLine(-1, "");
    }
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
        if(e->key() == Qt::Key_Space)
        {
            QPoint p = cursorRect().bottomRight();
            m_lst->move(p);
            Qt::WindowFlags flags = m_lst->windowFlags();
            m_lst->setWindowFlags(flags | Qt::WindowStaysOnTopHint);
            m_lst->show();
            m_lst->raise();
            populateCodeCompletionListbox();
            updateLineNumbers();
            return;
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
        if (   e->key() == Qt::Key_Home
            || e->key() == Qt::Key_End
            || e->key() == Qt::Key_Up
            || e->key() == Qt::Key_Down
            || e->key() == Qt::Key_PageUp
            || e->key() == Qt::Key_PageDown
           )
        {
            m_lst->keyPressEvent(e);
            return;
        }

        if(e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
        {
            QString tti = m_lst->currentItem()->text();
            if(!tti.isEmpty())
            {
                insertText(m_lst->currentItem()->text());
                emit pTextChanged();
            }
            else
            {
                auto w = m_lst->currentItem()->data(Qt::UserRole);
                insertText(w.toString());
                emit pTextChanged();
            }
            m_lst->hide();
            return;
        }
        else
        {
            QPlainTextEdit::keyPressEvent(e);
            populateCodeCompletionListbox();
            updateLineNumbers();
            return;
        }
    }

    QString ct = toPlainText();

    QPlainTextEdit::keyPressEvent(e);
    e->accept();

    QString ct2 = toPlainText();

    // see if it changed any of the text
    if( e && e->key() < 255 && e->key() > 0 && (( ct != ct2
         && ct != m_originalText
         && ct != m_lastSavedText)
         || std::isspace(e->key()) || std::isprint(e->key())))
    {
        emit pTextChanged();
    }

    resetHighlighter();
}

void TextEditWithCodeCompletion::setInitialText(const QString &s)
{
    m_originalText = s;
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
    while(cp > -1 && g.at(cp) != ' ' && g.at(cp) != '.' && g.at(cp) != ',' && !std::isspace(g.at(cp).toLatin1()))
    {
        wordBeforeCursor.prepend(g.at(cp--));
    }
    // now match wordBeforeCursor to text and see from which position we need to include the "text"
    int i = 0;
    while(i <text.length() && i<wordBeforeCursor.length() && wordBeforeCursor.at(i) == text.at(i)) i++;
    insertPlainText(text.mid(i));

    resetHighlighter();
}

QString TextEditWithCodeCompletion::fetchTheWordBeforeTheCursor()
{
    int cp = textCursor().position() - 1;

    // 1. find the word which was before the cursor
    QString g = toPlainText();
    QString wordBeforeCursor = "";
    // remove all the spaces that are between the cursor and the last word to left
    while(cp > 0 && std::isspace(g.at(cp).toLatin1()))
    {
        cp--;
    }

    // and find the word
    while(cp > -1 && g.at(cp) != ' ' && g.at(cp) != '=' && g.at(cp) != ')' && g.at(cp) != '(' && g.at(cp) != ',' && !std::isspace(g.at(cp).toLatin1()))
    {
        wordBeforeCursor.prepend(g.at(cp--));
    }

    return wordBeforeCursor;
}

QVector<QListWidgetItem *> TextEditWithCodeCompletion::generateColormapList()
{
    QVector<QListWidgetItem *> result;
    //QString h = "<html><table>";
    for(const auto& c : Colors::colormap)
    {
        QPixmap qpi(32, 32);
        auto cui = Colors::colormap.at(c.first);
        qpi.fill(QColor(cui.r, cui.g, cui.b, 255));

        QListWidgetItem* qlwi = new QListWidgetItem(QIcon(qpi), QString::fromStdString(c.first));
        result.push_back(qlwi);

        //h+= "<tr><td>" + QString::fromStdString(c.first) + "</td><td bgcolor='" + QColor(cui.r, cui.g, cui.b).name() + "'>&nbsp;</td></tr>";
    }
    //h += "</table></html>";
    //qDebug() << h;
    return result;
}

void TextEditWithCodeCompletion::populateCodeCompletionListbox()
{
    while(m_lst->count()>0)
    {
        m_lst->takeItem(0);
    }

    QString wordBeforeCursor = fetchTheWordBeforeTheCursor();

    qDebug() << wordBeforeCursor;

    if(wordBeforeCursor == "color")
    {
        static QVector<QListWidgetItem *> l = generateColormapList();
        for(const auto& qlwi : l)
        {
            m_lst->addItem(qlwi);
        }

        return;
    }

    if(wordBeforeCursor == Keywords::KW_PLOT)
    {
        auto fs = m_rp->getFunctionNames();
        for(const auto& f : fs)
        {
            QListWidgetItem* qlwi = new QListWidgetItem(QIcon(":/icons/icons/function.png"), QString::fromStdString(f));
            m_lst->addItem(qlwi);
        }

        for(const auto& f : supported_functions)
        {
            if(f.standalone_plottable)
            {
                auto qlwi = new QListWidgetItem;
                auto widget = new QWidget;
                auto widgetText =  new QLabel(QString::fromStdString("<span style=\"color:#00008b;\">" + f.name + "</span><span style=\"color:#000000;\"> (" + f.desc + ")</span>"));
                auto widgetLayout = new QHBoxLayout;

                widgetLayout->addWidget(widgetText);
                widgetLayout->setSizeConstraint(QLayout::SetFixedSize);
                widget->setLayout(widgetLayout);
                qlwi->setIcon(QIcon(":/icons/icons/function.png"));
                m_lst->addItem(qlwi);

                qlwi->setSizeHint(widget->sizeHint());
                qlwi->setData(Qt::UserRole, QString::fromStdString(f.name));
                m_lst->setItemWidget(qlwi, widget);
            }

        }

        return;
    }

    m_lst->hide();
}

void TextEditWithCodeCompletion::set_LastSavedText(const QString &newLastSavedText)
{
    m_lastSavedText = newLastSavedText;
}

QColor TextEditWithCodeCompletion::TablePositionInText::nextColor()
{
    // TODO: This is a little bit naive approach... Make a way to create a "shade" of various colors and put those in the list It'll look better
    static QVector<QColor> colors;
    colors.clear();
    colors << color1 << color2 << color3<< color4<< color5<< color6<< color7<< color8<< color9<< color10<< color11<< color12<< color13<< color14<< color15;

    return colors[++ colorCounter % colors.size()];
}
