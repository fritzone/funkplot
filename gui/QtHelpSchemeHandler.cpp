#include "QtHelpSchemeHandler.h"

void QtHelpSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job) {
    QUrl url = job->requestUrl();
    if(m_buffers.contains(url))
    {
        job->reply("text/html", m_buffers[url]);
        return;
    }

    auto data = new QByteArray;
    *data = mHelpEngine->fileData(url);
    auto buffer = new QBuffer(data);
    buffer->setParent(this);
    if (url.scheme() == "qthelp")
    {
        m_buffers[url] = buffer;
        job->reply("text/html", buffer);
    }
}
