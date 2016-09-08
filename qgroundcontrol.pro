# -------------------------------------------------
# APM Planner - Micro Air Vehicle Groundstation
# Please see our website at <http://ardupilot.com>
# Maintainer:
# Bill Bonney <billbonney@communitech.com>
# Based of QGroundControl by Lorenz Meier <lm@inf.ethz.ch>
#
# (c) 2009-2011 QGroundControl Developers
# (c) 2013 Bill Bonney <billbonney@communistech.com>
# (c) 2013 Michael Carpenter <malcom2073@gmail.com>
#
# This file is part of the open groundstation project
# APM Planner is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# APM Planner is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with QGroundControl. If not, see <http://www.gnu.org/licenses/>.
# -------------------------------------------------


message(Qt version $$[QT_VERSION])

# Setup our supported build types. We do this once here and then use the defined config scopes
# to allow us to easily modify suported build types in one place instead of duplicated throughout
# the project file.

DEFINES+=DISABLE_3DMOUSE    # Disable 3D mice support for now
#DEFINES+=ENABLE_CAMRAVIEW   # Example to include camraview

linux-g++-64 {
    message(Linux build x64_86)
    CONFIG += LinuxBuild
    DEFINES += Q_LINUX_64
    DEFINES += FLITE_AUDIO_ENABLED

    DISTRO = $$system(lsb_release -i)

    contains( DISTRO, "Ubuntu" ) {
        message(Ubuntu Build)
        DEFINES += Q_UBUNTU
    }

    contains( DISTRO, "Arch" ) {
        message(ArchLinux Build)
        DEFINES += Q_ARCHLINUX
    }

} else: linux-g++ {
    message(Linux build x86)
    CONFIG += LinuxBuild
    DEFINES += Q_LINUX_32
    DEFINES += FLITE_AUDIO_ENABLED

    DISTRO = $$system(lsb_release -i)

    contains( DISTRO, "Ubuntu" ) {
        message(Ubuntu Build)
        DEFINES += Q_UBUNTU
    }

    contains( DISTRO, "Arch" ) {
        message(ArchLinux Build)
        DEFINES += Q_ARCHLINUX
    }

} else : win32-msvc2012 | win32-msvc2013 {
    message(Windows build)
    CONFIG += WindowsBuild
}  else : win32-g++|win64-g++ {
    message(Windows Cross Build)
    CONFIG += WindowsCrossBuild
} else : macx-clang | macx-g++ {
    message(Mac build)
    CONFIG += MacBuild
} else {
    error(Unsupported build type)
}

# Setup our supported build flavors

CONFIG(debug, debug|release) {
    message(Debug flavor)
    CONFIG += DebugBuild
} else:CONFIG(release, debug|release) {
    message(Release flavor)
    CONFIG += ReleaseBuild
} else {
    error(Unsupported build flavor)
}

# Setup our build directories

BASEDIR = $${IN_PWD}
DebugBuild {
    DESTDIR = $${OUT_PWD}/debug
    BUILDDIR = $${OUT_PWD}/build-debug
}
ReleaseBuild {
    DESTDIR = $${OUT_PWD}/release
    BUILDDIR = $${OUT_PWD}/build-release
}
OBJECTS_DIR = $${BUILDDIR}/obj
MOC_DIR = $${BUILDDIR}/moc
UI_DIR = $${BUILDDIR}/ui
RCC_DIR = $${BUILDDIR}/rcc
LANGUAGE = C++

TEMPLATE = app
TARGET = apmplanner2

message(BASEDIR $$BASEDIR DESTDIR $$DESTDIR TARGET $$TARGET)

# Qt configuration
CONFIG += qt \
    thread

QT += network \
    opengl \
    svg \
    xml \
    sql \
    widgets \
    serialport \
    script\
    quick \
    printsupport \
    qml \
    quickwidgets

##  testlib is needed even in release flavor for QSignalSpy support
QT += testlib

#Not sure what we were doing here, will have to ask
#!NOTOUCH {
#    gittouch.commands = touch qgroundcontrol.pro
#    QMAKE_EXTRA_TARGETS += gittouch
#    POST_TARGETDEPS += gittouch
#}

# Turn off serial port warnings
DEFINES += _TTY_NOWARN_

#Turn on camera view
#DEFINES += AMERAVIEW

#include (libs/mavlink/include/mavlink/v1.0-qt/mavlink.pri)
#
# OS Specific settings
#

RaspberryPiBuild {
   DEFINES -= CAMERAVIEW
}

MacBuild {
    QT += multimedia

    QMAKE_INFO_PLIST = Custom-Info.plist
    CONFIG += x86_64
    CONFIG -= x86
    CONFIG += c++11 #C++11 support
    QMAKE_MAC_SDK = macosx10.11 # Required for Xcode7.0
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    ICON = $$BASEDIR/files/APMIcons/icon.icns
    QMAKE_INFO_PLIST = APMPlanner.plist   # Sets the pretty name for the build

    DEFINES += GIT_COMMIT=$$system(git describe --dirty=-DEV --always)
    DEFINES += GIT_HASH=$$system(git log -n 1 --pretty=format:%H)

    LIBS += -lz
    LIBS += -lssl -lcrypto
    LIBS += -framework ApplicationServices
}

LinuxBuild {
    DEFINES += __STDC_LIMIT_MACROS

    DEFINES += GIT_COMMIT=$$system(git describe --dirty=-DEV --always)
    DEFINES += GIT_HASH=$$system(git log -n 1 --pretty=format:%H)

    LIBS += -lsndfile -lasound
    LIBS += -lz
    LIBS += -lssl -lcrypto
    DEFINES += OPENSSL
}

