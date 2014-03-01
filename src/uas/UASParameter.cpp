#include "UASParameter.h"

UASParameter::UASParameter(QObject *parent) :
    QObject(parent),
    m_paramComponent(0),
    m_modified(false)
{
}

UASParameter::UASParameter(QString &name, int component, QVariant &value, uint16_t index, QObject *parent) :
    QObject(parent),
    m_modified(false)
{
    m_paramName = name;
    m_paramComponent = component;
    m_paramValue = value;
    m_paramIndex = index;
}

void UASParameter::setName(const QString newName)
{
    m_paramName = newName;
}

const QString& UASParameter::name()
{
    return m_paramName;
}

void UASParameter::setComponent(int aComponent)
{
    m_paramComponent = aComponent;
}

int UASParameter::component()
{
    return m_paramComponent;
}

void UASParameter::setValue(const QVariant value)
{
    if (value.toDouble() != m_paramValue.toDouble()){
        m_paramValue = value;
        m_modified = true;
    } else {
        m_modified = false;
    }
}

const QVariant& UASParameter::value()
{
    return m_paramValue;
}

void UASParameter::setIndex(int index)
{
    m_paramIndex = index;
}

int UASParameter::index()
{
    return m_paramIndex;
}

void UASParameter::setDisplayName(const QString& aName)
{
    m_displayName = aName;
}

const QString& UASParameter::displayName()
{
    return m_displayName;
}

void UASParameter::setDescription(const QString& aDescription)
{
    m_description = aDescription;
}

const QString& UASParameter::description()
{
    return m_description;
}

void UASParameter::setUnit(const QString& aUnit)
{
    m_unit = aUnit;
}

const QString& UASParameter::unit()
{
    return m_unit;
}

bool UASParameter::isModified()
{
    return m_modified;
}

void UASParameter::setModified(bool isModified)
{
    m_modified = isModified;
}
