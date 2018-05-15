Building for Linux - The Easy Way™
==================================

Here's full instructions on how to build the 'The Easy Way™' see [here](https://discuss.ardupilot.org/t/problems-installing-and-or-building-2-0-20-on-ubuntu-14-04/9819/15?u=billb) for why

Download http://download.qt.io/official_releases/qt/5.9/5.9.3/qt-opensource-linux-x64-5.9.3.run from the Qt website and follow the instructions below in terminal

```
cd ~/Downloads/
chmod +x qt-opensource-linux-x64-5.9.3.run 
./qt-opensource-linux-x64-5.9.3.run

# Make sure you select the Desktop gcc 64bit option to install in the wizard

# Change your setup to use installed version of qmake	
sudo rm /usr/bin/qmake
sudo ln -s $HOME/Qt5.9.3/5.9.3/gcc_64/bin/qmake /usr/bin/qmake

# Build APM Planner 2.0
mkdir ~/workspace
cd ~/workspace/
git clone https://github.com/diydrones/apm_planner
qmake apm_planner.pro 
make -j4 # use -j8 on i7

# Run APM Planner 2.0
./release/apmplanner2
```