WindowsBuild {
    DEFINES += __STDC_LIMIT_MACROS

    # Specify multi-process compilation within Visual Studio.
    # (drastically improves compilation times for multi-core computers)
    QMAKE_CXXFLAGS_DEBUG += -MP
    QMAKE_CXXFLAGS_RELEASE += -MP

    # QWebkit is not needed on MS-Windows compilation environment
    CONFIG -= webkit

    RC_FILE = $$BASEDIR/qgroundcontrol.rc

    DEFINES += GIT_COMMIT=$$system(git describe --dirty=-DEV --always)
    DEFINES += GIT_HASH=$$system(git log -n 1 --pretty=format:%H)
}

WindowsCrossBuild {
    QT += script
    # Windows version cross compiled on linux using
    DEFINES += __STDC_LIMIT_MACROS

    # QWebkit is not needed on MS-Windows compilation environment
    CONFIG -= webkit

    RC_FILE = $$BASEDIR/qgroundcontrol.rc
    LIBS += -lz
    CONFIG += exceptions rtti

    DEFINES += GIT_COMMIT=$$system(git describe --dirty=-DEV --always)
    DEFINES += GIT_HASH=$$system(git log -n 1 --pretty=format:%H)
}

#
# Build flavor specific settings
#

DebugBuild {
#Let console be defined by the IDE/Build process.
    #CONFIG += console
}

ReleaseBuild {
    DEFINES += QT_NO_DEBUG

    WindowsBuild {
        # Use link time code generation for beteer optimization (I believe this is supported in msvc express, but not 100% sure)
        QMAKE_LFLAGS_LTCG = /LTCG
        QMAKE_CFLAGS_LTCG = -GL
    }
}

#
# Unit Test specific configuration goes here (debug only)
#

#DebugBuild {
#    INCLUDEPATH += \
#        src/qgcunittest

#    HEADERS += \
#        src/qgcunittest/AutoTest.h \
#        src/qgcunittest/UASUnitTest.h \
#        src/qgcunittest/MockUASManager.h \
#        src/qgcunittest/MockUAS.h \
#        src/qgcunittest/MockQGCUASParamManager.h \
#        src/qgcunittest/MultiSignalSpy.h \
#        src/qgcunittest/TCPLinkTest.h \
#        src/qgcunittest/FlightModeConfigTest.h

#    SOURCES += \
#        src/qgcunittest/UASUnitTest.cc \
#        src/qgcunittest/MockUASManager.cc \
#        src/qgcunittest/MockUAS.cc \
#        src/qgcunittest/MockQGCUASParamManager.cc \
#        src/qgcunittest/MultiSignalSpy.cc \
#        src/qgcunittest/TCPLinkTest.cc \
#        src/qgcunittest/FlightModeConfigTest.cc
#}

#
# External library configuration
#

include(QGCExternalLibs.pri)

#
# Post link configuration
#

include(QGCSetup.pri)

#
# Main QGroundControl portion of project file
#

RESOURCES += qgroundcontrol.qrc

TRANSLATIONS += \
    es-MX.ts \
    en-US.ts

DEPENDPATH += \
    . \
    plugins

INCLUDEPATH += .

INCLUDEPATH += \
    src \
    src/ui \
    src/ui/linechart \
    src/ui/uas \
    src/ui/map \
    src/uas \
    src/comm \
    include/ui \
    src/input \
    src/lib/qmapcontrol \
    src/ui/mavlink \
    src/ui/param \
    src/ui/watchdog \
    src/ui/map3D \
    src/ui/mission \
    src/ui/designer \
    src/ui/configuration \
    src/output

