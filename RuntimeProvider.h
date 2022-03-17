#ifndef RUNTIMEPROVIDER_H
#define RUNTIMEPROVIDER_H

#include <string>
#include <QMap>
#include <QString>
#include <QPointF>

class MainWindow;

class RuntimeProvider
{
public:

    explicit RuntimeProvider();

    int defd(const std::string& s);
    double value(const std::string& s);
    void setValue(const QString&s, double v);
    void setValue(const QString&s, QPointF v);

    QMap<QString, double>& variables();

    MainWindow* window() const;
    void setWindow(MainWindow* w);
private:

    QMap<QString, double> m_vars;
    QMap<QString, QPointF> m_points;

    MainWindow* m_window;
};

#endif // RUNTIMEPROVIDER_H
