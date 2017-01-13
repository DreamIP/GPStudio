# GPStudio gui-tools
This part need to be compile on your machine

## How to build ??

Packages needed :
### with Qt5 :
	sudo apt-get install g++ libusb-1.0-0-dev qtbase5-dev qtbase5-dev-tools qtscript5-dev libqt5svg5-dev qttools5-dev
### with Qt4 :
	sudo apt-get install g++ libusb-1.0-0-dev libqt4-dev qt4-dev-tools libqtscript4-core

in the root of repo :

	mkdir build
	cd build
	qmake ../src
	make -j9

## Run dependency (Qt5)
 * libc6
 * libgcc1
 * libstdc++6
 * libqt5core5a
 * libqt5gui5
 * libqt5widgets5
 * libqt5network5
 * libqt5script5
 * libqt5xml5

## To cross build to windows
	sudo apt-get install wine mingw-w64 qt4-qmake qt4-dev-tools libqt4-dev libusb-1.0-0-dev g++-multilib libusb-1.0-0-dev:i386

	mkdir build-win
	cd build-win
	/opt/qt-5.5-win32/bin/qmake ../src/gpstudio.pro
	make release -j9
