#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Function.h"
#include "util.h"
#include "colors.h"

#include <QGraphicsTextItem>
#include <QDebug>
#include <QSharedPointer>

#include <functional>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    rp.setWindow(this);

    ui->setupUi(this);
    sc = new QGraphicsScene(ui->graphicsView->rect());
    ui->graphicsView->setScene(sc);

    sc->setBackgroundBrush(Qt::white);
    drawCoordinateSystem();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reportError(QString err)
{
    ui->errorText->setText(err + " <b>[" + currentStatement + "]</b>");

    qWarning() << err;
}

QSharedPointer<Statement> MainWindow::createFunction(const QString &codeline)
{
    QString f_body = codeline.mid(STR_FUNCTION.length());
    Function* f = new Function(f_body.toLocal8Bit().data());
    QSharedPointer<FunctionDefinition> fd;
    fd.reset(new FunctionDefinition(codeline) );
    fd->f = QSharedPointer<Function>(f);
    functions.push_back(fd);

    return fd;
}

QSharedPointer<Statement> MainWindow::createSett(const QString &codeline)
{
    QString s_body = codeline.mid(STR_SET.length());
    // set what?
    QString what;
    while(!s_body.isEmpty() && !s_body[0].isSpace())
    {
        what += s_body[0];
        s_body = s_body.mid(1);
    }
    if(what != "color")
    {
        throw syntax_error_exception("Syntax error: only colors can be set for now");
    }
    s_body = s_body.mid(1);
    // to what, can be: colorname, #RRGGBB or R,G,B (real numbers for this situation)
    QString to_what;
    while(!s_body.isEmpty())
    {
        to_what += s_body[0];
        s_body = s_body.mid(1);
    }
    QSharedPointer<Sett> sett(new Sett(codeline));
    sett->what = what;
    sett->value = to_what;
    return sett;
}

QSharedPointer<Statement> MainWindow::createLoop(const QString &codeline, QStringList &codelines)
{
    QString l_decl = codeline.mid(STR_FOREACH.length());
    QVector<QSharedPointer<Statement>> loop_body;
    QSharedPointer<Loop> result = nullptr;

    // the loop variable
    QString loop_variable;
    while(!l_decl.isEmpty() && !l_decl[0].isSpace())
    {
        loop_variable += l_decl[0];
        l_decl = l_decl.mid(1);
    }
    // skip the space
    l_decl = l_decl.mid(1);

    // in keyword
    if(l_decl.startsWith(STR_IN))
    {
        // skip it
        l_decl = l_decl.mid(STR_IN.length());

        // see what are we looping through
        QString loop_over;
        while(!l_decl.isEmpty() && !l_decl[0].isSpace() && !(l_decl[0] == '(') && !(l_decl[0] == '['))
        {
            loop_over += l_decl[0];
            l_decl = l_decl.mid(1);
        }
        // skip the space
        consumeSpace(l_decl);

        // create the correct loop object
        result.reset(new Loop(codeline));
        result->loop_variable = loop_variable;
        if(loop_over == Keywords::KW_RANGE && (l_decl[0] == '(' || l_decl[0] == '['))
        {
            auto ritl = new RangeIteratorLoopTarget(result);
            result->loop_target.reset(ritl);
            l_decl = l_decl.mid(1);
            QString range_start = "";
            while(!l_decl.isEmpty() && !l_decl[0].isSpace() && !(l_decl[0] == ','))
            {
                range_start += l_decl[0];
                l_decl = l_decl.mid(1);
            }
            consumeSpace(l_decl);
            if(l_decl[0] == ',')
            {
                l_decl = l_decl.mid(1);
                consumeSpace(l_decl);
            }

            ritl->startSt = "rsf_" +QString::number(nextNumber()) + "($) = " + range_start;
            ritl->startFun = QSharedPointer<Function>(new Function(ritl->startSt.toLocal8Bit().data()));

            QString range_end = "";
            while(!l_decl.isEmpty() && !l_decl[0].isSpace() && !(l_decl[0] == ')' || l_decl[0] == ']'))
            {
                range_end += l_decl[0];
                l_decl = l_decl.mid(1);
            }

            ritl->endSt = "ref_" +QString::number(nextNumber()) + "($) = " + range_end;
            ritl->endFun = QSharedPointer<Function>(new Function(ritl->endSt.toLocal8Bit().data()));

            if(l_decl[0] == ')')
            {
                l_decl = l_decl.mid(1);
            }
            consumeSpace(l_decl);
            // do we have a step
            if(l_decl.startsWith(Keywords::KW_STEP))
            {
                l_decl = l_decl.mid(STR_STEP.length());
                consumeSpace(l_decl);
                QString step;
                while(!l_decl.isEmpty() && !l_decl[0].isSpace())
                {
                    step += l_decl[0];
                    l_decl = l_decl.mid(1);
                }

                ritl->stepSt = "rpf_" +QString::number(nextNumber()) + "($) = " + step;
                ritl->stepFun = QSharedPointer<Function>(new Function(ritl->stepSt.toLocal8Bit().data()));

                consumeSpace(l_decl);
            }
            else
            {
                ritl->stepSt = "rpf_" +QString::number(nextNumber()) + "($) = 0.1";
                ritl->stepFun = QSharedPointer<Function>(new Function(ritl->stepSt.toLocal8Bit().data()));
            }
        }
        else
        {
            result->loop_target = QSharedPointer<LoopTarget>(new FunctionIteratorLoopTarget(result));
            result->loop_target->name = loop_over;
        }

        if(!l_decl.startsWith(Keywords::KW_DO))
        {
            throw syntax_error_exception("foreach does not contain the do keyword");
        }

        bool done = false;
        while(!codelines.isEmpty() && !done)
        {
            QSharedPointer<Statement> st = nullptr;

            try
            {
                st = resolveCodeline(codelines, result->body, result);
            }
            catch(...)
            {
                throw;
            }

            if(st)
            {
                done = st->keyword() == Keywords::KW_DONE;
            }
            else
            {
                throw syntax_error_exception("something is wrong with this expression: %s", codeline.toStdString().c_str());
            }
        }

        if(!done)
        {
            throw syntax_error_exception("foreach body does not end with done");
        }

    }
    else
    {
        throw syntax_error_exception("foreach does not contain the in keyword");
    }

    return result;

}

