#ifndef SYSTEMSETTINGS_H
#define SYSTEMSETTINGS_H

#include <QSettings>
#include <memory>

class SystemSettings
{
public:
    // Returns a reference to a static QSettings object
    static QSettings& instance();

private:
    // Prevent instantiation
    SystemSettings() = delete;
    ~SystemSettings() = delete;
    SystemSettings(const SystemSettings&) = delete;
    SystemSettings& operator=(const SystemSettings&) = delete;
};

#endif // SYSTEMSETTINGS_H
