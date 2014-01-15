!include "MUI2.nsh"
!define MUI_ICON "..\files\APMIcons\ap_rc.ico"
Name "APM Planner 2"

!ifndef QTDIR
  !error "Please define QT installation directory via /DQTDIR=/home/michael/QtWin32"
!endif

!system 'python apm_installer.py ../files apm_install.list apm_uninstall.list y'
!system 'python apm_installer.py ../qml qml_install.list qml_uninstall.list y'
!system 'python apm_installer.py ${QTDIR}\plugins qt_install.list qt_uninstall.list n'

OutFile "apmplanner2-installer-win32.exe"

InstallDir $PROGRAMFILES\APMPlanner2

Page license 
Page directory
Page components
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

LicenseData ..\license.txt

Section "APM Planner 2 files"

  SetOutPath $INSTDIR
  File ../release/apmplanner2.exe
  !include apm_install.list
  !include qml_install.list
  ;File /r ..\release\*.*
  ;File /r ..\libs\mavlink
  ;File /r ..\files
  SetOutPath $INSTDIR
  File ../libs/lib/sdl/win32/SDL.dll
  File ../libs/thirdParty/libxbee/lib/libxbee.dll
  SetOutPath $INSTDIR\avrdude
  File ../avrdude/avrdude.exe
  File ../avrdude/libusb0.dll
  File ../avrdude/avrdude.conf
  SetOutPath $INSTDIR\uploader
  File ../uploader/px4uploader.exe
  File ../uploader/Ionic.Zip.Reduced.dll
  File ../uploader/BouncyCastle.Crypto.dll

  WriteUninstaller $INSTDIR\APMPlanner2_uninstall.exe
SectionEnd 

Section /o "JIT Debugger"
  SetOutPath $INSTDIR
  File "../drmingw.exe"
  File "../exchndl.dll"
  File "../mgwhelp.dll"
  nsExec::Exec "$INSTDIR\drmingw.exe -i -a"
SectionEnd 

Section "Qt components"
  !include qt_install.list
  SetOutPath $INSTDIR
  File /home/michael/QtWin32/lib/zlib1.dll
  File /home/michael/QtWin32/lib/libgcc_s_sjlj-1.dll
  File /home/michael/QtWin32/lib/libstdc++-6.dll
  File /home/michael/QtWin32/lib/libwinpthread-1.dll
  File /home/michael/QtWin32/lib/QtCore4.dll
  File /home/michael/QtWin32/lib/QtGui4.dll
  File /home/michael/QtWin32/lib/QtScript4.dll
  File /home/michael/QtWin32/lib/QtMultimedia4.dll
  File /home/michael/QtWin32/lib/QtDeclarative4.dll
  File /home/michael/QtWin32/lib/phonon4.dll
  File /home/michael/QtWin32/lib/QtNetwork4.dll
  File /home/michael/QtWin32/lib/QtOpenGL4.dll
  File /home/michael/QtWin32/lib/QtSql4.dll
  File /home/michael/QtWin32/lib/QtSvg4.dll
  File /home/michael/QtWin32/lib/QtTest4.dll
  File /home/michael/QtWin32/lib/QtWebkit4.dll
  File /home/michael/QtWin32/lib/QtXml4.dll
  File /home/michael/QtWin32/lib/QtXmlPatterns4.dll
SectionEnd

Section "Uninstall"
  !include apm_uninstall.list
  !include qml_uninstall.list
  !include qt_uninstall.list
  Delete $INSTDIR\apmplanner2.exe
  Delete $INSTDIR\SDL.dll
  Delete $INSTDIR\libxbee.dll
  Delete $INSTDIR\APMPlanner2_uninstall.exe
  Delete $INSTDIR\libgcc_s_sjlj-1.dll
  Delete $INSTDIR\libstdc++-6.dll
  Delete $INSTDIR\libwinpthread-1.dll
  Delete $INSTDIR\QtCore4.dll
  Delete $INSTDIR\QtGui4.dll
  Delete $INSTDIR\QtMultimedia4.dll
  Delete $INSTDIR\phonon4.dll
  Delete $INSTDIR\QtNetwork4.dll
  Delete $INSTDIR\QtOpenGL4.dll
  Delete $INSTDIR\QtSql4.dll
  Delete $INSTDIR\QtSvg4.dll
  Delete $INSTDIR\QtTest4.dll
  Delete $INSTDIR\QtWebkit4.dll
  Delete $INSTDIR\QtXml4.dll
  Delete $INSTDIR\QtXmlPatterns4.dll
  Delete $INSTDIR\zlib1.dll
  nsExec::Exec "$INSTDIR\drmingw.exe -u"
  Delete $INSTDIR\drmingw.exe"
  Delete $INSTDIR\exchndl.dll"
  Delete $INSTDIR\mgwhelp.dll"
  Delete $APPDATA\diydrones\*.*
  RMDir $APPDATA\diydrones

  ;Delete $INSTDIR\*.*
  RMDir $INSTDIR
  SetShellVarContext all
  Delete "$SMPROGRAMS\APMPlanner2\*.*"
  RMDir "$SMPROGRAMS\APMPlanner2\"
SectionEnd

Section "Create Start Menu Shortcuts"
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\APMPlanner2"
  CreateShortCut "$SMPROGRAMS\APMPlanner2\uninstall.lnk" "$INSTDIR\APMPlanner2_uninstall.exe" "" "$INSTDIR\APMPlanner2_uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\APMPlanner2\APMPlanner2.lnk" "$INSTDIR\apmplanner2.exe" "" "$INSTDIR\apmplanner2.exe" 0
SectionEnd
