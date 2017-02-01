#!/bin/bash

rm usb3.io

gpdevice new -n usb3

# flows
gpdevice addflow -n out0 -d out -s 8
gpdevice addflow -n out1 -d out -s 8
gpdevice addflow -n in0 -d in -s 8
gpdevice addflow -n in1 -d in -s 8

# reset
#gpdevice addreset -n reset_n -d in -g reset_n

# external ports
gpdevice addextport -n ftreset_n -t out -s 1
gpdevice addextport -n ftclk -t in -s 1
gpdevice addextport -n be -t inout -s 2
gpdevice addextport -n data -t inout -s 16

gpdevice addextport -n txe_n -t in -s 1
gpdevice addextport -n rxf_n -t in -s 1
gpdevice addextport -n siwu_n -t out -s 1
gpdevice addextport -n wr_n -t out -s 1
gpdevice addextport -n rd_n -t out -s 1
gpdevice addextport -n oe_n -t out -s 1

gpdevice generate -o hdl/

gpdevice addfile -p hdl/usb3.vhd -t vhdl -g hdl
