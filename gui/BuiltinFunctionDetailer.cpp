#include "BuiltinFunctionDetailer.h"
#include "Builtin.h"
#include "ui_BuiltinFunctionDetailer.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QDebug>

#include <algorithm>


BuiltinFunctionDetailer::BuiltinFunctionDetailer(QSharedPointer<Builtin> b, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BuiltinFunctionDetailer), m_builtin(b)
{
    ui->setupUi(this);

    populateBuiltin(b);
}

BuiltinFunctionDetailer::~BuiltinFunctionDetailer()
{
    delete ui;
}

void BuiltinFunctionDetailer::populateBuiltin(QSharedPointer<Builtin> b)
{
    qInfo() << "Populating:" << b->getName();

    setWindowTitle(b->getName());
    ui->lblName->setText(b->getName());

    ui->txtDescription->setText(b->getDescription());
    ui->imgFormula->setPixmap(QPixmap {":/builtin/" + b->getKey() + ".png"});
    ui->imgPlot->setPixmap(QPixmap {":/builtin/" + b->getKey() + ".g.png"});

    ui->lblDummy->hide();

    for(const auto&p : b->getParameters())
    {
        QLabel* lbl = new QLabel(p.name + " = ", this);
        m_dynamicWidgets.append(lbl);

        QFont f = lbl->font();
        f.setBold(true);
        f.setItalic(true);
        f.setPointSize(16);
        f.setFamily("Times New Roman");

        lbl->setFont(f);
        QLineEdit* te = new QLineEdit(this);
        m_dynamicWidgets.append(te);

        ui->formLayout->addRow(lbl, te);
    }

    m_builtin = b;
}

void BuiltinFunctionDetailer::on_toolButton_2_clicked()
{
    auto it = std::find_if(Builtin::m_allBuiltins.begin(), Builtin::m_allBuiltins.end(), [this](QSharedPointer<Builtin> p)
                           {
                               return p->getName() == m_builtin->getName();
                           }
                           );

    if(it + 1 == Builtin::m_allBuiltins.end())
    {
        it = Builtin::m_allBuiltins.begin();
    }
    else
    {
        it ++;
    }

    for(auto& w : m_dynamicWidgets)
    {
        delete w;
    }
    m_dynamicWidgets.clear();

    populateBuiltin(*it);
}


void BuiltinFunctionDetailer::on_toolButton_clicked()
{
    auto it = std::find_if(Builtin::m_allBuiltins.begin(), Builtin::m_allBuiltins.end(), [this](QSharedPointer<Builtin> p)
                           {
                               return p->getName() == m_builtin->getName();
                           }
                           );

    if(it == Builtin::m_allBuiltins.begin())
    {
        it = Builtin::m_allBuiltins.end() - 1;
    }
    else
    {
        it --;
    }

    for(auto& w : m_dynamicWidgets)
    {
        delete w;
    }
    m_dynamicWidgets.clear();

    populateBuiltin(*it);
}
