#include "Builtin.h"
#include "Parametric.h"
#include "BuiltinDictionary.h"

#include <QJsonArray>
#include <QJsonObject>

QMap<QString, QString> BuiltinDictionary::m_map;
QVector<QSharedPointer<Builtin>> Builtin::m_allBuiltins;

Builtin::Builtin(const QJsonObject &o) : m_name (o["name"].toString()),
    m_description(o["description"].toString()),
    m_key(o["key"].toString())
{
    QJsonObject eqO = o["equation"].toObject();

    m_description = BuiltinDictionary::prepareText(m_description);

    if(eqO["type"] == "parametric")
    {
        m_parForm.reset(new Parametric(-1, ""));

        m_parForm->functions.first.reset(new Function(QString(("f(t)=") + eqO["formula"].toObject()["x"].toString()).toLocal8Bit().data(), nullptr));
        m_parForm->functions.second.reset(new Function(QString(("f(t)=") + eqO["formula"].toObject()["y"].toString()).toLocal8Bit().data(), nullptr));
        m_eqType = EquationType::PARAMETRIC;
    }
    else if(eqO["type"] == "polar")
    {
        m_cartForm.reset( new Function (QString(("f(t)=") + eqO["formula"].toObject()["r"].toString()).toLocal8Bit().data(), nullptr));
        m_eqType = EquationType::POLAR;
    }

    QJsonArray pars = o["parameter"].toArray();
    for(size_t i = 0; i<pars.size(); i++)
    {
        QString desc = pars[i].toObject()["description"].toString();
        Parameter p {pars[i].toObject()["name"].toString(), desc};
        m_parameters.push_back(p);
    }
}

QString Builtin::getName() const
{
    return m_name;
}

QString Builtin::getDescription() const
{
    return m_description;
}

QString Builtin::getKey() const
{
    return m_key;
}

const QVector<Builtin::Parameter>& Builtin::getParameters() const
{
    return m_parameters;
}
