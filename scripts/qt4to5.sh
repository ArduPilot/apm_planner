#!/bin/bash
if [ $# -eq 0 ]
then
	QT_VERSION="5.2.1"
	QT_PATH=${QT_VERSION}/${QT_VERSION}
else
	QT_VERSION=$1
	QT_PATH=${QT_VERSION}/${QT_VERSION:0:3}
fi

rm /usr/bin/qmake
echo "Creating a new link to" ${QT_PATH}
if ln -s ${HOME}/QT_PATH/clang_64/bin/qmake /usr/bin/qmake
then
	echo Switched to Version
	qmake -v
else
	echo "failed to create link"
fi
