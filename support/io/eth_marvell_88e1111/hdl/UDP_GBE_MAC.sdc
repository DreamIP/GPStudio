set_time_format -unit ns -decimal_places 3
#**************************************************************
# Create Clock
#**************************************************************

create_clock -name {altera_reserved_tck} -period 100.000 -waveform { 0.000 50.000 } [get_ports {altera_reserved_tck}]
##create_clock -name {clk_125M_i} -period 8.000 -waveform { 0.000 4.000 } [get_ports {clk_125M_i}]
create_clock -name {CLK50M} -period 20.000 -waveform { 0.000 10.000 } [get_ports {CLK50M}]
create_clock -name {ENET1_RX_CLK} -period 8.000 -waveform { 0.000 4.000 } [get_ports {ENET1_RX_CLK}]
create_clock -name {ENET1_GTX_CLK} -period 8.000 -waveform { 0.000 4.000 } [get_ports {ENET1_GTX_CLK}]

#**************************************************************
# Create Generated Clock
#**************************************************************

derive_pll_clocks

#**************************************************************
# Set Clock Uncertainty
#**************************************************************
derive_clock_uncertainty

#**************************************************************
# Set Input Delay
#**************************************************************
#set_input_delay -add_delay -max -clock [get_clocks {ENET1_RX_CLK}]  0.500 [get_ports {ENET1_RX_DATA*}]
#set_input_delay -add_delay -min -clock [get_clocks {ENET1_RX_CLK}]  -0.500 [get_ports {ENET1_RX_DATA*}]
#
#set_input_delay -add_delay -max -clock [get_clocks {ENET1_RX_CLK}]  0.500 [get_ports {ENET1_RX_DV}]
#set_input_delay -add_delay -min -clock [get_clocks {ENET1_RX_CLK}]  -0.500 [get_ports {ENET1_RX_DV}]
#**************************************************************
# Set Output Delay
#**************************************************************
#set_output_delay -add_delay -max -clock [get_clocks {ENET1_GTX_CLK}]  1.000 [get_ports {ENET1_TX_DATA*}]
#set_output_delay -add_delay -min -clock [get_clocks {ENET1_GTX_CLK}]  -0.800 [get_ports {ENET1_TX_DATA*}]
#
#set_output_delay -add_delay -max -clock [get_clocks {ENET1_GTX_CLK}]  1.000 [get_ports {ENET1_TX_EN}]
#set_output_delay -add_delay -min -clock [get_clocks {ENET1_GTX_CLK}]  -0.800 [get_ports {ENET1_TX_EN}]