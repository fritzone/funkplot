#include "ListWidgetForCodeCompletion.h"
#include "TextEditWithCodeCompletion.h"

void ListWidgetForCodeCompletion::keyPressEvent(QKeyEvent *event)
{
    int t = event->key();
    if(t != Qt::Key_Up && t != Qt::Key_Down && t != Qt::Key_Enter && t != Qt::Key_Return && t != Qt::Key_PageDown && t!= Qt::Key_PageUp && t!= Qt::Key_Home && t!=Qt::Key_End)
    {
        qobject_cast<TextEditWithCodeCompletion*>(parent())->keyPressEvent(event);
        return;
    }

    if(t == Qt::Key_Return || t == Qt::Key_Enter)
    {
        if(count() == 0)
        {
            hide();
            return;
        }

        QString g = item(0)->text();
        QListWidgetItem* itm = 0;
        QVariant v;
        if(selectedItems().length())
        {
            itm = selectedItems().at(0);
            g = itm->text();
            v = itm->data(Qt::UserRole);
        }
        hide();
        parentWidget()->setFocus(Qt::OtherFocusReason);
        qobject_cast<TextEditWithCodeCompletion*>(parent())->resetBackgrounds();
        qobject_cast<TextEditWithCodeCompletion*>(parent())->insertText(g);

        if(v.isValid())
        {
            QString n = v.toString();
            if(n.at(0) == '@')
            {
                qobject_cast<TextEditWithCodeCompletion*>(parent())->insertFunctionParantheses();
            }
        }

    }
    else
    {
        QListWidget::keyPressEvent(event);
    }

}
