#include "FrameForLineNumbers.h"
#include <QPainter>
#include <QDebug>
#include <QFont>
#include <QToolTip>
#include <QApplication>

FrameForLineNumbers::FrameForLineNumbers(QWidget *parent) : QFrame(parent), m_nrs()
{
    setUpdatesEnabled(true);

    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(sizePolicy.hasHeightForWidth());
    setSizePolicy(sizePolicy);
    setMinimumSize(QSize(48, 0));
    setMaximumSize(QSize(48, 16777215));
    setFrameShape(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);

    QPalette plette2 = palette(); // define pallete for textEdit..
    plette2.setColor(QPalette::Base, Qt::white); // set color "Red" for textedit base
    plette2.setColor(QPalette::Text, Qt::black); // set text color which is selected from color pallete
    setPalette(plette2); // change textedit palette
}

void FrameForLineNumbers::beginLineNumbers()
{
    m_nrs.clear();
}

void FrameForLineNumbers::addLineNumber(int nr, int y, bool disabled)
{
    NumberPosition np(nr, y, disabled);
    // qDebug() << nr <<" @ " << y << " d=" << disabled;
    m_nrs.append(np);
}

void FrameForLineNumbers::endLineNumbers()
{
    update();
}

void FrameForLineNumbers::highlightLine(int l, QString a)
{
    m_highlightedLine = l;
    update();
    repaint();
    qApp->processEvents();

    qDebug() << m_highlightedLinePos;

    if(l >= 0)
    {
        QToolTip::showText(this->mapToGlobal(m_highlightedLinePos), a, this, QRect(this->mapToGlobal(m_highlightedLinePos), QSize{400, 200}));
    }
}

void FrameForLineNumbers::paintEvent(QPaintEvent *)
{
    QPainter a(this);

    a.fillRect(QRect{0, 0, rect().width(), rect().height()}, Qt::white);
    a.drawLine(rect().width() - 1, 0, rect().width() - 1, rect().height());

    QFont f;
    f.setFamily("Courier New");
    f.setBold(false);
    f.setPixelSize(15);
    a.setFont(f);

    for(int i=0; i<m_nrs.size(); i++)
    {
        QString s = QString::number(m_nrs.at(i).nr);
        if(m_nrs.at(i).nr < 10) s = "   " + s;
        else
        if(m_nrs.at(i).nr < 100) s = "  " + s;
        else
        if(m_nrs.at(i).nr < 1000) s = " " + s;

        QRect r(0, m_nrs.at(i).y, this->rect().width(), i < m_nrs.size() - 1 ? m_nrs.at(i + 1).y - m_nrs.at(i).y : 16);
        if(m_nrs[i].disabled)
        {
            a.setPen(Qt::darkGray);
        }
        else
        {
            a.setPen(Qt::black);
        }

        if(i == m_highlightedLine - 1)
        {
            a.setPen(Qt::red);
            m_highlightedLinePos = r.topRight();
        }

        QTextOption to;
        a.drawText(r, s, to);
   }

    a.end();
}
