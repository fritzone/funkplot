#ifndef MOUSESENSITIVELABEL_H
#define MOUSESENSITIVELABEL_H

#include <QLabel>
#include <QObject>
#include <QTextEdit>

class MouseSensitiveLabel : public QTextEdit
{
public:
    MouseSensitiveLabel(QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags());

    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // MOUSESENSITIVELABEL_H
