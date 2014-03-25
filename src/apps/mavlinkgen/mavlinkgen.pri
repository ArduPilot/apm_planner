# Third-party includes.
# if you include this file with the commands below into
# your Qt project, you can enable your application
# to generate MAVLink code easily.

###### EXAMPLE BEGIN

## Include MAVLink generator
#DEPENDPATH += \
#    src/apps/mavlinkgen
#
#INCLUDEPATH += \
#    src/apps/mavlinkgen
#    src/apps/mavlinkgen/ui \
#    src/apps/mavlinkgen/generator
#
#include(src/apps/mavlinkgen/mavlinkgen.pri)

###### EXAMPLE END



INCLUDEPATH += .\
    $$PWD/ui \
    $$PWD/generator

FORMS += \
    $$PWD/ui/XMLCommProtocolWidget.ui

HEADERS += \
    $$PWD/ui/DomItem.h \
    $$PWD/ui/DomModel.h \
    $$PWD/ui/QGCMAVLinkTextEdit.h \
    $$PWD/ui/XMLCommProtocolWidget.h \
    $$PWD/generator/MAVLinkXMLParser.h \
    $$PWD/generator/MAVLinkXMLParserV10.h

SOURCES += \
    $$PWD/ui/DomItem.cc \
    $$PWD/ui/DomModel.cc \
    $$PWD/ui/QGCMAVLinkTextEdit.cc \
    $$PWD/ui/XMLCommProtocolWidget.cc \
    $$PWD/generator/MAVLinkXMLParser.cc \
    $$PWD/generator/MAVLinkXMLParserV10.cc

RESOURCES += \
    $$PWD/mavlinkgen.qrc
