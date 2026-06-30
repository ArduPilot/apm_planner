cmake_minimum_required(VERSION 3.14)
project(quazip LANGUAGES C CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(HEADERS
    crypt.h
    ioapi.h
    JlCompress.h
    quaadler32.h
    quachecksum32.h
    quacrc32.h
    quazip.h
    quazipfile.h
    quazipfileinfo.h
    quazipnewinfo.h
    unzip.h
    zip.h
)

set(SOURCES
    JlCompress.cpp
    qioapi.cpp
    quaadler32.cpp
    quacrc32.cpp
    quagzipfile.cpp
    quaziodevice.cpp
    quazip.cpp
    quazipdir.cpp
    quazipfile.cpp
    quazipfileinfo.cpp
    quazipnewinfo.cpp
    unzip.c
    zip.c
)

add_library(quazip STATIC ${SOURCES} ${HEADERS})

target_include_directories(quazip PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
target_link_libraries(quazip PUBLIC Qt${QT_VERSION_MAJOR}::Core)
if(QT_VERSION_MAJOR EQUAL 6)
    # QTextCodec (part of QuaZip's public API) lives in Core5Compat on Qt6.
    find_package(Qt6 REQUIRED COMPONENTS Core5Compat)
    target_link_libraries(quazip PUBLIC Qt6::Core5Compat)
endif()
