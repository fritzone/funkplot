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
    void insertText(const QString&);
    void resetBackgrounds();
    void insertFunctionParantheses();
    void setLineNumberFrame(FrameForLineNumbers* f)
    {
        m_frameForLineNumbers = f;
    }
    void updateLineNumbers();

    void disableRow(int row)
    {
        m_disabledRows.append(row);
    }

    void enableRow(int row)
    {
        m_disabledRows.remove(m_disabledRows.indexOf(row));
    }

    void resetHighlighter();

    QString fetchTheWordBeforeTheCursor();

private slots:
    void onTimer();
    void onListItemDoubleClicked(QModelIndex);
    void onVScroll(int);

private:
    void populateCodeCompletionListbox();

private:
    ListWidgetForCodeCompletion* m_lst;
    QTimer m_timer;
    QColor m_currentBgColor;
    QVector<TablePositionInText> m_lastTablePositions;
    QSharedPointer<Highlighter> m_highlighter;
    QStringList m_tabs;
    RuntimeProvider* m_rp = nullptr;
    FrameForLineNumbers *m_frameForLineNumbers;
    QVector<int> m_disabledRows {};
};

#endif // QTEXTEDITWITHCODECOMPLETION_H
