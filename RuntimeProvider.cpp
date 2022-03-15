#include "RuntimeProvider.h"

RuntimeProvider::RuntimeProvider() : m_vars()
{

}

int RuntimeProvider::defd(const std::string &s)
{
    if(m_vars.contains(QString::fromStdString(s)))
    {
        return 1;
    }

    return 0;
}

double RuntimeProvider::value(const std::string &s)
{
    if(defd(s))
    {
        return m_vars.value(QString::fromStdString(s));
    }

    return std::numeric_limits<double>::quiet_NaN();
}

void RuntimeProvider::setValue(const QString &s, double v)
{
    m_vars[s] = v;
}

QMap<QString, double> &RuntimeProvider::variables()
{
    return m_vars;
}

MainWindow *RuntimeProvider::window() const
{
    return m_window;
}

void RuntimeProvider::setWindow(MainWindow *w)
{
    m_window = w;
}
