#ifndef UASPARAMETER_H
#define UASPARAMETER_H

#include <stdint.h>
#include <QObject>
#include <QVariant>

class UASParameter : public QObject
{
    Q_OBJECT

public:
    explicit UASParameter(QObject *parent = 0);
    explicit UASParameter(QString& name, int component, QVariant& value, uint16_t index,
                          QObject *parent = 0);

public:
    void setName(const QString newName);
    const QString& name();

    void setComponent(int aComponent);
    int component();

    void setValue(const QVariant value);
    const QVariant &value();

    void setIndex(int index);
    int index();

    // Metadata
    void setDisplayName(const QString& aName);
    const QString& displayName();

    void setDescription(const QString& aDescription);
    const QString& description();

    void setUnit(const QString& aUnit);
    const QString& unit();

    // state
    bool isModified();
    void setModified(bool modified);

private:
    QString m_paramName;
    int m_paramComponent;
    QVariant m_paramValue;
    uint16_t m_paramIndex;

    QString m_displayName;
    QString m_description;
    QString m_unit;

    bool m_modified;

};

#endif // UASPARAMETER_H