QSharedPointer<Function> MainWindow::getFunction(const QString &name, QSharedPointer<Assignment>& assignment)
{
    QSharedPointer<Function> result;

    auto it = std::find_if(functions.begin(), functions.end(), [&name](QSharedPointer<FunctionDefinition> f) -> bool {
        return f->f->get_name() == name.toStdString();
    });

    if(it != functions.end())
    {

        result = ((*it)->f);
    }

    if(!result)
    {
        // second: or an assignment bound to a function
        for(const auto& adef : qAsConst(assignments))
        {
            if(adef->varName == name)
            {
                if(!adef->providedFunction().isEmpty())
                {
                    result = getFunction(adef->providedFunction(), assignment);
                    if(result)
                    {
                        assignment = adef;
                        break;
                    }
                }
            }
        }
    }

    return result;
}



QSharedPointer<Statement> MainWindow::resolveCodeline(QStringList& codelines, QVector<QSharedPointer<Statement>>& statements, QSharedPointer<Statement> parentScope)
{
    QString codeline = codelines[0];
    codelines.pop_front();
    codeline = codeline.simplified();

    try
    {

        QSharedPointer<Statement> createdStatement;

        if(codeline.startsWith(STR_FUNCTION))
        {
            statements.append(createdStatement = createFunction(codeline));
        }
        if(codeline.startsWith(STR_PLOT))
        {
            statements.append(createdStatement = createPlot(codeline));
        }
        if(codeline.startsWith(STR_LET)) // variable assignment
        {
            statements.append(createdStatement = resolveObjectAssignment(codeline));
        }
        if(codeline.startsWith(STR_SET)) // setting the color, line width, rotation, etc ...
        {
            statements.append(createdStatement = createSett(codeline));
        }
        if(codeline.startsWith(STR_FOREACH)) // looping over a set of points or something else
        {
            statements.append(createdStatement = createLoop(codeline, codelines));
        }
        if(codeline.startsWith(Keywords::KW_DONE)) // looping over a set of points or something else
        {
            statements.append(createdStatement = QSharedPointer<Done>(new Done(codeline)));
        }

        if(createdStatement)
        {
            createdStatement->parent = parentScope;
            createdStatement->runtimeProvider = &rp;
        }

    }
    catch(std::exception& ex)
    {
        ui->errorText->setText(ex.what());
        return nullptr;
    }
    if(!statements.isEmpty())
    {
        return statements.constLast();
    }

    return nullptr;
}

