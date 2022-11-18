#ifndef PALETTELISTFORM_H
#define PALETTELISTFORM_H

#include <QDialog>
#include <QWidget>

namespace Ui {
class PaletteListForm;
}

class PaletteListForm : public QDialog
{
    Q_OBJECT

public:
    explicit PaletteListForm(QWidget *parent = nullptr);
    ~PaletteListForm();
    static QStringList retrievePalettes();

signals:

    void paletteChosen(QString);

private slots:
    void on_toolButton_2_clicked();

private slots:
    void on_toolButton_clicked();

private:
    Ui::PaletteListForm *ui;
};

#endif // PALETTELISTFORM_H
