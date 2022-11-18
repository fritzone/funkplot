#include "Function.h"
#include <ImageDrawer.h>
#include "RuntimeProvider.h"

#include <QFile>
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
    qInfo() << argv[0] << "called with" << argc << "arguments";
    if(argc < 2)
    {
        qWarning() << "Usage: " << argv[0] << "file.fnk result.png";
        exit(1);
    }
    qInstallMessageHandler(myMessageOutput);

    QString file = argv[1];

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
    ImageDrawer* imgDrawer = new ImageDrawer();
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
        [](QString s) {},
        [&p, &size](int r, int g, int b, int a, int s) {
            std::cerr << "COLOR" << r << g << b << a << "SIZE:" <<  s;
            p = QPen{QColor {r , g , b , a}}; size = s;
        },
        [&rp, &executor](QSharedPointer<Plot> p) {  rp->genericPlotIterator(p, executor); }
    };


    QStringList codelines = s.split("\n");
    rp->parse(codelines);
    rp->execute();

    //m_df->populateDrawerData(imgDrawer);
    imgDrawer->redrawEverything();
    imgDrawer->save(argv[2]);

    delete rp;

}
