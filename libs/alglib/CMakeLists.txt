cmake_minimum_required(VERSION 3.14)
project(alglib LANGUAGES CXX)

# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Headers
set(HEADERS
    src/ap.h
    src/alglibinternal.h
    src/interpolation.h
    src/optimization.h
    src/linalg.h
    src/alglibmisc.h
    src/diffequations.h
    src/integration.h
    src/solvers.h
    src/specialfunctions.cpp # Unusual: header with .cpp extension, keep as in qmake
)

# Sources
set(SOURCES
    src/ap.cpp
    src/alglibinternal.cpp
    src/interpolation.cpp
    src/optimization.cpp
    src/linalg.cpp
    src/alglibmisc.cpp
    src/diffequations.cpp
    src/integration.cpp
    src/solvers.cpp
    src/specialfunctions.cpp
)

add_library(alglib STATIC
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(alglib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
