#!/bin/bash

#run inside the /Script dir
cd ../

NOW=$(date +"%Y%m%d")

#build APM Planner
qmake-qt4 PREFIX=~/Documents/APMPlanner2-$NOW/usr qgroundcontrol.pro
make --jobs=3
make install

#Create folder structure
mkdir ~/Documents/APMPlanner2-$NOW/DEBIAN
install -m 755 -d ~/Documents/APMPlanner2-$NOW/usr/share/doc/APMPlanner2

#copy deb support files
cp -r -f ./scripts/debian/control ~/Documents/APMPlanner2-$NOW/DEBIAN/control
install -m 644 ./license.txt ~/Documents/APMPlanner2-$NOW/usr/share/doc/APMPlanner2/copyright
install -m 644 ./scripts/debian/changelog ~/Documents/APMPlanner2-$NOW/usr/share/doc/APMPlanner2/changelog
install -m 755 ./scripts/debian/postinst ~/Documents/APMPlanner2-$NOW/DEBIAN/postinst
install -m 755 ./scripts/debian/postrm ~/Documents/APMPlanner2-$NOW/DEBIAN/postrm
gzip -9 ~/Documents/APMPlanner2-$NOW/usr/share/doc/APMPlanner2/changelog

#create the pacakge and check compliance (report.txt)
fakeroot dpkg-deb -b ~/Documents/APMPlanner2-$NOW
lintian ~/Documents/APMPlanner2-$NOW.deb > ~/Documents/report.txt



