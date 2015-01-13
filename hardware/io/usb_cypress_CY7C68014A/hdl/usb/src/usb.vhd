library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.ComFlow_pkg.all;

entity usb is
	generic(
		MASTER_ADDR_WIDTH : integer
	);
	port(
		clk_proc : in std_logic;
		reset : out std_logic;

		------ external ports ------
		usb_rst : in std_logic;
		usb_ifclk : in std_logic;
		usb_flaga : in std_logic;
		usb_flagb : in std_logic;
		usb_flagc : in std_logic;
		usb_flagd : in std_logic;
		usb_fd_io : inout std_logic_vector(15 downto 0);
		usb_sloe : out std_logic;
		usb_slrd : out std_logic;
		usb_slwr : out std_logic;
		usb_pktend : out std_logic;
		usb_addr : out std_logic_vector(1 downto 0);

		------ in0 flow ------
		in0_data : in std_logic_vector(15 downto 0);
		in0_fv : in std_logic;
		in0_dv : in std_logic;

		------ out0 flow ------
		out0_data : out std_logic_vector(15 downto 0);
		out0_fv : out std_logic;
		out0_dv : out std_logic;

		---- ===== Masters =====

		------ bus_master ------
		master_addr_o : out std_logic_vector(MASTER_ADDR_WIDTH-1 downto 0);
		master_wr_o : out std_logic;
		master_rd_o : out std_logic;
		master_datawr_o : out std_logic_vector(31 downto 0);
		master_datard_i : in std_logic_vector(31 downto 0);

		---- ===== Slaves =====

		------ bus_sl ------
		addr_rel_i : in std_logic_vector(3 downto 0);
		wr_i : in std_logic;
		rd_i : in std_logic;
		datawr_i : in std_logic_vector(31 downto 0);
		datard_o : out std_logic_vector(31 downto 0)
	);
end entity;


architecture rtl of usb is

-- SIGNAUX INTERNES NON CONNECTE
signal flow_out_sel_s : std_logic:='0';
signal source_sel_s:std_logic:='0';

-- SLAVE BUS 
	signal enable_s : std_logic :='0';

-- SYNC BUS
	signal enable_s_fvsync : std_logic:='0';
	
-- USBFLOW_IN
	signal flow_in1_data_s  : std_logic_vector(15 downto 0):=(others=>'0');
	signal flow_in1_wr_s : std_logic:='0';
	signal flow_in1_full_s :	std_logic:='0';
	signal flow_in1_pktend_s : std_logic:='0';
	
-- USBFLOW OUT
	signal flow_out_data_s : std_logic_vector(15 downto 0):=(others=>'0');
	signal flow_out_empty_s : std_logic:='0';
	signal flow_out_rd_s : std_logic:='0';
	signal flow_out_rdy_s : std_logic:='0';

-- FLOW_PARAMS
	signal update_port_s : std_logic:='0';

-- USB_SM_INST

	
begin

reset <= usb_rst;
		
USB_SM_INST : usb_sm
    generic map (
	  FLOW_STATUS_ID => 253,
	  NB_FLOW => 2, -- NBFLOW is declared in ComFlow_pkg package
	  IDFLOW =>  (1, 128)
	  )
    port map (
		usb_ifclk    => usb_ifclk,
		usb_flaga    => usb_flaga,
		usb_flagb    => usb_flagb,
		usb_flagc    => usb_flagc,
		usb_flagd    => usb_flagd,
		usb_fd_io    => usb_fd_io,
		usb_sloe     => usb_sloe,
		usb_slrd     => usb_slrd,
		usb_slwr     => usb_slwr,
		usb_pktend   => usb_pktend,
		usb_rst		 => usb_rst,
		usb_addr     => usb_addr,

		flow_in_data_o  => flow_in1_data_s,
		flow_in_wr_o => flow_in1_wr_s,
		flow_in_full_i => flow_in1_full_s,
		flow_in_end_o =>  flow_in1_pktend_s,

		flow_out_data_i  => flow_out_data_s,
		flow_out_rd_o => flow_out_rd_s,
		flow_out_empty_i => flow_out_empty_s,
		flow_out_rdy_i => flow_out_rdy_s
		);
		
SLAVE_BUS_INST: component enable_gen	
		generic map(
			DATA_WIDTH => 32,
			N_WORDS => 16
		)
		port map (
			clk_i => clk_proc,
			rst_n_i => usb_rst,	
			addr_i => addr_rel_i,		--(addr_rel_0_o),
			wr_i => wr_i,				--(wr_0_o),
			datawr_i => datawr_i, 		--(data_wr_0_o)	
			en_o => enable_s,
			flow_out_sel_o => flow_out_sel_s,
			source_sel_o => source_sel_s
			);
		
ENABLE_SYNC_FV: component fv_synchro_signal
  port map(
	fv_i => in0_fv,
	signal_i => enable_s,
	signal_o => enable_s_fvsync,
	clk_i => clk_proc,
	rst_n_i => usb_rst
);

--FLOW_IN
USBFLOW_IN: component flow_in
  generic map(
	FIFO_DEPTH => 1024,
	FLOW_ID => 1,
	FLAGS_CODES => InitFlagCodes
    )
  port map(
	data_wr_i =>flow_in1_wr_s,
	data_i => flow_in1_data_s,
	pktend_i => flow_in1_pktend_s,
	enable_i => enable_s_fvsync,

	data_o => out0_data,
	fv_o => out0_fv,
	dv_o => out0_dv,
	flow_full_o => flow_in1_full_s,
		
	clk_in_i =>usb_ifclk,
	clk_out_i =>usb_ifclk,
	rst_n_i =>usb_rst
    );


--FLOW OUT
USBFLOW_OUT: component flow_out 
  generic map (
	FIFO_DEPTH => 512*64,
	FLOW_ID => 128,
	--PACKET_SIZE => TX_PACKET_SIZE,
	PACKET_SIZE => 256, -- header inclus
	FLAGS_CODES => InitFlagCodes
    )
  port map(
	data_i => in0_data,
	fv_i => in0_fv,
	dv_i => in0_dv,
	rdreq_i => flow_out_rd_s,
	enable_i => enable_s_fvsync,
	data_o => flow_out_data_s,
	flow_rdy_o=> flow_out_rdy_s,
	f_empty_o => flow_out_empty_s,
	
	clk_in_i => clk_proc, 
	clk_out_i => usb_ifclk,
	rst_n_i=> usb_rst
);

--  FLOW_PARAMS module --> Bus Interconnect Master
FLOW_PARAMS: component flow_wishbone 
		generic map(
		FIFO_DEPTH => 64, 
		FLOW_ID_SET => 15,
		MASTER_ADDR_WIDTH=>MASTER_ADDR_WIDTH
		)
		port map(
			data_wr_i => flow_in1_wr_s,
			data_i => flow_in1_data_s,
			pktend_i => flow_in1_pktend_s,
			
			param_addr_o => master_addr_o,
			param_data_o => master_datawr_o,
			param_wr_o => master_wr_o,
			
			-- rajouter fin d'ecriture dans la memoire...
			tmp_update_port_o => update_port_s,
			clk_in_i => usb_ifclk,
			clk_out_i => clk_proc, 
			rst_n_i => usb_rst
		);
end rtl;