void MainWindow::on_toolButton_clicked()
{

    plots.clear();
    functions.clear();
    assignments.clear();
    m_setts.clear();
    statements.clear();
    rp.variables().clear();

    QStringList codelines = ui->textEdit->toPlainText().split("\n");
    while(!codelines.empty())
    {
        resolveCodeline(codelines, statements, nullptr);
    }

    drawnLines.clear();
    drawnPoints.clear();
    drawCoordinateSystem();

    try
    {
        for(const auto& stmt : qAsConst(statements))
        {
            currentStatement = stmt->statement;
            stmt->execute(&this->rp);
        }
    }
    catch(std::exception& ex)
    {
        ui->errorText->setText(ex.what());

    }

}

void MainWindow::drawCoordinateSystem()
{
    sc->clear();
    sc->addLine(sceneX(0), sceneY(0), sceneX(coordEndX()), sceneY(0), QPen(Qt::red));
    sc->addLine(sceneX(0), sceneY(0), sceneX(coordStartX()), sceneY(0), QPen(Qt::red));
    sc->addLine(sceneX(0), sceneY(0), sceneX(0), sceneY(coordEndY()), QPen(Qt::red));
    sc->addLine(sceneX(0), sceneY(0), sceneX(0), sceneY(coordStartY()), QPen(Qt::red));
    sc->addLine(sceneX(0), sceneY(coordEndY()), sceneX(0) - 6, sceneY(coordEndY()) + 6, QPen(Qt::red));
    sc->addLine(sceneX(0), sceneY(coordEndY()), sceneX(0) + 6, sceneY(coordEndY()) + 6, QPen(Qt::red));
    sc->addLine(sceneX(coordEndX()), sceneY(0), sceneX(coordEndX()) - 6, sceneY(0) - 6, QPen(Qt::red));
    sc->addLine(sceneX(coordEndX()), sceneY(0), sceneX(coordEndX()) - 6, sceneY(0) + 6, QPen(Qt::red));

    for(double x = -1.0; x > coordStartX(); x -= 1.0)
    {
        QPoint pointOnCoordLine{sceneX(x), sceneY(0)};
        sc->addLine(pointOnCoordLine.x(), pointOnCoordLine.y() - 3 ,pointOnCoordLine.x(), pointOnCoordLine.y() + 3 , QPen(Qt::red));

        QGraphicsTextItem *text = sc->addText(QString::number(x, 'f', 0));
        text->setPos(pointOnCoordLine.x() - text->boundingRect().width() / 2, pointOnCoordLine.y() + 6);
    }

    for(double x = 1.0; x < coordEndX(); x += 1.0)
    {
        QPoint pointOnCoordLine{sceneX(x), sceneY(0)};
        sc->addLine(pointOnCoordLine.x(), pointOnCoordLine.y() - 3 ,pointOnCoordLine.x(), pointOnCoordLine.y() + 3 , QPen(Qt::red));

        QGraphicsTextItem *text = sc->addText(QString::number(x, 'f', 0));
        text->setPos(pointOnCoordLine.x() - text->boundingRect().width() / 2, pointOnCoordLine.y() + 6);
    }

    for(double y = -1.0; y > coordStartY(); y -= 1.0)
    {
        QPoint pointOnCoordLine{sceneX(0), sceneY(y)};
        sc->addLine(pointOnCoordLine.x(), pointOnCoordLine.y() - 3 ,pointOnCoordLine.x(), pointOnCoordLine.y() + 3 , QPen(Qt::red));

        QGraphicsTextItem *text = sc->addText(QString::number(y, 'f', 0));
        text->setPos(pointOnCoordLine.x() + 6, pointOnCoordLine.y() - text->boundingRect().height() / 2);
    }

    for(double y = 1.0; y < coordEndY(); y += 1.0)
    {
        QPoint pointOnCoordLine{sceneX(0), sceneY(y)};
        sc->addLine(pointOnCoordLine.x(), pointOnCoordLine.y() - 3 ,pointOnCoordLine.x(), pointOnCoordLine.y() + 3 , QPen(Qt::red));

        QGraphicsTextItem *text = sc->addText(QString::number(y, 'f', 0));
        text->setPos(pointOnCoordLine.x() + 6, pointOnCoordLine.y() - text->boundingRect().height() / 2);
    }
}

