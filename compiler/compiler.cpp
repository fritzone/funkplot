#include "Function.h"
#include <Set.h>
#include <ImageDrawer.h>
#include "RuntimeProvider.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPen>

#include <iostream>
#include <tuple>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
    case QtInfoMsg:
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    }
}

int main(int argc, char* argv[])
{

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("funkplot - command line compiler");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationName("The Unauthorized Frog");

    QCommandLineParser parser;
    parser.setApplicationDescription("Funkplot Compiler");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("source", QCoreApplication::translate("main", "Source file to interpret."));
    parser.addPositionalArgument("destination", QCoreApplication::translate("main", "Destination image file."));

    QCommandLineOption summarizeOption(QStringList() << "summarize", QCoreApplication::translate("main", "Summarize the applcation [default = summary.json]"),  QCoreApplication::translate("main", "summary"));
    parser.addOption(summarizeOption);
    QCommandLineOption widthOption(QStringList() << "width", QCoreApplication::translate("main", "The width of the result file [default = 640]"),  QCoreApplication::translate("main", "width"));
    parser.addOption(widthOption);
    QCommandLineOption heightOption(QStringList() << "height", QCoreApplication::translate("main", "The height of the result file [default = 480]"),  QCoreApplication::translate("main", "height"));
    parser.addOption(heightOption);

    parser.process(app);

    int width = parser.value(widthOption).toInt(); if(width <= 0) { width = 640; }
    int height = parser.value(heightOption).toInt(); if(height <= 0) { height = 480; }

    if(parser.positionalArguments().size() < 2)
    {
        parser.showHelp(1);
    }
    qInstallMessageHandler(myMessageOutput);

    QString file = parser.positionalArguments().at(0);

    QFile f(file);
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        return 1;
    }
    QTextStream in(&f);
    QString s = in.readAll();

    double cx, cy;
    bool first = true;

    QVector<QPointF> points;
    ImageDrawer* imgDrawer = new ImageDrawer(width, height);
    QPen p;
    int size = 1;

    auto executor = [&cx, &cy, &first, &points, imgDrawer, p, size](double x, double y, bool continuous)
    {
        points.append({x, y});
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
                imgDrawer->addLine( QLineF{static_cast<qreal>(cx), static_cast<qreal>(cy), static_cast<qreal>(x), static_cast<qreal>(y)}, p, size);
                cx = x;
                cy = y;
            }
        }
        else
        {
            imgDrawer->addPoint({x, y}, p, size);
        }
    };

    RuntimeProvider* rp = nullptr;
    rp = new RuntimeProvider{
        [](int l, int c, QString e) { qWarning() << "ERROR" << c << "AT" << l << e;},
        [](QString s) {},
        [imgDrawer, &p, &size](double x, double y) { imgDrawer->addPoint({x, y}, p, size); },
        [imgDrawer, &p, &size](double x1, double y1, double x2, double y2) { imgDrawer->addLine(QLineF{x1, y1, x2, y2}, p, size); },
        [](QString s) {},
        [&p, &size](int r, int g, int b, int a, int s) {
            p = QPen{QColor {r , g , b , a}}; size = s;
        },
        [&rp, &executor](QSharedPointer<Plot> p) {  rp->genericPlotIterator(p, executor); }
    };

    registerClasses();

    QObject::connect(rp, SIGNAL(rotationAngleChange(double)), imgDrawer, SLOT(setRotationAngle(double)));
    QObject::connect(rp, SIGNAL(zoomFactorChange(double)), imgDrawer, SLOT(setZoomFactor(double)));
    QObject::connect(rp, SIGNAL(gridChange(bool)), imgDrawer, SLOT(setShowGrid(bool)));
    QObject::connect(rp, SIGNAL(coordEndYChange(double)), imgDrawer, SLOT(setCoordEndY(double)));
    QObject::connect(rp, SIGNAL(coordStartYChange(double)), imgDrawer, SLOT(setCoordStartY(double)));
    QObject::connect(rp, SIGNAL(coordEndXChange(double)), imgDrawer, SLOT(setCoordEndX(double)));
    QObject::connect(rp, SIGNAL(coordStartXChange(double)), imgDrawer, SLOT(setCoordStartX(double)));

    QStringList codelines = s.split("\n");

    rp->parse(codelines);
    // see if we have coordinate system modifiers in there
    auto sts = rp->getStatements();
    for(auto& st : sts)
    {
        auto stsm = dynamic_cast<Set*>(st.get());
        if(stsm)
        {
            if(stsm->what ==  SetTargets::TARGET_COORDINATES)
            {
                stsm->execute(rp);
            }
        }
    }

    rp->parse(codelines);
    rp->execute();

    imgDrawer->drawCoordinateSystem();
    imgDrawer->redrawEverything();
    imgDrawer->save(parser.positionalArguments().at(1));

    QString summaryOpt = parser.value(summarizeOption);
    QJsonDocument d;
    QJsonArray variables;

    const auto& av = rp->getAllVariables();
    for(const auto& v : av)
    {
        QJsonObject obj;
        obj["n"] = v.c_str();
        variables.append(obj);
    }

    d.setArray(variables);

    delete rp;

}