FORMS += \
    src/ui/MainWindow.ui \
    src/ui/CommSettings.ui \
    src/ui/SerialSettings.ui \
    src/ui/UASControl.ui \
    src/ui/UASList.ui \
    src/ui/UASInfo.ui \
    src/ui/UASView.ui \
    src/ui/ParameterInterface.ui \
    src/ui/WaypointList.ui \
    src/ui/ObjectDetectionView.ui \
    src/ui/JoystickWidget.ui \
    src/ui/HDDisplay.ui \
    src/ui/MAVLinkSettingsWidget.ui \
    src/ui/AudioOutputWidget.ui \
    src/ui/QGCSensorSettingsWidget.ui \
    src/ui/watchdog/WatchdogControl.ui \
    src/ui/watchdog/WatchdogProcessView.ui \
    src/ui/watchdog/WatchdogView.ui \
    src/ui/QGCFirmwareUpdate.ui \
    src/ui/QGCPxImuFirmwareUpdate.ui \
    src/ui/QMap3D.ui \
    src/ui/SlugsDataSensorView.ui \
    src/ui/SlugsHilSim.ui \
    src/ui/SlugsPadCameraControl.ui \
    src/ui/uas/QGCUnconnectedInfoWidget.ui \
    src/ui/designer/QGCToolWidget.ui \
    src/ui/designer/QGCParamSlider.ui \
    src/ui/designer/QGCActionButton.ui \
    src/ui/designer/QGCCommandButton.ui \
    src/ui/QGCMAVLinkLogPlayer.ui \
    src/ui/QGCWaypointListMulti.ui \
    src/ui/QGCUDPLinkConfiguration.ui \
    src/ui/QGCUDPClientLinkConfiguration.ui \
    src/ui/QGCTCPLinkConfiguration.ui \
    src/ui/QGCSettingsWidget.ui \
    src/ui/map/QGCMapTool.ui \
    src/ui/map/QGCMapToolBar.ui \
    src/ui/QGCMAVLinkInspector.ui \
    src/ui/WaypointViewOnlyView.ui \
    src/ui/WaypointEditableView.ui \
    src/ui/UnconnectedUASInfoWidget.ui \
    src/ui/mavlink/QGCMAVLinkMessageSender.ui \
    src/ui/firmwareupdate/QGCFirmwareUpdateWidget.ui \
    src/ui/QGCPluginHost.ui \
    src/ui/firmwareupdate/QGCPX4FirmwareUpdate.ui \
    src/ui/mission/QGCMissionOther.ui \
    src/ui/mission/QGCMissionNavWaypoint.ui \
    src/ui/mission/QGCMissionDoJump.ui \
    src/ui/mission/QGCMissionDoSetServo.ui \
    src/ui/mission/QGCMissionDoRepeatServo.ui \
    src/ui/mission/QGCMissionDoDigicamControl.ui \
    src/ui/mission/QGCMissionDoChangeSpeed.ui \
    src/ui/mission/QGCMissionDoSetCamTriggDist.ui \
    src/ui/mission/QGCMissionDoSetHome.ui \
    src/ui/mission/QGCMissionDoSetRelay.ui \
    src/ui/mission/QGCMissionDoSetReverse.ui \
    src/ui/mission/QGCMissionDoSetROI.ui \
    src/ui/mission/QGCMissionDoMountControl.ui \
    src/ui/mission/QGCMissionDoRepeatRelay.ui \
    src/ui/mission/QGCMissionConditionDelay.ui \
    src/ui/mission/QGCMissionConditionYaw.ui \
    src/ui/mission/QGCMissionConditionDistance.ui \
    src/ui/mission/QGCMissionNavLoiterUnlim.ui \
    src/ui/mission/QGCMissionNavLoiterTurns.ui \
    src/ui/mission/QGCMissionNavLoiterTime.ui \
    src/ui/mission/QGCMissionNavLoiterToAlt.ui \
    src/ui/mission/QGCMissionNavReturnToLaunch.ui \
    src/ui/mission/QGCMissionNavLand.ui \
    src/ui/mission/QGCMissionNavTakeoff.ui \
    src/ui/mission/QGCMissionNavSweep.ui \
    src/ui/mission/QGCMissionNavContinueChangeAlt.ui \
    src/ui/mission/QGCMissionDoStartSearch.ui \
    src/ui/mission/QGCMissionDoFinishSearch.ui \
    src/ui/QGCVehicleConfig.ui \
    src/ui/QGCHilConfiguration.ui \
    src/ui/QGCHilFlightGearConfiguration.ui \
    src/ui/QGCHilJSBSimConfiguration.ui \
#    src/ui/QGCHilXPlaneConfiguration.ui \
    src/ui/designer/QGCComboBox.ui \
    src/ui/designer/QGCTextLabel.ui \
    src/ui/uas/UASQuickView.ui \
    src/ui/uas/UASQuickViewItemSelect.ui \
    src/ui/uas/UASActionsWidget.ui \
    src/ui/QGCTabbedInfoView.ui \
    src/ui/UASRawStatusView.ui \
    src/ui/uas/QGCMessageView.ui \
    src/ui/configuration/ApmHardwareConfig.ui \
    src/ui/configuration/ApmSoftwareConfig.ui \
    src/ui/configuration/FrameTypeConfig.ui \
    src/ui/configuration/CompassConfig.ui \
    src/ui/configuration/AccelCalibrationConfig.ui \
    src/ui/configuration/RadioCalibrationConfig.ui \
    src/ui/configuration/FlightModeConfig.ui \
    src/ui/configuration/Radio3DRConfig.ui \
    src/ui/configuration/BatteryMonitorConfig.ui \
    src/ui/configuration/RangeFinderConfig.ui \
    src/ui/configuration/AirspeedConfig.ui \
    src/ui/configuration/OpticalFlowConfig.ui \
    src/ui/configuration/OsdConfig.ui \
    src/ui/configuration/AntennaTrackerConfig.ui \
    src/ui/configuration/CameraGimbalConfig.ui \
    src/ui/configuration/BasicPidConfig.ui \
    src/ui/configuration/StandardParamConfig.ui \
    src/ui/configuration/GeoFenceConfig.ui \
    src/ui/configuration/FailSafeConfig.ui \
    src/ui/configuration/AdvancedParamConfig.ui \
    src/ui/configuration/ArduCopterPidConfig.ui \
    src/ui/configuration/CopterPidConfig.ui \
    src/ui/configuration/ApmPlaneLevel.ui \
    src/ui/configuration/ParamWidget.ui \
    src/ui/configuration/ArduPlanePidConfig.ui \
    src/ui/configuration/AdvParameterList.ui \
    src/ui/configuration/ArduRoverPidConfig.ui \
    src/ui/configuration/TerminalConsole.ui \
    src/ui/configuration/LogConsole.ui \
    src/ui/configuration/SerialSettingsDialog.ui \
    src/ui/configuration/ApmFirmwareConfig.ui \
    src/ui/DebugOutput.ui \
    src/ui/configuration/SetupWarningMessage.ui \
    src/ui/uas/APMShortcutModesDialog.ui \
    src/ui/configuration/DownloadRemoteParamsDialog.ui \
    src/ui/configuration/ParamCompareDialog.ui \
    src/ui/AP2DataPlot2D.ui \
    src/ui/dataselectionscreen.ui \
    src/ui/AboutDialog.ui \
    src/ui/AP2DataPlotAxisDialog.ui \
    src/ui/AutoUpdateDialog.ui \
    src/uas/LogDownloadDialog.ui \
    src/ui/configuration/CompassMotorCalibrationDialog.ui \
    src/ui/MissionElevationDisplay.ui \
    src/ui/uas/PreFlightCalibrationDialog.ui \
    src/ui/configuration/RadioFlashWizard.ui

