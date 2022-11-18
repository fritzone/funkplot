#include "Statement.h"
#include "RuntimeProvider.h"

#include <QDebug>

Statement::Statement(int ln, const QString &s) : statement(s), runtimeProvider(RuntimeProvider::get()), lineNumber(ln)
{
    // qDebug() << "Create: Line:" << ln << "Code:" << s;
}

