<?xml version="1.0" encoding="UTF-8"?>
<io driver="gps" categ="" pi_size_addr_rel="2" desc="">
  <parts>
    <part name="main">
      <svg width="160" height="110">
        <rect width="160" height="110" x="0" y="0" fill="lightgray"/>
        <g>
            <path d="M28.793,71.207c-5.062-5.065-7.6-11.705-7.62-18.356l-6.515-0.002c0.021,8.32,3.193,16.635,9.526,22.967 c6.335,6.331,14.647,9.504,22.967,9.526l-0.001-6.515C40.5,78.806,33.856,76.268,28.793,71.207z"></path>
            <path d="M18.423,81.575C10.501,73.654,6.535,63.251,6.514,52.85L0,52.846c0.02,12.074,4.623,24.145,13.815,33.34 c9.193,9.188,21.261,13.796,33.337,13.812v-6.515C36.745,93.467,26.346,89.495,18.423,81.575z"></path>
        </g>
        <path d="M76.157,43.675l-2.863-0.341c3.781-4.642,3.509-11.487-0.819-15.811c-4.321-4.327-11.166-4.599-15.811-0.818l-0.339-2.862 L32.482,0L11.531,20.949l23.842,23.843l2.864,0.341c-3.779,4.643-3.506,11.485,0.819,15.811c4.325,4.325,11.167,4.596,15.81,0.819 l0.339,2.862L79.05,88.466L100,67.519L76.157,43.675z M38.384,38.586L20.749,20.949L32.482,9.218l17.637,17.636l0.669,5.64 l-6.762,6.761L38.384,38.586z M79.05,79.249L61.412,61.613l-0.668-5.64l6.761-6.761l5.642,0.668L90.78,67.519L79.05,79.249z"></path>
      </svg>
    </part>
  </parts>
  <files>
    <file name="gps_acqui.vhd" type="vhdl" group="hdl" path="hdl/gps_acqui.vhd"/>
    <file name="gps_receiver.vhd" type="vhdl" group="hdl" path="hdl/gps_receiver.vhd"/>
    <file name="gps_fifo.vhd" type="vhdl" group="hdl" path="hdl/gps_fifo.vhd"/>
    <file name="gps_transmitter.vhd" type="vhdl" group="hdl" path="hdl/gps_transmitter.vhd"/>
    <file name="gps_clkgen.vhd" type="vhdl" group="hdl" path="hdl/gps_clkgen.vhd"/>
    <file name="gps.vhd" type="vhdl" group="hdl" path="hdl/gps.vhd"/>
    <file name="gps_slave.vhd" type="vhdl" group="hdl" path="hdl/gps_slave.vhd"/>
    <file name="gps_pkg.vhd" type="vhdl" group="hdl" path="hdl/gps_pkg.vhd"/>
    <file name="gps.md" type="doc" group="doc" path="doc/gps.md"/>
    <file name="schema_gps.tex" type="doc" group="doc" path="doc/schema_gps.tex"/>
    <file name="gps_schema_global.tex" type="doc" group="doc" path="doc/gps_schema_global.tex"/>
    <file name="gps.io" type="io" group="blockdef" path="gps.io"/>
  </files>
  <resets>
    <reset name="reset_n" group="reset_n" direction="in"/>
  </resets>
  <flows>
    <flow name="out" size="8" type="out">
      <properties>
        <property name="datatype" type="flowtype" value="vector"/>
        <property name="itemdatatype" type="hwtype" value="s16"/>
        <property name="swtype" type="swtype" value="float"/>
        <property name="scaling" type="function" value="item.value*2"/>
      </properties>
    </flow>
  </flows>
  <params>
    <param name="enable_reg" regaddr="0" propertymap="enable.value"/>
    <param name="sat_reg" regaddr="1" propertymap="sat_mode.bits"/>
    <param name="update_reg" regaddr="2" propertymap="update.bits"/>
  </params>
  <properties>
    <property name="enable" caption="Enable" type="bool"/>
    <property name="sat_mode" caption="Mode" type="enum">
      <enums>
        <enum name="s0" value="0" caption="GPS/Glonass"/>
        <enum name="s1" value="1" caption="GPS"/>
      </enums>
    </property>
    <property name="update" caption="Update rate" type="enum">
      <enums>
        <enum name="u1" value="1" caption="1 Hz"/>
        <enum name="u2" value="2" caption="2 Hz"/>
        <enum name="u4" value="4" caption="4 Hz"/>
        <enum name="u5" value="5" caption="5 Hz"/>
        <enum name="u8" value="8" caption="8 Hz"/>
        <enum name="u10" value="10" caption="10 Hz"/>
        <enum name="u20" value="20" caption="20 Hz"/>
        <enum name="u25" value="25" caption="25 Hz"/>
        <enum name="u40" value="40" caption="40 Hz"/>
      </enums>
    </property>
  </properties>
  <ports>
    <port name="RXD" type="in" size="1"/>
    <port name="TXD" type="out" size="1"/>
  </ports>
  <pins/>
</io>
