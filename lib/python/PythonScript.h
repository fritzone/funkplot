#ifndef PYTHONSCRIPT_H
#define PYTHONSCRIPT_H

#include "Statement.h"
#include "Keywords.h"

struct PythonScript : public Statement
{
    explicit PythonScript(int ln, const QString& s) : Statement(ln, s) {}

    QString keyword() const override
    {
        return ScriptingLangugages::PYTHON;
    }

    bool execute(RuntimeProvider* rp) override;

    QStringList pythonCodeLines;

};

#endif // PYTHONSCRIPT_H
