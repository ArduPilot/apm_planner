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

COPY_RESOURCE_LIST = \
    $$BASEDIR/files \
    $$BASEDIR/qml \
    $$BASEDIR/data \
    $$BASEDIR/sik_uploader
    
WindowsBuild {
	DESTDIR_COPY_RESOURCE_LIST = $$replace(DESTDIR,"/","\\")
    COPY_RESOURCE_LIST = $$replace(COPY_RESOURCE_LIST, "/","\\")
    CONCATCMD = $$escape_expand(\\n)
}

LinuxBuild {
    DESTDIR_COPY_RESOURCE_LIST = $$DESTDIR
    CONCATCMD = &&
}

MacBuild {
    DESTDIR_COPY_RESOURCE_LIST = $$DESTDIR/$${TARGET}.app/Contents/MacOS
    CONCATCMD = &&
}
    
for(COPY_DIR, COPY_RESOURCE_LIST):QMAKE_POST_LINK += $$CONCATCMD $$QMAKE_COPY_DIR $${COPY_DIR} $$DESTDIR_COPY_RESOURCE_LIST

#
# Perform platform specific setup
#
message(QTDIR $$[QT_INSTALL_PREFIX])
MacBuild {
    # Copy libraries and frameworks into app package
    QMAKE_POST_LINK += && $$QMAKE_COPY_DIR -L $$BASEDIR/libs/lib/Frameworks $$DESTDIR/$${TARGET}.app/Contents/Frameworks
    QMAKE_POST_LINK += && $$QMAKE_COPY_DIR -L $$[QT_INSTALL_PREFIX]/qml/QtQuick.2 $$DESTDIR/$${TARGET}.app/Contents/MacOS/qml/QtQuick.2

    # SDL Framework
    QMAKE_POST_LINK += && install_name_tool -change "@rpath/SDL2.framework/Versions/A/SDL2" "@executable_path/../Frameworks/SDL2.framework/Versions/A/SDL2" $$DESTDIR/$${TARGET}.app/Contents/MacOS/$${TARGET}
}

WindowsBuild {
	# Copy dependencies
	BASEDIR_WIN = $$replace(BASEDIR,"/","\\")
	DESTDIR_WIN = $$replace(DESTDIR,"/","\\")

    QMAKE_POST_LINK += $$escape_expand(\\n) $$quote($$QMAKE_COPY_DIR "$$(QTDIR)\\plugins" "$$DESTDIR_WIN")

    COPY_FILE_DESTDIR = $$DESTDIR_WIN
	DebugBuild: DLL_QT_DEBUGCHAR = "d"
    ReleaseBuild: DLL_QT_DEBUGCHAR = ""
    COPY_FILE_LIST = \
        $$BASEDIR_WIN\\libs\\lib\\sdl\\win32\\SDL.dll \
        $$BASEDIR_WIN\\libs\\thirdParty\\libxbee\\lib\\libxbee.dll \
        $$(QTDIR)\\bin\\Qt5WebKitWidgets$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5MultimediaWidgets$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5Multimedia$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5Gui$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5Core$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\icuin51.dll \
        $$(QTDIR)\\bin\\icuuc51.dll \
        $$(QTDIR)\\bin\\icudt51.dll \
        $$(QTDIR)\\bin\\Qt5Network$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5Widgets$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5OpenGL$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5PrintSupport$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5WebKit$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5Quick$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5Qml$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5Sql$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5Positioning$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5Sensors$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5Declarative$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5XmlPatterns$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5Xml$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5Script$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5Svg$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5Test$${DLL_QT_DEBUGCHAR}.dll \
        $$(QTDIR)\\bin\\Qt5SerialPort$${DLL_QT_DEBUGCHAR}.dll

    for(COPY_FILE, COPY_FILE_LIST) {
        QMAKE_POST_LINK += $$escape_expand(\\n) $$quote($$QMAKE_COPY "$$COPY_FILE" "$$COPY_FILE_DESTDIR")
    }

	ReleaseBuild {
		QMAKE_POST_LINK += $$escape_expand(\\n) $$quote(del /F "$$DESTDIR_WIN\\$${TARGET}.exp")

		# Copy Visual Studio DLLs
		# Note that this is only done for release because the debugging versions of these DLLs cannot be redistributed.
		# I'm not certain of the path for VS2008, so this only works for VS2010.
		win32-msvc2010 {
			QMAKE_POST_LINK += $$escape_expand(\\n) $$quote(xcopy /D /Y "\"C:\\Program Files \(x86\)\\Microsoft Visual Studio 10.0\\VC\\redist\\x86\\Microsoft.VC100.CRT\\*.dll\""  "$$DESTDIR_WIN\\")
		}
	}
}

LinuxBuild {
        #Installer section
        BINDIR = $$PREFIX/bin
        DATADIR =$$PREFIX/share

        DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"

        #MAKE INSTALL - copy files
        INSTALLS += target datafiles desktopLink menuLink permFolders permFiles

        target.path =$$BINDIR

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
