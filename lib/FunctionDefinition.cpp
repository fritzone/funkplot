#include "Function.h"
#include "FunctionDefinition.h"
#include "RuntimeProvider.h"
#include "util.h"

class RuntimeProvider;

bool FunctionDefinition::execute(RuntimeProvider *mw)
{
    return true;
}

QVector<QSharedPointer<Statement>> FunctionDefinition::create(int ln, const QString &codeline, QStringList &codelines, StatementCallback cb, StatementReaderCallback)
{
    QVector<QSharedPointer<Statement>> result;

    QString f_body = codeline.mid(Keywords::KW_FUNCTION.length());
    consumeSpace(f_body);

    QSharedPointer<FunctionDefinition> fd;
    fd.reset(new FunctionDefinition(ln, codeline) );
    Function* f = new Function(f_body.toLocal8Bit().data(), fd.get());

    fd->f = QSharedPointer<Function>(f);
    RuntimeProvider::get()->addFunction(fd);
    result.append(fd);

    return handleStatementCallback(result, cb);
}
