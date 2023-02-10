#include "BuiltinFunctionDetailer.h"
#include "Builtin.h"
#include "ui_BuiltinFunctionDetailer.h"

#include <QLineEdit>
#include <QTextEdit>

BuiltinFunctionDetailer::BuiltinFunctionDetailer(QSharedPointer<Builtin> b, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BuiltinFunctionDetailer)
{
    ui->setupUi(this);

    setWindowTitle(b->getName());
    ui->lblName->setText(b->getName());

    ui->txtDescription->setText(b->getDescription());
    ui->imgFormula->setPixmap(QPixmap {":/builtin/" + b->getKey() + ".png"});

    ui->lblDummy->hide();

    for(const auto&p : b->getParameters())
    {
        QLabel* lbl = new QLabel(p.name + " = ");
        QFont f = lbl->font();
        f.setBold(true);
        f.setItalic(true);
        f.setPointSize(16);
        f.setFamily("Times new Roman");

        lbl->setFont(f);
        QLineEdit* te = new QLineEdit();
        ui->formLayout->addRow(lbl, te);
    }
}

BuiltinFunctionDetailer::~BuiltinFunctionDetailer()
{
    delete ui;
}
