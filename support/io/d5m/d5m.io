<?xml version="1.0" encoding="UTF-8"?>
<io driver="d5m" categ="imagesensor" pi_size_addr_rel="4" desc="">
  <files>
    <file name="d5m.io" type="io" group="blockdef" path="d5m.io"/>
    <file name="d5m.vhd" type="vhdl" group="hdl" path="hdl/d5m.vhd"/>
    <file name="d5m_slave.vhd" type="vhdl" group="hdl" path="hdl/d5m_slave.vhd"/>
    <file name="video_sampler.v" type="verilog" group="hdl" path="hwlib:video_sampler/video_sampler.v"/>
    <file name="d5m_controller.vhd" type="vhdl" group="hdl" path="hdl/d5m_controller.vhd"/>
    <file name="I2C_CCD_Config.v" type="verilog" group="hdl" path="hdl/I2C_CCD_Config.v"/>
    <file name="CCD_Capture.v" type="verilog" group="hdl" path="hdl/CCD_Capture.v"/>
    <file name="RGB2GRY.vhd" type="vhdl" group="hdl" path="hdl/RGB2GRY.vhd"/>
    <file name="RAW2RGB.v" type="verilog" group="hdl" path="hdl/RAW2RGB.v"/>
  </files>
  <resets>
    <reset name="reset_n" group="reset_n" direction="in"/>
  </resets>
  <ports>
    <port name="ccd_pixclk" type="in" size="1"/>
    <port name="ccd_data" type="in" size="12"/>
    <port name="ccd_xclkin" type="out" size="1"/>
    <port name="ccd_reset" type="out" size="1"/>
    <port name="ccd_trigger" type="out" size="1"/>
    <port name="ccd_lval" type="in" size="1"/>
    <port name="ccd_fval" type="in" size="1"/>
    <port name="i2c_sdata" type="inout" size="1"/>
    <port name="i2c_sclk" type="out" size="1"/>
  </ports>
  <flows>
    <flow name="data" size="8" type="out">
      <properties>
        <property name="datatype" type="flowtype" value="image"/>
        <property name="width" type="int" value="1280"/>
        <property name="height" type="int" value="1024"/>
      </properties>
    </flow>
  </flows>
  <params>
    <param name="status_reg" regaddr="0">
      <bitfields>
        <bitfield name="enable_bit" bitfield="0" propertymap="enable.value"/>
      </bitfields>
    </param>
  </params>
  <properties>
    <property name="enable" type="bool" value="0"/>
  </properties>
  <pins/>
</io>
