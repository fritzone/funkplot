#include "Polar.h"
#include "RuntimeProvider.h"


QVector<QSharedPointer<Statement> > Polar::create(int ln, const QString &codeline, QStringList &codelines, StatementCallback cb, StatementReaderCallback srcb)
{
    QSharedPointer<Polar> result;
    result.reset(new Polar(ln, codeline));


    QString plot_body = codeline.mid(Keywords::KW_POLAR.length());
    consumeSpace(plot_body);

    auto plotCreator = HandlerStore::instance().getHandler("plot");

    QStringList e {};
    Statement::StatementCallback exec = [](QSharedPointer<Statement>)->bool{return true;};
    StatementReaderCallback srexec = [](int&, QStringList&, QVector<QSharedPointer<Statement>>&, QSharedPointer<Statement>) { return nullptr;};
    auto plotVec = plotCreator->execute(ln, plot_body, e, exec, srexec);
    if(!plotVec.isEmpty())
    {
        result->target = plotVec.at(0).dynamicCast<Plot>();
        result->target->polarPlot = true;
    }

    return handleStatementCallback(vectorize(result), cb);
}

bool Polar::execute(RuntimeProvider *rp)
{
    return target->execute(rp);
}