HEADERS += \
    src/MG.h \
    src/QGCCore.h \
    src/uas/UASInterface.h \
    src/uas/UAS.h \
    src/uas/UASManager.h \
    src/comm/LinkManager.h \
    src/comm/LinkInterface.h \
    src/comm/SerialLinkInterface.h \
    src/comm/ProtocolInterface.h \
    src/comm/QGCFlightGearLink.h \
    src/comm/QGCJSBSimLink.h \
#    src/comm/QGCXPlaneLink.h \
    src/comm/serialconnection.h \
    src/ui/CommConfigurationWindow.h \
    src/ui/SerialConfigurationWindow.h \
    src/ui/MainWindow.h \
    src/ui/uas/UASControlWidget.h \
    src/ui/uas/UASListWidget.h \
    src/ui/uas/UASInfoWidget.h \
    src/ui/HUD.h \
    src/configuration.h \
    src/ui/uas/UASView.h \
    src/comm/MAVLinkSimulationLink.h \
    src/comm/UDPLink.h \
    src/comm/UDPClientLink.h \
    src/comm/TCPLink.h \
    src/ui/ParameterInterface.h \
    src/ui/WaypointList.h \
    src/ui/WaypointNavigation.h \
    src/Waypoint.h \
    src/ui/ObjectDetectionView.h \
    src/input/JoystickInput.h \
    src/ui/JoystickWidget.h \
    src/ui/HDDisplay.h \
    src/ui/MAVLinkSettingsWidget.h \
    src/ui/AudioOutputWidget.h \
    src/GAudioOutput.h \
    src/LogCompressor.h \
    src/ui/QGCParamWidget.h \
    src/ui/QGCSensorSettingsWidget.h \
    src/uas/SlugsMAV.h \
    src/uas/PxQuadMAV.h \
    src/uas/ArduPilotMegaMAV.h \
    src/uas/senseSoarMAV.h \
    src/ui/watchdog/WatchdogControl.h \
    src/ui/watchdog/WatchdogProcessView.h \
    src/ui/watchdog/WatchdogView.h \
    src/uas/UASWaypointManager.h \
    src/ui/HSIDisplay.h \
    src/QGC.h \
    src/ui/QGCFirmwareUpdate.h \
    src/ui/QGCPxImuFirmwareUpdate.h \
    src/ui/RadioCalibration/RadioCalibrationData.h \
    src/comm/QGCMAVLink.h \
    src/ui/SlugsDataSensorView.h \
    src/ui/SlugsHilSim.h \
    src/ui/SlugsPadCameraControl.h \
    src/ui/QGCMainWindowAPConfigurator.h \
    src/comm/MAVLinkSwarmSimulationLink.h \
    src/ui/uas/QGCUnconnectedInfoWidget.h \
    src/ui/designer/QGCToolWidget.h \
    src/ui/designer/QGCParamSlider.h \
    src/ui/designer/QGCCommandButton.h \
    src/ui/designer/QGCToolWidgetItem.h \
    src/ui/QGCMAVLinkLogPlayer.h \
    src/comm/MAVLinkSimulationWaypointPlanner.h \
    src/comm/MAVLinkSimulationMAV.h \
    src/uas/QGCMAVLinkUASFactory.h \
    src/ui/QGCWaypointListMulti.h \
    src/ui/QGCUDPLinkConfiguration.h \
    src/ui/QGCUDPClientLinkConfiguration.h \
    src/ui/QGCTCPLinkConfiguration.h \
    src/ui/QGCSettingsWidget.h \
    src/uas/QGCUASParamManager.h \
    src/ui/map/QGCMapWidget.h \
    src/ui/map/MAV2DIcon.h \
    src/ui/map/Waypoint2DIcon.h \
    src/ui/map/QGCMapTool.h \
    src/ui/map/QGCMapToolBar.h \
    src/QGCGeo.h \
    src/ui/QGCToolBar.h \
    src/ui/QGCStatusBar.h \
    src/ui/QGCMAVLinkInspector.h \
    src/ui/WaypointViewOnlyView.h \
    src/ui/WaypointEditableView.h \
    src/ui/UnconnectedUASInfoWidget.h \
    src/ui/QGCRGBDView.h \
    src/ui/mavlink/QGCMAVLinkMessageSender.h \
    src/ui/firmwareupdate/QGCFirmwareUpdateWidget.h \
    src/ui/QGCPluginHost.h \
    src/ui/firmwareupdate/QGCPX4FirmwareUpdate.h \
    src/ui/mission/QGCMissionOther.h \
    src/ui/mission/QGCMissionNavWaypoint.h \
    src/ui/mission/QGCMissionDoJump.h \
    src/ui/mission/QGCMissionDoSetServo.h \
    src/ui/mission/QGCMissionDoRepeatServo.h \
    src/ui/mission/QGCMissionDoDigicamControl.h \
    src/ui/mission/QGCMissionDoChangeSpeed.h \
    src/ui/mission/QGCMissionDoSetCamTriggDist.h \
    src/ui/mission/QGCMissionDoSetHome.h \
    src/ui/mission/QGCMissionDoSetRelay.h \
    src/ui/mission/QGCMissionDoSetReverse.h \
    src/ui/mission/QGCMissionDoSetROI.h \
    src/ui/mission/QGCMissionDoMountControl.h \
    src/ui/mission/QGCMissionDoRepeatRelay.h \
    src/ui/mission/QGCMissionConditionDelay.h \
    src/ui/mission/QGCMissionConditionYaw.h \
    src/ui/mission/QGCMissionConditionDistance.h \
    src/ui/mission/QGCMissionNavLoiterUnlim.h \
    src/ui/mission/QGCMissionNavLoiterTurns.h \
    src/ui/mission/QGCMissionNavLoiterTime.h \
    src/ui/mission/QGCMissionNavLoiterToAlt.h \
    src/ui/mission/QGCMissionNavReturnToLaunch.h \
    src/ui/mission/QGCMissionNavLand.h \
    src/ui/mission/QGCMissionNavTakeoff.h \
    src/ui/mission/QGCMissionNavSweep.h \
    src/ui/mission/QGCMissionNavContinueChangeAlt.h \
    src/ui/mission/QGCMissionDoStartSearch.h \
    src/ui/mission/QGCMissionDoFinishSearch.h \
    src/ui/QGCVehicleConfig.h \
    src/comm/QGCHilLink.h \
    src/ui/QGCHilConfiguration.h \
    src/ui/QGCHilFlightGearConfiguration.h \
    src/ui/QGCHilJSBSimConfiguration.h \
