#!/bin/bash

#run inside the /Script dir
cd ../../

#build APM Planner
qmake-qt4 ./apm_planner/qgroundcontrol.pro
make --jobs=3

#Create folder structure
NOW=$(date +"%Y%m%d")
mkdir apmplanner-$NOW
mkdir ./apmplanner-$NOW/debian
mkdir ./apmplanner-$NOW/usr
mkdir ./apmplanner-$NOW/usr/bin
mkdir -p ./apmplanner-$NOW/usr/share/apmplanner2
mkdir -p ./apmplanner-$NOW/usr/share/doc/apmplanner2
mkdir -p ./apmplanner-$NOW/usr/share/menu

#copy files over
cp -r -f ./release/apmplanner2 ./apmplanner-$NOW/usr/bin
cp -r -f ./release/data ./apmplanner-$NOW/usr/share/apmplanner2/data
cp -r -f ./release/files ./apmplanner-$NOW/usr/share/apmplanner2/files
cp -r -f ./release/qml ./apmplanner-$NOW/usr/share/apmplanner2/qml

#copy deb support files
cp -r -f ./apm_planner/scripts/debian/control ./apmplanner-$NOW/debian/control
cp -f ./apm_planner/scripts/debian/copyright ./apmplanner-$NOW/usr/share/doc/apmplanner2
cp -f ./apm_planner/scripts/debian/changelog ./apmplanner-$NOW/usr/share/doc/apmplanner2/changelog
cp -r -f ./apm_planner/scripts/debian/postinst ./apmplanner-$NOW/debian/postinst
cp -r -f ./apm_planner/scripts/debian/postrm ./apmplanner-$NOW/debian/postrm
cp -f ./apm_planner/scripts/debian/apmplanner2 ./apmplanner-$NOW/usr/share/menu/apmplanner2
gzip -9 ./apmplanner-$NOW/usr/share/doc/apmplanner2/changelog

#make symbolic links to folders
cd ./apmplanner-$NOW/usr/bin
ln -s -r ../share/apmplanner2/files
ln -s -r ../share/apmplanner2/data
ln -s -r ../share/apmplanner2/qml
cd ../../../

#strip symbols from the binary
strip -s  ./apmplanner-$NOW/usr/bin/apmplanner2
mv ./apmplanner-$NOW/debian ./apmplanner-$NOW/DEBIAN

#fix up permissions
find ./apmplanner-$NOW/usr -type d -exec chmod 755 {} \;
find ./apmplanner-$NOW/usr -type f -exec chmod 644 {} \;

#create the pacakge and check compliance (report.txt)
fakeroot dpkg-deb -b ./apmplanner-$NOW
lintian apmplanner-$NOW.deb > report.txt



