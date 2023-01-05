#include "Statement.h"
#include "RuntimeProvider.h"

#include <QDebug>

Statement::Statement(int ln, const QString &s) : statement(s), runtimeProvider(RuntimeProvider::get()), lineNumber(ln)
{
    // qDebug() << "Create: Line:" << ln << "Code:" << s;
}

const QVector<QSharedPointer<Statement> > &Statement::handleStatementCallback(const QVector<QSharedPointer<Statement> > & result, StatementCallback cb)
{
    for(auto& s : result)
    {
        cb(s);
    }

    return result;
}



QVector<QSharedPointer<Statement> > Statement::vectorize(QSharedPointer<Statement> a)
{
    QVector<QSharedPointer<Statement>> result;

    result.append(a);

    return result;
}
