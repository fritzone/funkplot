#include "PaletteListForm.h"
#include "ui_PaletteListForm.h"

#include <QDir>
#include <QPainter>
#include <QPen>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QDebug>

QStringList PaletteListForm::retrievePalettes()
{
    QDir directory(":/palettes/upals/");
    QStringList tmpPaletteList = directory.entryList(QStringList("*.map"));
    QStringList paletteList;

    for(auto s : tmpPaletteList)
    {
        paletteList.append(s.replace(".map", ""));
    }

    return paletteList;
}

PaletteListForm::PaletteListForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PaletteListForm)
{
    ui->setupUi(this);

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(2);
    setStyleSheet("QHeaderView::section:horizontal {margin-right: 2; border: 1px solid}");

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList paletteList = retrievePalettes();

    for(const auto& p : paletteList)
    {
        QFile f(":/palettes/upals/" + p + ".map");
        QPixmap* pm = new QPixmap(256, 48);
        QPainter* painter = new QPainter(pm);

        if(f.open(QIODevice::Text | QIODevice::ReadOnly))
        {
            QTextStream stream(&f);
            int colc = 0;
            while (!stream.atEnd())
            {
                QString line = stream.readLine();
#if QT_VERSION > QT_VERSION_CHECK(5, 15, 0)
                QStringList rgbs = line.split(" ", Qt::SkipEmptyParts);
#else
                QStringList rgbs = line.split(" ", QString::SkipEmptyParts);
#endif
                if(rgbs.size() != 3)
                {
                    continue;
                }
                QPen pen(QColor(rgbs[0].toInt(), rgbs[1].toInt(), rgbs[2].toInt()));
                painter->setPen(pen);
                painter->drawLine(colc, 0, colc, 48);
                colc ++;
            }
        }

        QTableWidgetItem* qtlwiImg = new QTableWidgetItem;
        qtlwiImg->setData(Qt::DecorationRole, *pm);
        ui->tableWidget->insertRow ( ui->tableWidget->rowCount() );
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, qtlwiImg);

        QTableWidgetItem* qtlwiName = new QTableWidgetItem(p);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, qtlwiName);

        delete painter;
        delete pm;

    }
    //h += "</table></html>";
    //qDebug() << h;
    ui->tableWidget->resizeColumnToContents(1);
}

PaletteListForm::~PaletteListForm()
{
    delete ui;
}

void PaletteListForm::on_toolButton_clicked()
{
    auto l = ui->tableWidget->selectedItems();
    if(l.size() < 2)
    {
        emit paletteChosen("");
        return;
    }

    emit paletteChosen(l[1]->text());

    close();
}


void PaletteListForm::on_toolButton_2_clicked()
{
    close();
}

