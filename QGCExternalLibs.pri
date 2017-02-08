#
# Tell the Linux build to look in a few additional places for libs
#

LinuxBuild {
	LIBS += \
		-L/usr/lib

    linux-g++-64 {
        LIBS += \
            -L/usr/local/lib64 \
            -L/usr/lib64
	}
}

#
# Add in a few missing headers to windows
#

WindowsBuild {
    INCLUDEPATH += libs/lib/msinttypes
}

#
# MAVLink (Set for MAVLink 1.0)
#

MAVLINKPATH_REL = libs/mavlink/include/mavlink/v1.0
MAVLINKPATH = $$BASEDIR/$$MAVLINKPATH_REL
MAVLINK_CONF = ardupilotmega
DEFINES += MAVLINK_NO_DATA

# First we select the dialect, checking for valid user selection
# Users can override all other settings by specifying MAVLINK_CONF as an argument to qmake
!isEmpty(MAVLINK_CONF) {
    message($$sprintf("Using MAVLink dialect '%1'.", $$MAVLINK_CONF))
}

# Then we add the proper include paths dependent on the dialect.
INCLUDEPATH += $$MAVLINKPATH

exists($$MAVLINKPATH/common) {
    !isEmpty(MAVLINK_CONF) {
        count(MAVLINK_CONF, 1) {
            exists($$MAVLINKPATH/$$MAVLINK_CONF) {
                INCLUDEPATH += $$MAVLINKPATH/$$MAVLINK_CONF
                DEFINES += $$sprintf('QGC_USE_%1_MESSAGES', $$upper($$MAVLINK_CONF))
            } else {
                error($$sprintf("MAVLink dialect '%1' does not exist at '%2'!", $$MAVLINK_CONF, $$MAVLINKPATH_REL))
            }
        } else {
            error(Only a single mavlink dialect can be specified in MAVLINK_CONF)
        }
    } else {
        INCLUDEPATH += $$MAVLINKPATH/common
    }
} else {
    error($$sprintf("MAVLink folder does not exist at '%1'! Run 'git submodule init && git submodule update' on the command line.",$$MAVLINKPATH_REL))
}


#
# Google Earth
#

MacBuild | WindowsBuild : contains(GOOGLEEARTH, enable) { #fix this to make sense ;)
    message(Including support for Google Earth)

    HEADERS += src/ui/map3D/QGCGoogleEarthView.h
    SOURCES += src/ui/map3D/QGCGoogleEarthView.cc
    WindowsBuild {
        CONFIG += qaxcontainer
    }
} else {
    message(Skipping support for Google Earth)
}

#
# libfreenect Kinect support
#

MacBuild | LinuxBuild {
    exists(/opt/local/include/libfreenect) | exists(/usr/local/include/libfreenect) {
        message("Including support for libfreenect")

        #INCLUDEPATH += /usr/include/libusb-1.0
        DEFINES += QGC_LIBFREENECT_ENABLED
        LIBS += -lfreenect
        HEADERS += src/input/Freenect.h
        SOURCES += src/input/Freenect.cc
    } else {
        message("Skipping support for libfreenect")
    }
} else {
    message("Skipping support for libfreenect")
}

#
# AGLLIB math library
#
include(libs/alglib/alglib.pri)
DEFINES += NOMINMAX
#
# OPMapControl library (from OpenPilot)
#

#include(libs/utils/utils_external.pri)
include(libs/opmapcontrol/opmapcontrol_external.pri)

DEPENDPATH += \
    libs/utils \
    libs/utils/src \
    libs/opmapcontrol \
    libs/opmapcontrol/src \
    libs/opmapcontrol/src/mapwidget

INCLUDEPATH += \
    libs/utils \
    libs \
    libs/opmapcontrol

WindowsBuild {
    # Used to enumerate serial ports by QSerialPort
	LIBS += -lsetupapi
}

#
# Zip Access Tool (http://quazip.sourceforge.net)
#
include (libs/thirdParty/quazip/quazip.pri)

#
# XBee wireless
#

WindowsBuild {
    #message(Including support for XBee)

    #DEFINES += XBEELINK

    #INCLUDEPATH += libs/thirdParty/libxbee

    #HEADERS += \
    #    src/comm/XbeeLinkInterface.h \
    #    src/comm/XbeeLink.h \
    #    src/comm/HexSpinBox.h \
    #    src/ui/XbeeConfigurationWindow.h \
    #    src/comm/CallConv.h

    #SOURCES += \
    #    src/comm/XbeeLink.cpp \
    #    src/comm/HexSpinBox.cpp \
    #    src/ui/XbeeConfigurationWindow.cpp

    #WindowsBuild {
    #    LIBS += -llibs/thirdParty/libxbee/lib/libxbee
    #}

    #LinuxBuild {
    #    LIBS += -lxbee
    #}
} else:WindowsCrossBuild {
    #LIBS += -llibs/thirdParty/libxbee/lib/libxbee
    message(Skipping support for XBee)
} else {
    message(Skipping support for XBee)
}

