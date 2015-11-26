APM Planner 2.0
===============

Project:
http://github.com/diydrones/apm_planner

Files:
https://github.com/diydrones/apm_planner

Credits:
http://planner2.ardupilot.com/credits-and-contributors/

Developer Chat: [![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/diydrones/apm_planner?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

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

(**NOTE:** There is an error in 14.04LTS with libxcb crashing sometimes, try this [fix](https://gist.github.com/slimsag/a26d838ccc4480ce21bc))

1) Install the required packages: 

Be sure to run apt-get update first

```
sudo apt-get update
sudo apt-get install qt5-qmake qt5-default \
  qtscript5-dev libqt5webkit5-dev libqt5serialport5-dev \
  libqt5svg5-dev qtdeclarative5-qtquick2-plugin
sudo apt-get install git libsdl1.2-dev  libsndfile-dev \
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

To build on Windows:
* Both 32 and 64 bit applications are supported, the following steps can be used for either. 
* Install Qt 5.5.1 32bit MSVC2013 with the [Qt installer](http://www.qt.io/download-open-source).
* Install the [Visual Studio 2013 compiler](http://www.visualstudio.com/downloads/download-visual-studio-vs#d-express-windows-desktop).

Check QtCreator configuration:
* Start QtCreator
    - click on the Tools menu item then Options
    - Select Build & Run on the left hand side
    - Compilers tab
        * Ensure Microsoft Visual C++ Compiler 12.0 is listed under Auto-detected
    - Qt Versions Tab:
		* Ensure Qt 5.5.1 MSVC2013 32bit is listed under Auto-detected
		* If Qt 5.5.1 MSVC2013 32bit is not listed:
			- Click Add
			- Find the qmake.exe (typically in c:\Qt\5.5.1\bin\qmake.exe)
			- Click Apply
    - Kits tab:
        * Ensure Desktop Qt 5.5.1 MSVC2013 32bit is listed under Auto-detected
		* If Qt 5.5.1 MSVC2013 32bit is not listed:
			- Click Add, name it Qt 5.5.1 MSVC 32bit
			- Select the MSVC compiler (Microsoft Visual C++ Compiler 12.0 (x86)
			- Select the Qt version (Qt 5.5.1 MSVC2013 32bit)
			- Click Apply
    - Click Ok
* QtCreator is now configured.
* Click on File then Open, find qgroundcontrol.pro
    - It will ask you to configure project, you want to make sure Qt 5.5.1 MSVC 32bit is selected, and click "Configure Project"
* Go to "Projects" tab on the left hand side, ensure "Shadow Build" is selected.
* Build->Build qgroundcontrol
* Run and enjoy!

Installing this compiled version: 
* To Do
