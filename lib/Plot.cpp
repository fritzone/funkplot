#include "Plot.h"
#include "FunctionDefinition.h"
#include "RuntimeProvider.h"
#include "Function.h"

Plot::Plot(int ln, const QString &s) : Statement(ln, s)
{}

QString Plot::keyword() const
{
    return kw();
}

QVector<QSharedPointer<Statement> > Plot::create(int ln, const QString &codeline, QStringList &codelines, StatementCallback cb, StatementReaderCallback srcb)
{
    QVector<QSharedPointer<Statement> > result;

    QSharedPointer<Plot> plotData;
    plotData.reset(new Plot(ln, codeline));

    QString plot_body = codeline.mid(Keywords::KW_PLOT.length());
    consumeSpace(plot_body);

    // function name
    QString fnai;
    QString funToPlot = extract_proper_expression(plot_body,
                                                  fnai,
                                                  {' ', '['},
                                                  {Keywords::KW_COUNTS, Keywords::KW_OVER, Keywords::KW_CONTINUOUS, Keywords::KW_FOR},
                                                  false);

    // let's see if we have a parametric function for this
    auto pf = RuntimeProvider::get()->getParametricFunction(funToPlot);

    // let's see if we have a function for this already
    QSharedPointer<Function> ff = RuntimeProvider::get()->getFunction(funToPlot);

    if(!ff && !pf)
    {
        // or maybe an assignment
        auto a = RuntimeProvider::get()->getAssignment(funToPlot);
        if(!a)
        {
            QString tov = RuntimeProvider::get()->typeOfVariable(funToPlot);
            // let's see if there is a variable defined with this name, which was not assigned yet. That's an error
            if(Types::all().contains(tov) && tov != Types::TYPE_UNKNOWN)
            {
                // is this a direct indexed plot?
                if(funToPlot.indexOf('[') == -1)
                {
                    throw syntax_error_exception(ERRORCODE(49), "Invalid plot: <b>%s</b> was declared, but not defined to hold a value", funToPlot.toStdString().c_str());
                }
            }

            // let's see if we are plotting a single point of a series of plots: plot ps[2]

            auto tempFun = Function::temporaryFunction(funToPlot, plotData.get());
            if(fnai == Keywords::KW_FOR)
            {
                QString tmpCodeline = codeline;
                tmpCodeline = tmpCodeline.mid(tmpCodeline.indexOf(fnai) + Keywords::KW_FOR.length());
                consumeSpace(tmpCodeline);
                QString funVar = getDelimitedId(tmpCodeline);
                tempFun.get()->add_variable(funVar.toStdString().c_str()); // mostly the plots happen with "x"

                // making it consume the rest of the codeline too
                plot_body = tmpCodeline;
            }

            tempFun->Calculate();


            QSharedPointer<FunctionDefinition> fd;
            fd.reset(new FunctionDefinition(ln, codeline) );
            QString plgfn = "plot_fn_" +QString::number(RuntimeProvider::get()->getFunctions().size()) + "(x) =";

            // make the funToPlot look acceptable by adding (x) to each builtin function if any
            QString funToPlotFinal = funToPlot.simplified();
            // firstly remove the space before each parenthesis if found
            funToPlotFinal.replace(" (", "(");
            // then each func(x) will be replaced with func to not to have (x)(x)
            for(const auto& f : supported_functions)
            {
                if(f.standalone_plottable)
                {
                    funToPlotFinal.replace(QString::fromStdString(f.name) + "(x)", QString::fromStdString(f.name));
                }
            }

            for(const auto& f : supported_functions)
            {
                if(f.standalone_plottable)
                {
                    funToPlotFinal.replace(QString::fromStdString(f.name) + "(", QString::fromStdString(f.name) + "#");
                }
            }

            // and the other way around to make it legal
            for(const auto& f : supported_functions)
            {
                if(f.standalone_plottable)
                {
                    funToPlotFinal.replace(QString::fromStdString(f.name), QString::fromStdString(f.name) + "(x)");
                }
            }
            funToPlotFinal.replace("(x)#", "(");

            qDebug() << funToPlotFinal;

            Function* f = new Function(QString(plgfn + funToPlotFinal).toStdString().c_str(), plotData.get());

            fd->f = QSharedPointer<Function>(f);
            funToPlot = QString::fromStdString(fd->f->get_name());
            RuntimeProvider::get()->addFunction(fd);

        }
        else
        {
            if(RuntimeProvider::get()->typeOfVariable(funToPlot) == Types::TYPE_NUMBER)
            {
                throw syntax_error_exception(ERRORCODE(50), "Invalid plot: <b>%s</b> was declared to be <b>numeric</b>, but it is impossible to plot a number", funToPlot.toStdString().c_str());
            }
        }
    }

    plotData->plotTarget = funToPlot;
    consumeSpace(plot_body);

    // do we have a continuous keyword and nothing else?
    if(codeline.startsWith(Keywords::KW_CONTINUOUS) || fnai == Keywords::KW_CONTINUOUS)
    {
        plotData->continuous = true;
        plot_body = plot_body.mid(Keywords::KW_CONTINUOUS.length());
        consumeSpace(plot_body);
    };

    // over keyword
    if(plot_body.startsWith(Keywords::KW_OVER) || fnai == Keywords::KW_OVER)
    {
        resolveOverKeyword(plot_body, plotData, plotData.get());
    }
    else
    if(plot_body.startsWith(Keywords::KW_COUNTS) || fnai == Keywords::KW_COUNTS)
    {
        resolveCountsKeyword(plot_body, plotData, plotData.get());
    }

    if(!plot_body.isEmpty() && plot_body.indexOf("]") != -1)
    {
        plotData->plotTarget = funToPlot + "[" + plot_body;
    }

    result.append(plotData);

    return handleStatementCallback(result, cb);

}

bool Plot::execute(RuntimeProvider *rp)
{
    rp->drawPlot(this->sharedFromThis());
    return true;
}
