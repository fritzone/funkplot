#ifndef BUILTINDICTIONARY_H
#define BUILTINDICTIONARY_H

#include <QMap>
#include <QString>

struct BuiltinDictionary
{
    static void addEntry(const QString& k, const QString& v)
    {
        m_map[k] = v;
    }

    static QString entry(const QString& k)
    {
        if(m_map.contains(k))
        {
            return m_map[k];
        }

        return "";
    }

    static QString prepareText(const QString& t)
    {
        QString result = t;
        for(auto& s : m_map.keys())
        {
            result = result.replace(s, "<b><i>" + s + "</i></b>");
        }
        return result;
    }

    static QMap<QString, QString> m_map;
};

#endif // BUILTINDICTIONARY_H
