#!/bin/bash

rm dynroi.proc
#rm -rf hdl

# block and flows
gpproc new -n dynroi
gpproc sethelp -v "Dynamic region of interest extractor"
gpproc setcateg -v "segmentation"
gpproc addflow -n in -d in -s 8

gpproc addflow -n out -d out -s 8

gpproc showblock

# IP infos
gpproc setinfo -n "author" -v "Raphael Bouterige"
gpproc setinfo -n "company" -v "Institut Pascal"
gpproc setinfo -n "revision" -v "1.0"
gpproc setinfo -n "releasedate" -v "2017-01-03"

# PI parameters
gpproc setpisizeaddr -v 2

# register status_reg for enable and bypass properties
gpproc addparam -n status_reg -r 0
gpproc addproperty -n enable -t bool -v 1
gpproc addbitfield -n status_reg.enable_bit -b 0 -m enable.value
gpproc addproperty -n bypass -t bool -v 0
gpproc addbitfield -n status_reg.bypass_bit -b 1 -m bypass.value

# register input flow size
gpproc addparam -n in_size_reg -r 1
gpproc addbitfield -n in_size_reg.in_w_reg -b 11-0 -m in.width.value
gpproc addbitfield -n in_size_reg.in_h_reg -b 27-16 -m in.height.value

# properties on flow out
gpproc addproperty -n out.datatype -t flowtype -v image

gpproc addproperty -n out.width -t int -m in.width.value
gpproc addproperty -n out.height -t int -m in.height.value

# visual settings
gpproc setdraw -f dynroi.svg

#gpproc generate -o hdl
gpproc addfile -p hdl/dynroi.vhd -t vhdl -g hdl
gpproc addfile -p hdl/dynroi_process.vhd -t vhdl -g hdl
gpproc addfile -p hdl/dynroi_slave.vhd -t vhdl -g hdl
gpproc addfile -p doc/dynroi.tex -t tex -g doc
gpproc addfile -p doc/dynroi.pdf -t pdf -g doc
