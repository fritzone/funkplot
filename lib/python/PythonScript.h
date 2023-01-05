#ifndef PYTHONSCRIPT_H
#define PYTHONSCRIPT_H

#include "Statement.h"
#include "Keywords.h"
#include "StatementHandler.h"

struct PythonScript : public Statement
{
    PythonScript() = default;
    explicit PythonScript(int ln, const QString& s) : Statement(ln, s) {}

    QString keyword() const override
    {
        return ScriptingLangugages::PYTHON;
    }

    bool execute(RuntimeProvider* rp) override;
    static QString kw()
    {
        return ScriptingLangugages::PYTHON;
    }

    static QVector<QSharedPointer<Statement>> create(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, StatementReaderCallback srcb);

    QStringList pythonCodeLines;

};

REGISTER_STATEMENTHANDLER(PythonScript)

#endif // PYTHONSCRIPT_H