#    src/ui/QGCHilXPlaneConfiguration.h \
    src/ui/designer/QGCComboBox.h \
    src/ui/designer/QGCTextLabel.h \
    src/ui/submainwindow.h \
    src/ui/dockwidgettitlebareventfilter.h \
    src/ui/uas/UASQuickView.h \
    src/ui/uas/UASQuickViewItem.h \
    src/ui/uas/UASQuickViewItemSelect.h \
    src/ui/uas/UASQuickViewTextItem.h \
    src/ui/uas/UASQuickViewGaugeItem.h \
    src/ui/uas/UASActionsWidget.h \
    src/ui/designer/QGCRadioChannelDisplay.h \
    src/ui/QGCTabbedInfoView.h \
    src/ui/UASRawStatusView.h \
    src/ui/PrimaryFlightDisplay.h \
    src/ui/uas/QGCMessageView.h \
    src/ui/configuration/ApmHardwareConfig.h \
    src/ui/configuration/ApmSoftwareConfig.h \
    src/ui/configuration/FrameTypeConfig.h \
    src/ui/configuration/CompassConfig.h \
    src/ui/configuration/AccelCalibrationConfig.h \
    src/ui/configuration/RadioCalibrationConfig.h \
    src/ui/configuration/FlightModeConfig.h \
    src/ui/configuration/Radio3DRConfig.h \
    src/ui/configuration/BatteryMonitorConfig.h \
    src/ui/configuration/RangeFinderConfig.h \
    src/ui/configuration/AirspeedConfig.h \
    src/ui/configuration/OpticalFlowConfig.h \
    src/ui/configuration/OsdConfig.h \
    src/ui/configuration/AntennaTrackerConfig.h \
    src/ui/configuration/CameraGimbalConfig.h \
    src/ui/configuration/AP2ConfigWidget.h \
    src/ui/configuration/BasicPidConfig.h \
    src/ui/configuration/StandardParamConfig.h \
    src/ui/configuration/GeoFenceConfig.h \
    src/ui/configuration/FailSafeConfig.h \
    src/ui/configuration/AdvancedParamConfig.h \
    src/ui/configuration/ArduCopterPidConfig.h \
    src/ui/configuration/CopterPidConfig.h \
    src/ui/ApmToolBar.h \
    src/ui/configuration/PX4FirmwareUploader.h \
    src/ui/configuration/ApmPlaneLevel.h \
    src/ui/configuration/ParamWidget.h \
    src/ui/configuration/ArduPlanePidConfig.h \
    src/ui/configuration/AdvParameterList.h \
    src/ui/configuration/ArduRoverPidConfig.h \
    src/ui/configuration/Console.h \
    src/ui/configuration/SerialSettingsDialog.h \
    src/ui/configuration/TerminalConsole.h \
    src/ui/configuration/LogConsole.h \
    src/ui/configuration/ApmHighlighter.h \
    src/ui/configuration/ApmFirmwareConfig.h \
    src/ui/designer/QGCMouseWheelEventFilter.h \
    src/ui/DebugOutput.h \
    src/ui/configuration/APDoubleSpinBox.h \
    src/ui/configuration/APSpinBox.h \
    src/ui/configuration/Radio3DRSettings.h \
    src/ui/configuration/SetupWarningMessage.h \
    src/ui/uas/APMShortcutModesDialog.h \
    src/ui/configuration/DownloadRemoteParamsDialog.h \
    src/ui/configuration/ParamCompareDialog.h \
    src/uas/UASParameter.h \
    src/output/kmlcreator.h \
    src/output/logdata.h \
    src/ui/AP2DataPlot2D.h \
    src/ui/AP2DataPlotThread.h \
    src/ui/dataselectionscreen.h \
    src/ui/qcustomplot.h \
    src/globalobject.h \
    src/ui/AboutDialog.h \
    src/ui/uas/UASQuickViewTextLabel.h \
    src/uas/ApmUiHelpers.h \
    src/ui/AP2DataPlotAxisDialog.h \
    src/comm/arduino_intelhex.h \
    src/comm/arduinoflash.h \
    src/audio/AlsaAudio.h \
    src/ui/AutoUpdateCheck.h \
    src/ui/AutoUpdateDialog.h \
    src/uas/LogDownloadDialog.h \
    src/comm/TLogReplayLink.h \
    src/ui/PrimaryFlightDisplayQML.h \
    src/ui/configuration/CompassMotorCalibrationDialog.h \
    src/comm/MAVLinkDecoder.h \
    src/comm/MAVLinkProtocol.h \
    src/ui/MissionElevationDisplay.h \
    src/ui/GoogleElevationData.h \
    src/comm/UASObject.h \
    src/comm/VehicleOverview.h \
    src/comm/RelPositionOverview.h \
    src/comm/AbsPositionOverview.h \
    src/comm/MissionOverview.h \
    src/ui/AP2DataPlot2DModel.h \
    src/ui/uas/PreFlightCalibrationDialog.h \
    src/ui/configuration/RadioFlashWizard.h \
    src/ui/GraphTreeWidgetItem.h \
    src/comm/LinkManagerFactory.h \
    src/ui/VibrationMonitor.h \
    src/ui/EKFMonitor.h \
    src/Settings.h \
    src/logging.h \
    src/uas/APMFirmwareVersion.h

