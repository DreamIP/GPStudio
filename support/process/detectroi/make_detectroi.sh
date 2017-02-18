#!/bin/bash

rm detectROI.proc
rm -rf hdl

# block and flows
gpproc new -n detectroi

gpproc sethelp -v "Detection of the smallest non-black part of image"
gpproc setcateg -v "segmentation"

gpproc addflow -n in -d in -s 8
gpproc addflow -n coord -d out -s 8

gpproc showblock

# IP infos
gpproc setinfo -n "author" -v "Raphael Bouterige"
gpproc setinfo -n "company" -v "Institut Pascal"
gpproc setinfo -n "revision" -v "1.1"
gpproc setinfo -n "releasedate" -v "2016-12-12"

# PI parameters
gpproc setpisizeaddr -v 2

# register status_reg for enable and bypass properties
gpproc addparam -n status_reg -r 0
gpproc addproperty -n enable -t bool -v 0
gpproc addbitfield -n status_reg.enable_bit -b 0 -m enable.value

# register input flow size
gpproc addparam -n in_size_reg -r 1
gpproc addbitfield -n in_size_reg.in_w_reg -b 11-0 -m in.width.value
gpproc addbitfield -n in_size_reg.in_h_reg -b 27-16 -m in.height.value

# output status definition
gpproc addproperty -n coord.datatype -t flowtype -v features
gpproc addproperty -n coord.featuretype -t featuretype -v rect

# visual settings
gpproc setdraw -f detectroi.svg

gpproc generate -o hdl
gpproc addfile -p hdl/detectroi.vhd -t vhdl -g hdl
gpproc addfile -p hdl/detectroi_process.vhd -t vhdl -g hdl
gpproc addfile -p hdl/detectroi_slave.vhd -t vhdl -g hdl
