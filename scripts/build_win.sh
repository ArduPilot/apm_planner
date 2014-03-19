#!/bin/sh

QMAKE=/mnt/michaelc/mxe/usr/i686-pc-mingw32/qt/bin/qmake
QTDIR=/mnt/michaelc/QtWin32
NSIS=/mnt/michaelc/mxe/usr/bin/i686-pc-mingw32-makensis

cd ..
$QMAKE -spec win32-x-g++ qgroundcontrol.pro
make clean
make
cd deploy
$NSIS -DQTDIR=$QTDIR apm_installer_crossplat.nsi
