INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
INCLUDEPATH += $$[QT_INSTALL_PREFIX]/src/3rdparty/zlib
win32-x-g++|win64-x-g++|win32 {
    DEFINES +=QUAZIP_STATIC
}
HEADERS += $$PWD/crypt.h \
           $$PWD/ioapi.h \
           $$PWD/JlCompress.h \
           $$PWD/quaadler32.h \
           $$PWD/quachecksum32.h \
           $$PWD/quacrc32.h \
           $$PWD/quazip.h \
           $$PWD/quazipfile.h \
           $$PWD/quazipfileinfo.h \
           $$PWD/quazipnewinfo.h \
           $$PWD/unzip.h \
           $$PWD/zip.h
SOURCES += $$PWD/qioapi.cpp \
           $$PWD/JlCompress.cpp \
           $$PWD/quaadler32.cpp \
           $$PWD/quacrc32.cpp \
           $$PWD/quazip.cpp \
           $$PWD/quazipfile.cpp \
           $$PWD/quazipnewinfo.cpp \
           $$PWD/quazipfileinfo.cpp \
           $$PWD/unzip.c \
           $$PWD/zip.c