void MainWindow::consumeSpace(QString &s)
{
    while(!s.isEmpty() && s[0].isSpace())
    {
        s = s.mid(1);
    }
}

QString MainWindow::getDelimitedId(QString &s, QSet<char> delims, char& delim)
{
    QString result;
    consumeSpace(s);
    // name of the assigned variable
    while(!s.isEmpty() && !delims.contains(s[0].toLatin1()))
    {
        result += s[0];
        s = s.mid(1);
    }
    delim = s[0].toLatin1();
    // skip the delimiter
    s = s.mid(1);
    // and the space folowing that
    consumeSpace(s);

    return result;
}

QString MainWindow::getDelimitedId(QString& s, QSet<char> delim)
{
    char d;
    return getDelimitedId(s, delim, d);
}

int MainWindow::nextNumber()
{
    static int nr = 0;
    return nr++;
}

int MainWindow::sceneX(double x)
{
    int zeroX = sc->sceneRect().width() / 2;

    return zeroX + x * zoomFactor();
}

int MainWindow::sceneY(double y)
{
    int zeroY = sc->sceneRect().height() / 2;

    return zeroY - y * zoomFactor();
}

double MainWindow::coordStartX()
{
    return -15.0;
}

double MainWindow::coordEndX()
{
    return 15.0;
}

double MainWindow::coordStartY()
{
    return -10.0;
}

double MainWindow::coordEndY()
{
    return 10.0;
}

double MainWindow::zoomFactor()
{
    return 40.0;
}

QSharedPointer<Statement> MainWindow::createPlot(const QString& codeline)
{
    QSharedPointer<Plot> plotData;
    plotData.reset(new Plot(codeline));

    QString plot_body = codeline.mid(STR_PLOT.length());
    // function name
    QString funToPlot;
    while(!plot_body.isEmpty() && !plot_body[0].isSpace())
    {
        funToPlot += plot_body[0];
        plot_body = plot_body.mid(1);
    }
    plotData->plotTarget = funToPlot;

    plot_body = plot_body.mid(1);
    // over keyword
    if(plot_body.startsWith(Keywords::KW_OVER))
    {
        plot_body = plot_body.mid(Keywords::KW_OVER.length());
        consumeSpace(plot_body);
        if(!plot_body.startsWith("(") || !plot_body.startsWith("["))
        {
            char close_char = plot_body[0].toLatin1() == '(' ? ')' : ']';

            plot_body = plot_body.mid(1);
            // first parameter
            QString first_par;
            while(!(plot_body[0].toLower() == ','))
            {
                first_par += plot_body[0];
                plot_body = plot_body.mid(1);
            }
            // skip comma
            plot_body = plot_body.mid(1);
            QString second_par;
            while(!(plot_body[0].toLower() == close_char))
            {
                second_par += plot_body[0];
                plot_body = plot_body.mid(1);
            }
            plot_body = plot_body.mid(1);
            plot_body = plot_body.simplified();
            if(plot_body == Keywords::KW_CONTINUOUS)
            {
                plotData->continuous = true;
            }
            else if (plot_body.startsWith(STR_STEP))
            {
                plot_body = plot_body.mid(STR_STEP.length());
                double stp = plot_body.toDouble();
                plotData->step = stp;
            }

            QString fakeFunSt = "fpds" + QString::number(plots.size()) + "($) = " + first_par;
            QString fakeFunEn = "fpde" + QString::number(plots.size()) + "($) = " + second_par;

            plotData->start = QSharedPointer<Function>(new Function(fakeFunSt.toLocal8Bit().data()));
            plotData->end = QSharedPointer<Function>(new Function(fakeFunEn.toLocal8Bit().data()));
        }
        else
        {
            throw syntax_error_exception("Syntax error: keyword over not followed by interval");
        }
        plots.append(plotData);
    }

    return plotData;
}

