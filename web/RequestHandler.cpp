#include "RequestHandler.h"
#include "RuntimeProvider.h"

#include <ImageDrawer.h>
RequestHandler::RequestHandler()
{
    router.addRoute("POST", "^/draw$", this, &RequestHandler::handleDrawRequest);
    router.addRoute("GET", "^/live.html$", this, &RequestHandler::handleLiveHtmlRequest);
    router.addRoute("GET", "^/play.png$", this, &RequestHandler::handlePlayPngRequest);
    router.addRoute("GET", "^/favicon.ico$", this, &RequestHandler::handleFaviconRequest);
}

HttpPromise RequestHandler::handle(HttpDataPtr data)
{
    bool foundRoute;
    HttpPromise promise = router.route(data, &foundRoute);
    if (foundRoute)
    {
        return promise;
    }

    data->response->setStatus(HttpStatus::InternalServerError);
    return HttpPromise::resolve(data);
}

HttpPromise RequestHandler::handleDrawRequest(HttpDataPtr data)
{
    QByteArray content;
    QStringList errors;

    auto result = HttpStatus::Ok;

    auto formFields = data->request->formFields();
    for (const auto& kv : formFields)
    {
        if(kv.first == "script")
        {
            auto script = QString(QByteArray::fromBase64(kv.second.toLocal8Bit()));

            qInfo() << script;

            double cx, cy;
            bool first = true;

            QVector<QPointF> points;
            ImageDrawer* imgDrawer = new ImageDrawer(800, 600);
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
                [&errors](int l, int c, QString e) {
                    QString error = QString("ERROR AT ") + QString::number(l) + ": " + e;
                    if(errors.indexOf(error) == -1) errors << error;
                },
                [](QString s) {},
                [imgDrawer, &p, &size](double x, double y) { imgDrawer->addPoint({x, y}, p, size); },
                [](QString s) {},
                [&p, &size](int r, int g, int b, int a, int s) {
                    p = QPen{QColor {r , g , b , a}}; size = s;
                },
                [&rp, &executor](QSharedPointer<Plot> p) {  rp->genericPlotIterator(p, executor); }
            };


            QObject::connect(rp, SIGNAL(rotationAngleChange(double)), imgDrawer, SLOT(setRotationAngle(double)));
            QObject::connect(rp, SIGNAL(zoomFactorChange(double)), imgDrawer, SLOT(setZoomFactor(double)));
            QObject::connect(rp, SIGNAL(gridChange(bool)), imgDrawer, SLOT(setShowGrid(bool)));
            QObject::connect(rp, SIGNAL(coordEndYChange(double)), imgDrawer, SLOT(setCoordEndY(double)));
            QObject::connect(rp, SIGNAL(coordStartYChange(double)), imgDrawer, SLOT(setCoordStartY(double)));
            QObject::connect(rp, SIGNAL(coordEndXChange(double)), imgDrawer, SLOT(setCoordEndX(double)));
            QObject::connect(rp, SIGNAL(coordStartXChange(double)), imgDrawer, SLOT(setCoordStartX(double)));

            QStringList codelines = script.split("\n");

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

            content = imgDrawer->saveToBuffer();
        }
    }

    if(errors.isEmpty())
    {
        QString b64d = QString("data:image/png;base64, ") + QString(content.toBase64());
        data->response->setBody(b64d.toLocal8Bit());

        qInfo() << "Served";
    }
    else
    {
        QJsonDocument d;
        QJsonArray ja;
        for(const auto& s : qAsConst(errors))
        {
            QJsonObject ec;
            ec["E"] = s;
            ja.append(ec);
        }
        d.setArray(ja);
        data->response->setBody(QString(d.toJson()).toLocal8Bit());
    }

    data->response->setStatus(result);
    return HttpPromise::resolve(data);
}

HttpPromise RequestHandler::handleLiveHtmlRequest(HttpDataPtr data)
{
    data->response->sendFile(":/site/live.html");
    auto result = HttpStatus::Ok;

    data->response->setStatus(result);

    return HttpPromise::resolve(data);
}

HttpPromise RequestHandler::handlePlayPngRequest(HttpDataPtr data)
{
    data->response->sendFile(":/site/play.png");
    auto result = HttpStatus::Ok;

    data->response->setStatus(result);

    return HttpPromise::resolve(data);
}

HttpPromise RequestHandler::handleFaviconRequest(HttpDataPtr data)
{
    data->response->sendFile(":/site/favicon.ico");
    auto result = HttpStatus::Ok;

    data->response->setStatus(result);

    return HttpPromise::resolve(data);

}

