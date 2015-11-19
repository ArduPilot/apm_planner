#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = 0);
    Q_INVOKABLE void setValue(const QString& key, const QVariant& value);
    Q_INVOKABLE void setValueWithPrefix(const QString& prefix, const QString& key, const QVariant& value);
    Q_INVOKABLE QVariant value(const QString& key, const QVariant& defaultValue = QVariant());
    Q_INVOKABLE QVariant valueWithPrefix(const QString& group, const QString& key, const QVariant& defaultValue = QVariant());

signals:

public slots:
private:
    QSettings m_settings;
};

#endif // SETTINGS_H