QSharedPointer<Assignment> MainWindow::providePointsOfDefinition(const QString& codeline, QString assignment_body, const QString& varName, const QString& targetProperties)
{
    if(!assignment_body.startsWith(STR_OF))
    {
        throw syntax_error_exception("Invalid assignment: %s (missing of keyword)", codeline.toStdString().c_str());
    }

    // skipping the of keyword
    assignment_body = assignment_body.mid(STR_OF.length());
    consumeSpace(assignment_body);

    QSharedPointer<PointsOfObjectAssignment> assignmentData;
    assignmentData.reset(new PointsOfObjectAssignment(codeline));

    assignmentData->varName = varName;
    assignmentData->targetProperties = targetProperties;

    // what object do we want the points of
    assignmentData->ofWhat = getDelimitedId(assignment_body);
    QSharedPointer<Assignment> assignment;

    if(!getFunction(assignmentData->ofWhat, assignment))
    {
        throw syntax_error_exception("Invalid assignment: %s. No such function: %s", codeline.toStdString().c_str(), assignmentData->ofWhat.toStdString().c_str());
    }

    // over keyword
    if(assignment_body.startsWith(Keywords::KW_OVER))
    {
        assignment_body = assignment_body.mid(Keywords::KW_OVER.length());
        consumeSpace(assignment_body);

        if(!assignment_body.startsWith("(") || !assignment_body.startsWith("["))
        {
            char close_char = assignment_body[0].toLatin1() == '(' ? ')' : ']';

            assignment_body = assignment_body.mid(1);
            // first parameter
            QString first_par;
            while(!(assignment_body[0].toLower() == ','))
            {
                first_par += assignment_body[0];
                assignment_body = assignment_body.mid(1);
            }
            // skip comma
            assignment_body = assignment_body.mid(1);
            QString second_par;
            while(!(assignment_body[0].toLower() == close_char))
            {
                second_par += assignment_body[0];
                assignment_body = assignment_body.mid(1);
            }
            assignment_body = assignment_body.mid(1);
            assignment_body = assignment_body.simplified();
            if(assignment_body == Keywords::KW_CONTINUOUS)
            {
                assignmentData->continuous = true;
            }
            else if (assignment_body.startsWith(STR_STEP))
            {
                assignment_body = assignment_body.mid(STR_STEP.length());
                double stp = assignment_body.toDouble();
                assignmentData->step = stp;
            }

            QString fakeFunSt = "afpds" + QString::number(assignments.size()) + "($) = " + first_par;
            QString fakeFunEn = "afpde" + QString::number(assignments.size()) + "($) = " + second_par;

            assignmentData->start = QSharedPointer<Function>(new Function(fakeFunSt.toLocal8Bit().data()));
            assignmentData->end = QSharedPointer<Function>(new Function(fakeFunEn.toLocal8Bit().data()));

        }
    }

    return assignmentData;
}

QSharedPointer<Statement> MainWindow::resolveObjectAssignment(const QString &codeline)
{


    // let keyword
    QString assignment_body = codeline.mid(STR_LET.length());
    consumeSpace(assignment_body);

    // variable
    QString varName = getDelimitedId(assignment_body);

    // assignment operator
    if(assignment_body[0] != '=')
    {
        throw syntax_error_exception("Invalid assignment: %s (missing assignment operator)", codeline.toStdString().c_str());
    }

    // skip "= "
    assignment_body = assignment_body.mid(1);
    consumeSpace(assignment_body);

    // what feature do we want to assign to
    QString targetProperties = getDelimitedId(assignment_body);

    // if it's not points, for the moment it is an arythmetic calculation
    if(targetProperties != "points")
    {
        if(targetProperties == "point") // or just a simple point
        {
            QString nextWord = getDelimitedId(assignment_body);
            if(nextWord != "at")
            {
                throw syntax_error_exception("Invalid point assignment: %s (missing at keyword)", codeline.toStdString().c_str());
            }
            QString px = getDelimitedId(assignment_body, {','});
            QString py = getDelimitedId(assignment_body);

            QString fakeFunStx = "xafpds" + QString::number(assignments.size()) + "($) = " + px;
            QString fakeFunSty = "yafpds" + QString::number(assignments.size()) + "($) = " + py;

            QSharedPointer<PointDefinitionAssignment> assignmentData;
            assignmentData.reset(new PointDefinitionAssignment(codeline));

            assignmentData->x = QSharedPointer<Function>(new Function(fakeFunStx.toLocal8Bit().data()));
            assignmentData->y = QSharedPointer<Function>(new Function(fakeFunSty.toLocal8Bit().data()));
            assignmentData->varName = varName;
            assignments.append(assignmentData);

            return assignmentData;
        }
        else
        {
            QSharedPointer<ArythmeticAssignment> assignmentData;
            assignmentData.reset(new ArythmeticAssignment(codeline));

            QString expression = targetProperties;
            expression += assignment_body;
            QString fakeFunSt = "xafpds" + QString::number(assignments.size()) + "($) = " + expression;
            assignmentData->arythmetic = QSharedPointer<Function>(new Function(fakeFunSt.toLocal8Bit().data()));
            assignments.append(assignmentData);

            assignmentData->varName = varName;
            assignments.append(assignmentData);

            return assignmentData;

        }
    }

    // here we are sure we want the points of a plot

    // of keyword
    QSharedPointer<Assignment> assignmentData = providePointsOfDefinition(codeline, assignment_body, varName, targetProperties);
    assignments.append(assignmentData);
    return assignmentData;
}

