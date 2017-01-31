# GPStudio glossary
 
## attribute
[backend code](../scripts/model/attribute.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_attribute.h)

Attribute is contained into the Board class to define the specific attributes
of the board and in Pin class to define each features of pin dedicated to the
toolchain.

## block
[backend code](../scripts/model/block.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_block.h)

Block is the base block definition for all blocks (IO, IOCom and process).
It is a specialisation of component. 

In addition of attribute of component, it contains bus interfaces for PI
slave and master interfaces, addr and master count.

## board
[backend code](../scripts/model/board.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_board.h)
Board is the class to load .dev support platform file and manage IO support
for the board. It also load the toolchain with attribute.

## clock
[backend code](../scripts/model/clock.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_clock.h)

The Clock class is used to define a clock source or a clock input in
Board, Block or Component.

Clock instance is present in Board, Block or Component.

It's possible to define a clock with 3 ways :

    - typical frequency and eventualy a clock shift given in degrees
    - an interval of frequency given with min and max
    - a clock domain and a ratio

## clockdomain
[backend code](../scripts/model/clockdomain.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_clockdomain.h)

ClockDomain permits to save typical frequency of a clock domain. Use it
in addition to clock with shift or ratio. Clock defined with the same
clockdomain ensure to be syncronized.

## comconnect
[backend code](../scripts/model/comconnect.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_comconnect.h)

It links the id of the software protocol to the name of the harware
communication flow or PI interface

## comdriver
[backend code](../scripts/model/comdriver.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_comdriver.h)

Contains all parameters about the software driver to use and communication
links and protocol declaration.

## comparam
[backend code](../scripts/model/comparam.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_comparam.h)

The ComParam class gives all specifics parameters to software driver
to etablish the connection and alk to the camera

## componentpart
[backend code](../scripts/model/componentpart.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_componentpart.h)

Multiple parts could be used to define a component with different graphical part.
The default part is named main.

## componentpartflow
[backend code](../scripts/model/componentpartflow.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_componentpartflow.h)

Multiple part flow could be used to define a part of a component with different
flow (position, name of the flow).

## componentpartproperty
[backend code](../scripts/model/componentpartproperty.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_componentpartproperty.h)

Multiple part flow could be used to define a part of a component with graphical
draw of property. In gpviewer, the corresponding property will be draw on the
specified part with the given position and size.

## component
[backend code](../scripts/model/component.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_component.h)
Component is the the definition of hardware components. It could be
used in a block to indicate the inclusion of the block or in the
component library to define them.

It needs to be specialised, it only contains the list of :

 - implementation files (vhdl, verilog, C, C++, ...), documentation files
Block::$files
 - hardware parameters (generic for VHDL, param for verilog constant for
C/C++) or register for hardware implementation Block::$params
 - properties for high level software Block::$properties
 - flows interface input and output Block::$flows
 - clocks inputs (all blocks) and clocks generator (IO blocks only) Block::$clocks
 - reset inputs (all blocks) and reset generators (IO blocks only) Block::$resets
 - attributes for special attributes compilation toolchain Block::$attributes

A component could be included in a block or as an extension but could not be
instantiated in a node.

## file
[backend code](../scripts/model/file.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_file.h)

Model class to have informations about file implementation in a Component.
They are sorted by group to give the utility of the file. File are stored
in Component as a list.

## flow
[backend code](../scripts/model/flow.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_flow.h)

Model class to define flow hardware interface in a Component or Block.
Flow are stored in Component::flows as a list. A flow interface have information
about the direction (in or out), the size of the data bus in bit and the name.

## flowconnect
[backend code](../scripts/model/flowconnect.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_flowconnect.h)

Define flow connection between two flow interface of blocks, an output
flow to an input flow interface. FlowConnect are used in FI::flow_connects
as a list. The set of flowconnect is used to define the flow interconnect
architechture.

## gpviewer
[backend code](../scripts/model/gpviewer.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_gpviewer.h)

Structure that store all gpviewer dedicated parameters. At the momement,
this structure only contain the viewer list.

## info
[backend code](../scripts/model/info.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_info.h)

The Info class is used to indicate information for IPs. Info is contained 
into Component class to give information for IPs like author, email,
version, company, licence...

## interfacebus
[backend code](../scripts/model/interfacebus.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_interfacebus.h)

InterfaceBus are used in Block::interfaces as a list. An interface as slave,
(pi_slave) or master (pi_master), slave connection (pi_slave_conn) or master
connection (pi_master_conn)

## io
[backend code](../scripts/model/io.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_io.h)

IO is the specialised implementation of Block for sensors and
communication blocks. In addition to simple Block, it allows the use of
pins and external port to declare physical interface.
It contains the list of  pins and ports as external io pins,
ext_ports.

## iocom
[backend code](../scripts/model/iocom.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_iocom.h)

IOCom is the specialised implementation of IO. It allows the use of 
communication links and protocol declaration.

