cmake_minimum_required(VERSION 3.14)
project(apmplanner2 LANGUAGES CXX)

# C++ Standard & Qt features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt5 components
find_package(Qt5 REQUIRED COMPONENTS
    Widgets Network OpenGL Svg Xml Sql SerialPort Quick PrintSupport Qml QuickWidgets Test DataVisualization
)

# SDL2 (handled per platform below)
if(UNIX) # Install for Apple and Linux
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
endif()

# MAVLink include
set(MAVLINKPATH "${CMAKE_SOURCE_DIR}/libs/mavlink/include/mavlink/v2.0")
set(MAVLINK_CONF "ardupilotmega")
if(NOT EXISTS "${MAVLINKPATH}/${MAVLINK_CONF}")
    message(
        FATAL_ERROR "MAVLink dialect '${MAVLINK_CONF}' does not exist at '${MAVLINKPATH}'! Run 'git submodule update --init' to fetch it."
    )
endif()

# AGLLIB math library & OPMapControl
add_subdirectory(libs/alglib EXCLUDE_FROM_ALL)
add_subdirectory(libs/opmapcontrol EXCLUDE_FROM_ALL)

# External: quazip (can be via add_subdirectory or find_package if installed)
add_subdirectory(libs/thirdParty/quazip EXCLUDE_FROM_ALL)