void MainWindow::drawPlot(QSharedPointer<Plot> plot)
{
    double cx, cy;
    bool first = true;

    auto executor = [this, &cx, &cy, &first](double x, double y, bool continuous)
    {
        if(continuous)
        {
            if(first)
            {
                cx = x;
                cy = y;
                first = false;
            }
            else
            {
                sc->addLine( sceneX(cx), sceneY(cy), sceneX(x), sceneY(y), drawingPen);
                drawnLines.append({{cx, cy, x, y}, drawingPen});
                cx = x;
                cy = y;
            }
        }
        else
        {
            sc->addEllipse(sceneX(x), sceneY(y), 1.0, 1.0, drawingPen);
            drawnPoints.append({x, y});
        }
    };

    genericPlotIterator(plot, executor);

    qDebug() << sc->items().count() << "lines";
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    sc->setSceneRect(ui->graphicsView->rect());
    qDebug() << sc->sceneRect();
    QRect wrect = rect();
    ui->splitter->setSizes({wrect.height() / 3, 2 * wrect.height() / 3} );
    redrawEverything();
}

void MainWindow::redrawEverything()
{
    drawCoordinateSystem();
    for(const auto&l : qAsConst(drawnLines))
    {
        sc->addLine(sceneX(l.line.x1()), sceneY(l.line.y1()), sceneX(l.line.x2()), sceneY(l.line.y2()), l.pen);
    }

    for(const auto&p : qAsConst(drawnPoints))
    {
        sc->addEllipse(sceneX(p.x()), sceneY(p.y()), 1.0, 1.0);
    }

}

void MainWindow::drawPoint(std::tuple<QSharedPointer<Function>, QSharedPointer<Function> > fcp)
{
    double x = std::get<0>(fcp)->Calculate(&rp);
    double y = std::get<1>(fcp)->Calculate(&rp);
    sc->addEllipse(sceneX(x), sceneY(y), 1.0, 1.0, drawingPen);
    drawnPoints.append({x, y});

}

QVector<QSharedPointer<Assignment> >& MainWindow::get_assignments()
{
    return assignments;
}

void MainWindow::setCurrentStatement(const QString &newCurrentStatement)
{
    currentStatement = newCurrentStatement;
}


void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    sc->setSceneRect(ui->graphicsView->rect());
    qDebug() << sc->sceneRect() << pos << index;
    redrawEverything();
}

bool Plot::execute(RuntimeProvider *rp)
{
    rp->window()->drawPlot(this->sharedFromThis());
    return true;
}

bool ArythmeticAssignment::execute(RuntimeProvider *rp)
{
    if(arythmetic)
    {
        double v = arythmetic->Calculate(rp);
        rp->variables()[varName] = v;
    }

    return true;
}

bool FunctionDefinition::execute(RuntimeProvider *mw)
{
    return true;
}

static int hex2int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}

static int hex2int( QChar hexchar )
{
    int v;
    if ( hexchar.isDigit() )
        v = hexchar.digitValue();
    else if ( hexchar >= 'A' && hexchar <= 'F' )
        v = hexchar.cell() - 'A' + 10;
    else if ( hexchar >= 'a' && hexchar <= 'f' )
        v = hexchar.cell() - 'a' + 10;
    else
        v = -1;
    return v;
}

