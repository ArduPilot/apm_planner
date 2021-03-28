#!/bin/bash

# Stop in case of error
set -e 
set -x

USER=$(id -u)
GROUP=$(id -g)

SOURCEDIR=$PWD/../..
LINUXDEPLOY=linuxdeployqt-x86_64.AppImage

wget https://github.com/probonopd/linuxdeployqt/releases/download/7/linuxdeployqt-7-x86_64.AppImage -O $SOURCEDIR/$LINUXDEPLOY

chmod +x $SOURCEDIR/$LINUXDEPLOY
chmod +x $SOURCEDIR/$LINUXDEPLOYQT

# build and start docker
docker build -t apm-planner-build:latest .

docker run --rm --user $USER:$GROUP -v $SOURCEDIR/:/home/build/planner -it apm-planner-build:latest