# Gather sources/headers/forms/resources
set(SOURCES
    qgroundcontrol.qrc
    src/GAudioOutput.cc
    src/GAudioOutput.h
    src/LogCompressor.cc
    src/LogCompressor.h
    src/MG.h
    src/QGC.cc
    src/QGC.h
    src/QGCCore.cc
    src/QGCCore.h
    src/QGCGeo.cc
    src/QGCGeo.h
    src/Settings.cpp
    src/Settings.h
    src/Waypoint.cc
    src/Waypoint.h
    src/audio/AlsaAudio.cc
    src/audio/AlsaAudio.h
    src/comm/AbsPositionOverview.cc
    src/comm/AbsPositionOverview.h
    src/comm/LinkInterface.cpp
    src/comm/LinkInterface.h
    src/comm/LinkManager.cc
    src/comm/LinkManager.h
    src/comm/LinkManagerFactory.cpp
    src/comm/LinkManagerFactory.h
    src/comm/MAVLinkDecoder.cc
    src/comm/MAVLinkDecoder.h
    src/comm/MAVLinkProtocol.cc
    src/comm/MAVLinkProtocol.h
    src/comm/MAVLinkSimulationLink.cc
    src/comm/MAVLinkSimulationLink.h
    src/comm/MAVLinkSimulationMAV.cc
    src/comm/MAVLinkSimulationMAV.h
    src/comm/MAVLinkSimulationWaypointPlanner.cc
    src/comm/MAVLinkSimulationWaypointPlanner.h
    src/comm/MAVLinkSwarmSimulationLink.cc
    src/comm/MAVLinkSwarmSimulationLink.h
    src/comm/MissionOverview.cc
    src/comm/MissionOverview.h
    src/comm/ProtocolInterface.h
    src/comm/QGCFlightGearLink.cc
    src/comm/QGCFlightGearLink.h
    src/comm/QGCHilLink.h
    src/comm/QGCJSBSimLink.cc
    src/comm/QGCJSBSimLink.h
    src/comm/QGCMAVLink.h
    src/comm/RelPositionOverview.cc
    src/comm/RelPositionOverview.h
    src/comm/SerialLinkInterface.h
    src/comm/TCPLink.cc
    src/comm/TCPLink.h
    src/comm/TLogReplayLink.cc
    src/comm/TLogReplayLink.h
    src/comm/UASObject.cc
    src/comm/UASObject.h
    src/comm/UDPClientLink.cc
    src/comm/UDPClientLink.h
    src/comm/UDPLink.cc
    src/comm/UDPLink.h
    src/comm/VehicleOverview.cc
    src/comm/VehicleOverview.h
    src/comm/arduino_intelhex.cpp
    src/comm/arduino_intelhex.h
    src/comm/arduinoflash.cc
    src/comm/arduinoflash.h
    src/comm/serialconnection.cc
    src/comm/serialconnection.h
    src/configuration.h
    src/globalobject.cc
    src/globalobject.h
    src/input/JoystickInput.cc
    src/input/JoystickInput.h
    src/logging.h
    src/main.cc
    src/output/kmlcreator.cc
    src/output/kmlcreator.h
    src/output/logdata.cc
    src/output/logdata.h
    src/uas/APMFirmwareVersion.cpp
    src/uas/APMFirmwareVersion.h
    src/uas/ApmUiHelpers.cc
    src/uas/ApmUiHelpers.h
    src/uas/ArduPilotMegaMAV.cc
    src/uas/ArduPilotMegaMAV.h
    src/uas/LogDownloadDialog.cc
    src/uas/LogDownloadDialog.h
    src/uas/LogDownloadDialog.ui
    src/uas/PxQuadMAV.cc
    src/uas/PxQuadMAV.h
    src/uas/QGCMAVLinkUASFactory.cc
    src/uas/QGCMAVLinkUASFactory.h
    src/uas/QGCUASParamManager.cc
    src/uas/QGCUASParamManager.h
    src/uas/SlugsMAV.cc
    src/uas/SlugsMAV.h
    src/uas/UAS.cc
    src/uas/UAS.h
    src/uas/UASInterface.h
    src/uas/UASManager.cc
    src/uas/UASManager.h
    src/uas/UASParameter.cpp
    src/uas/UASParameter.h
    src/uas/UASWaypointManager.cc
    src/uas/UASWaypointManager.h
    src/uas/senseSoarMAV.cpp
    src/uas/senseSoarMAV.h
    src/ui/AP2DataPlot2D.cpp
    src/ui/AP2DataPlot2D.h
    src/ui/AP2DataPlot2D.ui
    src/ui/AP2DataPlotAxisDialog.cc
    src/ui/AP2DataPlotAxisDialog.h
    src/ui/AP2DataPlotAxisDialog.ui
    src/ui/AP2DataPlotThread.cc
    src/ui/AP2DataPlotThread.h
    src/ui/AboutDialog.cc
    src/ui/AboutDialog.h
    src/ui/AboutDialog.ui
    src/ui/ApmToolBar.cc
    src/ui/ApmToolBar.h
    src/ui/AudioOutputWidget.cc
    src/ui/AudioOutputWidget.h
    src/ui/AudioOutputWidget.ui
    src/ui/AutoUpdateCheck.cc
    src/ui/AutoUpdateCheck.h
    src/ui/AutoUpdateDialog.cc
    src/ui/AutoUpdateDialog.h
    src/ui/AutoUpdateDialog.ui
    src/ui/CommConfigurationWindow.cc
    src/ui/CommConfigurationWindow.h
    src/ui/CommSettings.ui
    src/ui/DebugOutput.cc
    src/ui/DebugOutput.h
    src/ui/DebugOutput.ui
    src/ui/EKFMonitor.cpp
    src/ui/EKFMonitor.h
    src/ui/GoogleElevationData.cpp
    src/ui/GoogleElevationData.h
    src/ui/GraphTreeWidgetItem.cc
    src/ui/GraphTreeWidgetItem.h
    src/ui/HDDisplay.cc
    src/ui/HDDisplay.h
    src/ui/HDDisplay.ui
    src/ui/HSIDisplay.cc
    src/ui/HSIDisplay.h
    src/ui/HUD.cc
    src/ui/HUD.h
    src/ui/JoystickWidget.cc
    src/ui/JoystickWidget.h
    src/ui/JoystickWidget.ui
    src/ui/Loghandling/AP2DataPlotStatus.cpp
    src/ui/Loghandling/AP2DataPlotStatus.h
    src/ui/Loghandling/AsciiLogParser.cpp
    src/ui/Loghandling/AsciiLogParser.h
    src/ui/Loghandling/BinLogParser.cpp
    src/ui/Loghandling/BinLogParser.h
    src/ui/Loghandling/ILogParser.h
    src/ui/Loghandling/IParserCallback.h
    src/ui/Loghandling/LogAnalysis.cpp
    src/ui/Loghandling/LogAnalysis.h
    src/ui/Loghandling/LogAnalysis.ui
    src/ui/Loghandling/LogAnalysisMap.cpp
    src/ui/Loghandling/LogAnalysisMap.h
    src/ui/Loghandling/LogAnalysisMap.ui
    src/ui/Loghandling/LogExporter.cpp
    src/ui/Loghandling/LogExporter.h
    src/ui/Loghandling/LogParserBase.cpp
    src/ui/Loghandling/LogParserBase.h
    src/ui/Loghandling/LogdataStorage.cpp
    src/ui/Loghandling/LogdataStorage.h
    src/ui/Loghandling/PresetManager.cpp
    src/ui/Loghandling/PresetManager.h
    src/ui/Loghandling/TlogParser.cpp
    src/ui/Loghandling/TlogParser.h
    src/ui/MAVLinkSettingsWidget.cc
    src/ui/MAVLinkSettingsWidget.h
    src/ui/MAVLinkSettingsWidget.ui
    src/ui/MainWindow.cc
    src/ui/MainWindow.h
    src/ui/MainWindow.ui
    src/ui/MissionElevationDisplay.cpp
    src/ui/MissionElevationDisplay.h
    src/ui/MissionElevationDisplay.ui
    src/ui/ObjectDetectionView.cc
    src/ui/ObjectDetectionView.h
    src/ui/ObjectDetectionView.ui
    src/ui/ParameterInterface.cc
    src/ui/ParameterInterface.h
    src/ui/ParameterInterface.ui
    src/ui/PrimaryFlightDisplay.cc
    src/ui/PrimaryFlightDisplay.h
    src/ui/PrimaryFlightDisplayQML.cpp
    src/ui/PrimaryFlightDisplayQML.h
    src/ui/QGCHilConfiguration.cc
    src/ui/QGCHilConfiguration.h
    src/ui/QGCHilConfiguration.ui
    src/ui/QGCHilFlightGearConfiguration.cc
    src/ui/QGCHilFlightGearConfiguration.h
    src/ui/QGCHilFlightGearConfiguration.ui
    src/ui/QGCHilJSBSimConfiguration.cc
    src/ui/QGCHilJSBSimConfiguration.h
    src/ui/QGCHilJSBSimConfiguration.ui
    src/ui/QGCMAVLinkInspector.cc
    src/ui/QGCMAVLinkInspector.h
    src/ui/QGCMAVLinkInspector.ui
    src/ui/QGCMAVLinkLogPlayer.cc
    src/ui/QGCMAVLinkLogPlayer.h
    src/ui/QGCMAVLinkLogPlayer.ui
    src/ui/QGCMainWindowAPConfigurator.cc
    src/ui/QGCMainWindowAPConfigurator.h
    src/ui/QGCParamWidget.cc
    src/ui/QGCParamWidget.h
    src/ui/QGCPluginHost.cc
    src/ui/QGCPluginHost.h
    src/ui/QGCPluginHost.ui
    src/ui/QGCRGBDView.cc
    src/ui/QGCRGBDView.h
    src/ui/QGCSensorSettingsWidget.cc
    src/ui/QGCSensorSettingsWidget.h
    src/ui/QGCSensorSettingsWidget.ui
    src/ui/QGCSettingsWidget.cc
    src/ui/QGCSettingsWidget.h
    src/ui/QGCSettingsWidget.ui
    src/ui/QGCStatusBar.cc
    src/ui/QGCStatusBar.h
    src/ui/QGCTCPLinkConfiguration.cc
    src/ui/QGCTCPLinkConfiguration.h
    src/ui/QGCTCPLinkConfiguration.ui
    src/ui/QGCTabbedInfoView.cpp
    src/ui/QGCTabbedInfoView.h
    src/ui/QGCTabbedInfoView.ui
    src/ui/QGCToolBar.cc
    src/ui/QGCToolBar.h
    src/ui/QGCUDPClientLinkConfiguration.cc
    src/ui/QGCUDPClientLinkConfiguration.h
    src/ui/QGCUDPClientLinkConfiguration.ui
    src/ui/QGCUDPLinkConfiguration.cc
    src/ui/QGCUDPLinkConfiguration.h
    src/ui/QGCUDPLinkConfiguration.ui
    src/ui/QGCVehicleConfig.cc
    src/ui/QGCVehicleConfig.h
    src/ui/QGCVehicleConfig.ui
    src/ui/QGCWaypointListMulti.cc
    src/ui/QGCWaypointListMulti.h
    src/ui/QGCWaypointListMulti.ui
    src/ui/RadioCalibration/RadioCalibrationData.cc
    src/ui/RadioCalibration/RadioCalibrationData.h
    src/ui/SerialConfigurationWindow.cc
    src/ui/SerialConfigurationWindow.h
    src/ui/SerialSettings.ui
    src/ui/SlugsDataSensorView.cc
    src/ui/SlugsDataSensorView.h
    src/ui/SlugsDataSensorView.ui
    src/ui/SlugsHilSim.cc
    src/ui/SlugsHilSim.h
    src/ui/SlugsHilSim.ui
    src/ui/SlugsPadCameraControl.cpp
    src/ui/SlugsPadCameraControl.h
    src/ui/SlugsPadCameraControl.ui
    src/ui/UASRawStatusView.cpp
    src/ui/UASRawStatusView.h
    src/ui/UASRawStatusView.ui
    src/ui/UnconnectedUASInfoWidget.cc
    src/ui/UnconnectedUASInfoWidget.h
    src/ui/UnconnectedUASInfoWidget.ui
    src/ui/VibrationMonitor.cpp
    src/ui/VibrationMonitor.h
    src/ui/WaypointEditableView.cc
    src/ui/WaypointEditableView.h
    src/ui/WaypointEditableView.ui
    src/ui/WaypointList.cc
    src/ui/WaypointList.h
    src/ui/WaypointList.ui
    src/ui/WaypointNavigation.cc
    src/ui/WaypointNavigation.h
    src/ui/WaypointViewOnlyView.cc
    src/ui/WaypointViewOnlyView.h
    src/ui/WaypointViewOnlyView.ui
    src/ui/configuration/AP2ConfigWidget.cc
    src/ui/configuration/AP2ConfigWidget.h
    src/ui/configuration/APDoubleSpinBox.cc
    src/ui/configuration/APDoubleSpinBox.h
    src/ui/configuration/APSpinBox.cc
    src/ui/configuration/APSpinBox.h
    src/ui/configuration/AccelCalibrationConfig.cc
    src/ui/configuration/AccelCalibrationConfig.h
    src/ui/configuration/AccelCalibrationConfig.ui
    src/ui/configuration/AdvParameterList.cc
    src/ui/configuration/AdvParameterList.h
    src/ui/configuration/AdvParameterList.ui
    src/ui/configuration/AdvancedParamConfig.cc
    src/ui/configuration/AdvancedParamConfig.h
    src/ui/configuration/AdvancedParamConfig.ui
    src/ui/configuration/AirspeedConfig.cc
    src/ui/configuration/AirspeedConfig.h
    src/ui/configuration/AirspeedConfig.ui
    src/ui/configuration/AntennaTrackerConfig.cc
    src/ui/configuration/AntennaTrackerConfig.h
    src/ui/configuration/AntennaTrackerConfig.ui
    src/ui/configuration/ApmCustomFirmwareConfig.cpp
    src/ui/configuration/ApmCustomFirmwareConfig.h
    src/ui/configuration/ApmHardwareConfig.cc
    src/ui/configuration/ApmHardwareConfig.h
    src/ui/configuration/ApmHardwareConfig.ui
    src/ui/configuration/ApmHighlighter.cc
    src/ui/configuration/ApmHighlighter.h
    src/ui/configuration/ApmPlaneLevel.cc
    src/ui/configuration/ApmPlaneLevel.h
    src/ui/configuration/ApmPlaneLevel.ui
    src/ui/configuration/ApmSoftwareConfig.cc
    src/ui/configuration/ApmSoftwareConfig.h
    src/ui/configuration/ApmSoftwareConfig.ui
    src/ui/configuration/ArduCopterPidConfig.cc
    src/ui/configuration/ArduCopterPidConfig.h
    src/ui/configuration/ArduCopterPidConfig.ui
    src/ui/configuration/ArduPlanePidConfig.cc
    src/ui/configuration/ArduPlanePidConfig.h
    src/ui/configuration/ArduPlanePidConfig.ui
    src/ui/configuration/ArduRoverPidConfig.cc
    src/ui/configuration/ArduRoverPidConfig.h
    src/ui/configuration/ArduRoverPidConfig.ui
    src/ui/configuration/BasicPidConfig.cc
    src/ui/configuration/BasicPidConfig.h
    src/ui/configuration/BasicPidConfig.ui
    src/ui/configuration/BatteryMonitorConfig.cc
    src/ui/configuration/BatteryMonitorConfig.h
    src/ui/configuration/BatteryMonitorConfig.ui
    src/ui/configuration/CameraGimbalConfig.cc
    src/ui/configuration/CameraGimbalConfig.h
    src/ui/configuration/CameraGimbalConfig.ui
    src/ui/configuration/CompassConfig.cc
    src/ui/configuration/CompassConfig.h
    src/ui/configuration/CompassConfig.ui
    src/ui/configuration/CompassMotorCalibrationDialog.cpp
    src/ui/configuration/CompassMotorCalibrationDialog.h
    src/ui/configuration/CompassMotorCalibrationDialog.ui
    src/ui/configuration/CompassOnboardCalibrationDialog.cpp
    src/ui/configuration/CompassOnboardCalibrationDialog.h
    src/ui/configuration/CompassOnboardCalibrationDialog.ui
    src/ui/configuration/Console.cc
    src/ui/configuration/Console.h
    src/ui/configuration/CopterPidConfig.cc
    src/ui/configuration/CopterPidConfig.h
    src/ui/configuration/CopterPidConfig.ui
    src/ui/configuration/DownloadRemoteParamsDialog.cc
    src/ui/configuration/DownloadRemoteParamsDialog.h
    src/ui/configuration/DownloadRemoteParamsDialog.ui
    src/ui/configuration/FailSafeConfig.cc
    src/ui/configuration/FailSafeConfig.h
    src/ui/configuration/FailSafeConfig.ui
    src/ui/configuration/FlightModeConfig.cc
    src/ui/configuration/FlightModeConfig.h
    src/ui/configuration/FlightModeConfig.ui
    src/ui/configuration/FrameTypeConfig.cc
    src/ui/configuration/FrameTypeConfig.h
    src/ui/configuration/FrameTypeConfig.ui
    src/ui/configuration/FrameTypeConfigNew.ui
    src/ui/configuration/FrameTypeConfigOld.ui
    src/ui/configuration/GeoFenceConfig.cc
    src/ui/configuration/GeoFenceConfig.h
    src/ui/configuration/GeoFenceConfig.ui
    src/ui/configuration/LogConsole.cc
    src/ui/configuration/LogConsole.h
    src/ui/configuration/LogConsole.ui
    src/ui/configuration/OpticalFlowConfig.cc
    src/ui/configuration/OpticalFlowConfig.h
    src/ui/configuration/OpticalFlowConfig.ui
    src/ui/configuration/OsdConfig.cc
    src/ui/configuration/OsdConfig.h
    src/ui/configuration/OsdConfig.ui
    src/ui/configuration/PX4FirmwareUploader.cc
    src/ui/configuration/PX4FirmwareUploader.h
    src/ui/configuration/ParamCompareDialog.cpp
    src/ui/configuration/ParamCompareDialog.h
    src/ui/configuration/ParamCompareDialog.ui
    src/ui/configuration/ParamWidget.cc
    src/ui/configuration/ParamWidget.h
    src/ui/configuration/ParamWidget.ui
    src/ui/configuration/Radio3DRConfig.cc
    src/ui/configuration/Radio3DRConfig.h
    src/ui/configuration/Radio3DRConfig.ui
    src/ui/configuration/Radio3DRSettings.cc
    src/ui/configuration/Radio3DRSettings.h
    src/ui/configuration/RadioCalibrationConfig.cc
    src/ui/configuration/RadioCalibrationConfig.h
    src/ui/configuration/RadioCalibrationConfig.ui
    src/ui/configuration/RadioFlashWizard.cpp
    src/ui/configuration/RadioFlashWizard.h
    src/ui/configuration/RadioFlashWizard.ui
    src/ui/configuration/RangeFinderConfig.cc
    src/ui/configuration/RangeFinderConfig.h
    src/ui/configuration/RangeFinderConfig.ui
    src/ui/configuration/SerialSettingsDialog.cc
    src/ui/configuration/SerialSettingsDialog.h
    src/ui/configuration/SerialSettingsDialog.ui
    src/ui/configuration/StandardParamConfig.cc
    src/ui/configuration/StandardParamConfig.h
    src/ui/configuration/StandardParamConfig.ui
    src/ui/configuration/TerminalConsole.cc
    src/ui/configuration/TerminalConsole.h
    src/ui/configuration/TerminalConsole.ui
    src/ui/configuration/apmcustomfirmwareconfig.ui
    src/ui/dataselectionscreen.cpp
    src/ui/dataselectionscreen.h
    src/ui/dataselectionscreen.ui
    src/ui/designer/QGCActionButton.ui
    src/ui/designer/QGCComboBox.cc
    src/ui/designer/QGCComboBox.h
    src/ui/designer/QGCComboBox.ui
    src/ui/designer/QGCCommandButton.cc
    src/ui/designer/QGCCommandButton.h
    src/ui/designer/QGCCommandButton.ui
    src/ui/designer/QGCMouseWheelEventFilter.cc
    src/ui/designer/QGCMouseWheelEventFilter.h
    src/ui/designer/QGCParamSlider.cc
    src/ui/designer/QGCParamSlider.h
    src/ui/designer/QGCParamSlider.ui
    src/ui/designer/QGCRadioChannelDisplay.cpp
    src/ui/designer/QGCRadioChannelDisplay.h
    src/ui/designer/QGCTextLabel.cc
    src/ui/designer/QGCTextLabel.h
    src/ui/designer/QGCTextLabel.ui
    src/ui/designer/QGCToolWidget.cc
    src/ui/designer/QGCToolWidget.h
    src/ui/designer/QGCToolWidget.ui
    src/ui/designer/QGCToolWidgetItem.cc
    src/ui/designer/QGCToolWidgetItem.h
    src/ui/dockwidgettitlebareventfilter.cpp
    src/ui/dockwidgettitlebareventfilter.h
    src/ui/map/MAV2DIcon.cc
    src/ui/map/MAV2DIcon.h
    src/ui/map/QGCMapTool.cc
    src/ui/map/QGCMapTool.h
    src/ui/map/QGCMapTool.ui
    src/ui/map/QGCMapToolBar.cc
    src/ui/map/QGCMapToolBar.h
    src/ui/map/QGCMapToolBar.ui
    src/ui/map/QGCMapWidget.cc
    src/ui/map/QGCMapWidget.h
    src/ui/map/Waypoint2DIcon.cc
    src/ui/map/Waypoint2DIcon.h
    src/ui/mission/QGCMissionConditionDelay.cc
    src/ui/mission/QGCMissionConditionDelay.h
    src/ui/mission/QGCMissionConditionDelay.ui
    src/ui/mission/QGCMissionConditionDistance.cc
    src/ui/mission/QGCMissionConditionDistance.h
    src/ui/mission/QGCMissionConditionDistance.ui
    src/ui/mission/QGCMissionConditionYaw.cc
    src/ui/mission/QGCMissionConditionYaw.h
    src/ui/mission/QGCMissionConditionYaw.ui
    src/ui/mission/QGCMissionDoChangeSpeed.cc
    src/ui/mission/QGCMissionDoChangeSpeed.h
    src/ui/mission/QGCMissionDoChangeSpeed.ui
    src/ui/mission/QGCMissionDoDigicamControl.cc
    src/ui/mission/QGCMissionDoDigicamControl.h
    src/ui/mission/QGCMissionDoDigicamControl.ui
    src/ui/mission/QGCMissionDoFinishSearch.cc
    src/ui/mission/QGCMissionDoFinishSearch.h
    src/ui/mission/QGCMissionDoFinishSearch.ui
    src/ui/mission/QGCMissionDoJump.cc
    src/ui/mission/QGCMissionDoJump.h
    src/ui/mission/QGCMissionDoJump.ui
    src/ui/mission/QGCMissionDoMountControl.cc
    src/ui/mission/QGCMissionDoMountControl.h
    src/ui/mission/QGCMissionDoMountControl.ui
    src/ui/mission/QGCMissionDoRepeatRelay.cc
    src/ui/mission/QGCMissionDoRepeatRelay.h
    src/ui/mission/QGCMissionDoRepeatRelay.ui
    src/ui/mission/QGCMissionDoRepeatServo.cc
    src/ui/mission/QGCMissionDoRepeatServo.h
    src/ui/mission/QGCMissionDoRepeatServo.ui
    src/ui/mission/QGCMissionDoSetCamTriggDist.cc
    src/ui/mission/QGCMissionDoSetCamTriggDist.h
    src/ui/mission/QGCMissionDoSetCamTriggDist.ui
    src/ui/mission/QGCMissionDoSetHome.cc
    src/ui/mission/QGCMissionDoSetHome.h
    src/ui/mission/QGCMissionDoSetHome.ui
    src/ui/mission/QGCMissionDoSetROI.cc
    src/ui/mission/QGCMissionDoSetROI.h
    src/ui/mission/QGCMissionDoSetROI.ui
    src/ui/mission/QGCMissionDoSetRelay.cc
    src/ui/mission/QGCMissionDoSetRelay.h
    src/ui/mission/QGCMissionDoSetRelay.ui
    src/ui/mission/QGCMissionDoSetReverse.cc
    src/ui/mission/QGCMissionDoSetReverse.h
    src/ui/mission/QGCMissionDoSetReverse.ui
    src/ui/mission/QGCMissionDoSetServo.cc
    src/ui/mission/QGCMissionDoSetServo.h
    src/ui/mission/QGCMissionDoSetServo.ui
    src/ui/mission/QGCMissionDoStartSearch.cc
    src/ui/mission/QGCMissionDoStartSearch.h
    src/ui/mission/QGCMissionDoStartSearch.ui
    src/ui/mission/QGCMissionNavContinueChangeAlt.cc
    src/ui/mission/QGCMissionNavContinueChangeAlt.h
    src/ui/mission/QGCMissionNavContinueChangeAlt.ui
    src/ui/mission/QGCMissionNavLand.cc
    src/ui/mission/QGCMissionNavLand.h
    src/ui/mission/QGCMissionNavLand.ui
    src/ui/mission/QGCMissionNavLoiterTime.cc
    src/ui/mission/QGCMissionNavLoiterTime.h
    src/ui/mission/QGCMissionNavLoiterTime.ui
    src/ui/mission/QGCMissionNavLoiterToAlt.cc
    src/ui/mission/QGCMissionNavLoiterToAlt.h
    src/ui/mission/QGCMissionNavLoiterToAlt.ui
    src/ui/mission/QGCMissionNavLoiterTurns.cc
    src/ui/mission/QGCMissionNavLoiterTurns.h
    src/ui/mission/QGCMissionNavLoiterTurns.ui
    src/ui/mission/QGCMissionNavLoiterUnlim.cc
    src/ui/mission/QGCMissionNavLoiterUnlim.h
    src/ui/mission/QGCMissionNavLoiterUnlim.ui
    src/ui/mission/QGCMissionNavReturnToLaunch.cc
    src/ui/mission/QGCMissionNavReturnToLaunch.h
    src/ui/mission/QGCMissionNavReturnToLaunch.ui
    src/ui/mission/QGCMissionNavSweep.cc
    src/ui/mission/QGCMissionNavSweep.h
    src/ui/mission/QGCMissionNavSweep.ui
    src/ui/mission/QGCMissionNavTakeoff.cc
    src/ui/mission/QGCMissionNavTakeoff.h
    src/ui/mission/QGCMissionNavTakeoff.ui
    src/ui/mission/QGCMissionNavWaypoint.cc
    src/ui/mission/QGCMissionNavWaypoint.h
    src/ui/mission/QGCMissionNavWaypoint.ui
    src/ui/mission/QGCMissionOther.cc
    src/ui/mission/QGCMissionOther.h
    src/ui/mission/QGCMissionOther.ui
    src/ui/qcustomplot.cpp
    src/ui/qcustomplot.h
    src/ui/submainwindow.cpp
    src/ui/submainwindow.h
    src/ui/uas/APMShortcutModesDialog.cpp
    src/ui/uas/APMShortcutModesDialog.h
    src/ui/uas/APMShortcutModesDialog.ui
    src/ui/uas/PreFlightCalibrationDialog.cpp
    src/ui/uas/PreFlightCalibrationDialog.h
    src/ui/uas/PreFlightCalibrationDialog.ui
    src/ui/uas/QGCMessageView.cc
    src/ui/uas/QGCMessageView.h
    src/ui/uas/QGCMessageView.ui
    src/ui/uas/QGCUnconnectedInfoWidget.cc
    src/ui/uas/QGCUnconnectedInfoWidget.h
    src/ui/uas/QGCUnconnectedInfoWidget.ui
    src/ui/uas/UASActionsWidget.cpp
    src/ui/uas/UASActionsWidget.h
    src/ui/uas/UASActionsWidget.ui
    src/ui/uas/UASControl.ui
    src/ui/uas/UASControlWidget.cc
    src/ui/uas/UASControlWidget.h
    src/ui/uas/UASInfo.ui
    src/ui/uas/UASInfoWidget.cc
    src/ui/uas/UASInfoWidget.h
    src/ui/uas/UASList.ui
    src/ui/uas/UASListWidget.cc
    src/ui/uas/UASListWidget.h
    src/ui/uas/UASQuickView.cc
    src/ui/uas/UASQuickView.h
    src/ui/uas/UASQuickView.ui
    src/ui/uas/UASQuickViewGaugeItem.cc
    src/ui/uas/UASQuickViewGaugeItem.h
    src/ui/uas/UASQuickViewItem.cc
    src/ui/uas/UASQuickViewItem.h
    src/ui/uas/UASQuickViewItemSelect.cc
    src/ui/uas/UASQuickViewItemSelect.h
    src/ui/uas/UASQuickViewItemSelect.ui
    src/ui/uas/UASQuickViewTextItem.cc
    src/ui/uas/UASQuickViewTextItem.h
    src/ui/uas/UASQuickViewTextLabel.cc
    src/ui/uas/UASQuickViewTextLabel.h
    src/ui/uas/UASView.cc
    src/ui/uas/UASView.h
    src/ui/uas/UASView.ui
    src/ui/watchdog/WatchdogControl.cc
    src/ui/watchdog/WatchdogControl.h
    src/ui/watchdog/WatchdogControl.ui
    src/ui/watchdog/WatchdogProcessView.cc
    src/ui/watchdog/WatchdogProcessView.h
    src/ui/watchdog/WatchdogProcessView.ui
    src/ui/watchdog/WatchdogView.cc
    src/ui/watchdog/WatchdogView.h
    src/ui/watchdog/WatchdogView.ui
#    src/comm/QGCXPlaneLink.cc
#    src/comm/QGCXPlaneLink.h
#    src/ui/QGCHilXPlaneConfiguration.cc
#    src/ui/QGCHilXPlaneConfiguration.h
#    src/ui/QGCHilXPlaneConfiguration.ui
)

