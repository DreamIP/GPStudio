#!/bin/bash

rm d5m.io

gpdevice new -n d5m
gpdevice setcateg -v imagesensor

#Files
# GPStudio generated files
gpdevice addfile -p hdl/d5m.vhd -t vhdl -g hdl
gpdevice addfile -p hdl/d5m_slave.vhd -t vhdl -g hdl
# GPStudio library files
gpdevice addfile -p "hwlib:video_sampler/video_sampler.v" -t verilog -g hdl
# Manually written files
gpdevice addfile -p hdl/d5m_controller.vhd -t vhdl -g hdl
gpdevice addfile -p hdl/I2C_CCD_Config.v -t verilog -g hdl
gpdevice addfile -p hdl/CCD_Capture.v -t verilog -g hdl
gpdevice addfile -p hdl/RGB2GRY.vhd -t vhdl -g hdl
gpdevice addfile -p hdl/RAW2RGB.v -t verilog -g hdl

#Flows

gpdevice addflow -n out -d out -s 8

#Flow properties
gpdevice addproperty -n out.datatype -t flowtype -v image
gpdevice addproperty -n out.width -t int -v 1280
gpdevice addproperty -n out.height -t int -v 1024

#Reset
#gpdevice addreset -n reset_n -d in -g reset_n

#External ports
gpdevice addextport -n ccd_pixclk -t in -s 1
gpdevice addextport -n ccd_data -t in -s 12
gpdevice addextport -n ccd_xclkin -t out -s 1
gpdevice addextport -n ccd_reset -t out -s 1
gpdevice addextport -n ccd_trigger -t out -s 1
gpdevice addextport -n ccd_lval -t in -s 1
gpdevice addextport -n ccd_fval -t in -s 1
gpdevice addextport -n i2c_sdata -t inout -s 1
gpdevice addextport -n i2c_sclk -t out -s 1
      
gpdevice setpisizeaddr -v 4

# register status_reg for enable property
gpdevice addparam -n status_reg -r 0
gpdevice addproperty -n enable -t bool -v 0
gpdevice addbitfield -n status_reg.enable_bit -b 0 -m enable.value

#Generate device
gpdevice generate -o hdl/


