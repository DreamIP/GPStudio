set_time_format -unit ns -decimal_places 3
#**************************************************************
# Create Clock
#**************************************************************

create_clock -name {altera_reserved_tck} -period 100.000 -waveform { 0.000 50.000 } [get_ports {altera_reserved_tck}]
create_clock -name {ethernet_rgmii_rx_clk} -period 8.000 -waveform { 0.000 4.000 } [get_ports {ethernet_rgmii_rx_clk}]
create_clock -name {ethernet_rgmii_gtx_clk} -period 8.000 -waveform { 0.000 4.000 } [get_ports {ethernet_rgmii_gtx_clk}]

create_clock -name {clk_50M_i} -period 20.000 -waveform { 0.000 10.000 } [get_ports {clk_50M_i}]
create_clock -name {clk_125M_i} -period 8.000 -waveform { 0.000 4.000 } [get_ports {clk_125M_i}]

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
set_input_delay -add_delay -max -clock [get_clocks {ethernet_rgmii_rx_clk}]  0.500 [get_ports {ethernet_rgmii_rx_data*}]
set_input_delay -add_delay -min -clock [get_clocks {ethernet_rgmii_rx_clk}]  -0.500 [get_ports {ethernet_rgmii_rx_data*}]

set_input_delay -add_delay -max -clock [get_clocks {ethernet_rgmii_rx_clk}]  0.500 [get_ports {ethernet_rgmii_rx_dv}]
set_input_delay -add_delay -min -clock [get_clocks {ethernet_rgmii_rx_clk}]  -0.500 [get_ports {ethernet_rgmii_rx_dv}]
#**************************************************************
# Set Output Delay
#**************************************************************
set_output_delay -add_delay -max -clock [get_clocks {ethernet_rgmii_gtx_clk}]  1.000 [get_ports {ethernet_rgmii_tx_data*}]
set_output_delay -add_delay -min -clock [get_clocks {ethernet_rgmii_gtx_clk}]  -0.800 [get_ports {ethernet_rgmii_tx_data*}]

set_output_delay -add_delay -max -clock [get_clocks {ethernet_rgmii_gtx_clk}]  1.000 [get_ports {ethernet_rgmii_tx_en}]
set_output_delay -add_delay -min -clock [get_clocks {ethernet_rgmii_gtx_clk}]  -0.800 [get_ports {ethernet_rgmii_tx_en}]
