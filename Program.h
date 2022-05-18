#ifndef PROGRAM_H
#define PROGRAM_H

#include <QObject>

class RuntimeProvider;
class CodeEditorTabPage;

class Program : public QObject
{
    Q_OBJECT

public:
    Program(QWidget *tabContainer, RuntimeProvider *rp);
    void setCurrentStatement(const QString &newCurrentStatement);
    void run();

    void highlightLine(int,QString);

    CodeEditorTabPage* m_tabPage = nullptr;
    QString currentStatement;
    RuntimeProvider *m_rp = nullptr;

public slots:

    void paletteChosenFromMenu(QString);
};

#endif // PROGRAM_H
