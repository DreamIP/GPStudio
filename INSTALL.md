# GPStudio installation

Important, if you are using a zip or tar package, you need to add the
bin directory to your PATH. On windows, add also thirdparts/ and 
thirdparts/php.

You may need a distribution of quartus 13.1 for dreamcam.

## Dependencies

* make (with bin in your path) to generate your projects
* php5-cli (with php bin in your path) to execute command lines tools and 
back-end generation
* Qt (with qmake bin in your path) Qt 4.7 or more recent version (Qt5 
supported) for GUI tools
* quartus (with bin in your path) if you want to use Atlera branded FPGA
[Quartus download](https://www.altera.com/downloads/download-center.html)
* libusb for USB camera comunication

For PHP7, you need to install : php7.0-mbstring php7.0-zip php7.0-xml

    sudo apt-get install php7.0-mbstring php7.0-zip php7.0-xml

Optionnal :

* graphviz (optional) to draw process connection in command line mode with dot

> sudo apt-get install make php5-cli 

If you use php7, add the folowing package :

> sudo apt-get install php7.0-gd

## Devices rights
To avoid to lauch viewer and programmer with root privilege, you need to
copy rules from share/ directory to /etc/udev/rules.d/. This is done by
install.sh.

## GUI tools compilation

### Command line mode

To compile gpviewer and gpnode_gui, open a terminal in GPStudio root 
directory and just type:

> make gui-tools

In case it does not work, check if you have a distribution of Qt:

> qmake -v

To install Qt5 with apt (linux Debian/Ubuntu):

> sudo apt-get install g++ libusb-1.0-0-dev qtbase5-dev qtbase5-dev-tools

You may also need:

> sudo apt-get install qtscript5-dev libqt5svg5-dev qttools5-dev

To install Qt4 with apt (old Debian/Ubuntu versions):

> sudo apt-get install g++ libusb-1.0-0-dev libqt4-dev

You may also need:

> sudo apt-get qt4-dev-tools libqtscript4-core

You also can download it from <https://www.qt.io/download-open-source/>

### Graphical mode

Download a Qt package with Qt Creator at <https://www.qt.io/download-open-source/>
and install it.

Open Qt Creator and open the project gui-tools/src/gpstudio.pro.

Configure the project to be compiled in gui-tools/build. (important)

Click on compile and run or Ctrl + R. Enjoy.
