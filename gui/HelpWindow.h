#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QWidget>
#include <QtHelp/QHelpEngine>

class QWebEngineView;
class MyPage;

namespace Ui {
class HelpWindow;
}

class HelpWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HelpWindow(QWidget *parent = nullptr);
    ~HelpWindow();

private:
    Ui::HelpWindow *ui;
    QHelpEngine *helpEngine = nullptr;
    QWebEngineView *view = nullptr;
    MyPage* m_page = nullptr;
};

#endif // HELPWINDOW_H
