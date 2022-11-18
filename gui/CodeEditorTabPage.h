#ifndef CODEEDITORTABPAGE_H
#define CODEEDITORTABPAGE_H

#include "FrameForLineNumbers.h"
#include "TextEditWithCodeCompletion.h"

#include <QWidget>

namespace Ui {
class CodeEditorTabPage;
}

class CodeEditorTabPage : public QWidget
{
    Q_OBJECT

public:
    explicit CodeEditorTabPage(QWidget *parent, RuntimeProvider *rp);
    ~CodeEditorTabPage();

    TextEditWithCodeCompletion *getTextEdit() const;
    void highlightLine(int, const QString&);
    void appendText(const QString&);
    void restoreTextEditState();
    void saveTextEditState();
    void setText(const QStringList& qsl);

signals:

    void textChanged();

public slots:

    void onTextChanged();

private:
    Ui::CodeEditorTabPage *ui;
    FrameForLineNumbers* m_frmLineNrs = nullptr;
    QTextCursor m_cursor;
    TextEditWithCodeCompletion *m_textEdit = nullptr;

};

#endif // CODEEDITORTABPAGE_H
