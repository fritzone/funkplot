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

    void populateBuiltin(QSharedPointer<Builtin> b);

private slots:
    void on_toolButton_clicked();
    void on_toolButton_2_clicked();
    void on_pushButton_clicked();

signals:

    void onBuiltinUsed(QString);

private:
    Ui::BuiltinFunctionDetailer *ui;
    QSharedPointer<Builtin> m_builtin;
    QVector<QWidget*> m_dynamicWidgets;
};

#endif // BUILTINFUNCTIONDETAILER_H
