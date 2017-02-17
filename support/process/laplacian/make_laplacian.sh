#!/bin/bash

rm laplacian.proc
#rm -rf hdl

# block and flows
gpproc new -n laplacian
gpproc sethelp -v "Laplacian operator"
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

# weights registers and properties
addr=2
gpproc addproperty -n weight -t matrix -v 3
for y in {0..2}
do
    for x in {0..2}
    do
        gpproc addproperty -n "weight.m${y}${x}" -t int -v0
        gpproc setproperty -n "weight.m${y}${x}" -r -127:128
        gpproc sethelp -n "weight.m${y}${x}" -v "kernel value for w[$y][$x]"
        gpproc addparam -n "w${y}${x}_reg" -r ${addr}
        gpproc addbitfield -n "w${y}${x}_reg.m${y}${x}" -b 7-0 -m "weight.m${y}${x}.value"
        gpproc sethelp -n "w${y}${x}_reg" -v "kernel register for w[$y][$x]"
        ((addr=addr+1))
    done
done
gpproc setproperty -n "weight.m11" -v1

# filtertype, basic kernels
gpproc addproperty -n "filtertype" -t enum
gpproc setproperty -n "filtertype" -c <<- EOM "switch(filtertype.bits) { 
	case 0:
		weight.m00.value=0; weight.m01.value=1; weight.m02.value=0;
		weight.m10.value=1; weight.m11.value=-4; weight.m12.value=1;
		weight.m20.value=0; weight.m21.value=1; weight.m22.value=0;
		break;
	case 1:
		weight.m00.value=1; weight.m01.value=1; weight.m02.value=1;
		weight.m10.value=1; weight.m11.value=-8; weight.m12.value=1;
		weight.m20.value=1; weight.m21.value=1; weight.m22.value=1;

		break;
	case 2:
		weight.m00.value=1; weight.m01.value=-2; weight.m02.value=1;
		weight.m10.value=-2; weight.m11.value=4; weight.m12.value=-2;
		weight.m20.value=1; weight.m21.value=-2; weight.m22.value=1;

		break;
	}"
EOM
gpproc sethelp -n "filtertype" -v "Pre configured standard kernells"
gpproc addenum -n filtertype.4_connexe -v 0
gpproc addenum -n filtertype.8_connexe -v 1
gpproc addenum -n filtertype.Robinson -v 2

# visual settings
gpproc setdraw -f laplacian.svg

# generate patterns files
# gpproc generate -o hdl
gpproc addfile -p hdl/laplacian.vhd -t vhdl -g hdl
gpproc addfile -p hdl/laplacian_process.vhd -t vhdl -g hdl
gpproc addfile -p hdl/laplacian_slave.vhd -t vhdl -g hdl

gpproc addfile -p doc/laplacian.tex -t tex -g doc
gpproc addfile -p doc/laplacian.pdf -t pdf -g doc
