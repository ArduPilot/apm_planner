!include "MUI2.nsh"
!define MUI_ICON "..\files\APMIcons\ap_rc.ico"

!include x64.nsh

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

Section "APM Planner 2 files" APM_FILES

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

Section "JIT Debugger" JIT_DEBUGGER
  SetOutPath $INSTDIR
  File "../drmingw.exe"
  File "../exchndl.dll"
  File "../mgwhelp.dll"
  ExecWait "$INSTDIR\drmingw.exe -i -a"
SectionEnd 

Section "OpenSSL" OPENSSL
  NSISdl::download http://firmware.diydrones.com/Tools/APMPlanner/supportinstalls/Win32OpenSSL_Light-1_0_0l.exe Win32OpenSSL_Light-1_0_0l.exe
  ExecWait "Win32OpenSSL_Light-1_0_0l.exe /verysilent /sp-"
SectionEnd

section "64-bit Drivers" DRIVERS_64
  SetOutPath $INSTDIR\Drivers
  File "../Drivers/Arduino MEGA 2560.cat"
  File "../Drivers/Arduino MEGA 2560.inf"
  File ../Drivers/dpinst.xml
  File ../Drivers/DPInstx64.exe
  File ../Drivers/DPInstx86.exe
  File ../Drivers/px4fmu_2.0.0.3.cat
  File ../Drivers/px4fmu_2.0.0.3.inf
  ExecWait "$INSTDIR\Drivers\DPInstx64.exe /LM /SW"
sectionEND

section "32-bit Drivers" DRIVERS_32
  SetOutPath $INSTDIR\Drivers
  File "../Drivers/Arduino MEGA 2560.cat"
  File "../Drivers/Arduino MEGA 2560.inf"
  File ../Drivers/dpinst.xml
  File ../Drivers/DPInstx64.exe
  File ../Drivers/DPInstx86.exe
  File ../Drivers/px4fmu_2.0.0.3.cat
  File ../Drivers/px4fmu_2.0.0.3.inf
  ExecWait "$INSTDIR\Drivers\DPInstx86.exe /LM /SW"
sectionEND

Section "Qt components" QT_FILES
  !include qt_install.list
  SetOutPath $INSTDIR
  File ${QTDIR}\lib\zlib1.dll
  File ${QTDIR}\lib\libgcc_s_sjlj-1.dll
  File ${QTDIR}\lib\libstdc++-6.dll
  File ${QTDIR}\lib\libwinpthread-1.dll
  File ${QTDIR}\lib\QtCore4.dll
  File ${QTDIR}\lib\QtGui4.dll
  File ${QTDIR}\lib\QtScript4.dll
  File ${QTDIR}\lib\QtMultimedia4.dll
  File ${QTDIR}\lib\QtDeclarative4.dll
  File ${QTDIR}\lib\phonon4.dll
  File ${QTDIR}\lib\QtNetwork4.dll
  File ${QTDIR}\lib\QtOpenGL4.dll
  File ${QTDIR}\lib\QtSql4.dll
  File ${QTDIR}\lib\QtSvg4.dll
  File ${QTDIR}\lib\QtTest4.dll
  File ${QTDIR}\lib\QtWebkit4.dll
  File ${QTDIR}\lib\QtXml4.dll
  File ${QTDIR}\lib\QtXmlPatterns4.dll

SectionEnd

Section "Uninstall"
  SetShellVarContext all
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
  Delete $INSTDIR\QtScript4.dll
  Delete $INSTDIR\QtDeclarative4.dll
  Delete $INSTDIR\zlib1.dll
  ExecWait "$INSTDIR\drmingw.exe -u"
  Delete $INSTDIR\drmingw.exe"
  Delete $INSTDIR\exchndl.dll"
  Delete $INSTDIR\mgwhelp.dll"
  Delete $APPDATA\diydrones\*.*
  RMDir $APPDATA\diydrones
  Delete $INSTDIR\avrdude\*.*
  RMDir $INSTDIR\avrdude
  Delete $INSTDIR\Drivers\*.*
  RMDir $INSTDIR\Drivers
  Delete $INSTDIR\mavlink\*.*
  RMDir /R $INSTDIR\mavlink
  Delete $INSTDIR\uploader\*.*
  RMDir $INSTDIR\uploader
  Delete $INSTDIR\Win32OpenSSL_Light-1_0_0l.exe

  RMDir $INSTDIR
  SetShellVarContext all
  Delete "$SMPROGRAMS\APMPlanner2\*.*"
  RMDir "$SMPROGRAMS\APMPlanner2"
SectionEnd

Section "Create Start Menu Shortcuts"
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\APMPlanner2"
  CreateShortCut "$SMPROGRAMS\APMPlanner2\uninstall.lnk" "$INSTDIR\APMPlanner2_uninstall.exe" "" "$INSTDIR\APMPlanner2_uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\APMPlanner2\APMPlanner2.lnk" "$INSTDIR\apmplanner2.exe" "" "$INSTDIR\apmplanner2.exe" 0
SectionEnd

Function .onInit
  #Enable the proper sections, and set selection for JIT to off
  ${If} ${RunningX64}
    SectionSetFlags ${DRIVERS_32}  ${SF_RO}
    SectionSetFlags ${DRIVERS_64} ${SF_SELECTED}
  ${Else}
    SectionSetFlags ${DRIVERS_64} ${SF_RO}
    SectionSetFlags ${DRIVERS_32}  ${SF_SELECTED}
  ${EndIf}
  SectionSetFlags ${APM_FILES} ${SF_SELECTED}
  SectionSetFlags ${JIT_DEBUGGER} 0
  SectionSetFlags ${QT_FILES} ${SF_SELECTED}
  SectionSetFlags ${OPENSSL} ${SF_SELECTED}
FunctionEnd
