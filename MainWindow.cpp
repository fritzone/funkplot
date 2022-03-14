#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Function.h"
#include "util.h"
#include "colors.h"

#include <QGraphicsTextItem>
#include <QDebug>
#include <QSharedPointer>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    rp(&variables)
{
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
    ui->errorText->setText(err);

    qWarning() << err;
}

QSharedPointer<Statement> MainWindow::createFunction(const QString &codeline)
{
    QString f_body = codeline.mid(STR_FUNCTION.length());
    Function* f = new Function(f_body.toLocal8Bit().data());
    QSharedPointer<FunctionDefinition> fd;
    fd.reset(new FunctionDefinition);
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
    QSharedPointer<Sett> sett(new Sett);
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
        while(!l_decl.isEmpty() && !l_decl[0].isSpace())
        {
            loop_over += l_decl[0];
            l_decl = l_decl.mid(1);
        }

        // skip the space
        l_decl = l_decl.mid(1);
        if(!l_decl.startsWith(Keywords::KW_DO))
        {
            throw syntax_error_exception("foreach does not contain the do keyword");
        }

        result.reset(new Loop);

        result->loop_variable = loop_variable;
        result->loop_target = loop_over;

        bool done = false;
        while(!codelines.isEmpty() && !done)
        {
            QSharedPointer<Statement> st = nullptr;

            try
            {
                st = resolveCodeline(codelines, result->body);
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

QSharedPointer<Function> MainWindow::getFunction(const QString &name)
{
    auto it = std::find_if(functions.begin(), functions.end(), [&name](QSharedPointer<FunctionDefinition> f) -> bool {
        return f->f->get_name() == name.toStdString();
    });

    if(it != functions.end())
    {

        return ((*it)->f);
    }

    return nullptr;
}

QSharedPointer<Statement> MainWindow::resolveCodeline(QStringList& codelines, QVector<QSharedPointer<Statement>>& statements)
{
    QString codeline = codelines[0];
    codelines.pop_front();
    codeline = codeline.simplified();

    try
    {
        if(codeline.startsWith(STR_FUNCTION))
        {
            statements.append(createFunction(codeline));
        }
        if(codeline.startsWith(STR_PLOT))
        {
            statements.append(createPlot(codeline));
        }
        if(codeline.startsWith(STR_LET)) // variable assignment
        {
            statements.append(resolveObjectAssignment(codeline));
        }
        if(codeline.startsWith(STR_SET)) // setting the color, line width, rotation, etc ...
        {
            statements.append(createSett(codeline));
        }
        if(codeline.startsWith(STR_FOREACH)) // looping over a set of points or something else
        {
            statements.append(createLoop(codeline, codelines));
        }
        if(codeline.startsWith(Keywords::KW_DONE)) // looping over a set of points or something else
        {
            statements.append(QSharedPointer<Done>(new Done));
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
    variables.clear();

    QStringList codelines = ui->textEdit->toPlainText().split("\n");
    while(!codelines.empty())
    {
        resolveCodeline(codelines, statements);
    }

    drawnLines.clear();
    drawnPoints.clear();
    drawCoordinateSystem();

    try
    {
        for(const auto& stmt : qAsConst(statements))
        {
            stmt->execute(this);
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
    plotData.reset(new Plot);

    QString plot_body = codeline.mid(STR_PLOT.length());
    // function name
    QString funToPlot;
    while(!plot_body.isEmpty() && !plot_body[0].isSpace())
    {
        funToPlot += plot_body[0];
        plot_body = plot_body.mid(1);
    }
    plotData->theFunction = funToPlot;

    plot_body = plot_body.mid(1);
    // over keyword
    if(plot_body.startsWith(STR_OVER))
    {
        plot_body = plot_body.mid(STR_OVER.length());
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

            QString fakeFunSt = "fpds" + QString::number(plots.size()) + "(t) = " + first_par;
            QString fakeFunEn = "fpde" + QString::number(plots.size()) + "(t) = " + second_par;

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

QSharedPointer<Statement> MainWindow::resolveObjectAssignment(const QString &codeline)
{
    QSharedPointer<Assignment> assignmentData;
    assignmentData.reset(new Assignment);

    QString assignment_body = codeline.mid(STR_LET.length());

    // name of the assigned variable
    while(!assignment_body[0].isSpace())
    {
        assignmentData->varName += assignment_body[0];
        assignment_body = assignment_body.mid(1);
    }

    // assignment operator
    assignment_body = assignment_body.mid(1);
    if(assignment_body[0] != '=')
    {
        throw syntax_error_exception("Invalid assignment: %s (missing assignment operator)", codeline.toStdString().c_str());
    }
    // skip "= "
    assignment_body = assignment_body.mid(2);

    // what feature do we want to fetch
    while(!assignment_body.isEmpty() && !assignment_body[0].isSpace())
    {
        assignmentData->targetProperties += assignment_body[0];
        assignment_body = assignment_body.mid(1);
    }

    // if it's not points, for the moment it is an arythmetic calculation
    if(assignmentData->targetProperties != "points")
    {
//        throw syntax_error_exception("Unsupported assignment object: %s", assignmentData->targetProperties.toStdString().c_str());
        QString expression = assignmentData->targetProperties;
        assignmentData->targetProperties = "arythmetic";
        expression += assignment_body;
        QString fakeFunSt = "xafpds" + QString::number(assignments.size()) + "(t) = " + expression;
        assignmentData->arythmetic = QSharedPointer<Function>(new Function(fakeFunSt.toLocal8Bit().data()));
        assignments.append(assignmentData);
        return assignmentData;
    }


    // of keyword
    assignment_body = assignment_body.mid(1);
    if(!assignment_body.startsWith(STR_OF))
    {
        throw syntax_error_exception("Invalid assignment: %s (missing of keyword)", codeline.toStdString().c_str());
    }

    assignment_body = assignment_body.mid(STR_OF.length());

    // what object
    while(!assignment_body.isEmpty() && !assignment_body[0].isSpace())
    {
        assignmentData->ofWhat += assignment_body[0];
        assignment_body = assignment_body.mid(1);
    }

    if(!getFunction(assignmentData->ofWhat))
    {
        throw syntax_error_exception("Invalid assignment: %s. No such function: %s", codeline.toStdString().c_str(), assignmentData->ofWhat.toStdString().c_str());
    }

    // TODO: This is identical to plot, refactor it
    assignment_body = assignment_body.mid(1);
    // over keyword
    if(assignment_body.startsWith(STR_OVER))
    {
        assignment_body = assignment_body.mid(STR_OVER.length());
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

            QString fakeFunSt = "afpds" + QString::number(assignments.size()) + "(t) = " + first_par;
            QString fakeFunEn = "afpde" + QString::number(assignments.size()) + "(t) = " + second_par;

            assignmentData->start = QSharedPointer<Function>(new Function(fakeFunSt.toLocal8Bit().data()));
            assignmentData->end = QSharedPointer<Function>(new Function(fakeFunEn.toLocal8Bit().data()));

        }
    }
    assignments.append(assignmentData);
    return assignmentData;
}

void MainWindow::drawPlot(QSharedPointer<Plot> plot)
{
    bool drawingAnAssignment = false;
    QSharedPointer<Assignment> assignment;
    bool continuous = true;

    Function* funToUse = nullptr;
    bool funFound = false;
    for(const auto& fdef : qAsConst(functions))
    {
        if(fdef->f->get_name() == plot->theFunction.toStdString())
        {
            funToUse = fdef->f.get();
            funFound = true;
            break;
        }
    }

    if(!funFound)
    {
        for(const auto& adef : qAsConst(assignments))
        {
            auto f = getFunction(adef->ofWhat);
            if(f)
            {
                funToUse = f.get();
                funFound = true;
                drawingAnAssignment = true;
                assignment = adef;
            }
        }
    }

    if(!funFound)
    {
        reportError("Invalid function to plot: " + plot->theFunction);
        return;
    }

    double plotStart = std::numeric_limits<double>::quiet_NaN();
    double plotEnd = std::numeric_limits<double>::quiet_NaN();

    if(drawingAnAssignment)
    {
        if(!plot->start)
        {
            if(! assignment->start)
            {
                reportError("Invalid plotting interval. There is no clear start value defined for it.");
                return;
            }
            else
            {
                plotStart = assignment->start->Calculate(&rp);
            }
        }
        else
        {
            plotStart = plot->start->Calculate(&rp);
        }

        if(!plot->end)
        {
            if(! assignment->end)
            {
                reportError("Invalid plotting interval. There is no clear end value defined for it.");
                return;
            }
            else
            {
                plotEnd = assignment->end->Calculate(&rp);
            }
        }
        else
        {
            plotEnd = plot->end->Calculate(&rp);
        }
        continuous = assignment->continuous;
    }
    else
    {
        continuous = plot->continuous;
        plotStart = plot->start->Calculate(&rp);
        plotEnd = plot->end->Calculate(&rp);
    }

    auto pars = funToUse->get_domain_variables();
    if(pars.size() == 1)
    {
        double cx, cy;
        bool first = true;

        for(double x=plotStart; x<plotEnd; x+= plot->step)
        {

            funToUse->SetVariable(pars[0].c_str(), x);
            double y = funToUse->Calculate(&rp);

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
        }
    }
    else
    {
        reportError("Invalid function to plot: " + plot->theFunction + ". Multidomain functions are not supported yet");
        return;
    }

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


void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    sc->setSceneRect(ui->graphicsView->rect());
    qDebug() << sc->sceneRect() << pos << index;
    redrawEverything();
}

bool Plot::execute(MainWindow *mw)
{
    mw->drawPlot(this->sharedFromThis());
    return true;
}

bool Assignment::execute(MainWindow *mw)
{
    if(arythmetic)
    {
        double v = arythmetic->Calculate(&mw->rp);
        mw->variables[varName] = v;
    }

    return true;
}

bool FunctionDefinition::execute(MainWindow *mw)
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

bool Sett::execute(MainWindow *mw)
{
    if(what == "color")
    {
        std::string s = value.toStdString();

        if(Colors::colormap.count(s))
        {
            uint32_t cui = Colors::colormap.at(s);
            QColor c = Colors::QColorFromUint32(cui);
            mw->drawingPen = QPen(c);
        }
        else // is this an RGB color?
        if(value.startsWith("#"))
        {
            QColor c;
            c.setNamedColor(value);
            mw->drawingPen = QPen(c);
        }
        else // what remains is a list of RGB values
        {
            QStringList values = value.split(",");
            qreal a = 1.0;
            switch(values.length())
            {
                case 4: // R,G,B,A
                {
                    QString fakeFunA = "clr_red_f(t) = " + values[3].simplified();
                    QSharedPointer<Function> fa(new Function(fakeFunA.toLocal8Bit().data()));
                    a = fa->Calculate(&mw->rp);
                }
                [[fallthrough]];

                case 3: // R,G,B
                {
                    QString fakeFunR = "clr_red_f(t) = " + values[0].simplified();
                    QString fakeFunG = "clr_red_f(t) = " + values[1].simplified();
                    QString fakeFunB = "clr_red_f(t) = " + values[2].simplified();

                    QSharedPointer<Function> fr(new Function(fakeFunR.toLocal8Bit().data()));
                    QSharedPointer<Function> fg(new Function(fakeFunG.toLocal8Bit().data()));
                    QSharedPointer<Function> fb(new Function(fakeFunB.toLocal8Bit().data()));

                    qreal r = fr->Calculate(&mw->rp);
                    qreal g = fg->Calculate(&mw->rp);
                    qreal b = fb->Calculate(&mw->rp);

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
                    mw->drawingPen = QPen(c);
                    break;
                }
                case 2: // colorname,A
                {

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

bool Loop::execute(MainWindow *mw)
{
    return true;
}

