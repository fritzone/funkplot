#ifndef PROGRAM_H
#define PROGRAM_H

#include <QObject>
#include <QSharedPointer>

class RuntimeProvider;
class CodeEditorTabPage;

class Program : public QObject
{
    Q_OBJECT

public:
    Program(QString name, QWidget *tabContainer, RuntimeProvider *rp, int idx);
    ~Program();

    void setCurrentStatement(const QString &newCurrentStatement);
    bool run();
    bool stop();
    void highlightLine(int,QString);
    bool saveToFile(QString);
    bool alreadySaved();
    void setIndex(int);
    int index() const;
    void setCodelines(const QStringList&);
    QStringList codeLines() const;

    QSharedPointer<CodeEditorTabPage> m_tabPage = nullptr;
    QString currentStatement;
    RuntimeProvider *m_rp = nullptr;
    QString m_programName = "noname";
    QString m_saveName = "";
    int m_idx = 0;
    bool m_justSaved = true;


public slots:

    void paletteChosenFromMenu(QString);

private slots:

    void onTextChanged();

signals:

    void textChanged();
};

#endif // PROGRAM_H
