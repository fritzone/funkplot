#include "Else.h"

QVector<QSharedPointer<Statement> > Else::create(int ln, const QString &codeline, QStringList &codelines, StatementCallback cb, StatementReaderCallback srcb)
{
    return handleStatementCallback( vectorize (QSharedPointer<Else>(new Else(ln, codeline))), cb );

}
