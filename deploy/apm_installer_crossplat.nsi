
/******************************************************************************
    WORKAROUND - lnkX64IconFix
        This snippet was developed to address an issue with Windows 
        x64 incorrectly redirecting the shortcuts icon from $PROGRAMFILES32 
        to $PROGRAMFILES64.
 
    See Forum post: http://forums.winamp.com/newreply.php?do=postreply&t=327806
 
    Example:
        CreateShortcut "$SMPROGRAMS\My App\My App.lnk" "$INSTDIR\My App.exe" "" "$INSTDIR\My App.exe"
        ${lnkX64IconFix} "$SMPROGRAMS\My App\My App.lnk"
 
    Original Code by Anders [http://forums.winamp.com/member.php?u=70852]
 ******************************************************************************/
!ifndef ___lnkX64IconFix___
    !verbose push
    !verbose 0
 
    !include "LogicLib.nsh"
    !include "x64.nsh"
 
    !define ___lnkX64IconFix___
    !define lnkX64IconFix `!insertmacro _lnkX64IconFix`
    !macro _lnkX64IconFix _lnkPath
        !verbose push
        !verbose 0
        ${If} ${RunningX64}
            DetailPrint "WORKAROUND: 64bit OS Detected, Attempting to apply lnkX64IconFix"
            Push "${_lnkPath}"
            Call lnkX64IconFix
        ${EndIf}
        !verbose pop
    !macroend
 
    Function lnkX64IconFix ; _lnkPath
        Exch $5
        Push $0
        Push $1
        Push $2
        Push $3
        Push $4
        System::Call 'OLE32::CoCreateInstance(g "{00021401-0000-0000-c000-000000000046}",i 0,i 1,g "{000214ee-0000-0000-c000-000000000046}",*i.r1)i'
        ${If} $1 <> 0
            System::Call '$1->0(g "{0000010b-0000-0000-C000-000000000046}",*i.r2)'
            ${If} $2 <> 0
                System::Call '$2->5(w r5,i 2)i.r0'
                ${If} $0 = 0
                    System::Call '$1->0(g "{45e2b4ae-b1c3-11d0-b92f-00a0c90312e1}",*i.r3)i.r0'
                    ${If} $3 <> 0
                        System::Call '$3->5(i 0xA0000007)i.r0'
                        System::Call '$3->6(*i.r4)i.r0'
                        ${If} $0 = 0 
                            IntOp $4 $4 & 0xffffBFFF
                            System::Call '$3->7(ir4)i.r0'
                            ${If} $0 = 0 
                                System::Call '$2->6(i0,i0)'
                                DetailPrint "WORKAROUND: lnkX64IconFix Applied successfully"
                            ${EndIf}
                        ${EndIf}
                        System::Call $3->2()
                    ${EndIf}
                ${EndIf}
                System::Call $2->2()
            ${EndIf}
            System::Call $1->2()
        ${EndIf} 
        Pop $4
        Pop $3
        Pop $2
        Pop $1
        Pop $0
    FunctionEnd
    !verbose pop
!endif

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
!system 'python apm_installer.py ${QTDIR}\qml qt_install_qml.list qt_uninstall_qml.list n'

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
  File ../libs/lib/sdl/win32/SDL2.dll
  File ../libs/thirdParty/libxbee/lib/libxbee.dll
  SetOutPath $INSTDIR\avrdude
  File ../avrdude/avrdude.exe
  File ../avrdude/libusb0.dll
  File ../avrdude/avrdude.conf
  SetOutPath $INSTDIR\uploader
  File ../uploader/px4uploader.exe
  File ../uploader/Ionic.Zip.Reduced.dll
  File ../uploader/BouncyCastle.Crypto.dll
  File ../uploader/AP2OTPCheck.exe
  File ../uploader/validcertificates.xml

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
  SetOutPath $INSTDIR
  File vcredist_x86.exe
  DetailPrint "Installing MSVC2008 runtime"
  ExecWait "$INSTDIR/vcredist_x86.exe /q /norestart"
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
  SetOutPath $INSTDIR\qml
  !include qt_install_qml.list
  SetOutPath $INSTDIR
  File ${QTDIR}\bin\libgcc_s_dw2-1.dll
  File ${QTDIR}\bin\libstdc++-6.dll
  File ${QTDIR}\bin\libwinpthread-1.dll
  File ${QTDIR}\bin\Qt5WebKitWidgets.dll
  File ${QTDIR}\bin\Qt5MultimediaWidgets.dll
  File ${QTDIR}\bin\Qt5Multimedia.dll
  File ${QTDIR}\bin\Qt5Gui.dll
  File ${QTDIR}\bin\Qt5Core.dll
  File ${QTDIR}\bin\icuin52.dll
  File ${QTDIR}\bin\icuuc52.dll
  File ${QTDIR}\bin\icudt52.dll
  File ${QTDIR}\bin\Qt5Network.dll
  File ${QTDIR}\bin\Qt5Widgets.dll
  File ${QTDIR}\bin\Qt5OpenGL.dll
  File ${QTDIR}\bin\Qt5PrintSupport.dll
  File ${QTDIR}\bin\Qt5WebKit.dll
  File ${QTDIR}\bin\Qt5Quick.dll
  File ${QTDIR}\bin\Qt5Qml.dll
  File ${QTDIR}\bin\Qt5Sql.dll
  File ${QTDIR}\bin\Qt5Positioning.dll
  File ${QTDIR}\bin\Qt5Sensors.dll
  File ${QTDIR}\bin\Qt5Declarative.dll
  File ${QTDIR}\bin\Qt5XmlPatterns.dll
  File ${QTDIR}\bin\Qt5Xml.dll
  File ${QTDIR}\bin\Qt5Script.dll
  File ${QTDIR}\bin\Qt5Svg.dll
  File ${QTDIR}\bin\Qt5Test.dll
  File ${QTDIR}\bin\Qt5SerialPort.dll

  File ${QTDIR}\bin\libbz2-1.dll
  File ${QTDIR}\bin\libfreetype-6.dll
  File ${QTDIR}\bin\libglib-2.0-0.dll
  File ${QTDIR}\bin\libharfbuzz-0.dll
  File ${QTDIR}\bin\libiconv-2.dll
  File ${QTDIR}\bin\libintl-8.dll
  File ${QTDIR}\bin\libjpeg-8.dll
  File ${QTDIR}\bin\liblzma-5.dll
  File ${QTDIR}\bin\libpcre16-0.dll
  File ${QTDIR}\bin\libpng16-16.dll
  File ${QTDIR}\bin\libsqlite3-0.dll
  File ${QTDIR}\bin\libwebp-5.dll
  File ${QTDIR}\bin\libxml2-2.dll
  File ${QTDIR}\bin\libxslt-1.dll
  File ${QTDIR}\bin\zlib1.dll

