#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <qtpromise/include/QtPromise>
#include <QtConcurrent/QtConcurrent>
#include <httpServer/httpRequestHandler.h>
#include <httpServer/httpRequestRouter.h>


class RequestHandler : public HttpRequestHandler
{
    Q_OBJECT

private:
    HttpRequestRouter router;

public:
    RequestHandler();

    HttpPromise handle(HttpDataPtr data) override;
private:

    HttpPromise handleDrawRequest(HttpDataPtr data);
    HttpPromise handleLiveHtmlRequest(HttpDataPtr data);
    HttpPromise handlePlayPngRequest(HttpDataPtr data);
    HttpPromise handleFaviconRequest(HttpDataPtr data);
};

#endif // REQUESTHANDLER_H
