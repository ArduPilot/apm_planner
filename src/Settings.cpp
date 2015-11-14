#include "Settings.h"
#include "QsLog.h"

Settings::Settings(QObject *parent) :
    QObject(parent)
{
}

void Settings::setValue(const QString& key, const QVariant& value)
{
    m_settings.setValue(key, value);
}

void Settings::setValueWithPrefix(const QString& prefix, const QString& key, const QVariant& value)
{
    m_settings.beginGroup(prefix);
    m_settings.setValue(key, value);
    m_settings.endGroup();
}

QVariant Settings::value(const QString& key, const QVariant& defaultValue)
{
    m_settings.sync();
    QVariant value = m_settings.value(key, defaultValue);
    QLOG_DEBUG() << "Setting for " << key << ":" << value;
    return value;
}

QVariant Settings::valueWithPrefix(const QString& prefix, const QString& key, const QVariant& defaultValue)
{
    m_settings.beginGroup(prefix);
    return m_settings.value(key, defaultValue);
    m_settings.endGroup();
}
