#!/bin/bash

rm erode.proc
#rm -rf hdl

# block and flows
gpproc new -n erode
gpproc sethelp -v "Erosion process from image flow"
gpproc setcateg -v "morpho"
gpproc addflow -n in -d in -s 8
gpproc addflow -n out -d out -s 8
gpproc showblock

# IP infos
gpproc setinfo -n "author" -v "Raphael BOUTERIGE"
gpproc setinfo -n "company" -v "Institut Pascal"
gpproc setinfo -n "revision" -v "1.1"
gpproc setinfo -n "releasedate" -v "2016-10-25"

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
gpproc addbitfield -n widthimg_reg.value -b 15-0 -m in.width.value

## check usage
# input image height register
gpproc addparam -n heigtimg_reg -r 2
gpproc addbitfield -n heigtimg_reg.value -b 15-0 -m in.height.value

gpproc addparam -n LINE_WIDTH_MAX -t INTEGER -v 1280


# erosion matrix registers and properties
addr=3
gpproc addproperty -n "erosion_matrix" -t matrix -v 3
for y in {0..2}
do
    for x in {0..2}
    do
        gpproc addproperty -n "erosion_matrix.m${y}${x}" -t int -v 1
        gpproc setproperty -n "erosion_matrix.m${y}${x}" -r 0:1
        gpproc sethelp -n "erosion_matrix.m${y}${x}" -v "erosion matrix value for er[$y][$x]"
        gpproc addparam -n "er${y}${x}_reg" -r ${addr}
        gpproc addbitfield -n "er${y}${x}_reg.m${y}${x}" -b 7-0 -m "erosion_matrix.m${y}${x}.value"
        gpproc sethelp -n "er${y}${x}_reg" -v "erosion matrix register for er[$y][$x]"
        ((addr=addr+1))
    done
done
gpproc setproperty -n "erosion_matrix.m00" -v 0
gpproc setproperty -n "erosion_matrix.m02" -v 0
gpproc setproperty -n "erosion_matrix.m20" -v 0
gpproc setproperty -n "erosion_matrix.m22" -v 0

# visual settings
gpproc setdraw -f erode.svg

# generate patterns files
#gpproc generate -o hdl
gpproc addfile -p hdl/erode.vhd -t vhdl -g hdl
gpproc addfile -p hdl/erode_process.vhd -t vhdl -g hdl
gpproc addfile -p hdl/erode_slave.vhd -t vhdl -g hdl

gpproc addfile -p doc/erode.tex -t tex -g doc
gpproc addfile -p doc/erode.pdf -t pdf -g doc