SOURCES += src/main.cc \
    src/QGCCore.cc \
    src/uas/UASManager.cc \
    src/uas/UAS.cc \
    src/comm/LinkManager.cc \
    src/comm/LinkInterface.cpp \
    src/comm/QGCFlightGearLink.cc \
    src/comm/QGCJSBSimLink.cc \
#    src/comm/QGCXPlaneLink.cc \
    src/comm/serialconnection.cc \
    src/ui/CommConfigurationWindow.cc \
    src/ui/SerialConfigurationWindow.cc \
    src/ui/MainWindow.cc \
    src/ui/uas/UASControlWidget.cc \
    src/ui/uas/UASListWidget.cc \
    src/ui/uas/UASInfoWidget.cc \
    src/ui/HUD.cc \
    src/ui/uas/UASView.cc \
    src/comm/MAVLinkSimulationLink.cc \
    src/comm/UDPLink.cc \
    src/comm/UDPClientLink.cc \
    src/comm/TCPLink.cc \
    src/ui/ParameterInterface.cc \
    src/ui/WaypointList.cc \
    src/ui/WaypointNavigation.cc \
    src/Waypoint.cc \
    src/ui/ObjectDetectionView.cc \
    src/input/JoystickInput.cc \
    src/ui/JoystickWidget.cc \
    src/ui/HDDisplay.cc \
    src/ui/MAVLinkSettingsWidget.cc \
    src/ui/AudioOutputWidget.cc \
    src/GAudioOutput.cc \
    src/LogCompressor.cc \
    src/ui/QGCParamWidget.cc \
    src/ui/QGCSensorSettingsWidget.cc \
    src/uas/SlugsMAV.cc \
    src/uas/PxQuadMAV.cc \
    src/uas/ArduPilotMegaMAV.cc \
    src/uas/senseSoarMAV.cpp \
    src/ui/watchdog/WatchdogControl.cc \
    src/ui/watchdog/WatchdogProcessView.cc \
    src/ui/watchdog/WatchdogView.cc \
    src/uas/UASWaypointManager.cc \
    src/ui/HSIDisplay.cc \
    src/QGC.cc \
    src/ui/QGCFirmwareUpdate.cc \
    src/ui/QGCPxImuFirmwareUpdate.cc \
    src/ui/RadioCalibration/RadioCalibrationData.cc \
    src/ui/SlugsDataSensorView.cc \
    src/ui/SlugsHilSim.cc \
    src/ui/SlugsPadCameraControl.cpp \
    src/ui/QGCMainWindowAPConfigurator.cc \
    src/comm/MAVLinkSwarmSimulationLink.cc \
    src/ui/uas/QGCUnconnectedInfoWidget.cc \
    src/ui/designer/QGCToolWidget.cc \
    src/ui/designer/QGCParamSlider.cc \
    src/ui/designer/QGCCommandButton.cc \
    src/ui/designer/QGCToolWidgetItem.cc \
    src/ui/QGCMAVLinkLogPlayer.cc \
    src/comm/MAVLinkSimulationWaypointPlanner.cc \
    src/comm/MAVLinkSimulationMAV.cc \
    src/uas/QGCMAVLinkUASFactory.cc \
    src/ui/QGCWaypointListMulti.cc \
    src/ui/QGCUDPLinkConfiguration.cc \
    src/ui/QGCUDPClientLinkConfiguration.cc \
    src/ui/QGCTCPLinkConfiguration.cc \
    src/ui/QGCSettingsWidget.cc \
    src/uas/QGCUASParamManager.cc \
    src/ui/map/QGCMapWidget.cc \
    src/ui/map/MAV2DIcon.cc \
    src/ui/map/Waypoint2DIcon.cc \
    src/ui/map/QGCMapTool.cc \
    src/ui/map/QGCMapToolBar.cc \
    src/QGCGeo.cc \
    src/ui/QGCToolBar.cc \
    src/ui/QGCStatusBar.cc \
    src/ui/QGCMAVLinkInspector.cc \
    src/ui/WaypointViewOnlyView.cc \
    src/ui/WaypointEditableView.cc \
    src/ui/UnconnectedUASInfoWidget.cc \
    src/ui/QGCRGBDView.cc \
    src/ui/mavlink/QGCMAVLinkMessageSender.cc \
    src/ui/firmwareupdate/QGCFirmwareUpdateWidget.cc \
    src/ui/QGCPluginHost.cc \
    src/ui/firmwareupdate/QGCPX4FirmwareUpdate.cc \
    src/ui/mission/QGCMissionOther.cc \
    src/ui/mission/QGCMissionNavWaypoint.cc \
    src/ui/mission/QGCMissionDoJump.cc \
    src/ui/mission/QGCMissionDoSetServo.cc \
    src/ui/mission/QGCMissionDoRepeatServo.cc \
    src/ui/mission/QGCMissionDoDigicamControl.cc \
    src/ui/mission/QGCMissionDoChangeSpeed.cc \
    src/ui/mission/QGCMissionDoSetCamTriggDist.cc \
    src/ui/mission/QGCMissionDoSetHome.cc \
    src/ui/mission/QGCMissionDoSetRelay.cc \
    src/ui/mission/QGCMissionDoSetReverse.cc \
    src/ui/mission/QGCMissionDoSetROI.cc \
    src/ui/mission/QGCMissionDoMountControl.cc \
    src/ui/mission/QGCMissionDoRepeatRelay.cc \
    src/ui/mission/QGCMissionConditionDelay.cc \
    src/ui/mission/QGCMissionConditionYaw.cc \
    src/ui/mission/QGCMissionConditionDistance.cc \
    src/ui/mission/QGCMissionNavLoiterUnlim.cc \
    src/ui/mission/QGCMissionNavLoiterTurns.cc \
    src/ui/mission/QGCMissionNavLoiterTime.cc \
    src/ui/mission/QGCMissionNavLoiterToAlt.cc \
    src/ui/mission/QGCMissionNavReturnToLaunch.cc \
    src/ui/mission/QGCMissionNavLand.cc \
    src/ui/mission/QGCMissionNavTakeoff.cc \
    src/ui/mission/QGCMissionNavSweep.cc \
    src/ui/mission/QGCMissionNavContinueChangeAlt.cc \
    src/ui/mission/QGCMissionDoStartSearch.cc \
    src/ui/mission/QGCMissionDoFinishSearch.cc \
    src/ui/QGCVehicleConfig.cc \
    src/ui/QGCHilConfiguration.cc \
    src/ui/QGCHilFlightGearConfiguration.cc \
    src/ui/QGCHilJSBSimConfiguration.cc \
