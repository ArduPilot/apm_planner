#include "UASObject.h"

#include "VehicleOverview.h"
#include "RelPositionOverview.h"
#include "AbsPositionOverview.h"
#include "MissionOverview.h"

#include <QMetaType>

UASObject::UASObject(QObject *parent) : QObject(parent),
  m_vehicleOverview(NULL),
  m_relPositionOverview(NULL),
  m_absPositionOverview(NULL),
  m_missionOverview(NULL)
{
    m_vehicleOverview = new VehicleOverview(this);
    m_relPositionOverview = new RelPositionOverview(this);
    m_absPositionOverview = new AbsPositionOverview(this);
    m_missionOverview = new MissionOverview(this);
}

UASObject::~UASObject()
{
    delete m_vehicleOverview;
    m_vehicleOverview = NULL;
    delete m_relPositionOverview;
    m_relPositionOverview = NULL;
    delete m_absPositionOverview;
    m_absPositionOverview = NULL;
    delete m_missionOverview;
    m_missionOverview = NULL;
}

VehicleOverview* UASObject::getVehicleOverview()
{
    return m_vehicleOverview;
}

RelPositionOverview* UASObject::getRelPositionOverview()
{
    return m_relPositionOverview;
}

AbsPositionOverview* UASObject::getAbsPositionOverview()
{
    return m_absPositionOverview;
}

MissionOverview* UASObject::getMissionOverview()
{
    return m_missionOverview;
}

void UASObject::messageReceived(LinkInterface* link,mavlink_message_t message)
{
    m_vehicleOverview->messageReceived(link, message);
    m_relPositionOverview->messageReceived(link, message);
    m_absPositionOverview->messageReceived(link, message);
    m_missionOverview->messageReceived(link, message);
}