bool Sett::execute(RuntimeProvider *rp)
{
    if(what == "color")
    {
        std::string s = value.toStdString();

        if(Colors::colormap.count(s))
        {
            uint32_t cui = Colors::colormap.at(s);
            QColor c = Colors::QColorFromUint32(cui);
            rp->window()->drawingPen = QPen(c);
        }
        else // is this an RGB color?
        if(value.startsWith("#"))
        {
            QColor c;
            c.setNamedColor(value);
            rp->window()->drawingPen = QPen(c);
        }
        else // what remains is a list of RGB values
        {
            QStringList values = value.split(",");
            qreal a = 1.0;
            switch(values.length())
            {
                case 4: // R,G,B,A
                {
                    QString fakeFunA = "clr_red_f($) = " + values[3].simplified();
                    QSharedPointer<Function> fa(new Function(fakeFunA.toLocal8Bit().data()));
                    a = fa->Calculate(rp);
                }
                [[fallthrough]];

                case 3: // R,G,B
                {
                    QString fakeFunR = "clr_red_f($) = " + values[0].simplified();
                    QString fakeFunG = "clr_red_f($) = " + values[1].simplified();
                    QString fakeFunB = "clr_red_f($) = " + values[2].simplified();

                    QSharedPointer<Function> fr(new Function(fakeFunR.toLocal8Bit().data()));
                    QSharedPointer<Function> fg(new Function(fakeFunG.toLocal8Bit().data()));
                    QSharedPointer<Function> fb(new Function(fakeFunB.toLocal8Bit().data()));

                    qreal r = fr->Calculate(rp);
                    qreal g = fg->Calculate(rp);
                    qreal b = fb->Calculate(rp);

                    QColor c;

                    qreal finalR = r;
                    qreal finalG = g;
                    qreal finalB = b;
                    qreal finalA = a;

                    if(r <= 1.0)
                    {
                        finalR = 255.0 * r;
                    }
                    if(g <= 1.0)
                    {
                        finalG = 255.0 * g;
                    }
                    if(b <= 1.0)
                    {
                        finalB = 255.0 * b;
                    }
                    if(a <= 1.0)
                    {
                        finalA = 255.0 * a;
                    }

                    c.setRgb(static_cast<int>(finalR), static_cast<int>(finalG),
                             static_cast<int>(finalB), static_cast<int>(finalA));
                    rp->window()->drawingPen = QPen(c);
                    break;
                }
                case 2: // colorname,A
                {
                    if(Colors::colormap.count( values[0].simplified().toStdString() ))
                    {
                        uint32_t cui = Colors::colormap.at(s);
                        QColor c = Colors::QColorFromUint32(cui);

                        QString fakeFunA = "clr_f($) = " + values[1].simplified();
                        QSharedPointer<Function> fa(new Function(fakeFunA.toLocal8Bit().data()));
                        a = fa->Calculate(rp);
                        qreal finalA = a;

                        if(a <= 1.0)
                        {
                            finalA = 255.0 * a;
                        }

                        c.setAlpha(static_cast<int>(finalA));
                        rp->window()->drawingPen = QPen(c);
                    }
                    else // is this an RGB color?
                        if(value.startsWith("#"))
                        {
                            QColor c;
                            c.setNamedColor(value);
                            QString fakeFunA = "clr_f($) = " + values[1].simplified();
                            QSharedPointer<Function> fa(new Function(fakeFunA.toLocal8Bit().data()));
                            a = fa->Calculate(rp);
                            qreal finalA = a;

                            if(a <= 1.0)
                            {
                                finalA = 255.0 * a;
                            }

                            c.setAlpha(static_cast<int>(finalA));


                            rp->window()->drawingPen = QPen(c);
                        }
                }
                default:
                {
                    throw syntax_error_exception("Invalid color to set:", value);
                }
            }
        }
    }

    return true;
}

bool Loop::execute(RuntimeProvider *rp)
{

    auto looper = [&rp, this]()
    {
        try
        {
            for(const auto& stmt : qAsConst(body))
            {
                rp->window()->setCurrentStatement(stmt->statement);
                stmt->execute(rp);
            }
        }
        catch(std::exception& ex)
        {
            rp->window()->reportError(ex.what());

        }
    };

    try
    {
        rp->window()->setCurrentStatement(this->statement);
        loop_target->loop(looper, rp);
    }
    catch(std::exception& ex)
    {
        rp->window()->reportError(ex.what());
    }

    return true;
}

void Loop::updateLoopVariable(double v)
{
    runtimeProvider->setValue(loop_variable, v);
}

void Loop::updateLoopVariable(QPointF v)
{
    runtimeProvider->setValue(loop_variable, v);
}


