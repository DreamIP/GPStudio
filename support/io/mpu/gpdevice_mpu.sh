#!/bin/bash

gpdevice new -n mpu

#Files
gpdevice addfile -p hdl/mpu_acqui.vhd -t vhdl -g hdl
gpdevice addfile -p hdl/mpu_i2c.vhd -t vhdl -g hdl
gpdevice addfile -p hdl/mpu_i2c_master.vhd -t vhdl -g hdl
gpdevice addfile -p hdl/mpu_fifo.vhd -t vhdl -g hdl
gpdevice addfile -p hdl/mpu_pkg.vhd -t vhdl -g hdl
gpdevice addfile -p hdl/mpu.vhd -t vhdl -g hdl
gpdevice addfile -p hdl/mpu_slave.vhd -t vhdl -g hdl
gpdevice addfile -p hdl/mpu_offset_correction.vhd -t vhdl -g hdl

#Flows

gpdevice addflow -n accelero -d out -s 8
gpdevice addflow -n gyroscope -d out -s 8	
gpdevice addflow -n compass -d out -s 8

#Flow properties
gpdevice addproperty -n accelero.datatype -t flowtype -v vector
gpdevice addproperty -n accelero.itemdatatype -t hwtype -v s16
gpdevice addproperty -n accelero.swtype -t swtype -v float
gpdevice addproperty -n accelero.scaling -t function -v item.value*2

gpdevice addproperty -n gyroscope.datatype -t flowtype -v vector
gpdevice addproperty -n gyroscope.itemdatatype -t hwtype -v s16
gpdevice addproperty -n gyroscope.swtype -t swtype -v float
gpdevice addproperty -n gyroscope.scaling -t function -v item.value*2

gpdevice addproperty -n compass.datatype -t flowtype -v vector
gpdevice addproperty -n compass.itemdatatype -t hwtype -v s16
gpdevice addproperty -n compass.swtype -t swtype -v float
gpdevice addproperty -n compass.scaling -t function -v item.value*2

#Reset
gpdevice addreset -n reset_n -d in -g reset_n

#External ports
gpdevice addextport -n ad0 -t out -s 1
gpdevice addextport -n sda -t inout -s 1
gpdevice addextport -n scl -t inout -s 1

gpdevice setpisizeaddr -v 4

#Enable
gpdevice addparam -n enable_reg -m enable.value -r 0
gpdevice addproperty -n enable -t bool
gpdevice setproperty -n enable -l "Enable device"

#Gyro_config
gpdevice addparam -n gyro_config_reg -r 1 -m gyro.gyro_config.bits
gpdevice addproperty -n gyro -t group
gpdevice addproperty -n gyro.gyro_config -t enum
gpdevice addenum -n gyro.gyro_config.g0 -l "± 250 °/s" -v 0 
gpdevice addenum -n gyro.gyro_config.g1 -l "± 500 °/s" -v 1 
gpdevice addenum -n gyro.gyro_config.g2 -l "± 1000 °/s" -v 2 
gpdevice addenum -n gyro.gyro_config.g3 -l "± 2000 °/s" -v 3 
gpdevice setproperty -n gyro -l "Gyroscope"
gpdevice setproperty -n gyro.gyro_config -l "Sensibility"

#Accel_config
gpdevice addparam -n accel_config_reg -r 2 -m accel.accel_config.bits
gpdevice addproperty -n accel -t group
gpdevice addproperty -n accel.accel_config -t enum
gpdevice addenum -n accel.accel_config.a0 -l "± 2g" -v 0 
gpdevice addenum -n accel.accel_config.a1 -l "± 4g" -v 1 
gpdevice addenum -n accel.accel_config.a2 -l "± 8g" -v 2 
gpdevice addenum -n accel.accel_config.a3 -l "± 16g" -v 3 
gpdevice setproperty -n accel -l "Accelerometer"
gpdevice setproperty -n accel.accel_config -l "Sensibility"

#Sample_rate
gpdevice addparam -n spl_rate_reg -r 3 -m sample_rate.value
gpdevice addproperty -n sample_rate -t int
gpdevice setproperty -n sample_rate -v 30 -r 1:100 -s 1 
gpdevice setproperty -n sample_rate -l "Sample rate"

