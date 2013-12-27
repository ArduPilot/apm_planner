#!/bin/bash

#run inside the /Script dir
cd ../

NOW=$(date +"%Y%m%d")

#build APM Planner
qmake-qt4 PREFIX=~/Documents/APMPlanner-$NOW/usr qgroundcontrol.pro
make --jobs=3
make install

#Create folder structure
mkdir ~/Documents/APMPlanner-$NOW/DEBIAN
install -m 755 -d ~/Documents/APMPlanner-$NOW/usr/share/doc/APMPlanner

#copy deb support files
cp -r -f ./scripts/debian/control ~/Documents/APMPlanner-$NOW/DEBIAN/control
install -m 644 ./license.txt ~/Documents/APMPlanner-$NOW/usr/share/doc/APMPlanner/copyright
install -m 644 ./scripts/debian/changelog ~/Documents/APMPlanner-$NOW/usr/share/doc/APMPlanner/changelog
install -m 755 ./scripts/debian/postinst ~/Documents/APMPlanner-$NOW/DEBIAN/postinst
install -m 755 ./scripts/debian/postrm ~/Documents/APMPlanner-$NOW/DEBIAN/postrm
gzip -9 ~/Documents/APMPlanner-$NOW/usr/share/doc/APMPlanner/changelog

#create the pacakge and check compliance (report.txt)
fakeroot dpkg-deb -b ~/Documents/APMPlanner-$NOW
lintian ~/Documents/APMPlanner-$NOW.deb > ~/Documents/report.txt



