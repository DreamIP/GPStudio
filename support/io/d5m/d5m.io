<?xml version="1.0" encoding="UTF-8"?>
<io driver="d5m" categ="imagesensor" pi_size_addr_rel="4" desc="">
  <parts>
    <part name="main">
      <svg width="100pt" height="100pt">
        <g transform="translate(-169.5 -97.862)">
         <rect rx="3" ry="3" height="109.79" width="85" stroke="#000" y="100.15" x="189.5" fill="#9c9cff"/>
         <g fill="#fff">
          <rect rx="3" ry="3" height="7.0833" width="2.125" y="132.03" x="253.25"/>
          <rect rx="3" ry="3" height="7.0833" width="2.125" y="132.03" x="208.62"/>
          <rect rx="5.7" ry="5.7" height="42.5" width="42.5" stroke="#000" y="114.32" x="210.75"/>
          <ellipse rx="14.167" ry="14.167" stroke="#000" cy="135.57" cx="232"/>
         </g>
         <ellipse rx="14.614" ry="14.614" stroke="#000" cy="135.57" cx="232" stroke-dasharray="3.00000004, 1.00000001" fill="none"/>
         <g stroke="#000">
          <ellipse rx="1.4167" ry="1.4167" cy="135.57" cx="232" fill="#fff"/>
          <path fill="#fff" d="m208.62 139.11a3.5417 3.5417 0 0 1 -3.0672 -1.7708 3.5417 3.5417 0 0 1 0 -3.5417 3.5417 3.5417 0 0 1 3.0672 -1.7708"/>
          <path d="m208.62 132.03h2.125" fill-rule="evenodd" stroke-width="1px" fill="#ff0"/>
          <path d="m208.62 139.11h2.125" fill-rule="evenodd" stroke-width="1px" fill="#ff0"/>
          <ellipse rx=".70833" ry=".70833" cy="135.57" cx="208.62" fill="#fff"/>
          <path fill="#fff" transform="scale(-1,1)" d="m-255.38 139.11a3.5417 3.5417 0 0 1 -3.0672 -1.7708 3.5417 3.5417 0 0 1 0 -3.5417 3.5417 3.5417 0 0 1 3.0672 -1.7708"/>
          <path d="m255.38 132.03h-2.125" fill-rule="evenodd" stroke-width="1px" fill="#ff0"/>
          <path d="m255.38 139.11h-2.125" fill-rule="evenodd" stroke-width="1px" fill="#ff0"/>
          <ellipse rx=".70833" ry=".70833" transform="scale(-1,1)" cy="135.57" cx="-255.38" fill="#fff"/>
         </g>
         <rect rx=".0000068484" ry=".0000068484" height="10.625" width="70.833" stroke="#000" y="209.95" x="196.58" fill="none"/>
        </g>
      </svg>
    </part>
  </parts>
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
