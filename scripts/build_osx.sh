#!/bin/sh
echo "Build Step"
qmake -spec macx-g++ -config release "CONFIG += x86_64" qgroundcontrol.pro
make
