#include "SystemSettings.h"

QSettings& SystemSettings::instance()
{
    static QSettings settings(QSettings::IniFormat, QSettings::UserScope, "The Unauthorized Frog", "funkplot");
    settings.setFallbacksEnabled(false); // Ensure system-wide only
    return settings;
}
