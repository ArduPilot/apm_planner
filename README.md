APM Planner

Project:
http://github.com/diydrones/apm_planner

Files:
https://github.com/diydrones/apm_planner

Credits:
http://planner2.ardupilot.com/credits-and-contributors/


Documentation
=============
see http://planner2.ardupilot.com


Mac OS X
========

To build on Mac OS X (10.6 or later):

Install SDL
-----------
1) Download SDL from:  <http://www.libsdl.org/release/SDL-1.2.14.dmg>

2) From the SDL disk image, copy the `sdl.framework` bundle to `/Library/Frameworks` directory (if you are not an admin copy to `~/Library/Frameworks`)

Install QT
-----------
1) Download Qt 4.8.x (Does not currently work with Qt5.x)
   - you can verify the version by typing 'qmake -v' and it should report Qt 4.8.x as the version. 

2) Double click the package installer

Build APM Planner
--------------------
1) From the terminal go to the `apm_planner` directory created when you cloned the repository.

2) Run `qmake -spec macx-g++ qgroundcontrol.pro`

3) Run `make -j8`


Linux 
=====

Building on Linux (tested against Ubuntu 13.10):

1) Install the required packages:

```
sudo apt-get install phonon libqt4-dev libqt4-opengl-dev \
 libphonon-dev libphonon4 phonon-backend-gstreamer \
 qtcreator libsdl1.2-dev libflite1 flite1-dev build-essential \
 libopenscenegraph-dev libssl-dev libqt4-opengl-dev libudev-dev \
 libsndfile1-dev libqt4-sql-sqlite
```

2) Clone the repository in your workspace:

```
cd ~/workspace
git clone https://github.com/diydrones/apm_planner
```

3) Build APM Planner:

```
cd ~/workspace/apm_planner
qmake-qt4 qgroundcontrol.pro
make
```

Or try `qmake qgroundcontrol.pro` if the `qmake-qt4` command doesn't exist on your version of Ubuntu. This will only work if the Qt version install on your machine is Qt4.8.x, this can be checked using `qmake -v'

4) Run APM Planner:

```
./release/apmplanner2
```

5) Permanent installation (optional, if you'd like to install APM Planner in a fixed location)
 
There are two ways to do this:
a) You can build a .deb using ```scripts/LinuxBuildPackage.sh```, and then install the deb via ```dpkg -i ~/Documents/APMPlanner2-$NOW.deb``` (where $NOW is today's date). This should add it to your launcher too.

b) Alternatively, run ```sudo make install```. This will place the binary in your /bin/ folder and corresponding files in /share/.

Windows
=======

You have three options in Windows. These are listed from easiest to most difficult

* Option 1: VS2010 should work with makefiles and nmake.
* Option 2: Mingw: This should work, no guarantees.
* Option 3: VS2012 requires a recompile of Qt, but otherwise should work with makefiles and nmake. This will not be described here.


Option 1: VS2010
-------

1. Download MSVc2010 express edition. You can get it from the Microsoft website
2. Download Qt libraries for windows (4.8.6, for MSVC2010) from http://qt-project.org/downloads
3. Download and install QtCreator
4. Download and install msysgit http://msysgit.github.io/
    - Open up git bash
    - git clone git://github.com/diydrones/apm_planner.git
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
        * Find qmake.exe (typically in c:\Qt\4.8.6\bin\qmake.exe)
        * click Apply
    - Kits tab:
        * Click Add, name it Qt 4.8.6 - MSVC
        * Select the MSVC compiler, and Qt version you just created.
        * click Apply
    - Click Ok
6. QtCreator is now configured.
7. Click on File then Open, find qgroundcontrol.pro
    - It will ask you to configure project, you want to make sure Qt 4.8.6 - MSVC is selected, and click "Configure Project"
8. Go to "Projects" tab on the left hand side, deselect "Shadow Build".
9. Build->Build qgroundcontrol
10. Run and enjoy!



Option 2: MinGW
-------

Install everything to a path where there are NO spaces. This is important.

1. Download Mingw 4.8.2 from http://sourceforge.net/projects/mingwbuilds/ (mingw-builds-install.exe)
    - Install with options: x32 4.8.1 posix dwarf rev5
2. Download and install Qt libraries for Windows (4.8.6, for MINGW 4.8.2) from http://qt-project.org/downloads
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
        * Find qmake.exe (typically in c:\Qt\4.8.6\bin\qmake.exe)
        * click Apply
    - Kits tab:
        * Click Add, name it Qt 4.8.6 - MinGW
        * Select the compiler, debugger, and Qt version you just created.
        * click Apply
        * Click Ok
6. QtCreator is now configured.
7. Click on File then Open, find qgroundcontrol.pro
    - It will ask you to configure project, you want to make sure Qt 4.8.6 - MinGW is selected, and click "Configure Project"
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
