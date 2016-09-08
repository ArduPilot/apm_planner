# -------------------------------------------------
# QGroundControl - Micro Air Vehicle Groundstation
# Please see our website at <http://qgroundcontrol.org>
# Maintainer:
# Lorenz Meier <lm@inf.ethz.ch>
# (c) 2009-2011 QGroundControl Developers
# This file is part of the open groundstation project
# QGroundControl is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# QGroundControl is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with QGroundControl. If not, see <http://www.gnu.org/licenses/>.
# -------------------------------------------------

QMAKE_POST_LINK += $$quote(echo "Copying files")

#
# Copy the application resources to the associated place alongside the application
#

# Dev note: Used to move "files","data","sik_uploader", and "qml".
# Fow now only the last two

LinuxBuild {
    DESTDIR_COPY_RESOURCE_LIST = $$DESTDIR
    QMAKE_POST_LINK += && $$QMAKE_COPY_DIR $$BASEDIR/qml $$DESTDIR_COPY_RESOURCE_LIST
    QMAKE_POST_LINK += && $$QMAKE_COPY_DIR $$BASEDIR/sik_uploader $$DESTDIR_COPY_RESOURCE_LIST
}

MacBuild {
    DESTDIR_COPY_RESOURCE_LIST = $$DESTDIR/$${TARGET}.app/Contents/MacOS
    QMAKE_POST_LINK += && $$QMAKE_COPY_DIR $$BASEDIR/qml $$DESTDIR_COPY_RESOURCE_LIST
    QMAKE_POST_LINK += && $$QMAKE_COPY_DIR $$BASEDIR/sik_uploader $$DESTDIR_COPY_RESOURCE_LIST
}

WindowsCrossBuild {
    DESTDIR_COPY_RESOURCE_LIST = $$DESTDIR
    QMAKE_POST_LINK += && $$QMAKE_COPY_DIR $$BASEDIR/qml $$DESTDIR_COPY_RESOURCE_LIST
    QMAKE_POST_LINK += && $$QMAKE_COPY_DIR $$BASEDIR/sik_uploader $$DESTDIR_COPY_RESOURCE_LIST
}

# Windows version of QMAKE_COPY_DIR of course doesn't work the same as Mac/Linux. It will only
# copy the contents of the source directory. It doesn't create the top level source directory
# in the target.
WindowsBuild {
    # Make sure to keep both side of this if using the same set of directories
    DESTDIR_COPY_RESOURCE_LIST = $$replace(DESTDIR,"/","\\")
    BASEDIR_COPY_RESOURCE_LIST = $$replace(BASEDIR,"/","\\")
    QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY_DIR \"$$BASEDIR_COPY_RESOURCE_LIST\\qml\" \"$$DESTDIR_COPY_RESOURCE_LIST\\qml\"
    QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY_DIR \"$$BASEDIR_COPY_RESOURCE_LIST\\sik_uploader\" \"$$DESTDIR_COPY_RESOURCE_LIST\\sik_uploader\"
}

#
# Perform platform specific setup
#

LinuxBuild {
        #Installer section
        BINDIR = $$PREFIX/bin
        DATADIR =$$PREFIX/share

        DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"

        #MAKE INSTALL - copy files
        INSTALLS += target radioup datafiles desktopLink menuLink

        target.path =$$BINDIR

        radioup.path = $$BINDIR
        radioup.files += $$BASEDIR/sik_uploader

        datafiles.path = $$DATADIR/APMPlanner2
        datafiles.files += $$BASEDIR/files
        datafiles.files += $$BASEDIR/data
        datafiles.files += $$BASEDIR/qml

        #fix up file permissions. Bit of a hack job
        #permFolders.path = $$DATADIR/APMPlanner2
        #permFolders.commands += find $$DATADIR -type d -exec chmod 755 {} \\;
        #permFiles.path = $$DATADIR/APMPlanner2
        #permFiles.commands += find $$DATADIR -type f -exec chmod 644 {} \\;

        #create menu links
        desktopLink.path = $$DATADIR/menu
        desktopLink.files += $$BASEDIR/debian/apmplanner2
        menuLink.path = $$DATADIR/applications
        menuLink.files += $$BASEDIR/debian/apmplanner2.desktop
}

