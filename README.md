# GPStudio

A toolchain for FPGA-based smart camera that helps devellopers to
create high performance image processing.

website : <http://gpstudio.univ-bpclermont.fr>

Actual version 1.10

Find a changelog in [CHANGELOG.md](CHANGELOG.md)

## Installation
Yo can install GPStudio by cloning this repository or directly download 
a binary package from <http://gpstudio.univ-bpclermont.fr/download>.

For installing, follow instructions given in [INSTALL.md](INSTALL.md)

## Tools
GPStudio is composed by a set of command line and graphical tools.

gpnode to manage GPStudio camera projects with board/sensor selection
and image processing creation (command line and GUI).

gpcomp, gpproc and gpdevice to create your custom components, block
processing and device driver block (only in command line).

gplib to view the content of the IPs library (only in command line).

gpviewer to visualise result of your process (GUI only).

## Supported FPGA family

Altera families:

* MAX X
* Cyclone III
* Cyclone IV
* Cyclone V
* Stratix V

Xilinx not yet supported for project creation but have been planned.

## License
> This program is free software: you can redistribute it and/or modify
> it under the terms of the GNU General Public License as published by
> the Free Software Foundation, either version 3 of the License, or
> (at your option) any later version.
> 
> This program is distributed in the hope that it will be useful,
> but WITHOUT ANY WARRANTY; without even the implied warranty of
> MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
> GNU General Public License for more details.
> 
> You should have received a copy of the GNU General Public License
> along with this program.  If not, see <http://www.gnu.org/licenses/>
