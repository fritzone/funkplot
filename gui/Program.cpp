#include "Program.h"

#include "CodeEditorTabPage.h"
#include "RuntimeProvider.h"

#include <QMessageBox>
#include <QFile>

Program::Program(QString name, QWidget *tabContainer, RuntimeProvider *rp, int idx) :
    QObject(nullptr), m_rp(rp), m_programName(name), m_idx(idx)
{
    m_tabPage.reset(new CodeEditorTabPage(tabContainer, rp));

    QObject::connect(m_tabPage.get(), SIGNAL(textChanged()), this, SIGNAL(textChanged()));
    QObject::connect(m_tabPage.get(), SIGNAL(textChanged()), this, SLOT(onTextChanged()));

}

Program::~Program()
{
    qInfo() << "Deleting:" << m_programName;
}

void Program::setCurrentStatement(const QString &newCurrentStatement)
{
    currentStatement = newCurrentStatement;

}

bool Program::run()
{
    m_rp->reset();
    QStringList codelines = m_tabPage->getTextEdit()->toPlainText().split("\n");
    if(!m_rp->parse(codelines))
    {
        return false;
    }

    m_rp->execute();
    return true;
}

bool Program::stop()
{
    m_rp->stopExecution();
    return true;
}

void Program::highlightLine(int l, QString s)
{
    m_tabPage->highlightLine(l, s);
}

bool Program::saveToFile(QString filename)
{
    QStringList l = m_tabPage->getTextEdit()->toPlainText().split("\n");
    m_tabPage->getTextEdit()->set_LastSavedText(m_tabPage->getTextEdit()->toPlainText());
    QFile fOut(filename);
    if (fOut.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream s(&fOut);
        for (int i = 0; i < l.size(); ++i)
            s << l.at(i) << '\n';
    }
    else
    {
        QMessageBox::critical(m_tabPage.get(), tr("Cannot save file"), tr("Cannot save file: ") + fOut.errorString(), QMessageBox::Ok);
        return false;
    }
    fOut.close();
    m_saveName = filename;
    m_justSaved = true;
    m_programName = m_saveName;
    return true;
}

bool Program::alreadySaved()
{
    return !m_saveName.isEmpty();
}

void Program::setIndex(int idx)
{
    m_idx = idx;
}

int Program::index() const
{
    return m_idx;
}

void Program::setCodelines(const QStringList &cls)
{
    m_tabPage->setText(cls);
}

QStringList Program::codeLines() const
{
    return m_tabPage->getTextEdit()->toPlainText().split("\n");
}

void Program::paletteChosenFromMenu(QString p)
{
    qDebug() << "Palette chosen:" << p;
    m_tabPage->appendText("set palette " + p + "\n");
}

void Program::onTextChanged()
{
    m_justSaved = false;
}
