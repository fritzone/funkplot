#include "Done.h"

QVector<QSharedPointer<Statement> > Done::create(int ln, const QString &codeline, QStringList &codelines, StatementCallback cb, StatementReaderCallback srcb)
{
    return handleStatementCallback( vectorize (QSharedPointer<Done>(new Done(ln, codeline))), cb );
}