#
# [OPTIONAL] Magellan 3DxWare library. Provides support for 3DConnexion's 3D mice.
#
contains(DEFINES, DISABLE_3DMOUSE) {
    message("Skipping support for Magellan 3DxWare (manual override from command line)")
    DEFINES -= DISABLE_3DMOUSE
# Otherwise the user can still disable this feature in the user_config.pri file.
} else:exists(user_config.pri):infile(user_config.pri, DEFINES, DISABLE_3DMOUSE) {
    message("Skipping support for 3DConnexion mice (manual override from user_config.pri)")
} else:LinuxBuild {
    exists(/usr/local/lib/libxdrvlib.so) {
        message("Including support for Magellan 3DxWare")

        DEFINES += MOUSE_ENABLED_LINUX
        DEFINES += ParameterCheck
        # Hack: Has to be defined for magellan usage

        INCLUDEPATH *= /usr/local/include
        HEADERS += src/input/Mouse6dofInput.h
        SOURCES += src/input/Mouse6dofInput.cpp
        LIBS += -L/usr/local/lib/ -lxdrvlib
    } else {
        warning("Skipping support for Magellan 3DxWare (missing libraries, see README)")
    }
} else:WindowsBuild {
    message("Including support for Magellan 3DxWare")

    DEFINES += MOUSE_ENABLED_WIN

    INCLUDEPATH += libs/thirdParty/3DMouse/win

    HEADERS += \
        libs/thirdParty/3DMouse/win/I3dMouseParams.h \
        libs/thirdParty/3DMouse/win/MouseParameters.h \
        libs/thirdParty/3DMouse/win/Mouse3DInput.h \
        src/input/Mouse6dofInput.h

    SOURCES += \
        libs/thirdParty/3DMouse/win/MouseParameters.cpp \
        libs/thirdParty/3DMouse/win/Mouse3DInput.cpp \
        src/input/Mouse6dofInput.cpp
} else {
    message("Skipping support for Magellan 3DxWare (unsupported platform)")
}

#
# Opal RT-LAB Library
#

WindowsBuild : win32 : exists(src/lib/opalrt/OpalApi.h) : exists(C:/OPAL-RT/RT-LAB7.2.4/Common/bin) {
    message("Including support for Opal-RT")

    DEFINES += OPAL_RT

    INCLUDEPATH +=
        src/lib/opalrt
        libs/lib/opal/include \

    FORMS += src/ui/OpalLinkSettings.ui

    HEADERS += \
        src/comm/OpalRT.h \
        src/comm/OpalLink.h \
        src/comm/Parameter.h \
        src/comm/QGCParamID.h \
        src/comm/ParameterList.h \
        src/ui/OpalLinkConfigurationWindow.h

    SOURCES += \
        src/comm/OpalRT.cc \
        src/comm/OpalLink.cc \
        src/comm/Parameter.cc \
        src/comm/QGCParamID.cc \
        src/comm/ParameterList.cc \
        src/ui/OpalLinkConfigurationWindow.cc

    LIBS += \
        -LC:/OPAL-RT/RT-LAB7.2.4/Common/bin \
        -lOpalApi
} else {
    message("Skipping support for Opal-RT")
}

#
# SDL
#

MacBuild {
    INCLUDEPATH += \
        $$BASEDIR/libs/lib/Frameworks/SDL2.framework/Headers

    LIBS += \
        -F$$BASEDIR/libs/lib/Frameworks \
        -framework SDL2
}

LinuxBuild {
	LIBS += \
            -lSDL2
}

WindowsBuild{
    INCLUDEPATH += \
        $$BASEDIR/libs/lib/sdl/msvc/include \

    contains(QT_ARCH, i386) {
        LIBS += \
            -L$$BASEDIR/libs/lib/sdl/msvc/lib/x86 \
            -lSDL2main \
            -lSDL2
    }else {
        LIBS += \
            -L$$BASEDIR/libs/lib/sdl/msvc/lib/x64 \
            -lSDL2main \
            -lSDL2
    }
}

WindowsCrossBuild {
    INCLUDEPATH += \
        $$BASEDIR/libs/lib/sdl/include \

    LIBS += \
        -L$$BASEDIR/libs/lib/sdl/win32 \
        -lSDL2
}

#
# Festival Lite speech synthesis engine
#

LinuxBuild{
    exists(/usr/include/flite/flite.h){
        LIBS += \
            -lflite_cmu_us_kal \
            -lflite_usenglish \
            -lflite_cmulex \
            -lflite
    } else {
        message(Skipping Flite Support)
        DEFINES -= FLITE_AUDIO_ENABLED
    }
}

