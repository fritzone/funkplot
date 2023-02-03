#ifndef BUILTIN_H
#define BUILTIN_H

#include <QSharedPointer>
#include <QString>

class Function;
class Parametric;
class QJsonObject;

enum class EquationType
{
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
    };

    explicit Builtin(const QJsonObject&);

    QString getName() const;

    QString getDescription() const;

    QString getKey() const;

    const QVector<Parameter>& getParameters() const;

private:

    QString m_name;
    QString m_description;
    QSharedPointer<Parametric> m_parForm {nullptr};
    QSharedPointer<Function> m_cartForm {nullptr};
    QString m_key;
    EquationType m_eqType;
    QVector<Parameter> m_parameters;
};

#endif // BUILTIN_H
