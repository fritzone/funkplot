#ifndef BUILTIN_H
#define BUILTIN_H

#include <QSharedPointer>
#include <QString>
#include <QMap>
#include <QPair>
#include <QVector>

class Function;
class Parametric;
class QJsonObject;

enum class EquationType
{
    UNKNOWN = 0,
    POLAR = 1,
    PARAMETRIC = 2,
    CARTESIAN = 3
};

class Builtin
{
public:

    struct Parameter
    {
        QString name;
        QString description;
        QString defaultValue;
    };

    explicit Builtin(const QJsonObject&);

    QString getName() const;

    QString getDescription() const;

    QString getKey() const;

    const QVector<Parameter>& getParameters() const;

    EquationType getEquationType() const;

    QMap<QString, QString> getFormula() const;

    QPair<QString, QString> getInterval() const;

    static QVector<QSharedPointer<Builtin>> m_allBuiltins;

private:

    QString m_name;
    QString m_description;
    QSharedPointer<Parametric> m_parForm {nullptr};
    QSharedPointer<Function> m_cartForm {nullptr};
    QString m_key;
    EquationType m_eqType {EquationType::UNKNOWN};
    QVector<Parameter> m_parameters;
    QMap<QString, QString> m_formula;
    QPair<QString, QString> m_interval;

};

#endif // BUILTIN_H