## lib
[backend code](../scripts/model/lib.php) [gui-tools model](../gui-tools/src/gpstudio_lib/lib_parser/lib.h)

Lib is a container that store all the IPs available in library path.

## libitem
[backend code](../scripts/model/libitem.php) [gui-tools model](../gui-tools/src/gpstudio_lib/lib_parser/blocklib.h)

LibItem is an item of the Lib container. It contains a copy of an IP
definition.

## node
[backend code](../scripts/model/node.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_node.h)

Node is the base class container that store all the configuration of a node.
A node in GPStudio is typically a smart camera or a server. It store the list
of blocks and the local configuration of the node with the definition of the
board.

Define it with gpnode (CLI tool) or gpnode_gui (GUI tool)

## param
[backend code](../scripts/model/param.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_param.h)

Param handle a constant parameter (generic for VHDL, param for verilog
constant for C/C++) or register for hardware implementation.

## parambitfield
[backend code](../scripts/model/parambitfield.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_parambitfield.h)

Bit field for param when param are registers. A param can contains
multiple bitfieds and each bit field can be composed by one or more bits.

## pin
[backend code](../scripts/model/pin.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_pin.h)

Pin is the physical mapping between external port of an IO block and 
chip pins.

## pll
[backend code](../scripts/model/pll.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_pll.h)

PLL is a conveniant system to help CI PLL assignation and computation.

## port
[backend code](../scripts/model/port.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_port.h)

Port is external port definition for IO block. The relation between port
and physical pin mapping is ensurred by pin in board definition.

## process
[backend code](../scripts/model/process.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_process.h)

Process is the specialised implementation of Block for processes.
Compared to Block, this class does not contain additionnal members. It just
contains library parser and helper for process. A process contain at least
one input flow and one output flow.

## property
[backend code](../scripts/model/property.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_property.h)

The Property class is used to define high level properties and only have
any sense at software level. Property is contained into the Block properties
and Flow properties. It maps mathematical relation between harware registers
and software logical properties. When a property change, all dependent
propreties are re-evaluated.

## propertyenum
[backend code](../scripts/model/propertyenum.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_propertyenum.h)

The PropertyEnum can be used to list the values that can take a property.
PropertyEnum is contained into the Property propertyenums when
property have a list of choices for value.

## reset
[backend code](../scripts/model/reset.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_reset.h)

The Reset class define a reset input or reset provider in a Component or Block.
Reset is used into the Component resets to list all the reset of the block.
A reset have information about the direction (in or out / provider or receiver),
the group of reset and the name.

## toolchain
[backend code](../scripts/model/toolchain.php)

Toolchain class define a toolchain for building a project. It exists a
base toolchain named HDL to create a fully standard HDL project. Another
toolchain named altera_quartus is based on the first one but also create
quartus specific project files. Vivado toolchain does not exist at the 
moment but you probably create it.

[HDL toolchain](support/toolchain/hdl/hdl.php)

[altera_quartus toolchain](support/toolchain/altera_quartus/altera_quartus.php)

## treeconnect
[backend code](../scripts/model/treeconnect.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_treeconnect.h)

The TreeConnect class define a connection between two flows. Compared to
FlowConnect, this interface is processed to obtain all the connection
with the generated MUX.

## viewer
[backend code](../scripts/model/viewer.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_viewer.h)

The Viewer class define a viewer input or viewer provider.
Viewer is used into the Block viewers to list all the viewer of the block.

## viewerflow
[backend code](../scripts/model/viewerflow.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_viewerflow.h)

The ViewerFlow class define a flow connection to a viewer. ViewerFlow
is used into the Viewer::$viewerflows to list associated flow on each viewer.
 
## clock interconnect CI
[backend code](../scripts/system_interconnect/ci.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_ciblock.h)

ClockInterconnect is the generated block to manage all the clock in the
node project. All the clocks pass through this block. It gets all the base
clock from board and IOs and provide all needed clock by using PLLs.
Generated PLLs are located inside this block. If a clock is too slow,
it generates a diviser based on a counter. The choice of clock association
inside PLL are processed with an algorithm that minimise the number of PLLs.

## flow interconnect FI
[backend code](../scripts/system_interconnect/fi.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_fiblock.h)

FlowInterconnect is the generated block to manage all the flows in the node project.
All the flow interface pass through this block. It manage all the flow connections
and generate flow multiplexer in case of multiple flow out are connected to an input.
The generated multiplexer, allows to dynamically change the source of the input flow.
Register are created in this block to manage multiplexer.

## parameter interconnect PI
[backend code](../scripts/system_interconnect/pi.php) [gui-tools model](../gui-tools/src/gpstudio_lib/model/model_piblock.h)

ParamInterconnect is the generated block to manage all the parameter interfaces.
It internally contain bus interconnect to all block that have a PI interface.
The generation of the block choose adress for each block.
