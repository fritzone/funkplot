#ifndef BUILTINFUNCTIONDETAILER_H
#define BUILTINFUNCTIONDETAILER_H

#include <QDialog>

class Builtin;

namespace Ui {
class BuiltinFunctionDetailer;
}

class BuiltinFunctionDetailer : public QDialog
{
    Q_OBJECT

public:
    explicit BuiltinFunctionDetailer(QSharedPointer<Builtin> b, QWidget *parent = nullptr);
    ~BuiltinFunctionDetailer();

private:
    Ui::BuiltinFunctionDetailer *ui;
};

#endif // BUILTINFUNCTIONDETAILER_H
