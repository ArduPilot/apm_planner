cmake_minimum_required(VERSION 3.14)
project(opmapcontrol LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_definitions(-DEXTERNAL_USE)

# Include directories and dependencies
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/internals
    ${CMAKE_CURRENT_SOURCE_DIR}/src/internals/projections
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mapwidget
)

# Headers
set(HEADERS
    opmapcontrol.h
    src/core/accessmode.h
    src/core/alllayersoftype.h
    src/core/cache.h
    src/core/cacheitemqueue.h
    src/core/debugheader.h
    src/core/diagnostics.h
    src/core/geodecoderstatus.h
    src/core/kibertilecache.h
    src/core/languagetype.h
    src/core/maptype.h
    src/core/memorycache.h
    src/core/opmaps.h
    src/core/placemark.h
    src/core/point.h
    src/core/providerstrings.h
    src/core/pureimage.h
    src/core/pureimagecache.h
    src/core/rawtile.h
    src/core/size.h
    src/core/tilecachequeue.h
    src/core/urlfactory.h
    src/internals/copyrightstrings.h
    src/internals/core.h
    src/internals/debugheader.h
    src/internals/loadtask.h
    src/internals/mousewheelzoomtype.h
    src/internals/pointlatlng.h
    src/internals/pureprojection.h
    src/internals/rectangle.h
    src/internals/rectlatlng.h
    src/internals/sizelatlng.h
    src/internals/tile.h
    src/internals/tilematrix.h
    src/mapwidget/gpsitem.h
    src/mapwidget/homeitem.h
    src/mapwidget/mapgraphicitem.h
    src/mapwidget/mapripform.h
    src/mapwidget/mapripper.h
    src/mapwidget/opmapwidget.h
    src/mapwidget/trailitem.h
    src/mapwidget/traillineitem.h
    src/mapwidget/uavitem.h
    src/mapwidget/uavmapfollowtype.h
    src/mapwidget/uavtrailtype.h
    src/mapwidget/waypointitem.h
    src/internals/projections/lks94projection.h
    src/internals/projections/mercatorprojection.h
    src/internals/projections/mercatorprojectionyandex.h
    src/internals/projections/platecarreeprojection.h
    src/internals/projections/platecarreeprojectionpergo.h
    src/mapwidget/waypointlineitem.h
    src/mapwidget/omapconfiguration.h
    src/mapwidget/graphicsitem.h
    src/mapwidget/graphicsusertypes.h
)

# Sources
set(SOURCES
    src/core/alllayersoftype.cpp
    src/core/cache.cpp
    src/core/cacheitemqueue.cpp
    src/core/diagnostics.cpp
    src/core/kibertilecache.cpp
    src/core/languagetype.cpp
    src/core/memorycache.cpp
    src/core/opmaps.cpp
    src/core/placemark.cpp
    src/core/point.cpp
    src/core/providerstrings.cpp
    src/core/pureimage.cpp
    src/core/pureimagecache.cpp
    src/core/rawtile.cpp
    src/core/size.cpp
    src/core/tilecachequeue.cpp
    src/core/urlfactory.cpp
    src/internals/core.cpp
    src/internals/loadtask.cpp
    src/internals/MouseWheelZoomType.cpp
    src/internals/pointlatlng.cpp
    src/internals/pureprojection.cpp
    src/internals/rectangle.cpp
    src/internals/rectlatlng.cpp
    src/internals/sizelatlng.cpp
    src/internals/tile.cpp
    src/internals/tilematrix.cpp
    src/mapwidget/configuration.cpp
    src/mapwidget/gpsitem.cpp
    src/mapwidget/homeitem.cpp
    src/mapwidget/mapgraphicitem.cpp
    src/mapwidget/mapripform.cpp
    src/mapwidget/mapripper.cpp
    src/mapwidget/opmapwidget.cpp
    src/mapwidget/trailitem.cpp
    src/mapwidget/traillineitem.cpp
    src/mapwidget/uavitem.cpp
    src/mapwidget/waypointitem.cpp
    src/internals/projections/lks94projection.cpp
    src/internals/projections/mercatorprojection.cpp
    src/internals/projections/mercatorprojectionyandex.cpp
    src/internals/projections/platecarreeprojection.cpp
    src/internals/projections/platecarreeprojectionpergo.cpp
    src/mapwidget/waypointlineitem.cpp
)

# Forms
set(FORMS
    src/mapwidget/mapripform.ui
)

# Resources
set(RESOURCES
    src/mapwidget/mapresources.qrc
)

# Library
add_library(opmapcontrol STATIC
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
    ${RESOURCES}
)

target_include_directories(opmapcontrol PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/internals>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/internals/projections>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/mapwidget>
)

target_compile_definitions(opmapcontrol PUBLIC EXTERNAL_USE)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
target_link_libraries(opmapcontrol PUBLIC Qt5::Core Qt5::Widgets)

find_package(Qt5 REQUIRED COMPONENTS Sql OpenGL Svg)
target_link_libraries(opmapcontrol PRIVATE Qt5::Sql Qt5::OpenGL Qt5::Svg)
