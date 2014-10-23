#!/bin/bash
if [ $# -eq 0 ]
then
	QT_VERSION="5.2.1"
	QT_PATH=Qt${QT_VERSION}/${QT_VERSION}
	BASE_DIR="/Developer/"
else
	QT_VERSION=$1
	QT_PATH=Qt${1}/${1:0:3}
	BASE_DIR="/Developer/"
fi

rm /usr/bin/qmake
echo "Creating a new link to" ${BASE_DIR}/${QT_PATH}/clang_64/bin/qmake
if ln -s ${BASE_DIR}/${QT_PATH}/clang_64/bin/qmake /usr/bin/qmake
then
	echo Switched to Version
	qmake -v
else
	echo "failed to create link"
fi
