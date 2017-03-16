<?xml version="1.0" encoding="UTF-8"?>
<io driver="vga_out" categ="display" pi_size_addr_rel="4" desc="">
  <parts>
    <part name="main">
      <svg height="100pt" width="100pt" version="1.1" viewBox="0 0 125.00001 125">
        <rect rx="3" ry="3" height="125" width="125" y="-4.8828e-7" x="0" fill="#fff"/>
        <rect rx="3" ry="3" height="77.066" width="107.52" stroke="#000" y="25.159" x="9.2691" fill="#fff"/>
        <path d="m40.254 9.5339c11.123 15.625 11.123 15.36 11.123 15.36l7.945-19.333" stroke="#000" stroke-width="1px" fill="none"/>
        <g stroke="#000" fill="#fff">
          <rect rx="3" ry="3" height="55.614" width="74.947" y="35.62" x="20.392"/>
          <g>
            <circle cx="107" cy="42.5" r="2.5"/>
            <circle cx="107" cy="52.833" r="2.5"/>
            <circle cx="107" cy="63.167" r="2.5"/>
            <circle cx="107" cy="73.5" r="2.5"/>
          </g>
        </g>
        <g stroke="#000">
          <path d="m30.693 91.234 14.307-16.234 10-20l10 15 23.689 21.234" stroke-width="1px" fill="none"/>
          <path d="m51.468 62.065c3.5878 2.8238 3.775 1.3257 3.775 1.3257l4.828-0.78424" stroke-width="1px" fill="none"/>
          <circle cx="40" cy="45" r="2.5" fill="#fff"/>
        </g>
        <g stroke="#000" stroke-width="1px" fill="none">
          <path d="m36 45h-3"/>
          <path d="m47 45h-3"/>
          <path d="m40 52v-3"/>
          <path d="m40 41v-3"/>
        </g>
        <g transform="matrix(.70711 -.70711 .70711 .70711 -21.149 40.42)" stroke="#000" stroke-width="1px" fill="none">
          <path d="m36 46.478h-3"/>
          <path d="m47 46.478h-3"/>
          <path d="m40 53.478v-3"/>
          <path d="m40 42.478v-3"/>
        </g>
      </svg>
    </part>
  </parts>
  <files>
    <file name="vga_out.io" type="io" group="blockdef" path="vga_out.io"/>
    <file name="vga_out.vhd" type="vhdl" group="hdl" path="hdl/vga_out.vhd"/>
    <file name="FrameBuffer.vhd" type="vhdl" group="hdl" path="hdl/FrameBuffer.vhd"/>
    <file name="vga_controller.vhd" type="vhdl" group="hdl" path="hdl/vga_controller.vhd"/>
    <file name="vga_generate.vhd" type="vhdl" group="hdl" path="hdl/vga_generate.vhd"/>
  </files>
  <resets>
    <reset name="reset_n" group="reset_n" direction="in"/>
  </resets>
  <ports>
    <port name="vga_blank_n" type="out" size="1"/>
    <port name="vga_r" type="out" size="8"/>
    <port name="vga_g" type="out" size="8"/>
    <port name="vga_b" type="out" size="8"/>
    <port name="vga_clk" type="out" size="1"/>
    <port name="vga_hs" type="out" size="1"/>
    <port name="vga_vs" type="out" size="1"/>
    <port name="vga_sync_n" type="out" size="1"/>
  </ports>
  <flows>
    <flow name="in" size="8" type="in"/>
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
  <clocks>
    <clock name="vga_clk108" typical="108000000" direction="in"/>
  </clocks>
  <pins/>
</io>
