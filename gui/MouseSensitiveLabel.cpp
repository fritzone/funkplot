#include "MouseSensitiveLabel.h"
#include "BuiltinDictionary.h"

#include <QDebug>
#include <QTextCursor>
#include <QMouseEvent>
#include <QToolTip>

MouseSensitiveLabel::MouseSensitiveLabel(QWidget *parent, Qt::WindowFlags f) :
    QTextEdit(parent)
{
    setMouseTracking(true);
    viewport()->setAutoFillBackground(false);
}

void MouseSensitiveLabel::mouseMoveEvent(QMouseEvent *event)
{
    QTextCursor cursor = cursorForPosition(event->pos());
    cursor.select(QTextCursor::WordUnderCursor);
    qInfo() << cursor.selectedText();
    QString de = BuiltinDictionary::entry(cursor.selectedText());
    if(de != "")
    {
        QToolTip::showText(mapToGlobal(event->pos()), de);
    }
}
