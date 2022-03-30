#ifndef QLISTWIDGETFORCODECOMPLETION_H
#define QLISTWIDGETFORCODECOMPLETION_H

#include <QListWidget>
#include <QDebug>
#include <QKeyEvent>

class ListWidgetForCodeCompletion : public QListWidget
{
    Q_OBJECT
public:

    ListWidgetForCodeCompletion(QWidget* p) : QListWidget(p){}
    QListWidgetItem *itemFromIndex(const QModelIndex &index) const
    {
        return QListWidget::itemFromIndex(index);
    }

    void keyPressEvent(QKeyEvent *event);
};

#endif // QLISTWIDGETFORCODECOMPLETION_H
