APM Planner 2.0
===============

Project:
http://github.com/diydrones/apm_planner

Files:
https://github.com/diydrones/apm_planner

Credits:
http://planner2.ardupilot.com/credits-and-contributors/

Developer Chat: https://gitter.im/diydrones/apm_planner

[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/diydrones/apm_planner?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

[Waffle.io](https://waffle.io/diydrones/apm_planner) Progress Graph

[![Throughput Graph](https://graphs.waffle.io/diydrones/apm_planner/throughput.svg)](https://waffle.io/diydrones/apm_planner/metrics)

Documentation
=============
see http://planner2.ardupilot.com

Mac OS X
========

To build on Mac OS X (10.6 or later):

Install QT
-----------
1) Download Qt 5.2.1 or greater (Does not work with Qt4.x)
   - you can verify the version by typing 'qmake -v' and it should report Qt 5.2.1 or greater as the version. 

2) Double click the package installer

Build APM Planner
--------------------
1) From the terminal go to the `apm_planner` directory created when you cloned the repository.

2) Run `qmake -spec macx-g++ qgroundcontrol.pro`

3) Run `make -j8`


Linux 
=====

Building on Linux (tested against Ubuntu 14.04 LTS):

1) Install the required packages: 

Be sure to run apt-get update first

```
sudo apt-get update
sudo apt-get install git qt5-qmake qt5-default \
  qtscript5-dev libqt5webkit5-dev libqt5serialport5-dev \
  libqt5svg5-dev libsdl1.2-dev  libsndfile-dev \
  flite1-dev libssl-dev libudev-dev libsdl2-dev
```

2) Clone the repository in your workspace:

```
cd ~/workspace
git clone https://github.com/diydrones/apm_planner
```

3) Build APM Planner:

```
cd ~/workspace/apm_planner
qmake qgroundcontrol.pro
make
```

4) Run APM Planner:

```
./release/apmplanner2
```

5) Permanent installation (optional, if you'd like to install APM Planner in a fixed location)
 
There are two ways to do this:

a) Using Debuild:
```
cd ~/workspace/apm_planner
debuild -us -uc -d
```
Then install the deb via 
```
dpkg -i ~/workspace/apmplanner2_2.0.XX_YYYY.deb
```
Where XX is the version number, and YYY is your architecture. This method should add it to your launcher too.

If it does not install due to dependancies, you can resolve them with
```
sudo apt-get install -f
````
Then attempt to install again.

b) Using make:
```
cd ~/workspace/apm_planner
sudo make install
```
This will place the binary in your /bin/ folder and corresponding files in /share/

Windows
=======

You have three options in Windows. These are listed from easiest to most difficult

* Option 1: VS2010 should work with makefiles and nmake.
* Option 2: Mingw: This should work, no guarantees.
* Option 3: VS2012 requires a recompile of Qt, but otherwise should work with makefiles and nmake. This will not be described here.


Option 1: VS2010 [TODO: Update for Qt5]
-------

1. Download MSVc2010 express edition (You need SP1). You can get it from the Microsoft website
2. Download Qt libraries for windows (5.2.1, for MSVC2010(TODO?)) from http://qt-project.org/downloads
3. Download and install QtCreator
4. Download and install msysgit http://msysgit.github.io/
    - Open up git bash
    - git clone git://github.com/diydrones/apm_planner.git
    - Ensure that msysgit's bin folder is in your path, as it has touch.exe which is needed for the build
5. Start QtCreator and configure QtCreator:
    - click on the Tools menu item then Options
    - Select Build & Run on the left hand side
    - Compilers tab
        * Ensure Microsoft visual c++ Compiler 10.0 shows up under Auto-Detected
        * click Add and select MinGW
        * Find where you installed your compiler, inside the bin directory select g++.exe
        * Click apply
    - Qt Versions Tab:
        * click Add
        * Find qmake.exe (typically in c:\Qt\5.2.1\bin\qmake.exe)
        * click Apply
    - Kits tab:
        * Click Add, name it Qt 5.2.1 - MSVC
        * Select the MSVC compiler, and Qt version you just created.
        * click Apply
    - Click Ok
6. QtCreator is now configured.
7. Click on File then Open, find qgroundcontrol.pro
    - It will ask you to configure project, you want to make sure Qt 5.2.1 - MSVC is selected, and click "Configure Project"
8. Go to "Projects" tab on the left hand side, deselect "Shadow Build".
9. Build->Build qgroundcontrol
10. Run and enjoy!



Option 2: MinGW [TODO: update for Qt5]
-------

Install everything to a path where there are NO spaces. This is important.

1. Download Mingw 5.2.1 from http://sourceforge.net/projects/mingwbuilds/ (mingw-builds-install.exe)
    - Install with options: x32 5.2.1 posix dwarf rev5
2. Download and install Qt libraries for Windows (5.2.1, for MINGW 5.2.1) from http://qt-project.org/downloads
3. Download and install QtCreator
4. Download and install msysgit http://msysgit.github.io/
    - Open up Git Bash
    - git clone git://github.com/diydrones/apm_planner.git
5. Start QtCreator and configure QtCreator:
    - click on the Tools menu item then Options
    - Select Build & Run on the left hand side
    - Compilers tab
        * click Add and select MinGW
        * Find where you installed your compiler, inside the bin directory select g++.exe
        * Click apply
    - Debuggers Tab:
        * Click Add
        * Find gdb.exe, it will be in the same folder as g++.exe
        * Click Apply
    - Qt Versions Tab:
        * click Add
        * Find qmake.exe (typically in c:\Qt\5.2.1\bin\qmake.exe)
        * click Apply
    - Kits tab:
        * Click Add, name it Qt 5.2.1 - MinGW
        * Select the compiler, debugger, and Qt version you just created.
        * click Apply
        * Click Ok
6. QtCreator is now configured.
7. Click on File then Open, find qgroundcontrol.pro
    - It will ask you to configure project, you want to make sure Qt 5.2.1 - MinGW is selected, and click "Configure Project"
8. Go to "Projects" tab on the left hand side, deselect "Shadow Build".
9. Build->Build qgroundcontrol
10. Run and enjoy!


Repository Layout (2014-3-28: out-of-date, needs to be fixed)
=================
```
qgroundcontrol:
	demo-log.txt
	license.txt 
	qgcunittest.pro - For the unit tests.
	qgcunittest.pro.user
	qgcvideo.pro
	qgroundcontrol.pri - Used by qgroundcontrol.pro
	qgroundcontrol.pro - Project opened in QT to run qgc.
	qgroundcontrol.pro.user 
	qgroundcontrol.qrc - Holds many images.
	qgroundcontrol.rc - line of code to point toward the images
	qserialport.pri - generated by qmake.
	testlog.txt
	testlog2.txt 
	user_config.pri.dist - Custom message specs to be added here. 