#Compass gain
gpdevice addparam -n gain_compass_reg -r 4 -m comp.gain_compass.bits
gpdevice addproperty -n comp -t group
gpdevice addproperty -n comp.gain_compass -t enum
gpdevice addenum -n comp.gain_compass.c0 -l "± 0.88 Ga" -v 0 
gpdevice addenum -n comp.gain_compass.c1 -l "± 1.3 Ga" -v 1 
gpdevice addenum -n comp.gain_compass.c2 -l "± 1.9 Ga" -v 2 
gpdevice addenum -n comp.gain_compass.c3 -l "± 2.5 Ga" -v 3 
gpdevice addenum -n comp.gain_compass.c4 -l "± 4.0 Ga" -v 4 
gpdevice addenum -n comp.gain_compass.c5 -l "± 4.7 Ga" -v 5 
gpdevice addenum -n comp.gain_compass.c6 -l "± 5.6 Ga" -v 6 
gpdevice addenum -n comp.gain_compass.c7 -l "± 8.1 Ga" -v 7 
gpdevice setproperty -n comp -l "Compass"
gpdevice setproperty -n comp.gain_compass -l "Sensibility"

#Compass output rate
gpdevice addparam -n fz_compass_reg -r 5 -m comp.fz_compass.bits
gpdevice addproperty -n comp.fz_compass -t enum
gpdevice addenum -n comp.fz_compass.c0 -l "0.75 Hz" -v 0 
gpdevice addenum -n comp.fz_compass.c1 -l "1.5 Hz" -v 1 
gpdevice addenum -n comp.fz_compass.c2 -l "3 Hz" -v 2 
gpdevice addenum -n comp.fz_compass.c3 -l "7.5 Hz" -v 3 
gpdevice addenum -n comp.fz_compass.c4 -l "15 Hz" -v 4 
gpdevice addenum -n comp.fz_compass.c5 -l "30 Hz" -v 5 
gpdevice addenum -n comp.fz_compass.c6 -l "75 Hz" -v 6
gpdevice setproperty -n comp.fz_compass -l "Update rate"

#Offset accelero X
gpdevice addparam -n accel_off_x_reg -m accel.accel_offx.value
gpdevice fixparam -n accel_off_x_reg -v false
gpdevice setparam -n accel_off_x_reg -r 6
gpdevice addproperty -n accel.accel_offx -t int -v 0
gpdevice setproperty -n accel.accel_offx -l "Offset X"
gpdevice setproperty -n accel.accel_offx -r -16384:+16384 -s 1

#Offset accelero Y
gpdevice addparam -n accel_off_y_reg -m accel.accel_offy.value
gpdevice fixparam -n accel_off_y_reg -v false
gpdevice setparam -n accel_off_y_reg -r 7
gpdevice addproperty -n accel.accel_offy -t int -v 0
gpdevice setproperty -n accel.accel_offy -l "Offset Y"
gpdevice setproperty -n accel.accel_offy -r -16384:+16384 -s 1

#Offset accelero Z
gpdevice addparam -n accel_off_z_reg -m accel.accel_offz.value
gpdevice fixparam -n accel_off_z_reg -v false
gpdevice setparam -n accel_off_z_reg -r 8
gpdevice addproperty -n accel.accel_offz -t int -v 0
gpdevice setproperty -n accel.accel_offz -l "Offset Z"
gpdevice setproperty -n accel.accel_offz -r -16384:+16384 -s 1

#Offset gyro X
gpdevice addparam -n gyro_off_x_reg -m gyro.gyro_offx.value
gpdevice fixparam -n gyro_off_x_reg -v false
gpdevice setparam -n gyro_off_x_reg -r 9
gpdevice addproperty -n gyro.gyro_offx -t int -v 0
gpdevice setproperty -n gyro.gyro_offx -l "Offset X"
gpdevice setproperty -n gyro.gyro_offx -r -16384:+16384 -s 1	

#Offset gyro Y
gpdevice addparam -n gyro_off_y_reg -m gyro.gyro_offy.value
gpdevice fixparam -n gyro_off_y_reg -v false
gpdevice setparam -n gyro_off_y_reg -r 10
gpdevice addproperty -n gyro.gyro_offy -t int -v 0
gpdevice setproperty -n gyro.gyro_offy -l "Offset Y"
gpdevice setproperty -n gyro.gyro_offy -r -16384:+16384 -s 1

#Offset gyro Z
gpdevice addparam -n gyro_off_z_reg -m gyro.gyro_offz.value
gpdevice fixparam -n gyro_off_z_reg -v false
gpdevice setparam -n gyro_off_z_reg -r 11
gpdevice addproperty -n gyro.gyro_offz -t int -v 0
gpdevice setproperty -n gyro.gyro_offz -l "Offset Z"
gpdevice setproperty -n gyro.gyro_offz -r -16384:+16384 -s 1


#Generate device
gpdevice generate -o build/


