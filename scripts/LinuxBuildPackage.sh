#!/bin/bash

#run inside the /Script dir
cd ../

NOW=$(date +"%Y%m%d")
GIT_VERSION=$(git describe --abbrev=0)

#build APM Planner
qmake PREFIX=~/Documents/APMPlanner2_$GIT_VERSION/usr qgroundcontrol.pro
make --jobs=3
make install

#Create folder structure
mkdir ~/Documents/APMPlanner2_$GIT_VERSION/DEBIAN
install -m 755 -d ~/Documents/APMPlanner2_$GIT_VERSION/usr/share/doc/APMPlanner2

#copy deb support files
cp -r -f ./debian/control ~/Documents/APMPlanner2_$GIT_VERSION/DEBIAN/control
install -m 644 ./license.txt ~/Documents/APMPlanner2_$GIT_VERSION/usr/share/doc/APMPlanner2/copyright
install -m 644 ./debian/changelog ~/Documents/APMPlanner2_$GIT_VERSION/usr/share/doc/APMPlanner2/changelog
install -m 755 ./debian/postinst ~/Documents/APMPlanner2_$GIT_VERSION/DEBIAN/postinst
install -m 755 ./debian/postrm ~/Documents/APMPlanner2_$GIT_VERSION/DEBIAN/postrm
gzip -9 ~/Documents/APMPlanner2_$GIT_VERSION/usr/share/doc/APMPlanner2/changelog

#add version number to control file
sed -i "s/VERSION/$GIT_VERSION/g" ~/Documents/APMPlanner2_$GIT_VERSION/DEBIAN/control

#create the pacakge and check compliance (report.txt)
fakeroot dpkg-deb -b ~/Documents/APMPlanner2_$GIT_VERSION
lintian ~/Documents/APMPlanner2_$GIT_VERSION.deb > ~/Documents/report.txt