file(GLOB QML_FILES
    qml/*.qml
    qml/components/*.qml
    qml/resources/apmplanner/toolbar/*.png
    qml/VibrationMonitor.qml
    qml/EKFMonitor.qml
    qml/ApmToolBar.qml
)

# Platform-specific sources/defs/libs
if(WIN32)
    list(APPEND SOURCES
        # Add Windows-specific sources here
    )
    add_definitions(-D__STDC_LIMIT_MACROS)
    include_directories(libs/lib/msinttypes)
    set(ADDITIONAL_LIBS setupapi)
elseif(APPLE)
    list(APPEND SOURCES
        # Add Mac-specific sources here
    )
    add_definitions(-D__STDC_LIMIT_MACROS)
    set(ADDITIONAL_LIBS z)
    find_library(APPSERVICES ApplicationServices)
    find_package(Qt5Multimedia REQUIRED)
    list(APPEND ADDITIONAL_LIBS ${APPSERVICES} Qt5::Multimedia)
elseif(UNIX)
    add_definitions(-D__STDC_LIMIT_MACROS)
    set(ADDITIONAL_LIBS sndfile asound z ${ADDITIONAL_LIBS})
endif()

# Optional: Google Earth (add logic to enable/disable as needed)
option(ENABLE_GOOGLE_EARTH "Enable Google Earth support" OFF)
if(ENABLE_GOOGLE_EARTH)
    list(APPEND HEADERS src/ui/map3D/QGCGoogleEarthView.h)
    list(APPEND SOURCES src/ui/map3D/QGCGoogleEarthView.cc)
    if(WIN32)
        add_definitions(-DGOOGLEEARTH)
    endif()
endif()

# Optional: Flite (Festival Lite) voice synthesis
find_path(FLITE_INCLUDE_DIR flite/flite.h)
if(FLITE_INCLUDE_DIR)
    add_definitions(-DFLITE_AUDIO_ENABLED)
    set(FLITE_LIBS flite_cmu_us_kal flite_usenglish flite_cmulex flite)
    include_directories(${FLITE_INCLUDE_DIR})
else()
    message(STATUS "Flite not found, voice synthesis disabled")
endif()

# Optional: Magellan 3DxWare support (Linux/Mac/Windows)
option(ENABLE_3DMOUSE "Enable 3DConnexion 3D mouse support" OFF)
if(ENABLE_3DMOUSE)
    if(UNIX AND EXISTS "/usr/local/lib/libxdrvlib.so")
        add_definitions(-DMOUSE_ENABLED_LINUX -DParameterCheck)
        include_directories(/usr/local/include)
        list(APPEND HEADERS src/input/Mouse6dofInput.h)
        list(APPEND SOURCES src/input/Mouse6dofInput.cpp)
        list(APPEND ADDITIONAL_LIBS xdrvlib)
    elseif(WIN32)
        add_definitions(-DMOUSE_ENABLED_WIN)
        include_directories(libs/thirdParty/3DMouse/win)
        list(APPEND HEADERS
            libs/thirdParty/3DMouse/win/I3dMouseParams.h
            libs/thirdParty/3DMouse/win/MouseParameters.h
            libs/thirdParty/3DMouse/win/Mouse3DInput.h
            src/input/Mouse6dofInput.h
        )
        list(APPEND SOURCES
            libs/thirdParty/3DMouse/win/MouseParameters.cpp
            libs/thirdParty/3DMouse/win/Mouse3DInput.cpp
            src/input/Mouse6dofInput.cpp
        )
    endif()
endif()

option(ENABLE_CAMERAVIEW "Enable Camera View support" OFF)
if(ENABLE_CAMERAVIEW)
    message(STATUS "Including support for Camera View")
    set(CAMERA_VIEW_SOURCES src/ui/CameraView.cc)
    set(CAMERA_VIEW_HEADERS src/ui/CameraView.h)
    add_definitions(-DENABLE_CAMRAVIEW)
else()
    message(STATUS "Skipping support for Camera View")
    set(CAMERA_VIEW_SOURCES "")
    set(CAMERA_VIEW_HEADERS "")
endif()

# Main target
add_executable(apmplanner2
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
    ${QML_FILES}
    ${CAMERA_VIEW_SOURCES}
)

# Includes
target_include_directories(apmplanner2 PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include/ui
    ${MAVLINKPATH}
    ${MAVLINKPATH}/${MAVLINK_CONF}
    libs/utils libs libs/opmapcontrol
    ${SDL2_INCLUDE_DIR}
)
target_include_directories(apmplanner2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/Loghandling
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/linechart
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/uas
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/map
    ${CMAKE_CURRENT_SOURCE_DIR}/src/uas
    ${CMAKE_CURRENT_SOURCE_DIR}/src/comm
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/input
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/qmapcontrol
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/mavlink
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/param
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/watchdog
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/map3D
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/mission
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/designer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/configuration
    ${CMAKE_CURRENT_SOURCE_DIR}/src/output
)

# Link Qt and additional libs
target_link_libraries(apmplanner2
    Qt5::Widgets
    Qt5::Network
    Qt5::OpenGL
    Qt5::Svg
    Qt5::Xml
    Qt5::Sql
    Qt5::SerialPort
    Qt5::Quick
    Qt5::PrintSupport
    Qt5::Qml
    Qt5::QuickWidgets
    Qt5::Test
    Qt5::DataVisualization
    quazip
    alglib
    opmapcontrol
    ${ADDITIONAL_LIBS}
    ${FLITE_LIBS}
    ${SDL2_LIBRARIES}
)

# Definitions
target_compile_definitions(apmplanner2 PRIVATE
    DISABLE_3DMOUSE
    _TTY_NOWARN_
    $<$<CONFIG:Release>:QT_NO_DEBUG>
    QGC_USE_${MAVLINK_CONF}_MESSAGES
    NOMINMAX
)

# Platform-specific properties and post-build
if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE "icons.icns")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum OS X deployment version")

    set_source_files_properties(
        "${CMAKE_SOURCE_DIR}/files/APMIcons/icons.icns"
        PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )

    set_target_properties(apmplanner2 PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE ${MACOSX_BUNDLE_ICON_FILE}
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/files/macOS/Info.plist"
    )

    add_custom_command(TARGET apmplanner2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/qml"
            "$<TARGET_FILE_DIR:apmplanner2>/qml"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/sik_uploader"
            "$<TARGET_FILE_DIR:apmplanner2>/sik_uploader"
    )

    # Ensure .icns is included in the app bundle for debug builds
    add_custom_command(TARGET apmplanner2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/files/APMIcons/icon.icns"
            "$<TARGET_FILE_DIR:apmplanner2>/../Resources/icon.icns"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/files/macOS/Info.plist"
            "$<TARGET_FILE_DIR:apmplanner2>/../Info.plist"
    )

    install(FILES "${CMAKE_SOURCE_DIR}/files/APMIcons/icon.icns"
            DESTINATION "${CMAKE_INSTALL_PREFIX}/apmplanner2.app/Contents/Resources")

    configure_file(${CMAKE_SOURCE_DIR}/files/macOS/PkgInfo.in ${CMAKE_BINARY_DIR}/PkgInfo @ONLY)
    install(FILES ${CMAKE_BINARY_DIR}/PkgInfo
        DESTINATION ${CMAKE_INSTALL_PREFIX}/apmplanner2.app/Contents)


elseif(WIN32)
    add_custom_command(TARGET apmplanner2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/qml"
            "$<TARGET_FILE_DIR:apmplanner2>/qml"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/sik_uploader"
            "$<TARGET_FILE_DIR:apmplanner2>/sik_uploader"
        # Add DLLs if needed, e.g. SDL2.dll, libxbee.dll, etc.
    )
else() # Linux
    add_custom_command(TARGET apmplanner2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/qml"
            "$<TARGET_FILE_DIR:apmplanner2>/qml"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/sik_uploader"
            "$<TARGET_FILE_DIR:apmplanner2>/sik_uploader"
    )
endif()

# Install rules for Linux packaging
if(UNIX AND NOT APPLE)
    install(TARGETS apmplanner2 DESTINATION bin)
    install(FILES ${CMAKE_SOURCE_DIR}/sik_uploader/sik_uploader.py DESTINATION bin)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/files DESTINATION share/APMPlanner2)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/data DESTINATION share/APMPlanner2)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/qml DESTINATION share/APMPlanner2)
    install(FILES ${CMAKE_SOURCE_DIR}/common/apmplanner2.desktop DESTINATION share/applications)
endif()
