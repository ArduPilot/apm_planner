#ifndef UASOBJECT_H
#define UASOBJECT_H

#include <QObject>
#include "LinkInterface.h"
#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"
class UASObject : public QObject
{
    Q_OBJECT
public:
    explicit UASObject(QObject *parent = 0);

signals:

public slots:
    void messageReceived(LinkInterface* link,mavlink_message_t message);
};

#endif // UASOBJECT_H