MacBuild {
    # Copy libraries and frameworks into app package
    QMAKE_POST_LINK += && $$QMAKE_COPY_DIR -L $$BASEDIR/libs/lib/Frameworks $$DESTDIR/$${TARGET}.app/Contents/Frameworks
    QMAKE_POST_LINK += && $$QMAKE_COPY_DIR -L $$[QT_INSTALL_PREFIX]/qml/QtQuick $$DESTDIR/$${TARGET}.app/Contents/MacOS/qml/QtQuick
    QMAKE_POST_LINK += && $$QMAKE_COPY_DIR -L $$[QT_INSTALL_PREFIX]/qml/QtQuick.2 $$DESTDIR/$${TARGET}.app/Contents/MacOS/qml/QtQuick.2

    # SDL Framework
    QMAKE_POST_LINK += && install_name_tool -change "@rpath/SDL2.framework/Versions/A/SDL2" "@executable_path/../Frameworks/SDL2.framework/Versions/A/SDL2" $$DESTDIR/$${TARGET}.app/Contents/MacOS/$${TARGET}
}

WindowsCrossBuild {

    # Copy dependencies
    DebugBuild: DLL_QT_DEBUGCHAR = "d"
    ReleaseBuild: DLL_QT_DEBUGCHAR = ""

    COPY_FILE_LIST = \
        $$BASEDIR/libs/lib/sdl/win32/SDL2.dll \
        $$BASEDIR/libs/thirdParty/libxbee/lib/libxbee.dll \
        $$[QT_INSTALL_PREFIX]/bin/libgcc_s_dw2-1.dll \
        $$[QT_INSTALL_PREFIX]/bin/libwinpthread-1.dll \
        $$[QT_INSTALL_PREFIX]/bin/libstdc++-6.dll

    for(COPY_FILE, COPY_FILE_LIST) {
        QMAKE_POST_LINK += && $$QMAKE_COPY $$COPY_FILE $$DESTDIR_COPY_RESOURCE_LIST
    }

    DEPLOY_TARGET = $$DESTDIR/$${TARGET}.exe
    QMAKE_POST_LINK += && windeployqt --no-compiler-runtime --qmldir=$${BASEDIR}/qml $${DEPLOY_TARGET}
}

WindowsBuild {

    BASEDIR_WIN = $$replace(BASEDIR,"/","\\")
    DESTDIR_WIN = $$replace(DESTDIR,"/","\\")

    # Copy dependencies
    DebugBuild: DLL_QT_DEBUGCHAR = "d"
    ReleaseBuild: DLL_QT_DEBUGCHAR = ""

    COPY_FILE_LIST = \
        $$BASEDIR_WIN\\libs\\thirdParty\\libxbee\\lib\\libxbee.dll \

    contains(QT_ARCH, i386) {
        COPY_FILE_LIST += $$BASEDIR_WIN\\libs\\lib\\sdl\\msvc\\lib\\x86\\SDL2.dll
    }else {
        COPY_FILE_LIST += $$BASEDIR_WIN\\libs\\lib\\sdl\\msvc\\lib\\x64\\SDL2.dll
    }

    for(COPY_FILE, COPY_FILE_LIST) {
        QMAKE_POST_LINK += $$escape_expand(\\n) $$quote($$QMAKE_COPY "$$COPY_FILE" "$$DESTDIR_WIN")
    }

    ReleaseBuild {
        # Copy Visual Studio DLLs
        # Note that this is only done for release because the debugging versions of these DLLs cannot be redistributed.
        win32-msvc2010 {
                QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"C:\\Windows\\System32\\msvcp100.dll\"  \"$$DESTDIR_WIN\"
                QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"C:\\Windows\\System32\\msvcr100.dll\"  \"$$DESTDIR_WIN\"
        }
        else:win32-msvc2012 {
                QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"C:\\Windows\\System32\\msvcp110.dll\"  \"$$DESTDIR_WIN\"
                QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"C:\\Windows\\System32\\msvcr110.dll\"  \"$$DESTDIR_WIN\"
        }
        else:win32-msvc2013 {
                QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"C:\\Windows\\System32\\msvcp120.dll\"  \"$$DESTDIR_WIN\"
                QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"C:\\Windows\\System32\\msvcr120.dll\"  \"$$DESTDIR_WIN\"
        }
        else {
                error("Visual studio version not supported, installation cannot be completed.")
        }
    }
    DEPLOY_TARGET = $$shell_quote($$shell_path($$DESTDIR_WIN\\$${TARGET}.exe))
    QMAKE_POST_LINK += $$escape_expand(\\n) windeployqt --no-compiler-runtime --qmldir=$${BASEDIR_WIN}\\qml $${DEPLOY_TARGET}
}
