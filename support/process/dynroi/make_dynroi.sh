#!/bin/bash

rm dynroi.proc
rm -rf hdl

# block and flows
gpproc new -n dynroi

gpproc sethelp -v "Dynamic region of interest extractor"
gpproc setcateg -v "segmentation"

gpproc addflow -n BinImg -d in -s 8
gpproc addflow -n Img -d in -s 8

gpproc addflow -n roi -d out -s 8
gpproc addflow -n coord -d out -s 8

gpproc showblock

# IP infos
gpproc setinfo -n "author" -v "Raphael Bouterige"
gpproc setinfo -n "company" -v "Institut Pascal"
gpproc setinfo -n "revision" -v "1.1"
gpproc setinfo -n "releasedate" -v "2017-01-03"

# PI parameters
gpproc setpisizeaddr -v 2

# register status_reg for enable and bypass properties
gpproc addparam    -n status_reg -r 0
gpproc addproperty -n enable -t bool -v 0
gpproc addbitfield -n status_reg.enable_bit -b 0 -m enable.value

gpproc addproperty -n bypass -t bool -v 0
gpproc addbitfield -n status_reg.bypass_bit -b 1 -m bypass.value

#parameter to set a fixed resolution on output image
gpproc addproperty -n static_res -t bool -v 1
gpproc addbitfield -n status_reg.static_res_bit -b 2 -m static_res.value

# register input flow size
gpproc addparam -n inImg_size_reg -r 1
gpproc addbitfield -n inImg_size_reg.in_w_reg -b 11-0 -m Img.width.value
gpproc addbitfield -n inImg_size_reg.in_h_reg -b 27-16 -m Img.height.value
#bin img res
gpproc addparam -n BinImg_size_reg -r 2
gpproc addbitfield -n BinImg_size_reg.in_w_reg -b 11-0 -m BinImg.width.value
gpproc addbitfield -n BinImg_size_reg.in_h_reg -b 27-16 -m BinImg.height.value

# register output flow size
gpproc addparam -n out_size_reg -r 3
gpproc addbitfield -n out_size_reg.out_w_reg -b 11-0 -m w.value
gpproc addproperty -n w -t int -v 176
gpproc setproperty -n w -r 1:4095
gpproc addbitfield -n out_size_reg.out_h_reg -b 27-16 -m h.value
gpproc addproperty -n h -t int -v 144
gpproc setproperty -n h -r 1:4095

# properties on flow out
gpproc addproperty -n roi.datatype -t flowtype -v image

#if not static_res, GPViewer won't be able to display. use <rect>.
#gpproc addproperty -n roi.width -t int  -m "static_res.value ? w.value : Img.width.value"
#gpproc addproperty -n roi.height -t int -m "static_res.value ? h.value : Img.height.value"
gpproc addproperty -n roi.width -t int  -m "bypass.value ? Img.width.value : (static_res.value ? w.value : Img.width.value)"
gpproc addproperty -n roi.height -t int -m "bypass.value ? Img.height.value : (static_res.value ? h.value : Img.height.value)"

# output status definition
gpproc addproperty -n coord.datatype -t flowtype -v features
gpproc addproperty -n coord.featuretype -t featuretype -v rect

# visual settings
gpproc setdraw -f dynroi.svg

gpproc generate -o hdl
gpproc addfile -p hdl/dynroi.vhd -t vhdl -g hdl
gpproc addfile -p hdl/dynroi_process.vhd -t vhdl -g hdl
gpproc addfile -p hdl/dynroi_slave.vhd -t vhdl -g hdl
gpproc addfile -p doc/dynroi.tex -t tex -g doc
gpproc addfile -p doc/dynroi.pdf -t pdf -g doc
