APM Planner 2.0
===============

**CI Build:**  [![Build](https://github.com/ArduPilot/apm_planner/actions/workflows/build.yml/badge.svg)](https://github.com/ArduPilot/apm_planner/actions/workflows/build.yml)

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

APM Planner builds against **Qt 6** (preferred) and still supports Qt 5 for a
gradual transition. The CMake build auto-detects whichever Qt you point it at
via `CMAKE_PREFIX_PATH`.

Install Qt 6 (preferred)
------------------------
Preferred using "homebrew" https://brew.sh to install.

```
brew install qt cmake ninja sdl2
```

Build APM Planner (Qt 6)
------------------------

Using CMake

```
cd $HOME
git clone https://github.com/ArduPilot/apm_planner.git
cd apm_planner
mkdir build
cd build
cmake -G Ninja .. -DCMAKE_PREFIX_PATH="$(brew --prefix qt);$(brew --prefix)"
ninja
```

Once the build finishes, the app bundle is created in the `build` directory. Launch it with:

```
open apmplanner2.app
```

Build for Qt 5
--------------

Qt 5 is end-of-life but still supported. Install it with homebrew:

```
brew install qt@5 cmake ninja sdl2
brew link qt5 --force
```

NOTE: you may need to unlink qt6 `brew unlink qt6`

<details>
<summary>Alternatives</summary>

There are alternative ways to get Qt 5.15.x

- TARBALL, download the source from https://download.qt.io/official_releases/qt/5.15
- Qt Offline Installers - NOTE: The last one available is stuck at Qt5.15.2 :-(
- Using `aqt installer` - NOTE: Also stuck at 5.15.2

I suggest using the internet if you want to install this way. Brew is easyist IMHO!
</details>

Build with CMake, pointing `CMAKE_PREFIX_PATH` at the Qt 5 keg:

```
cd $HOME
git clone https://github.com/ArduPilot/apm_planner.git
cd apm_planner
mkdir build
cd build
cmake -G Ninja .. -DCMAKE_PREFIX_PATH="$(brew --prefix qt@5);$(brew --prefix)"
ninja
open apmplanner2.app
```

Using QMake (Qt 5 only)

```
cd $HOME
git clone https://github.com/ArduPilot/apm_planner.git
qmake -spec macx-clang apm_planner.pro
make -j$(nproc)
open ./release/apmplanner2.app
```

Linux 
=====

Install the required packages:
---------------------------------
<details>
<summary>Packages needed on Ubuntu 25.04</summary>
```
sudo apt-get update
sudo apt-get install git build-essential
sudo apt-get install qt5-qmake qtbase5-dev qtscript5-dev libqt5serialport5-dev libqt5svg5-dev \
    libqt5opengl5-dev qml-module-qtquick-controls libsdl1.2-dev libsndfile1-dev flite1-dev libssl-dev \
	libudev-dev libsdl2-dev
```
</details>

<details>
<summary>Packages needed on Ubuntu 22.04 LTS</summary>

(**NOTE:** On Ubuntu 22.04 use only the native (ubuntu) Qt version 5.15.3 as the official Qt 5.15.2 which 
comes with the Qt Maintenance tool only supports OpenSSL 1.1.1 which is not supported by Ubuntu 22.04 anymore.)

```
sudo apt-get update
sudo apt-get install git build-essential
sudo apt-get install qt5-qmake qtbase5-dev qtscript5-dev libqt5webkit5-dev libqt5serialport5-dev \
    libqt5svg5-dev libqt5opengl5-dev qml-module-qtquick-controls libsdl1.2-dev libsndfile1-dev flite1-dev \
	libssl-dev libudev-dev libsdl2-dev
```
</details>

<details>
<summary>Packages needed on Ubuntu 18.04 LTS</summary>

```
sudo apt-get update
sudo apt-get install git build-essential
sudo apt-get install qt5-qmake qt5-default qtscript5-dev libqt5webkit5-dev libqt5serialport5-dev \
  libqt5svg5-dev qtdeclarative5-qtquick2-plugin libqt5opengl5-dev qml-module-qtquick-controls libsdl1.2-dev \
  libsndfile-dev flite1-dev libssl-dev libudev-dev libsdl2-dev python-serial python-pexpect
```
</details>

<details><summary>Packages needed on Fedora (tested against Fedora 21)</summary>

```
sudo yum update
sudo yum install qt-devel qt5-qtscript-devel qt5-qtwebkit-devel qt5-qtserialport-devel qt5-qtsvg-devel \
  qt5-qtdeclarative-devel qt5-qtquick1-devel pyserial python-pexpect

sudo yum install SDL-devel libsndfile-devel flite-devel openssl-devel libudev-devel SDL2-devel
```
</details>

Building APM Planner:
---------------------
Using CMake

```
cd $HOME
git clone https://github.com/ArduPilot/apm_planner.git
cd apm_planner
mkdir build
cd build
cmake cmake -G Ninja .. 
./apmplanner2 # Runs the application
```


Using QMake

```
cd $HOME
git clone https://github.com/ArduPilot/apm_planner.git
cd apm_planner
qmake apm_planner.pro
make -j$(nproc)
./release/apmplanner2 # Runs the application
```

Windows
-------

APM Planner builds on Windows with CMake + Ninja against **Qt 6** (preferred) or
**Qt 5**, the same as the other platforms.

> **NOTE:** the Windows build is not yet covered by CI and has not been verified
> as thoroughly as macOS/Linux. Reports (and fixes) are welcome.

Install the prerequisites:

* **Qt** — use the [online Qt installer](https://www.qt.io/download-open-source).
  Select the MSVC 64-bit component for your chosen series (e.g. *Qt 6.8 MSVC2022
  64-bit*, or *Qt 5.15 MSVC2019 64-bit*).
* **Visual Studio** (2019 or 2022) with the *Desktop development with C++*
  workload — this provides the MSVC compiler, CMake and Ninja.
* **SDL2** — the Qt installer does not ship it; install via
  [`vcpkg`](https://vcpkg.io) (`vcpkg install sdl2:x64-windows`) and point
  `CMAKE_PREFIX_PATH` at it, or drop the SDL2 development files somewhere on the
  prefix path.

From a *Developer Command Prompt / PowerShell for VS* (so the MSVC toolchain is
on `PATH`):

```
git clone https://github.com/ArduPilot/apm_planner.git
cd apm_planner
mkdir build
cd build
cmake -G Ninja .. -DCMAKE_PREFIX_PATH="C:/Qt/6.8.1/msvc2022_64"
ninja
```

For a Qt 5 build, point `CMAKE_PREFIX_PATH` at the Qt 5 kit instead, e.g.
`-DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64"`. Separate multiple prefixes
(e.g. Qt plus vcpkg) with `;`.

Run the resulting `apmplanner2.exe` from the `build` directory. To gather the Qt
runtime DLLs next to the executable for distribution, use `windeployqt`:

```
windeployqt --qmldir ..\qml apmplanner2.exe
```

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