#    src/ui/QGCHilXPlaneConfiguration.cc \
    src/ui/designer/QGCComboBox.cc \
    src/ui/designer/QGCTextLabel.cc \
    src/ui/submainwindow.cpp \
    src/ui/dockwidgettitlebareventfilter.cpp \
    src/ui/uas/UASQuickViewItem.cc \
    src/ui/uas/UASQuickView.cc \
    src/ui/uas/UASQuickViewTextItem.cc \
    src/ui/uas/UASQuickViewGaugeItem.cc \
    src/ui/uas/UASQuickViewItemSelect.cc \
    src/ui/uas/UASActionsWidget.cpp \
    src/ui/designer/QGCRadioChannelDisplay.cpp \
    src/ui/QGCTabbedInfoView.cpp \
    src/ui/UASRawStatusView.cpp \
    src/ui/PrimaryFlightDisplay.cc \
    src/ui/uas/QGCMessageView.cc \
    src/ui/configuration/ApmHardwareConfig.cc \
    src/ui/configuration/ApmSoftwareConfig.cc \
    src/ui/configuration/FrameTypeConfig.cc \
    src/ui/configuration/CompassConfig.cc \
    src/ui/configuration/AccelCalibrationConfig.cc \
    src/ui/configuration/RadioCalibrationConfig.cc \
    src/ui/configuration/FlightModeConfig.cc \
    src/ui/configuration/Radio3DRConfig.cc \
    src/ui/configuration/BatteryMonitorConfig.cc \
    src/ui/configuration/RangeFinderConfig.cc \
    src/ui/configuration/AirspeedConfig.cc \
    src/ui/configuration/OpticalFlowConfig.cc \
    src/ui/configuration/OsdConfig.cc \
    src/ui/configuration/AntennaTrackerConfig.cc \
    src/ui/configuration/CameraGimbalConfig.cc \
    src/ui/configuration/AP2ConfigWidget.cc \
    src/ui/configuration/BasicPidConfig.cc \
    src/ui/configuration/StandardParamConfig.cc \
    src/ui/configuration/GeoFenceConfig.cc \
    src/ui/configuration/FailSafeConfig.cc \
    src/ui/configuration/AdvancedParamConfig.cc \
    src/ui/configuration/ArduCopterPidConfig.cc \
    src/ui/configuration/CopterPidConfig.cc \
    src/ui/ApmToolBar.cc \
    src/ui/configuration/PX4FirmwareUploader.cc \
    src/ui/configuration/ApmPlaneLevel.cc \
    src/ui/configuration/ParamWidget.cc \
    src/ui/configuration/ArduPlanePidConfig.cc \
    src/ui/configuration/AdvParameterList.cc \
    src/ui/configuration/ArduRoverPidConfig.cc \
    src/ui/configuration/TerminalConsole.cc \
    src/ui/configuration/LogConsole.cc \
    src/ui/configuration/Console.cc \
    src/ui/configuration/SerialSettingsDialog.cc \
    src/ui/configuration/ApmHighlighter.cc \
    src/ui/configuration/ApmFirmwareConfig.cc \
    src/ui/designer/QGCMouseWheelEventFilter.cc \
    src/ui/DebugOutput.cc \
    src/ui/configuration/APDoubleSpinBox.cc \
    src/ui/configuration/APSpinBox.cc \
    src/ui/configuration/Radio3DRSettings.cc \
    src/ui/configuration/SetupWarningMessage.cc \
    src/ui/uas/APMShortcutModesDialog.cpp \
    src/ui/configuration/DownloadRemoteParamsDialog.cc \
    src/ui/configuration/ParamCompareDialog.cpp \
    src/uas/UASParameter.cpp \
    src/output/kmlcreator.cc \
    src/output/logdata.cc \
    src/ui/AP2DataPlot2D.cpp \
    src/ui/AP2DataPlotThread.cc \
    src/ui/dataselectionscreen.cpp \
    src/ui/qcustomplot.cpp \
    src/globalobject.cc \
    src/ui/AboutDialog.cc \
    src/ui/uas/UASQuickViewTextLabel.cc \
    src/uas/ApmUiHelpers.cc \
    src/ui/AP2DataPlotAxisDialog.cc \
    src/comm/arduino_intelhex.cpp \
    src/comm/arduinoflash.cc \
    src/audio/AlsaAudio.cc \
    src/ui/AutoUpdateCheck.cc \
    src/ui/AutoUpdateDialog.cc \
    src/uas/LogDownloadDialog.cc \
    src/comm/TLogReplayLink.cc \
    src/ui/PrimaryFlightDisplayQML.cpp \
    src/ui/configuration/CompassMotorCalibrationDialog.cpp \
    src/comm/MAVLinkDecoder.cc \
    src/comm/MAVLinkProtocol.cc \
    src/ui/MissionElevationDisplay.cpp \
    src/ui/GoogleElevationData.cpp \
    src/comm/UASObject.cc \
    src/comm/VehicleOverview.cc \
    src/comm/RelPositionOverview.cc \
    src/comm/AbsPositionOverview.cc \
    src/comm/MissionOverview.cc \
    src/ui/AP2DataPlot2DModel.cc \
    src/ui/uas/PreFlightCalibrationDialog.cpp \
    src/ui/configuration/RadioFlashWizard.cpp \
    src/ui/GraphTreeWidgetItem.cc \
    src/comm/LinkManagerFactory.cpp \
    src/ui/VibrationMonitor.cpp \
    src/ui/EKFMonitor.cpp \
    src/Settings.cpp \
    src/uas/APMFirmwareVersion.cpp

