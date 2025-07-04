APM Planner 2.0
===============

**Travis CI Build:**  [![Build Status](https://travis-ci.org/ArduPilot/apm_planner.svg?branch=master)](https://travis-ci.org/ArduPilot/apm_planner)

**Support:**
[ardupilot.com Support Forum for APM Planner 2.0](https://discuss.ardupilot.org/c/ground-control-software/apm-planner-2-0/)

Support is handled in the forum, we keep issues here known problems and enhancements

**Developer Chat:** 

[APN Planner 2 on ArduPilot Discord Server](https://discordapp.com/channels/674039678562861068/1389950623155355708)

[ArduPilot Discord Server](https://discord.com/channels/674039678562861068/)

**Credits:**

Maintainer: [Bill Bonney](https://github.com/billbonney)

Contributors: Michael Carpenter, [Arne-W](https://github.com/Arne-W) et al.

https://ardupilot.org/planner2/docs/credits-and-contributors.html

Documentation
=============
see https://ardupilot.org/planner2/

Mac OS X
========

To build on Mac OS X (10.6 or later):

Install QT
-----------
Preferred using "homebrew" https://brew.sh to install.

```
brew install qt@5
brew link qt5 --force
```

NOTE: you may need to unlink qt6 `brew unlink qt6`

<details>
<summary>Alternative</summary>

1) Download Qt 5.x or greater (Does not work with Qt4.x)
   - you can verify the version by typing 'qmake -v' and it should report Qt 5.4.2 or greater as the version.
   - NOTE: Tested with Qt5.9.3 wth success

2) Double click the package installer
</details>

Build APM Planner
--------------------
1) From the terminal go to the `apm_planner` directory created when you cloned the repository.

2) Run `qmake -spec macx-clang apm_planner.pro` (old version `qmake -spec macx-g++ apm_planner.pro`)

3) Run `make -j8`

4) Run `open ./release/apmplanner2.app`


Linux 
=====

Install the required packages:
---------------------------------
<details>
<summary>Packages needed on Ubuntu 25.04</summary>
```
sudo apt-get update
sudo apt-get install qt5-qmake qtbase5-dev qtscript5-dev libqt5serialport5-dev libqt5svg5-dev libqt5opengl5-dev qml-module-qtquick-controls
sudo apt-get install git libsdl1.2-dev libsndfile1-dev flite1-dev libssl-dev libudev-dev libsdl2-dev
```
</details>

<details>
<summary>Packages needed on Ubuntu 22.04 LTS</summary>

(**NOTE:** On Ubuntu 22.04 use only the native (ubuntu) Qt version 5.15.3 as the official Qt 5.15.2 which comes with the Qt Maintenance tool only supports OpenSSL 1.1.1 which is not supported by Ubuntu 22.04 anymore.)

```
sudo apt-get update
sudo apt-get install qt5-qmake qtbase5-dev qtscript5-dev libqt5webkit5-dev libqt5serialport5-dev libqt5svg5-dev libqt5opengl5-dev qml-module-qtquick-controls
sudo apt-get install git libsdl1.2-dev libsndfile1-dev flite1-dev libssl-dev libudev-dev libsdl2-dev
```
</details>

<details>
<summary>Packages needed on Ubuntu 18.04 LTS</summary>

```
sudo apt-get update
sudo apt-get install qt5-qmake qt5-default \
  qtscript5-dev libqt5webkit5-dev libqt5serialport5-dev \
  libqt5svg5-dev qtdeclarative5-qtquick2-plugin \
  libqt5opengl5-dev qml-module-qtquick-controls
sudo apt-get install git libsdl1.2-dev  libsndfile-dev \
  flite1-dev libssl-dev libudev-dev libsdl2-dev python-serial python-pexpect
```
</details>

<details><summary>Packages needed on Fedora (tested against Fedora 21)</summary>

```
sudo yum update
sudo yum install qt-devel qt5-qtscript-devel \
  qt5-qtwebkit-devel qt5-qtserialport-devel qt5-qtsvg-devel \
  qt5-qtdeclarative-devel qt5-qtquick1-devel pyserial python-pexpect

sudo yum install SDL-devel libsndfile-devel \
  flite-devel openssl-devel libudev-devel SDL2-devel
```
</details>

Clone the repository in your workspace:
------------------------------------------

```
cd ~/workspace
git clone https://github.com/diydrones/apm_planner
```

Build APM Planner:
---------------------

```
cd ~/workspace/apm_planner
qmake apm_planner.pro
make
```

Run APM Planner:
-------------------

```
./release/apmplanner2
```

Windows
-------
NOTE: Needs Updating

<details>
<summary>Build on Windows Platform</summary>
 
To build on Windows there are two options:
* Option 1: Visual Studio 2013 native compile
    * Download and install [Visual Studio 2013 express](http://www.visualstudio.com/downloads/download-visual-studio-vs#d-express-windows-desktop)
* Option 2: MinGW cross-compile

Install Qt with the [online Qt installer](http://www.qt.io/download-open-source): 
* You will be presented with a list of Qt versions and compiler options to install
* You can install mulitple versions and compilers beside one another and choose which to use later 
* Select any one (or mulitple) of the following options, 
	* Qt 5.5 MSVC2013 32-bit
	* Qt 5.5 MSVC2013 64-bit
	* Qt 5.5 MinGW 4.9.2 32-bit (also select the same version of MinGW under Tools)

Configure QtCreator:
* The installer is pretty smart but it's good to double check everything was setup corretly
* Start QtCreator
    * Click on the *Tools* menu item then *Options*
    * Select *Build & Run* on the left hand side
    * Look at the *Compilers* tab
        * Under *Auto-detected* should be a list of compilers installed, such as:
            * Microsoft Visual C++ Compiler 12.0 (x86)
            * Microsoft Visual C++ Compiler 12.0 (amd64)
            * MinGW 4.9.2 32bit
        * If using MSVC there will be a few others listed as well but that is normal
    * Look at the *Qt Versions* Tab:
        * Under *Auto-detected* should be a list of the Qt versions you installed earlier:
		    * Qt 5.5.1 MSVC2013 32bit
		    * Qt 5.5.1 MSVC2013 32bit
		    * Qt 5.5.1 MinGW 32bit
		* If your desired Qt versions is not listed, or you installed one after the initial setup:
			* Click Add
			* Find the qmake.exe for the version you want
			    * For example: c:/Qt/5.5/msvc2013/bin/qmake.exe
			    * For example: c:/Qt/5.5/mingw492_32/bin/qmake.exe
			* Click Apply
    * Look under the *Kits* tab:
        * Under *Auto-detected* should be a list of the appropriate kits:
            * Desktop Qt 5.5.1 MSVC2013 32bit
            * Desktop Qt 5.5.1 MSVC2013 63bit
            * Desktop Qt 5.5.1 MinGB 32bit
		* If a kit with your desired Qt versions and/or compiler is not listed, or you installed a new Qt version or compiler after the initial setup:
			* Click *Add*, give it a nice name (like Qt 5.5.1 MSVC 32bit)
			* Select the desired compiler from the drop down
			* Select the Qt version (with matching compiler) from the drop down
			* Click Apply
    - Click *Ok* at the bottom of the window
* QtCreator is now configured for fun

Build APM Planner 2.0:
* Start QtCreator (if not already)
* Click on *File* then *Open File or Project*
* Find qgroundcontrol.pro, then click *Open*
    * The first time will ask you to configure project
    * Select the desired version (same list of Kits from above) 
    * Click *Configure Project*
* Go to *Projects* tab on the left hand side
    * Select the "Shadow Build" checkbox
    * Browse to a location where you want the application to build to
* From the *Build* drop down select *Build Project qgroundcontrol* (or Ctrl+B)
* Run the generated apmplanner2.exe and enjoy!

Installing this compiled version: 
* To Do

</details>

Alternatives:
-------------

NOTE: Needs Updating

<details>
<summary>Permanent installation</summary>

Permanent Installation
-----------------------------------------------------------------------------------------------

NOTE: (optional, if you'd like to install APM Planner in a fixed location)

There are two ways to do this:

a) Using Debuild:
```
cd ~/workspace/apm_planner/debian
./gitlog-to-changelog.pl > changelog

cd ..
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
</details>

<details>
<summary>OpenBSD</summary>
	
To build on OpenBSD 6.2
-------------------
1) install dependencies

```
pkg_add qt5 libsndfile sdl sdl2 flite py-serial py-pexpect openssl git
```

2) Clone the apm_planner repo

```
git clone git@github.com:ArduPilot/apm_planner.git
```

3) Build the project

```
cd apm_planner
```

```
qmake-qt5 apm_planner.pro
```

```
make
```

If the project compiled without errors you will find the binary in ./release
</details>

Repository Layout
=================
 (2014-3-28: out-of-date, needs to be fixed)
<details><summary>File Layout</summary>

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
	Data for stereo, waypoints and radio calibration. 
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

</details>
