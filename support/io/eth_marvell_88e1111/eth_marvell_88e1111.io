<?xml version="1.0" encoding="UTF-8"?>
<io driver="eth_marvell_88e1111" categ="communication" pi_size_addr_rel="4" master_count="1" desc="">
  <parts>
    <part name="inputs">
      <svg width="160" height="200">
        <rect width="160" height="200" x="0" y="0" fill="lightgray"/>
        <g transform="rotate(90)">
          <text x="60" y="-140" font-size = "14">To computer</text>
        </g>
        <g id="eth" transform="translate(30,55) scale(0.01)">
          <path d="M2910 6080 l0 -1240 585 0 585 0 0 -325 0 -325 -1955 0 -1955 0 0 -165 0 -165 850 0 850 0 -2 -347 -3 -348 -627 -3 -628 -2 0 -1240 0 -1240 1418 2 1417 3 3 1238 2 1237 -565 0 -565 0 0 350 0 350 2015 0 2015 0 0 -330 0 -330 -640 0 -640 0 0 -1245 0 -1245 1420 0 1420 0 -2 1243 -3 1242 -552 3 -553 2 0 330 0 330 515 0 515 0 0 165 0 165 -1650 0 -1650 0 2 323 3 322 608 3 607 2 -2 1238 -3 1237 -1417 3 -1418 2 0 -1240z m2390 -5 l0 -795 -975 0 -975 0 0 795 0 795 975 0 975 0 0 -795z m2160 -4120 l0 -795 -975 0 -975 0 0 795 0 795 975 0 975 0 0 -795z m-4460 -30 l0 -795 -975 0 -975 0 0 795 0 795 975 0 975 0 0 -795z"/>
        </g>
      </svg>
      <flows>
        <flow name="in0"/>
        <flow name="in1"/>
        <flow name="in2"/>
        <flow name="in3"/>
      </flows>
    </part>
    <part name="outputs">
      <svg width="160" height="200">
        <rect width="160" height="200" x="0" y="0" fill="lightgray"/>
        <g transform="rotate(90)">
          <text x="50" y="-10" font-size = "14">From computer</text>
        </g>
        <g id="eth" transform="translate(30,55) scale(0.01)">
          <path d="M2910 6080 l0 -1240 585 0 585 0 0 -325 0 -325 -1955 0 -1955 0 0 -165 0 -165 850 0 850 0 -2 -347 -3 -348 -627 -3 -628 -2 0 -1240 0 -1240 1418 2 1417 3 3 1238 2 1237 -565 0 -565 0 0 350 0 350 2015 0 2015 0 0 -330 0 -330 -640 0 -640 0 0 -1245 0 -1245 1420 0 1420 0 -2 1243 -3 1242 -552 3 -553 2 0 330 0 330 515 0 515 0 0 165 0 165 -1650 0 -1650 0 2 323 3 322 608 3 607 2 -2 1238 -3 1237 -1417 3 -1418 2 0 -1240z m2390 -5 l0 -795 -975 0 -975 0 0 795 0 795 975 0 975 0 0 -795z m2160 -4120 l0 -795 -975 0 -975 0 0 795 0 795 975 0 975 0 0 -795z m-4460 -30 l0 -795 -975 0 -975 0 0 795 0 795 975 0 975 0 0 -795z" fill="blue"/>
        </g>
      </svg>
      <flows>
        <flow name="out0"/>
        <flow name="out1"/>
      </flows>
    </part>
  </parts>
  <files>
    <file name="Marvell-Alaska-Ultra-88E1111-GbE.pdf" type="doc" group="doc" path="doc/Marvell-Alaska-Ultra-88E1111-GbE.pdf"/>

    <file name="arpv2.vhd" type="vhdl" group="hdl" path="hdl/ARP/arpv2.vhd"/>
    <file name="arp_RX.vhd" type="vhdl" group="hdl" path="hdl/ARP/arp_RX.vhd"/>
    <file name="arp_SYNC.vhd" type="vhdl" group="hdl" path="hdl/ARP/arp_SYNC.vhd"/>
    <file name="arp_STORE_br.vhd" type="vhdl" group="hdl" path="hdl/ARP/arp_STORE_br.vhd"/>
    <file name="arp_types.vhd" type="vhdl" group="hdl" path="hdl/ARP/arp_types.vhd"/>
    <file name="arp_TX.vhd" type="vhdl" group="hdl" path="hdl/ARP/arp_TX.vhd"/>
    <file name="arp.vhd" type="vhdl" group="hdl" path="hdl/ARP/arp.vhd"/>
    <file name="arp_REQ.vhd" type="vhdl" group="hdl" path="hdl/ARP/arp_REQ.vhd"/>

    <file name="tx_arbitrator.vhd" type="vhdl" group="hdl" path="hdl/other/tx_arbitrator.vhd"/>
    <file name="tx_arbitrator_over_ip.vhd" type="vhdl" group="hdl" path="hdl/other/tx_arbitrator_over_ip.vhd"/>

    <file name="ff_icmp_inst.vhd" type="vhdl" group="hdl" path="hdl/ICMP/ff_icmp_inst.vhd"/>
    <file name="ff_icmp.qip" type="vhdl" group="hdl" path="hdl/ICMP/ff_icmp.qip"/>
    <file name="ff_icmp.cmp" type="vhdl" group="hdl" path="hdl/ICMP/ff_icmp.cmp"/>
    <file name="ff_icmp.vhd" type="vhdl" group="hdl" path="hdl/ICMP/ff_icmp.vhd"/>
    <file name="icmp.vhd" type="vhdl" group="hdl" path="hdl/ICMP/icmp.vhd"/>

    <file name="rgmii_tx_top_2.vhd" type="vhdl" group="hdl" path="hdl/RGMII_MAC/rgmii_tx_top_2.vhd"/>
    <file name="rgmii_rx_top_2.vhd" type="vhdl" group="hdl" path="hdl/RGMII_MAC/rgmii_rx_top_2.vhd"/>
    <file name="rgmii_rx.vhd" type="vhdl" group="hdl" path="hdl/RGMII_MAC/rgmii_rx.vhd"/>
    <file name="rgmii_mdio.vhd" type="vhdl" group="hdl" path="hdl/RGMII_MAC/rgmii_mdio.vhd"/>
    <file name="fifo_tx_udp.cmp" type="vhdl" group="hdl" path="hdl/RGMII_MAC/fifo_tx_udp.cmp"/>
    <file name="fifo_tx_udp_inst.vhd" type="vhdl" group="hdl" path="hdl/RGMII_MAC/fifo_tx_udp_inst.vhd"/>
    <file name="eth_crc32.vhd" type="vhdl" group="hdl" path="hdl/RGMII_MAC/eth_crc32.vhd"/>
    <file name="eth_ddr_in.vhd" type="vhdl" group="hdl" path="hdl/RGMII_MAC/eth_ddr_in.vhd"/>
    <file name="eth_mdio.vhd" type="vhdl" group="hdl" path="hdl/RGMII_MAC/eth_mdio.vhd"/>
    <file name="gbe_mac.vhd" type="vhdl" group="hdl" path="hdl/RGMII_MAC/gbe_mac.vhd"/>
    <file name="fifo_tx_udp.vhd" type="vhdl" group="hdl" path="hdl/RGMII_MAC/fifo_tx_udp.vhd"/>
    <file name="rgmii1000_io.vhd" type="vhdl" group="hdl" path="hdl/RGMII_MAC/rgmii1000_io.vhd"/>
    <file name="rgmii_tx_2.vhd" type="vhdl" group="hdl" path="hdl/RGMII_MAC/rgmii_tx_2.vhd"/>
    <file name="fifo_tx_udp.qip" type="vhdl" group="hdl" path="hdl/RGMII_MAC/fifo_tx_udp.qip"/>
    <file name="eth_ddr_out.vhd" type="vhdl" group="hdl" path="hdl/RGMII_MAC/eth_ddr_out.vhd"/>
    <file name="rgmii1000_pll.vhd" type="vhdl" group="hdl" path="hdl/RGMII_MAC/rgmii1000_pll.vhd"/>

    <file name="eth_pkg.vhd" type="vhdl" group="hdl" path="hdl/types/eth_pkg.vhd"/>
    <file name="axi.vhd" type="vhdl" group="hdl" path="hdl/types/axi.vhd"/>
    <file name="ipv4_types.vhd" type="vhdl" group="hdl" path="hdl/types/ipv4_types.vhd"/>

    <file name="udp_ip_stack.qsf" type="vhdl" group="hdl" path="hdl/udp_ip_stack.qsf"/>
    <file name="IPv4_RX.vhd" type="vhdl" group="hdl" path="hdl/IP/IPv4_RX.vhd"/>
    <file name="IPv4.vhd" type="vhdl" group="hdl" path="hdl/IP/IPv4.vhd"/>
    <file name="IPv4_TX.vhd" type="vhdl" group="hdl" path="hdl/IP/IPv4_TX.vhd"/>
    <file name="IP_complete_nomac.vhd" type="vhdl" group="hdl" path="hdl/IP/IP_complete_nomac.vhd"/>

    <file name="UDP_TX.vhd" type="vhdl" group="hdl" path="hdl/UDP/UDP_TX.vhd"/>
    <file name="UDP_RX.vhd" type="vhdl" group="hdl" path="hdl/UDP/UDP_RX.vhd"/>
    <file name="UDP_Complete_nomac.vhd" type="vhdl" group="hdl" path="hdl/UDP/UDP_Complete_nomac.vhd"/>
    <file name="UDP_GBE_MAC.sdc" type="sdc" group="hdl" path="hdl/UDP_GBE_MAC.sdc"/>
    <file name="UDP_MAC_GE.vhd" type="vhdl" group="hdl" path="hdl/UDP_MAC_GE.vhd"/>

    <file name="eth_marvell_88e1111.vhd" type="vhdl" group="hdl" path="hdl/eth_marvell_88e1111.vhd"/>
    <file name="eth_marvell_88e1111.io" type="io" group="blockdef" path="usb_cypress_CY7C68014A.io"/>
  </files>
  <resets>
    <reset name="reset" group="reset_n" direction="out"/>
  </resets>
  <flows>
    <flow name="in0" size="8" type="in" desc="flow 0 return for supervise results of process"/>
    <flow name="in1" size="8" type="in" desc="flow 1 return for supervise results of process"/>
    <flow name="in2" size="8" type="in" desc="flow 2 return for supervise results of process"/>
    <flow name="in3" size="8" type="in" desc="flow 3 return for supervise results of process"/>
    <flow name="out0" size="8" type="out" desc="flow 0 return for supervise results of process">
      <properties>
        <property name="datatype" type="flowtype" value="image"/>
        <property name="width" type="int" value="320"/>
        <property name="height" type="int" value="240"/>
      </properties>
    </flow>
    <flow name="out1" size="8" type="out" desc="flow 1 return for supervise results of process">
      <properties>
        <property name="datatype" type="flowtype" value="image"/>
        <property name="width" type="int" value="320"/>
        <property name="height" type="int" value="240"/>
      </properties>
    </flow>
  </flows>
  <params>
    <param name="IN0_NBWORDS" type="int" hard="1" value="32768"/>
    <param name="IN1_NBWORDS" type="int" hard="1" value="32768"/>
    <param name="IN2_NBWORDS" type="int" hard="1" value="32768"/>
    <param name="IN3_NBWORDS" type="int" hard="1" value="32768"/>
    <param name="OUT0_NBWORDS" type="int" hard="1" value="1024"/>
    <param name="OUT1_NBWORDS" type="int" hard="1" value="1024"/>
    <param name="status" regaddr="0">
      <bitfields>
        <bitfield name="enable" bitfield="0" propertymap="enable.value"/>
      </bitfields>
    </param>
    <param name="flow_in0" regaddr="1">
      <bitfields>
        <bitfield name="enable" bitfield="0" propertymap="enableflow1.value"/>
      </bitfields>
    </param>
    <param name="flow_in1" regaddr="2">
      <bitfields>
        <bitfield name="enable" bitfield="0" propertymap="enableflow2.value"/>
      </bitfields>
    </param>
    <param name="flow_in2" regaddr="3">
      <bitfields>
        <bitfield name="enable" bitfield="0" propertymap="enableflow3.value"/>
      </bitfields>
    </param>
    <param name="flow_in3" regaddr="4">
      <bitfields>
        <bitfield name="enable" bitfield="0" propertymap="enableflow4.value"/>
      </bitfields>
    </param>
  </params>
  <properties>
    <property name="enable" caption="Global enable" type="bool" value="1" desc="Enable or disable process"/>
    <property name="enableflow1" caption="Enable Flow 1" type="bool" value="1"/>
    <property name="enableflow2" caption="Enable Flow 2" type="bool" value="1"/>
    <property name="enableflow3" caption="Enable Flow 3" type="bool" value="1"/>
    <property name="enableflow4" caption="Enable Flow 4" type="bool" value="1"/>
  </properties>
  <clocks>
    <clock name="clk_hal" typical="48000000" direction="out"/>
  </clocks>
  <ports>
    <port name="rst" type="in" size="1"/>
    <port name="ifclk" type="in" size="1"/>
    <port name="flaga" type="in" size="1"/>
    <port name="flagb" type="in" size="1"/>
    <port name="flagc" type="in" size="1"/>
    <port name="flagd" type="in" size="1"/>
    <port name="fd_io" type="inout" size="16"/>
    <port name="sloe" type="out" size="1"/>
    <port name="slrd" type="out" size="1"/>
    <port name="slwr" type="out" size="1"/>
    <port name="pktend" type="out" size="1"/>
    <port name="addr" type="out" size="2"/>
  </ports>
  <pins/>
  <com_driver driverio="usb">
    <com_connects>
      <com_connect link="out0" id="1" type="flowout"/>
      <com_connect link="out1" id="2" type="flowout"/>
      <com_connect link="in0" id="128" type="flowin"/>
      <com_connect link="in1" id="129" type="flowin"/>
      <com_connect link="in2" id="130" type="flowin"/>
      <com_connect link="in3" id="131" type="flowin"/>
      <com_connect link="" id="15" type="paramout"/>
    </com_connects>
    <com_params>
      <com_param name="vendorId" value="0x04B4"/>
      <com_param name="productId" value="0x1003"/>
      <com_param name="EPIN" value="0x86"/>
      <com_param name="EPOUT" value="0x02"/>
      <com_param name="interface" value="0"/>
    </com_params>
  </com_driver>
</io>
