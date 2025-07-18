#ifndef HTTP_SERVER_HTTP_COOKIE_H
#define HTTP_SERVER_HTTP_COOKIE_H

#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <QUrl>

#include "util.h"


struct HTTPSERVER_EXPORT HttpCookie
{
    QString name;
    QString value;
    int ageSeconds;
    QDateTime expiration;
    QString domain;
    QString path;
    bool secure;
    bool httpOnly;

    HttpCookie() {}
    HttpCookie(QString name, QString value, int ageSeconds = -1, QDateTime expiration = QDateTime(), QString domain = "",
        QString path = "/", bool secure = false, bool httpOnly = false) : name(name), value(value), ageSeconds(ageSeconds),
        expiration(expiration), domain(domain), path(path), secure(secure), httpOnly(httpOnly) {}

    QByteArray toByteArray() const
    {
        QByteArray buf;

        buf += name.toLatin1();
        buf += '=';
        buf += QUrl::toPercentEncoding(value);

        if (expiration.isValid())
            buf += QString("; Expires=" + expiration.toString(Qt::RFC2822Date)).toUtf8();

        if (ageSeconds > 0)
            buf += QString("; Max-Age=" + QString::number(ageSeconds)).toUtf8();

        if (!domain.isEmpty())
            buf += QString("; Domain=" + domain).toUtf8();

        if (!path.isEmpty())
            buf += "; Path=" + QUrl::toPercentEncoding(path);

        if (secure)
            buf += "; Secure";

        if (httpOnly)
            buf += "; HttpOnly";

        return buf;
    }
};

#endif // HTTP_SERVER_HTTP_COOKIE_H
