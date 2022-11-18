#ifndef EXPORTIMAGEDIALOG_H
#define EXPORTIMAGEDIALOG_H

#include <QDialog>

class QGraphicsPixmapItem;
class QGraphicsScene;
class ImageDrawer;
class DrawingForm;

namespace Ui {
class ExportImageDialog;
}

class ExportImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportImageDialog(DrawingForm* df, QWidget *parent = nullptr);
    ~ExportImageDialog();

    int width() const;
    int height() const;

    void setDf(DrawingForm *newDf);

private slots:
    void on_comboBox_currentTextChanged(const QString &s);

private:
    Ui::ExportImageDialog *ui;
    DrawingForm* m_df = nullptr;
    ImageDrawer* imgDrawer = nullptr;
    QGraphicsScene* scene = nullptr;
    QGraphicsPixmapItem *item = nullptr;
    int m_width = 640;
    int m_height = 480;
};

#endif // EXPORTIMAGEDIALOG_H