SectionEnd

Section "Uninstall"
  SetShellVarContext all
  !include apm_uninstall.list
  !include qml_uninstall.list
  !include qt_uninstall.list
  !include qt_uninstall_qml.list
  Delete $INSTDIR\apmplanner2.exe
  Delete $INSTDIR\SDL2.dll
  Delete $INSTDIR\libxbee.dll
  Delete $INSTDIR\APMPlanner2_uninstall.exe

  Delete $INSTDIR\libbz2-1.dll
  Delete $INSTDIR\libfreetype-6.dll
  Delete $INSTDIR\libglib-2.0-0.dll
  Delete $INSTDIR\libharfbuzz-0.dll
  Delete $INSTDIR\libiconv-2.dll
  Delete $INSTDIR\libintl-8.dll
  Delete $INSTDIR\libjpeg-8.dll
  Delete $INSTDIR\liblzma-5.dll
  Delete $INSTDIR\libpcre16-0.dll
  Delete $INSTDIR\libpng16-16.dll
  Delete $INSTDIR\libsqlite3-0.dll
  Delete $INSTDIR\libwebp-5.dll
  Delete $INSTDIR\libxml2-2.dll
  Delete $INSTDIR\libxslt-1.dll
  Delete $INSTDIR\zlib1.dll



  Delete $INSTDIR\libgcc_s_dw2-1.dll
  Delete $INSTDIR\libstdc++-6.dll
  Delete $INSTDIR\libwinpthread-1.dll
  Delete $INSTDIR\Qt5WebKitWidgets.dll
  Delete $INSTDIR\Qt5MultimediaWidgets.dll
  Delete $INSTDIR\Qt5Multimedia.dll
  Delete $INSTDIR\Qt5Gui.dll
  Delete $INSTDIR\Qt5Core.dll
  Delete $INSTDIR\icuin52.dll
  Delete $INSTDIR\icuuc52.dll
  Delete $INSTDIR\icudt52.dll
  Delete $INSTDIR\Qt5Network.dll
  Delete $INSTDIR\Qt5Widgets.dll
  Delete $INSTDIR\Qt5OpenGL.dll
  Delete $INSTDIR\Qt5PrintSupport.dll
  Delete $INSTDIR\Qt5WebKit.dll
  Delete $INSTDIR\Qt5Quick.dll
  Delete $INSTDIR\Qt5Qml.dll
  Delete $INSTDIR\Qt5Sql.dll
  Delete $INSTDIR\Qt5Positioning.dll
  Delete $INSTDIR\Qt5Sensors.dll
  Delete $INSTDIR\Qt5Declarative.dll
  Delete $INSTDIR\Qt5XmlPatterns.dll
  Delete $INSTDIR\Qt5Xml.dll
  Delete $INSTDIR\Qt5Script.dll
  Delete $INSTDIR\Qt5Svg.dll
  Delete $INSTDIR\Qt5Test.dll
  Delete $INSTDIR\Qt5SerialPort.dll
  Delete $INSTDIR\vcredist_x86.exe


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
  CreateShortCut "$SMPROGRAMS\APMPlanner2\uninstall.lnk" "$INSTDIR\APMPlanner2_uninstall.exe" "" "$INSTDIR\\APMPlanner2_uninstall.exe" 0
  ${lnkX64IconFix} "$SMPROGRAMS\APMPlanner2\uninstall.lnk"
  CreateShortCut "$SMPROGRAMS\APMPlanner2\APMPlanner2.lnk" "$INSTDIR\apmplanner2.exe" "" "$INSTDIR\\apmplanner2.exe" 0
  ${lnkX64IconFix} "$SMPROGRAMS\APMPlanner2\APMPlanner2.lnk"
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
