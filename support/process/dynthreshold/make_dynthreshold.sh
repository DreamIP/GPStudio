#!/bin/bash

rm dynthreshold.proc
rm -rf hdl

# block and flows
gpproc new -n dynthreshold
gpproc sethelp -v "Dynamic threshold based on a desired ratio of white"
gpproc addflow -n in -d in -s 8
gpproc addflow -n out -d out -s 8
gpproc showblock

# IP infos
gpproc setinfo -n "author" -v "Raphael BOUTERIGE"
gpproc setinfo -n "company" -v "Institut Pascal"
gpproc setinfo -n "revision" -v "1.0"
gpproc setinfo -n "releasedate" -v "2016-11-23"

# properties on flow out
gpproc addproperty -n out.datatype -t flowtype -v image
gpproc addproperty -n out.width -t int -m in.width.value
gpproc addproperty -n out.height -t int -m in.height.value

# PI parameters
gpproc setpisizeaddr -v 2 

# register status_reg for enable propertY
gpproc addparam -n status_reg -r 0
gpproc addproperty -n enable -t bool -v 1
gpproc addbitfield -n status_reg.enable_bit -b 0 -m enable.value

# input image width register
#gpproc addparam -n widthimg_reg -r 1
#gpproc addbitfield -n widthimg_reg.value -b 15-0 -m in.width.value

## check usage
# input image height register
#gpproc addparam -n heigtimg_reg -r 2
#gpproc addbitfield -n heigtimg_reg.value -b 15-0 -m in.height.value

# register threshold
gpproc addproperty -n "DesiredRatio" -t sint -v 5
gpproc setproperty -n "DesiredRatio" -r 0:100
gpproc addparam -n desired_ratio_reg -m "DesiredRatio".value -r 1

# visual settings
gpproc setdraw -f dynthreshold.svg

# generate patterns files
gpproc generate -o hdl
gpproc addfile -p hdl/dynthreshold.vhd -t vhdl -g hdl
gpproc addfile -p hdl/dynthreshold_process.vhd -t vhdl -g hdl
gpproc addfile -p hdl/dynthreshold_slave.vhd -t vhdl -g hdl
