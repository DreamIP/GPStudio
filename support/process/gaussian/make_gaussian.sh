#!/bin/bash

rm gaussian.proc
#rm -rf hdl

# block and flows
gpproc new -n gaussian
gpproc sethelp -v "Dynamic image convolution block"
gpproc addflow -n in -d in -s 8
gpproc addflow -n out -d out -s 8
gpproc showblock

# IP infos
gpproc setinfo -n "author" -v "Edouard Hendrickx"
gpproc setinfo -n "email" -v "edouard.hendrickx@univ-bpclermont.fr"
gpproc setinfo -n "company" -v "Institut Pascal"
gpproc setinfo -n "revision" -v "1.0"
gpproc setinfo -n "releasedate" -v "2016-08-25"

# properties on flow out
gpproc addproperty -n out.datatype -t flowtype -v image
gpproc addproperty -n out.width -t int -m in.width.value-2
gpproc addproperty -n out.height -t int -m in.height.value-2

# PI parameters
gpproc setpisizeaddr -v 4

# register status_reg for enable propertY
gpproc addparam -n status_reg -r 0
gpproc addproperty -n enable -t bool -v 1
gpproc addbitfield -n status_reg.enable_bit -b 0 -m enable.value

# input image width register
gpproc addparam -n widthimg_reg -r 1
gpproc addbitfield -n "widthimg_reg.width" -b 15-0 -m in.width.value

# visual settings
gpproc setdraw -f gaussian.svg

# generate patterns files
# gpproc generate -o hdl
gpproc addfile -p hdl/gaussian.vhd -t vhdl -g hdl
gpproc addfile -p hdl/gaussian_process.vhd -t vhdl -g hdl
gpproc addfile -p hdl/gaussian_slave.vhd -t vhdl -g hdl
gpproc addfile -p "hwlib:matrix_extractor/hdl/matrix_extractor.vhd" -t vhdl -g hdl
