#include <httpServer/httpServer.h>
#include <httpServer/httpServerConfig.h>
#include "RequestHandler.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        break;
    case QtWarningMsg:
    case QtInfoMsg:
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

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("funkplot - web server");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationName("The Unauthorized Frog");

    qInstallMessageHandler(myMessageOutput);

    HttpServerConfig config;
    config.port = 4099;
    config.requestTimeout = 20;
    config.responseTimeout = 5;
    config.maxMultipartSize = 512 * 1024;
    config.maxRequestSize = 512 * 1024;
    config.verbosity = HttpServerConfig::Verbose::All;

    RequestHandler *handler = new RequestHandler();
    HttpServer *server = new HttpServer(config, handler);

    server->listen();

    return app.exec();

}
