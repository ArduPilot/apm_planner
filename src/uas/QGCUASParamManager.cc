#include "QGCUASParamManager.h"
#include "UASInterface.h"

QGCUASParamManager::QGCUASParamManager(UASInterface* uas, QWidget *parent) :
    QWidget(parent),
    mav(uas),
    transmissionListMode(false),
    transmissionActive(false),
    transmissionTimeout(0),
    retransmissionTimeout(350),
    rewriteTimeout(500),
    retransmissionBurstRequestSize(5)
{
    uas->setParamManager(this);
}
QList<QString> QGCUASParamManager::getParameterNames(int component) const
{
    if (parameters.contains(component))
    {
        return parameters.value(component)->keys();
    }
    else
    {
        return QList<QString>();
    }
}
QList<QVariant> QGCUASParamManager::getParameterValues(int component) const
{
    if (parameters.contains(component))
    {
        return parameters.value(component)->values();
    }
    else
    {
        return QList<QVariant>();
    }
}
bool QGCUASParamManager::getParameterValue(int component, const QString& parameter, QVariant& value) const {
    if (!parameters.contains(component))
    {
        return false;
    }

    if (!parameters.value(component)->contains(parameter))
    {
        return false;
    }

    value = parameters.value(component)->value(parameter);

    return true;
}

QVariant QGCUASParamManager::getParameterValue(int component, const QString& parameter) const
{
    if (!parameters.contains(component))
    {
        return QVariant();
    }
    if (!parameters.value(component)->contains(parameter))
    {
        return QVariant();
    }
    return parameters.value(component)->value(parameter);
}

/**
 * The .. signal is emitted
 */
void QGCUASParamManager::requestParameterListUpdate(int component)
{
	Q_UNUSED(component);
}


