# Change Log

## Future Release [v1.21](https://github.com/DreamIP/GPStudio/tree/v1.21) (2017-02-01)
[Full changelog](https://github.com/DreamIP/GPStudio/compare/v1.20...dev)

### New Feature
* Messages in gpnode to inform end of process
* Tab in GPViewer to send picture to camera

### Changes
* Removed poppler to simplify dependencies

### Bug fixes
* Fixed toolchain windows for cross compile

## Release [v1.20](https://github.com/DreamIP/GPStudio/tree/v1.20) (2017-01-10)
[Full changelog](https://github.com/DreamIP/GPStudio/compare/v1.10...v1.20)

### New Feature
* HWLoop functionnalities (exemple project for test automation on real platform)
* Auto test bench creation
* Multi flows viewers (mask, rect, points...)
* viewers manager
* IP documentation integration in gpnode_gui
* clocks, parameters and flow size editable in graphical mode
* enhanced control on blocks view with gpviewer
* size of flow graphically visible in gpnode_gui

### New IPs
* roi
* gaussian
* sobel
* prewitt
* roberts
* laplacien
* dilate
* erode
* negate
* scharr
* threshold

### Changes
* Improved blocks view and interractions in gpnode_gui
* Improved tutorial, documentation completed

### Bug fixes
* PHP7 compatibility
* gpviewer stabilized on disconnect/connect camera
* gpnode_gui stabilized

## Release [v1.10](https://github.com/DreamIP/GPStudio/tree/v1.10) (2016-09-09)
[Full changelog](https://github.com/DreamIP/GPStudio/compare/v1.00...v1.10)

### New Feature
* Backend component introduction
* Informations (author, version, licence...) ingregrated to IP
* Base components for IP devellopement

### Changes
* gpnode GUI completed for node generation

### Bug fixes
* 

## Release [v1.00](https://github.com/DreamIP/GPStudio/tree/v1.00) (2016-06-15)
[Full changelog](https://github.com/DreamIP/GPStudio/compare/v0.95...v1.00)

### New Feature
* Properties on flows propagated by blocks
* Dynamic viewer adaptation
* Node editor and viewer
* Camera Explorer
* New GUI tool: gpnode (beta)

### Changes
* Complete review of gpviewer
* All propertymap expressed in global context in node_generated.xml

### Bug fixes
* FI default value

## Release [v0.95](https://github.com/DreamIP/GPStudio/tree/v0.95) (2016-02-29)
[Full changelog](https://github.com/DreamIP/GPStudio/compare/v0.95-beta...v0.95)

### New Feature
* gpdevice/gpproc added enum commands

### Changes
* By-pass process generated as default process

### Bug fixes
* Fixed io xml format
* Documentation review
* setpropertymap command
* block_generator slave fixed
* gpviewer cameracom can handle small data package

## Release [v0.95-beta](https://github.com/DreamIP/GPStudio/tree/v0.95-beta) (2016-02-04)
[Full changelog](https://github.com/DreamIP/GPStudio/compare/v0.95-pre...v0.95-beta)

### New Feature
* Command line tools now checks instance name validity
* hwlib introduced
* new tools : gpdevice and gpproc

### Changes
* gpnode completed bash completion
* command line tutorial improved with dynamic parameters samples
* gpblock splitted into gpdevice and gpproc

### Bug fixes
* Fixed gpblock bash completion without project
* CI bug with no defined clock

## Release [v0.95-pre](https://github.com/DreamIP/GPStudio/tree/v0.95-pre) (2016-01-27)
[Full changelog](https://github.com/DreamIP/GPStudio/compare/v0.90...v0.95-pre)

### New Feature
* gpblock added generation of slave registers feature

### Changes
* Improved bash completion
* gpblock complete commands set

### Bug fixes
* Fixed bash wrappers
* Various bugfix in gpblock

## Release [v0.90](https://github.com/DreamIP/GPStudio/tree/v0.90) (2016-01-06)
[Full changelog](https://github.com/DreamIP/GPStudio/compare/v0.90-pre...v0.90)

### New Feature
* Tutorial for using command line tools
* Introducing gpblock with a basic block generation

### Changes
* Improved documentation
* Improved distribution system

### Bug fixes
* Fixed windows version

## Release [v0.90-pre](https://github.com/DreamIP/GPStudio/tree/v0.90-pre) (2015-12-14)
[Full changelog](https://github.com/DreamIP/GPStudio/compare/v0.80...v0.90-pre)

### New Feature
* Automatic scripts to deploy
* Introducing gpnode and gplib command line tool instead of generate_node script
* Documentation for tools
* command line completion scripts

### Changes
* IP and toolchains moved to support directory
* Rename special blocks
* Full review of directory structure

## Release [v0.80](https://github.com/DreamIP/GPStudio/tree/v0.80) (2015-06-24)

Only an internal beta version, not distributed
