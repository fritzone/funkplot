#ifndef RUNTIMEPROVIDER_H
#define RUNTIMEPROVIDER_H

#include <string>
#include <QMap>
#include <QString>

class RuntimeProvider
{
public:

    explicit RuntimeProvider(QMap<QString, double>* vars);

    int defd(const std::string& s);
    double value(const std::string& s);

private:

    QMap<QString, double>* m_vars;
};

#endif // RUNTIMEPROVIDER_H
