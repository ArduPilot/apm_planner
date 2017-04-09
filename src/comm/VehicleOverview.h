#ifndef VEHICLEOVERVIEW_H
#define VEHICLEOVERVIEW_H

#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"
#include "LinkInterface.h"
#include <QObject>

class VehicleOverview : public QObject
{
    Q_OBJECT
    //Heartbeat properties
    Q_PROPERTY(unsigned int custom_mode READ getCustomMode WRITE setCustomMode NOTIFY customModeChanged)
    Q_PROPERTY(unsigned int type READ getType WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(unsigned int autopilot READ getAutopilot WRITE setAutopilot NOTIFY autopilotChanged)
    Q_PROPERTY(unsigned int base_mode READ getBaseMode WRITE setBaseMode NOTIFY baseModeChanged)
    Q_PROPERTY(unsigned int system_status READ getSystemStatus WRITE setSystemStatus NOTIFY systemStatusChanged)
    Q_PROPERTY(unsigned int mavlink_version READ getMavlinkVersion WRITE setMavlinkVersion NOTIFY mavlinkVersionChanged)

    //Sys_status properties
    Q_PROPERTY(unsigned int onboard_control_sensors_present READ getOnboardControlSensorsPresent WRITE setOnboardControlSensorsPresent NOTIFY onboardControlSensorsPresentChanged)
    Q_PROPERTY(unsigned int onboard_control_sensors_enabled READ getOnboardControlSensorsEnabled WRITE setOnboardControlSensorsEnabled NOTIFY onboardControlSensorsEnabledChanged)
    Q_PROPERTY(unsigned int onboard_control_sensors_health READ getOnboardControlSensorsHealth WRITE setOnboardControlSensorsHealth NOTIFY onboardControlSensorsHealthChanged)
    Q_PROPERTY(unsigned int load READ getLoad WRITE setLoad NOTIFY loadChanged)
    Q_PROPERTY(unsigned int voltage_battery READ getVoltageBattery WRITE setVoltageBattery NOTIFY voltageBatteryChanged)
    Q_PROPERTY(unsigned int drop_rate_comm READ getDropRateComm WRITE setDropRateComm NOTIFY dropRateCommChanged)
    Q_PROPERTY(unsigned int errors_comm READ getErrorsComm WRITE setErrorsComm NOTIFY errorsCommChanged)
    Q_PROPERTY(unsigned int errors_count1 READ getErrorsCount1 WRITE setErrorsCount1 NOTIFY errorsCount1Changed)
    Q_PROPERTY(unsigned int errors_count2 READ getErrorsCount2 WRITE setErrorsCount2 NOTIFY errorsCount2Changed)
    Q_PROPERTY(unsigned int errors_count3 READ getErrorsCount3 WRITE setErrorsCount3 NOTIFY errorsCount3Changed)
    Q_PROPERTY(unsigned int errors_count4 READ getErrorsCount4 WRITE setErrorsCount4 NOTIFY errorsCount4Changed)

    //nav_controller_output properties
    Q_PROPERTY(double nav_roll READ getNavRoll WRITE setNavRoll NOTIFY navRollChanged)
    Q_PROPERTY(double nav_pitch READ getNavPitch WRITE setNavPitch NOTIFY navPitchChanged)
    Q_PROPERTY(double alt_error READ getAltError WRITE setAltError NOTIFY altErrorChanged)
    Q_PROPERTY(double aspd_error READ getAspdError WRITE setAspdError NOTIFY aspdErrorChanged)
    Q_PROPERTY(double xtrack_error READ getXtrackError WRITE setXtrackError NOTIFY xtrackErrorChanged)
    Q_PROPERTY(int nav_bearing READ getNavBearing WRITE setNavBearing NOTIFY navBearingChanged)
    Q_PROPERTY(int target_bearing READ getTargetBearing WRITE setTargetBearing NOTIFY targetBearingChanged)
    Q_PROPERTY(unsigned int wp_dist READ getWpDist WRITE setWpDist NOTIFY wpDistChanged)

    //battery_status properties
    Q_PROPERTY(int current_consumed READ getCurrentConsumed WRITE setCurrentConsumed NOTIFY currentConsumedChanged)
    Q_PROPERTY(int energy_consumed READ getEnergyConsumed WRITE setEnergyConsumed NOTIFY energyConsumedChanged)
    Q_PROPERTY(unsigned int voltage_cell_1 READ getVoltageCell1 WRITE setVoltageCell1 NOTIFY voltageCell1Changed)
    Q_PROPERTY(unsigned int voltage_cell_2 READ getVoltageCell2 WRITE setVoltageCell2 NOTIFY voltageCell2Changed)
    Q_PROPERTY(unsigned int voltage_cell_3 READ getVoltageCell3 WRITE setVoltageCell3 NOTIFY voltageCell3Changed)
    Q_PROPERTY(unsigned int voltage_cell_4 READ getVoltageCell4 WRITE setVoltageCell4 NOTIFY voltageCell4Changed)
    Q_PROPERTY(unsigned int voltage_cell_5 READ getVoltageCell5 WRITE setVoltageCell5 NOTIFY voltageCell5Changed)
    Q_PROPERTY(unsigned int voltage_cell_6 READ getVoltageCell6 WRITE setVoltageCell6 NOTIFY voltageCell6Changed)
    Q_PROPERTY(int current_battery READ getCurrentBattery WRITE setCurrentBattery NOTIFY currentBatteryChanged)
    Q_PROPERTY(unsigned int accu_id READ getAccuId WRITE setAccuId NOTIFY accuIdChanged)
    Q_PROPERTY(int battery_remaining READ getBatteryRemaining WRITE setBatteryRemaining NOTIFY batteryRemainingChanged)

    //power status properties
    Q_PROPERTY(unsigned int Vcc READ getVcc WRITE setVcc NOTIFY VccChanged)
    Q_PROPERTY(unsigned int Vservo READ getVservo WRITE setVservo NOTIFY VservoChanged)
    Q_PROPERTY(unsigned int flags READ getFlags WRITE setFlags NOTIFY flagsChanged)

    //radiostatus properties
    Q_PROPERTY(unsigned int rxerrors READ getRxerrors WRITE setRxerrors NOTIFY rxerrorsChanged)
    Q_PROPERTY(unsigned int fixed READ getFixed WRITE setFixed NOTIFY fixedChanged)
    Q_PROPERTY(unsigned int rssi READ getRssi WRITE setRssi NOTIFY rssiChanged)
    Q_PROPERTY(unsigned int remrssi READ getRemrssi WRITE setRemrssi NOTIFY remrssiChanged)
    Q_PROPERTY(unsigned int txbuf READ getTxbuf WRITE setTxbuf NOTIFY txbufChanged)
    Q_PROPERTY(unsigned int noise READ getNoise WRITE setNoise NOTIFY noiseChanged)
    Q_PROPERTY(unsigned int remnoise READ getRemnoise WRITE setRemnoise NOTIFY remnoiseChanged)

    //User generated
    Q_PROPERTY(bool armed_state READ getArmedState WRITE setArmedState NOTIFY armedStateChanged)

    // Vibration Messages
    Q_PROPERTY(double vibration_x READ getVibrationX NOTIFY vibrationXChanged)
    Q_PROPERTY(double vibration_y READ getVibrationY NOTIFY vibrationYChanged)
    Q_PROPERTY(double vibration_z READ getVibrationZ NOTIFY vibrationZChanged)

    Q_PROPERTY(double clipping_0 READ getClipping0 NOTIFY clipping0Changed)
    Q_PROPERTY(double clipping_1 READ getClipping1 NOTIFY clipping1Changed)
    Q_PROPERTY(double clipping_2 READ getClipping2 NOTIFY clipping2Changed)

    // EKF Status Report
    Q_PROPERTY(double ekf_flags READ getEkfFlags NOTIFY ekfFlagsChanged)
    Q_PROPERTY(double ekf_velocity_variance READ getEkfVelocityVariance NOTIFY ekfVelocityVarianceChanged)
    Q_PROPERTY(double ekf_pos_horiz_variance READ getEkfPosHorizVariance NOTIFY ekfPosHorizVarianceChanged)
    Q_PROPERTY(double ekf_pos_vert_variance READ getEkfPosVertVariance NOTIFY ekfPosVertVarianceChanged)
    Q_PROPERTY(double ekf_compass_variance READ getEkfComapssVariance NOTIFY ekfCompassVarianceChanged)
    Q_PROPERTY(double ekf_terrain_alt_variance READ getEkfTerrainAltVariance NOTIFY ekfTerrainAltVarianceChanged)

public:
    //Heartbeat
    unsigned int getCustomMode() { return m_customMode; }
    unsigned int getType() { return m_type; }
    unsigned int getAutopilot() { return m_autopilot; }
    unsigned int getBaseMode() { return m_baseMode; }
    unsigned int getSystemStatus() { return m_systemStatus; }
    unsigned int getMavlinkVersion() { return m_mavlinkVersion; }
    void setCustomMode(unsigned int customMode) { if (m_customMode!=customMode){m_customMode = customMode; emit customModeChanged(customMode);}}
    void setType(unsigned int type) { if (m_type!=type){m_type = type; emit typeChanged(type);}}
    void setAutopilot(unsigned int autopilot) { if (m_autopilot!=autopilot){m_autopilot = autopilot; emit autopilotChanged(autopilot);}}
    void setBaseMode(unsigned int baseMode) { if (m_baseMode!=baseMode){m_baseMode = baseMode; emit baseModeChanged(baseMode);}}
    void setSystemStatus(unsigned int systemStatus) { if (m_systemStatus!=systemStatus){m_systemStatus = systemStatus; emit systemStatusChanged(systemStatus);}}
    void setMavlinkVersion(unsigned int mavlinkVersion) { if (m_mavlinkVersion!=mavlinkVersion){m_mavlinkVersion = mavlinkVersion; emit mavlinkVersionChanged(mavlinkVersion);}}

    //sys_status
    unsigned int getOnboardControlSensorsPresent() { return m_onboardControlSensorsPresent; }
    unsigned int getOnboardControlSensorsEnabled() { return m_onboardControlSensorsEnabled; }
    unsigned int getOnboardControlSensorsHealth() { return m_onboardControlSensorsHealth; }
    unsigned int getLoad() { return m_load; }
    unsigned int getVoltageBattery() { return m_voltageBattery; }
    unsigned int getDropRateComm() { return m_dropRateComm; }
    unsigned int getErrorsComm() { return m_errorsComm; }
    unsigned int getErrorsCount1() { return m_errorsCount1; }
    unsigned int getErrorsCount2() { return m_errorsCount2; }
    unsigned int getErrorsCount3() { return m_errorsCount3; }
    unsigned int getErrorsCount4() { return m_errorsCount4; }
    void setOnboardControlSensorsPresent(unsigned int onboardControlSensorsPresent) { if (m_onboardControlSensorsPresent!=onboardControlSensorsPresent){m_onboardControlSensorsPresent = onboardControlSensorsPresent; emit onboardControlSensorsPresentChanged(onboardControlSensorsPresent);}}
    void setOnboardControlSensorsEnabled(unsigned int onboardControlSensorsEnabled) { if (m_onboardControlSensorsEnabled!=onboardControlSensorsEnabled){m_onboardControlSensorsEnabled = onboardControlSensorsEnabled; emit onboardControlSensorsEnabledChanged(onboardControlSensorsEnabled);}}
    void setOnboardControlSensorsHealth(unsigned int onboardControlSensorsHealth) { if (m_onboardControlSensorsHealth!=onboardControlSensorsHealth){m_onboardControlSensorsHealth = onboardControlSensorsHealth; emit onboardControlSensorsHealthChanged(onboardControlSensorsHealth);}}
    void setLoad(unsigned int load) { if (m_load!=load){m_load = load; emit loadChanged(load);}}
    void setVoltageBattery(unsigned int voltageBattery) { if (m_voltageBattery!=voltageBattery){m_voltageBattery = voltageBattery; emit voltageBatteryChanged(voltageBattery);}}
    void setDropRateComm(unsigned int dropRateComm) { if (m_dropRateComm!=dropRateComm){m_dropRateComm = dropRateComm; emit dropRateCommChanged(dropRateComm);}}
    void setErrorsComm(unsigned int errorsComm) { if (m_errorsComm!=errorsComm){m_errorsComm = errorsComm; emit errorsCommChanged(errorsComm);}}
    void setErrorsCount1(unsigned int errorsCount1) { if (m_errorsCount1!=errorsCount1){m_errorsCount1 = errorsCount1; emit errorsCount1Changed(errorsCount1);}}
    void setErrorsCount2(unsigned int errorsCount2) { if (m_errorsCount2!=errorsCount2){m_errorsCount2 = errorsCount2; emit errorsCount2Changed(errorsCount2);}}
    void setErrorsCount3(unsigned int errorsCount3) { if (m_errorsCount3!=errorsCount3){m_errorsCount3 = errorsCount3; emit errorsCount3Changed(errorsCount3);}}
    void setErrorsCount4(unsigned int errorsCount4) { if (m_errorsCount4!=errorsCount4){m_errorsCount4 = errorsCount4; emit errorsCount4Changed(errorsCount4);}}

    //nav controller output
    double getNavRoll() { return m_navRoll; }
    double getNavPitch() { return m_navPitch; }
    double getAltError() { return m_altError; }
    double getAspdError() { return m_aspdError; }
    double getXtrackError() { return m_xtrackError; }
    int getNavBearing() { return m_navBearing; }
    int getTargetBearing() { return m_targetBearing; }
    unsigned int getWpDist() { return m_wpDist; }
    void setNavRoll(double navRoll) { if (m_navRoll!=navRoll){m_navRoll = navRoll; emit navRollChanged(navRoll);}}
    void setNavPitch(double navPitch) { if (m_navPitch!=navPitch){m_navPitch = navPitch; emit navPitchChanged(navPitch);}}
    void setAltError(double altError) { if (m_altError!=altError){m_altError = altError; emit altErrorChanged(altError);}}
    void setAspdError(double aspdError) { if (m_aspdError!=aspdError){m_aspdError = aspdError; emit aspdErrorChanged(aspdError);}}
    void setXtrackError(double xtrackError) { if (m_xtrackError!=xtrackError){m_xtrackError = xtrackError; emit xtrackErrorChanged(xtrackError);}}
    void setNavBearing(int navBearing) { if (m_navBearing!=navBearing){m_navBearing = navBearing; emit navBearingChanged(navBearing);}}
    void setTargetBearing(int targetBearing) { if (m_targetBearing!=targetBearing){m_targetBearing = targetBearing; emit targetBearingChanged(targetBearing);}}
    void setWpDist(unsigned int wpDist) { if (m_wpDist!=wpDist){m_wpDist = wpDist; emit wpDistChanged(wpDist);}}

    //battery status
    int getCurrentConsumed() { return m_currentConsumed; }
    int getEnergyConsumed() { return m_energyConsumed; }
    unsigned int getVoltageCell1() { return m_voltageCell1; }
    unsigned int getVoltageCell2() { return m_voltageCell2; }
    unsigned int getVoltageCell3() { return m_voltageCell3; }
    unsigned int getVoltageCell4() { return m_voltageCell4; }
    unsigned int getVoltageCell5() { return m_voltageCell5; }
    unsigned int getVoltageCell6() { return m_voltageCell6; }
    int getCurrentBattery() { return m_currentBattery; }
    unsigned int getAccuId() { return m_accuId; }
    int getBatteryRemaining() { return m_batteryRemaining; }
    void setCurrentConsumed(int currentConsumed) { if (m_currentConsumed!=currentConsumed){m_currentConsumed = currentConsumed; emit currentConsumedChanged(currentConsumed);}}
    void setEnergyConsumed(int energyConsumed) { if (m_energyConsumed!=energyConsumed){m_energyConsumed = energyConsumed; emit energyConsumedChanged(energyConsumed);}}
    void setVoltageCell1(unsigned int voltageCell1) { if (m_voltageCell1!=voltageCell1){m_voltageCell1 = voltageCell1; emit voltageCell1Changed(voltageCell1);}}
    void setVoltageCell2(unsigned int voltageCell2) { if (m_voltageCell2!=voltageCell2){m_voltageCell2 = voltageCell2; emit voltageCell2Changed(voltageCell2);}}
    void setVoltageCell3(unsigned int voltageCell3) { if (m_voltageCell3!=voltageCell3){m_voltageCell3 = voltageCell3; emit voltageCell3Changed(voltageCell3);}}
    void setVoltageCell4(unsigned int voltageCell4) { if (m_voltageCell4!=voltageCell4){m_voltageCell4 = voltageCell4; emit voltageCell4Changed(voltageCell4);}}
    void setVoltageCell5(unsigned int voltageCell5) { if (m_voltageCell5!=voltageCell5){m_voltageCell5 = voltageCell5; emit voltageCell5Changed(voltageCell5);}}
    void setVoltageCell6(unsigned int voltageCell6) { if (m_voltageCell6!=voltageCell6){m_voltageCell6 = voltageCell6; emit voltageCell6Changed(voltageCell6);}}
    void setCurrentBattery(int currentBattery) { if (m_currentBattery!=currentBattery){m_currentBattery = currentBattery; emit currentBatteryChanged(currentBattery);}}
    void setAccuId(unsigned int accuId) { if (m_accuId!=accuId){m_accuId = accuId; emit accuIdChanged(accuId);}}
    void setBatteryRemaining(int batteryRemaining) { if (m_batteryRemaining!=batteryRemaining){m_batteryRemaining = batteryRemaining; emit batteryRemainingChanged(batteryRemaining);}}

    //power_status
    unsigned int getVcc() { return m_Vcc; }
    unsigned int getVservo() { return m_Vservo; }
    unsigned int getFlags() { return m_flags; }
    void setVcc(unsigned int Vcc) { if (m_Vcc!=Vcc){m_Vcc = Vcc; emit VccChanged(Vcc);}}
    void setVservo(unsigned int Vservo) { if (m_Vservo!=Vservo){m_Vservo = Vservo; emit VservoChanged(Vservo);}}
    void setFlags(unsigned int flags) { if (m_flags!=flags){m_flags = flags; emit flagsChanged(flags);}}

    //radio_status
    unsigned int getRxerrors() { return m_rxerrors; }
    unsigned int getFixed() { return m_fixed; }
    unsigned int getRssi() { return m_rssi; }
    unsigned int getRemrssi() { return m_remrssi; }
    unsigned int getTxbuf() { return m_txbuf; }
    unsigned int getNoise() { return m_noise; }
    unsigned int getRemnoise() { return m_remnoise; }
    void setRxerrors(unsigned int rxerrors) { if (m_rxerrors!=rxerrors){m_rxerrors = rxerrors; emit rxerrorsChanged(rxerrors);}}
    void setFixed(unsigned int fixed) { if (m_fixed!=fixed){m_fixed = fixed; emit fixedChanged(fixed);}}
    void setRssi(unsigned int rssi) { if (m_rssi!=rssi){m_rssi = rssi; emit rssiChanged(rssi);}}
    void setRemrssi(unsigned int remrssi) { if (m_remrssi!=remrssi){m_remrssi = remrssi; emit remrssiChanged(remrssi);}}
    void setTxbuf(unsigned int txbuf) { if (m_txbuf!=txbuf){m_txbuf = txbuf; emit txbufChanged(txbuf);}}
    void setNoise(unsigned int noise) { if (m_noise!=noise){m_noise = noise; emit noiseChanged(noise);}}
    void setRemnoise(unsigned int remnoise) { if (m_remnoise!=remnoise){m_remnoise = remnoise; emit remnoiseChanged(remnoise);}}

    //User generated
    bool getArmedState() { return m_armedState; }
    void setArmedState(bool armedState) { if (m_armedState != armedState) { m_armedState = armedState; emit armedStateChanged(armedState); }}

    // Vibration
    double getVibrationX() { return m_vibrationX; }
    double getVibrationY() { return m_vibrationY; }
    double getVibrationZ() { return m_vibrationZ; }
    void setVibrationX(double newValue) { if (m_vibrationX != newValue) { m_vibrationX = newValue; emit vibrationXChanged(m_vibrationX); }}
    void setVibrationY(double newValue) { if (m_vibrationY != newValue) { m_vibrationY = newValue; emit vibrationYChanged(m_vibrationY); }}
    void setVibrationZ(double newValue) { if (m_vibrationZ != newValue) { m_vibrationZ = newValue; emit vibrationZChanged(m_vibrationZ); }}

    double getClipping0() { return m_clipping0; }
    double getClipping1() { return m_clipping1; }
    double getClipping2() { return m_clipping2; }
    void setClipping0(double newValue) { if (m_clipping0 != newValue) { m_clipping0 = newValue; emit clipping0Changed(m_clipping0); }}
    void setClipping1(double newValue) { if (m_clipping1 != newValue) { m_clipping1 = newValue; emit clipping1Changed(m_clipping1); }}
    void setClipping2(double newValue) { if (m_clipping2 != newValue) { m_clipping2 = newValue; emit clipping2Changed(m_clipping2); }}

    // EKF Status Report
    uint16_t getEkfFlags() { return m_ekfFlags; }
    float getEkfVelocityVariance() { return m_velocity_variance; }
    float getEkfPosHorizVariance() { return m_pos_horiz_variance; }
    float getEkfPosVertVariance() { return m_pos_vert_variance; }
    float getEkfComapssVariance() { return m_compass_variance; }
    float getEkfTerrainAltVariance() { return m_terrain_alt_variance; }

    void setEkfFlags(uint16_t newFlags) { if (m_ekfFlags != newFlags) { m_ekfFlags = newFlags; emit ekfFlagsChanged(m_ekfFlags); }}
    void setEkfVelocityVariance(float newValue) { if (m_velocity_variance != newValue) { m_velocity_variance = newValue; emit ekfVelocityVarianceChanged(newValue); }}
    void setEkfPosHorizVariance(float newValue) { if (m_pos_horiz_variance != newValue) { m_pos_horiz_variance = newValue; emit ekfPosHorizVarianceChanged(newValue); }}
    void setEkfPosVertVariance(float newValue) { if (m_pos_vert_variance != newValue) { m_pos_vert_variance = newValue; emit ekfPosVertVarianceChanged(newValue); }}
    void setEkfComapssVariance(float newValue) { if (m_compass_variance != newValue) { m_compass_variance = newValue; emit ekfCompassVarianceChanged(newValue); }}
    void setEkfTerrainAltVariance(float newValue) { if (m_terrain_alt_variance != newValue) { m_terrain_alt_variance = newValue; emit ekfTerrainAltVarianceChanged(newValue); }}

private:
    //Heartbeat
    unsigned int m_customMode;
    unsigned int m_type;
    unsigned int m_autopilot;
    unsigned int m_baseMode;
    unsigned int m_systemStatus;
    unsigned int m_mavlinkVersion;

    //Sys_status
    unsigned int m_onboardControlSensorsPresent;
    unsigned int m_onboardControlSensorsEnabled;
    unsigned int m_onboardControlSensorsHealth;
    unsigned int m_load;
    unsigned int m_voltageBattery;
    unsigned int m_dropRateComm;
    unsigned int m_errorsComm;
    unsigned int m_errorsCount1;
    unsigned int m_errorsCount2;
    unsigned int m_errorsCount3;
    unsigned int m_errorsCount4;

    //nav_controller_output
    double m_navRoll;
    double m_navPitch;
    double m_altError;
    double m_aspdError;
    double m_xtrackError;
    int m_navBearing;
    int m_targetBearing;
    unsigned int m_wpDist;

    //battery_status
    int m_currentConsumed;
    int m_energyConsumed;
    unsigned int m_voltageCell1;
    unsigned int m_voltageCell2;
    unsigned int m_voltageCell3;
    unsigned int m_voltageCell4;
    unsigned int m_voltageCell5;
    unsigned int m_voltageCell6;
    int m_currentBattery;
    unsigned int m_accuId;
    int m_batteryRemaining;

    //power_status
    unsigned int m_Vcc;
    unsigned int m_Vservo;
    unsigned int m_flags;

    //radio_status
    unsigned int m_rxerrors;
    unsigned int m_fixed;
    unsigned int m_rssi;
    unsigned int m_remrssi;
    unsigned int m_txbuf;
    unsigned int m_noise;
    unsigned int m_remnoise;

    //User Generated
    bool m_armedState;

    // Vibration
    double m_vibrationX;
    double m_vibrationY;
    double m_vibrationZ;

    int m_clipping0;
    int m_clipping1;
    int m_clipping2;

    // EKF Status report
    unsigned int m_ekfFlags;
    double m_velocity_variance;
    double m_pos_horiz_variance;
    double m_pos_vert_variance;
    double m_compass_variance;
    double m_terrain_alt_variance;

public:
    explicit VehicleOverview(QObject *parent = 0);
    ~VehicleOverview();

signals:
    //Heartbeat
    void customModeChanged(int);
    void typeChanged(unsigned int);
    void autopilotChanged(unsigned int);
    void baseModeChanged(unsigned int);
    void systemStatusChanged(unsigned int);
    void mavlinkVersionChanged(unsigned int);

    //sys_status
    void onboardControlSensorsPresentChanged(unsigned int);
    void onboardControlSensorsEnabledChanged(unsigned int);
    void onboardControlSensorsHealthChanged(unsigned int);
    void loadChanged(unsigned int);
    void voltageBatteryChanged(unsigned int);
    void dropRateCommChanged(unsigned int);
    void errorsCommChanged(unsigned int);
    void errorsCount1Changed(unsigned int);
    void errorsCount2Changed(unsigned int);
    void errorsCount3Changed(unsigned int);
    void errorsCount4Changed(unsigned int);

    //nav_controller_output
    void navRollChanged(double);
    void navPitchChanged(double);
    void altErrorChanged(double);
    void aspdErrorChanged(double);
    void xtrackErrorChanged(double);
    void navBearingChanged(int);
    void targetBearingChanged(int);
    void wpDistChanged(unsigned int);

    //battery_status
    void currentConsumedChanged(int);
    void energyConsumedChanged(int);
    void voltageCell1Changed(unsigned int);
    void voltageCell2Changed(unsigned int);
    void voltageCell3Changed(unsigned int);
    void voltageCell4Changed(unsigned int);
    void voltageCell5Changed(unsigned int);
    void voltageCell6Changed(unsigned int);
    void currentBatteryChanged(int);
    void accuIdChanged(unsigned int);
    void batteryRemainingChanged(int);

    //Power status
    void VccChanged(unsigned int);
    void VservoChanged(unsigned int);
    void flagsChanged(unsigned int);

    //radio_status
    void rxerrorsChanged(unsigned int);
    void fixedChanged(unsigned int);
    void rssiChanged(unsigned int);
    void remrssiChanged(unsigned int);
    void txbufChanged(unsigned int);
    void noiseChanged(unsigned int);
    void remnoiseChanged(unsigned int);

    //User Generated
    void armedStateChanged(bool);

    // Vibration
    void vibrationXChanged(double);
    void vibrationYChanged(double);
    void vibrationZChanged(double);

    void clipping0Changed(double);
    void clipping1Changed(double);
    void clipping2Changed(double);

    // EKF Status Report
    void ekfFlagsChanged(unsigned int);
    void ekfVelocityVarianceChanged(float);
    void ekfPosHorizVarianceChanged(float);
    void ekfPosVertVarianceChanged(float);
    void ekfCompassVarianceChanged(float);
    void ekfTerrainAltVarianceChanged(float);

    void valueChanged(const int uasId, const QString& name, const QString& unit, const QVariant& value, const quint64 msec);
public slots:
    void messageReceived(LinkInterface* link,mavlink_message_t message);

private:
    quint64 lastHeartbeat;
    unsigned int m_uasId;
    unsigned int m_mavType;
    void parseHeartbeat(LinkInterface *link, const mavlink_message_t &message, const mavlink_heartbeat_t &state);
    void parseBattery(LinkInterface *link, const mavlink_battery_status_t &state);
    void parseSysStatus(LinkInterface *link, const mavlink_sys_status_t &state);
    void parseVibration(LinkInterface *link, const mavlink_vibration_t &vibration);
    void parseEkfStatusReport(LinkInterface *link, const mavlink_ekf_status_report_t &report);
};

#endif // VEHICLEOVERVIEW_H
