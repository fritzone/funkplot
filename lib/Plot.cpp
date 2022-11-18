#include "Plot.h"
#include "RuntimeProvider.h"

Plot::Plot(int ln, const QString &s) : Statement(ln, s)
{}

QString Plot::keyword() const
{
    return Keywords::KW_PLOT;
}

bool Plot::execute(RuntimeProvider *rp)
{
    rp->drawPlot(this->sharedFromThis());
    return true;
}
