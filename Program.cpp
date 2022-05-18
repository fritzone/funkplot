#include "Program.h"

#include "CodeEditorTabPage.h"
#include "RuntimeProvider.h"

Program::Program(QWidget *tabContainer, RuntimeProvider *rp) : QObject(tabContainer), m_rp(rp)
{
    m_tabPage = new CodeEditorTabPage(tabContainer, rp);
}

void Program::setCurrentStatement(const QString &newCurrentStatement)
{
    currentStatement = newCurrentStatement;

}

void Program::run()
{
    m_rp->reset();
    QStringList codelines = m_tabPage->get_textEdit()->toPlainText().split("\n");
    m_rp->parse(codelines);
    m_rp->execute();

}

void Program::highlightLine(int l, QString s)
{
    m_tabPage->highlightLine(l, s);
}

void Program::paletteChosenFromMenu(QString p)
{
    qDebug() << "Palette chosen:" << p;
    m_tabPage->appendText("\nset palette " + p + "\n");
}
