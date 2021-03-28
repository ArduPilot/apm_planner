#!/bin/bash

# stop in case of error
set -e 
set -x

SOURCEPATH="/home/build/planner"

{ cd "$SOURCEPATH"

    # build apm planner
    qmake apm_planner.pro -spec linux-g++-64
    make -j$(nproc)

    # create the appimage
    # appimages do not work in docker due to the lack of fuse. So we simply extract the images
    "$SOURCEPATH/linuxdeployqt-x86_64.AppImage" --appimage-extract

    # setup path
    QTPATH="/opt/qt512"
    BINPATH="$SOURCEPATH/release"
    LINUXDEPLOY="$SOURCEPATH/squashfs-root/usr/bin/linuxdeployqt"
    BUILDINGPATH="$SOURCEPATH/AppImageBuild"
    DESKTOPFILE="/home/build/appimage/apmplanner2.desktop"
    METADATAFILE="/home/build/appimage/apmplanner2.appdata.xml"

   #----------------------------------------------

    export VERSION=2.0.28-rc1

    # Clean if output already exists
    rm -rf "$BUILDINGPATH"

    # Create directory structure for AppImage
    install -d "$BUILDINGPATH/usr/bin"
    install -d "$BUILDINGPATH/usr/lib"
    install -d "$BUILDINGPATH/usr/share/applications"
    install -d "$BUILDINGPATH/usr/share/icons/hicolor/16x16/apps"
    install -d "$BUILDINGPATH/usr/share/icons/hicolor/32x32/apps"
    install -d "$BUILDINGPATH/usr/share/icons/hicolor/128x128/apps"
    install -d "$BUILDINGPATH/usr/share/icons/hicolor/256x256/apps"
    install -d "$BUILDINGPATH/usr/share/icons/hicolor/512x512/apps"

    # copy icons
    cp "$SOURCEPATH/files/APMIcons/icon.iconset/icon_16x16.png" "$BUILDINGPATH/usr/share/icons/hicolor/16x16/apps/"
    cp "$SOURCEPATH/files/APMIcons/icon.iconset/icon_32x32.png" "$BUILDINGPATH/usr/share/icons/hicolor/32x32/apps/"
    cp "$SOURCEPATH/files/APMIcons/icon.iconset/icon_128x128.png" "$BUILDINGPATH/usr/share/icons/hicolor/128x128/apps/"
    cp "$SOURCEPATH/files/APMIcons/icon.iconset/icon_256x256.png" "$BUILDINGPATH/usr/share/icons/hicolor/256x256/apps/"
    cp "$SOURCEPATH/files/APMIcons/icon.iconset/icon_512x512.png" "$BUILDINGPATH/usr/share/icons/hicolor/512x512/apps/"

    # copy all parts of the release consisting of qml folder sik_uploader folder and the binary itself
    cp -R "$BINPATH/." "$BUILDINGPATH/usr/bin"

    # copy desktop file
    cp "$DESKTOPFILE" "$BUILDINGPATH/usr/share/applications/"

    # build the appimage
    $LINUXDEPLOY "$BUILDINGPATH/usr/share/applications/apmplanner2.desktop" -appimage -qmake="$QTPATH/bin/qmake" -extra-plugins=iconengines,platformthemes/libqgtk3.so -qmldir="$BINPATH/qml/"

}
