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

    TextEditWithCodeCompletion *get_textEdit() const;

private:
    Ui::CodeEditorTabPage *ui;

    TextEditWithCodeCompletion *textEdit = nullptr;
    FrameForLineNumbers* frmLineNrs = nullptr;

};

#endif // CODEEDITORTABPAGE_H
