#ifndef QTEXTEDITWITHCODECOMPLETION_H
#define QTEXTEDITWITHCODECOMPLETION_H

#include <QPlainTextEdit>
#include <QListWidget>
#include <QTimer>
#include <QObject>
#include <QVector>
#include <QFrame>

#include "Highlighter.h"
#include "ListWidgetForCodeCompletion.h"

class FrameForLineNumbers;

class TextEditWithCodeCompletion : public QPlainTextEdit
{
    Q_OBJECT

private:

    struct TablePositionInText
    {
        int b;
        int e;
        QColor c;
        static QColor nextColor();
        static int colorCounter;
    };

public:

    TextEditWithCodeCompletion(QWidget *p = 0, RuntimeProvider *rp = 0);

    void keyPressEvent ( QKeyEvent * e );
    void contextMenuEvent(QContextMenuEvent *event);
    void setInitialText(const QString&);
    void insertText(const QString&);
    void resetBackgrounds();
    void insertFunctionParantheses();
    void setLineNumberFrame(FrameForLineNumbers* f);
    void updateLineNumbers();
    void disableRow(int row);
    void enableRow(int row);
    void resetHighlighter();
    QString fetchTheWordBeforeTheCursor();
    QVector<QListWidgetItem*> generateColormapList();
    void set_LastSavedText(const QString &newLastSavedText);

private slots:
    void onTimer();
    void onListItemDoubleClicked(QModelIndex);
    void onVScroll(int);
    void saveRich();

public slots:

    void onTextChanged();

private:
    void populateCodeCompletionListbox();

signals:
    void pTextChanged();

private:
    ListWidgetForCodeCompletion* m_lst;
    QTimer m_timer;
    QColor m_currentBgColor;
    QVector<TablePositionInText> m_lastTablePositions;
    QSharedPointer<Highlighter> m_highlighter;
    RuntimeProvider* m_rp = nullptr;
    FrameForLineNumbers *m_frameForLineNumbers;
    QVector<int> m_disabledRows {};
    QString m_originalText;
    QString m_lastSavedText;
};

#endif // QTEXTEDITWITHCODECOMPLETION_H
