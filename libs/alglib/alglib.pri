# ALGLIB math library
message(Adding ALGLIB $$PWD)
INCLUDEPATH += $$PWD
HEADERS +=     $$PWD/src/ap.h \
    $$PWD/src/alglibinternal.h\
    $$PWD/src/interpolation.h \
    $$PWD/src/optimization.h \
    $$PWD/src/linalg.h \
    $$PWD/src/alglibmisc.h \
    $$PWD/src/diffequations.h \
    $$PWD/src/integration.h \
    $$PWD/src/solvers.h \
    $$PWD/src/specialfunctions.cpp

SOURCES +=     $$PWD/src/ap.cpp \
    $$PWD/src/alglibinternal.cpp \
    $$PWD/src/interpolation.cpp \
    $$PWD/src/optimization.cpp \
    $$PWD/src/linalg.cpp \
    $$PWD/src/alglibmisc.cpp \
    $$PWD/src/diffequations.cpp \
    $$PWD/src/integration.cpp \
    $$PWD/src/solvers.cpp \
    $$PWD/src/specialfunctions.cpp
