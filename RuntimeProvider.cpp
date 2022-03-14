#include "RuntimeProvider.h"

RuntimeProvider::RuntimeProvider(QMap<QString, double> *vars) : m_vars(vars)
{

}

int RuntimeProvider::defd(const std::string &s)
{
    if(m_vars->contains(QString::fromStdString(s)))
    {
        return 1;
    }

    return 0;
}

double RuntimeProvider::value(const std::string &s)
{
    if(defd(s))
    {
        return m_vars->value(QString::fromStdString(s));
    }

    return std::numeric_limits<double>::quiet_NaN();
}