data: 
	Maps from yahoo and kinect and earth. 
deploy: 
	Install and uninstall for win32.
	Create a debian packet.
	Create .DMG file for publishing for mac.
	Audio test on mac.	
doc: 
	Doxyfile is in this directory and information for creating html documentation for qgc.
files: 
	Has the audio for the vehicle and data output. 
		ardupilotmega: 
			widgets and tool tips for pilot heading for the fixed wing.
			tooltips for quadrotor
		flightgear:
			Aircraft: 
				Different types of planes and one jeep. 
			Protocol: 
				The protocol for the fixed_wings and quadrotor and quadhil.holds info about the fixed wing yaw, roll etc. 					Quadrotor. Agian holds info about yaw, roll etc.
		Pixhawk:
			Widgets for hexarotor. Widgets and tooltips for quadrotor.
		vehicles: 
			different vehicles. Seems to hold the different kinds of aircrafts as well as files for audio and the hexarotor 			and quadrotor.
		widgets: 
			Has a lot of widgets defined for buttons and sliders.

images: 
	For the UI. Has a bunch of different images such as images for applications or actions or buttons.
lib: 
	SDL is located in this direcotry. 
	Msinttypes: 
		Defines intteger types for microsoft visual studio. 
	sdl:
		Information about the library and to run the library on different platforms. 
mavlink: 
	The files for the library mavlink. 
qgcunittest: 
	Has the unittests for qgc
settings: 
	Parameter lists for alpha, bravo and charlie. 
	Data for stereo, waypoints and radio calibrartion. 
src:
	Code for QGCCore, audio output, configuration, waypoints, main and log compressor.
	apps - Code for mavlink generation and for a video application.
	comm - Code for linking to simulation, mavlink, udp, xbee, opal, flight gear and interface.
	Has other libraries. Qwt is in directory named lib. The other libraries are in libs.
	lib - qwt library
	libs - eigen, opmapcontrol, qestserialport, qtconcurrent, utils.
	input - joystick and freenect code.
	plugins - Qt project for PIXHAWK plugins.
	uas - Ardu pilot, UAS, mavlink factory, uas manager, interface, waypoint manager and slugs.
	ui - Has code for data plots, waypoint lists and window congfiguration. All of the ui code.
thirdParty: 
	Library called lxbee.
	Library called QSerialPort.
```
