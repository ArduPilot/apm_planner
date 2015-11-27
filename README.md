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
