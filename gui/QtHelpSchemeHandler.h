#ifndef QTHELPSCHEMEHANDLER_H
#define QTHELPSCHEMEHANDLER_H

#include <QObject>
#include <QBuffer>
#include <QtHelp/QHelpEngine>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlSchemeHandler>

class QHelpEngine;

class QtHelpSchemeHandler : public QWebEngineUrlSchemeHandler {
    Q_OBJECT
public:
    explicit QtHelpSchemeHandler(QHelpEngine* helpEngine) : mHelpEngine(helpEngine) {

    }

    virtual void requestStarted(QWebEngineUrlRequestJob* job) override;
private:
    QHelpEngine* mHelpEngine;
    QMap<QUrl, QBuffer*> m_buffers;
};

#endif // QTHELPSCHEMEHANDLER_H
