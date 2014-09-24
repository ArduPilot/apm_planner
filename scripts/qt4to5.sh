#!/bin/bash

QT_VERSION="5.2.1"

rm /usr/bin/qmake
echo "Creating a new link"
if ln -s ${HOME}/Qt${QT_VERSION}/${QT_VERSION}/clang_64/bin/qmake /usr/bin/qmake
then
	echo Switched to Version
	qmake -v
else
	echo "failed to create link"
fi
