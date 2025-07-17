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

find_package(Qt5 REQUIRED COMPONENTS Core)
target_link_libraries(quazip PUBLIC Qt5::Core)