MacBuild | WindowsBuild : contains(GOOGLEEARTH, enable) { #fix this to make sense ;)
    message(Including support for Google Earth)
    QT +=  webkit webkitwidgets
    HEADERS +=  src/ui/map3D/QGCWebPage.h
    SOURCES +=  src/ui/map3D/QGCWebPage.cc
} else {
    message(Skipping support for Google Earth)
}

contains(DEFINES, ENABLE_CAMRAVIEW){
    message(Including support for Camera View)
    HEADERS += src/ui/CameraView.h
    SOURCES += src/ui/CameraView.cc
} else {
    message(Skipping support for Camera View)
}


OTHER_FILES += \
    qml/components/DigitalDisplay.qml \
    qml/components/StatusDisplay.qml \
    qml/components/ModeDisplay.qml \
    qml/components/HeartbeatDisplay.qml \
    qml/PrimaryFlightDisplayQML.qml \
    qml/components/RollPitchIndicator.qml \
    qml/components/AltitudeIndicator.qml \
    qml/components/SpeedIndicator.qml \
    qml/components/CompassIndicator.qml \
    qml/components/PitchIndicator.qml \
    qml/components/StatusMessageIndicator.qml \
    qml/components/InformationOverlayIndicator.qml \

OTHER_FILES += \
    qml/ApmToolBar.qml \
    qml/components/Button.qml \
    qml/components/TextButton.qml \
    qml/resources/apmplanner/toolbar/connect.png \
    qml/resources/apmplanner/toolbar/flightplanner.png \
    qml/resources/apmplanner/toolbar/helpwizard.png \
    qml/resources/apmplanner/toolbar/light_initialsetup_icon.png \
    qml/resources/apmplanner/toolbar/terminal.png \
    qml/resources/apmplanner/toolbar/simulation.png \
    qml/resources/apmplanner/toolbar/light_tuningconfig_icon.png \
    qml/resources/apmplanner/toolbar/flightdata.png \
    qml/resources/apmplanner/toolbar/disconnect.png \
    qml/resources/apmplanner/toolbar/donate.png

OTHER_FILES += \
    qml/VibrationMonitor.qml \
    qml/EKFMonitor.qml

# Command Line Tools
OTHER_FILES += \
    libs/sik_uploader/sik_uploader.py

#qmlcomponents.path    += $${DESTDIR}$${TARGET}/components
#qmlcomponents.files   += ./components/Button.qml

#sources.files       += ApmToolBar.qml
#sources.path        += $$DESTDIR/qml
#target.path         += apmplanner2
#INSTALLS            += sources target

DISTFILES += \
    qml/components/BarGauge.qml