RangeIteratorLoopTarget::RangeIteratorLoopTarget(QSharedPointer<Loop> l)
{
    theLoop = l;
}

bool RangeIteratorLoopTarget::loop(LooperCallback lp, RuntimeProvider* rp)
{
    double v = startFun->Calculate(rp);
    double e = endFun->Calculate(rp);
    qDebug() << "v=" << v << "e=" << e;

    double stepv = stepFun->Calculate(rp);

    if(v < e && stepv < 0.0 || v > e && stepv > 0.0)
    {
        throw syntax_error_exception("Infinite loop detected, check your range");
    }

    do
    {
        theLoop->updateLoopVariable(v);
        lp();
        stepv = stepFun->Calculate(rp);
        v += stepv;
        qDebug() << "v=" << v << "e=" << e;

        if(stepv < 0.0 && v < e)
        {
            break;
        }

        if(stepv > 0.0 && v > e)
        {
            break;
        }


    }
    while(true);
    return true;
}

bool PointsOfObjectAssignment::execute(RuntimeProvider *rp)
{
    return true;
}

bool PointDefinitionAssignment::execute(RuntimeProvider *rp)
{
    return true;
}

FunctionIteratorLoopTarget::FunctionIteratorLoopTarget(QSharedPointer<Loop> l)
{
    theLoop = l;
}

bool FunctionIteratorLoopTarget::loop(LooperCallback lp, RuntimeProvider * rp)
{
    int fakeVarPos = -1;
    double cx, cy;
    bool first = true;

    auto points_of_loop_exec = [this, &cx, &cy, &first, &lp, &fakeVarPos, &rp](double x, double y, bool continuous)
    {

        QPointF p;
        if(continuous)
        {
            if(first)
            {
                p = QPointF(cx, cy);
                theLoop->updateLoopVariable( p );
                cx = x;
                cy = y;
                first = false;
            }
            else
            {
                p = QPointF(cx, cy);
                theLoop->updateLoopVariable(p);
                cx = x;
                cy = y;
            }
        }
        else
        {
            p = QPointF(x, y);
            theLoop->updateLoopVariable( p );
        }

        qDebug() << "Plotting:" << p;
        QSharedPointer<Assignment> assignmentData = rp->window()->get_assignments()[fakeVarPos];
        QString fakeFunStx = "xafpds($) = " + QString::number(p.x());
        QString fakeFunSty = "yafpds($) = " + QString::number(p.y());
        dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->x = QSharedPointer<Function>(new Function(fakeFunStx.toLocal8Bit().data()));
        dynamic_cast<PointDefinitionAssignment*>(assignmentData.get())->y = QSharedPointer<Function>(new Function(fakeFunSty.toLocal8Bit().data()));

        lp();
    };

    QSharedPointer<Assignment> assignment(nullptr);

    auto funToUse = rp->window()->getFunction(theLoop->loop_target->name, assignment);

    QSharedPointer<PointDefinitionAssignment> assignmentData;
    assignmentData.reset(new PointDefinitionAssignment(theLoop->statement));

    // create a new fake point for the point

    QString fakeFunStx = "xafpds($) = 0.0";
    QString fakeFunSty = "yafpds($) = 0.0";
    assignmentData->x = QSharedPointer<Function>(new Function(fakeFunStx.toLocal8Bit().data()));
    assignmentData->y = QSharedPointer<Function>(new Function(fakeFunSty.toLocal8Bit().data()));
    assignmentData->varName = theLoop->loop_variable;
    rp->window()->get_assignments().append(assignmentData);
    fakeVarPos = rp->window()->get_assignments().size() - 1;


//    rp->window()->genericFunctionIterator(plot, executor);

    double plotStart = -1.0;
    double plotEnd = 1.0;
    double step = 0.1;

    if(assignment)
    {
        if(assignment->startValueProvider())
        {
            plotStart = assignment->startValueProvider()->Calculate(rp);
        }
        if( assignment->endValueProvider())
        {
            plotEnd = assignment->endValueProvider()->Calculate(rp);
        }
    }


    auto pars = funToUse->get_domain_variables();
    if(pars.size() == 1)
    {
        for(double x=plotStart; x<plotEnd; x+= step)
        {

            funToUse->SetVariable(pars[0].c_str(), x);
            double y = funToUse->Calculate(rp);

            points_of_loop_exec(x, y, continuous);

        }
    }
}
