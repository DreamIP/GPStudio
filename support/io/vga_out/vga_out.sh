#!/bin/bash

rm vga_out.io

gpdevice new -n vga_out
gpdevice setcateg -v display

#Files
# GPStudio generated files
gpdevice addfile -p hdl/vga_out.vhd -t vhdl -g hdl
# GPStudio library files
# Manually written files
gpdevice addfile -p hdl/FrameBuffer.vhd -t vhdl -g hdl
gpdevice addfile -p hdl/vga_controller.vhd -t vhdl -g hdl
gpdevice addfile -p hdl/vga_generate.vhd -t vhdl -g hdl

#Clocks

gpdevice addclock -n vga_clk108 -d in
gpdevice setclock -n vga_clk108 -f 108M

#Flows

gpdevice addflow -n in -d in -s 8

#Flow properties

#Reset
#gpdevice addreset -n reset_n -d in -g reset_n

#External ports
gpdevice addextport -n vga_blank_n -t out -s 1
gpdevice addextport -n vga_r -t out -s 8
gpdevice addextport -n vga_g -t out -s 8
gpdevice addextport -n vga_b -t out -s 8
gpdevice addextport -n vga_clk -t out -s 1
gpdevice addextport -n vga_hs -t out -s 1
gpdevice addextport -n vga_vs -t out -s 1
gpdevice addextport -n vga_sync_n -t out -s 1
      
gpdevice setpisizeaddr -v 4

# register status_reg for enable property
gpdevice addparam -n status_reg -r 0
gpdevice addproperty -n enable -t bool -v 0
gpdevice addbitfield -n status_reg.enable_bit -b 0 -m enable.value

#Generate device

# Generate top, process and slave
# gpdevice generate -o hdl/ 
#gpdevice generatetop -o hdl/
#gpdevice generateprocess -o hdl/
#gpdevice generateslave -o hdl/
#gpdevice generatetb -o hdl/

gpdevice setdraw -f vga.svg


