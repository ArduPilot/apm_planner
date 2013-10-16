#!/bin/sh
if [ -d apmplanner2.app ]
then
    echo "remove old app package"
    rm -rf apmplanner2.app/
fi
qmake -spec macx-g++ -config release "CONFIG += x86_64" qgroundcontrol.pro
make
