-- **************************************************************************
--	FLOW IN 
-- **************************************************************************
-- This component is connected to USB Driver and generate FV/DV/data as outputs
-- 26/11/2014 - creation - C.Bourrasset
--------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- entity "uses" the package   
use work.ComFlow_pkg.all;

entity flow_out is
  generic (
	FIFO_DEPTH : POSITIVE := 1024;
	FLOW_ID : integer := 1;
	PACKET_SIZE : integer := 256; 
	FLAGS_CODES : my_array_t := InitFlagCodes
    );
  port(

	data_i : in std_logic_vector(15 downto 0);
	fv_i: in std_logic;
	dv_i : in std_logic;
	rdreq_i : in std_logic;
	enable_i : in std_logic;
	
	data_o : out std_logic_vector(15 downto 0);
	flow_rdy_o: out std_logic;	
	f_empty_o : out std_logic;
	
--	fifos_f_o : out std_logic;
	
	clk_in_i : in std_logic;
	clk_out_i :in std_logic;
	rst_n_i :in std_logic 
    );
	
end flow_out;

architecture rtl of flow_out is
---------------------------------------------------------
--	COMPONENT DECLARATION
---------------------------------------------------------
component  com_flow_fifo_tx 
  generic (
	FIFO_DEPTH : POSITIVE := 1024;
	FLOW_ID : integer := 1;
	PACKET_SIZE : integer := 256; 
	FLAGS_CODES : my_array_t := InitFlagCodes
    );
  port(
	data_wr_i : in std_logic;
    data_i : in std_logic_vector(15 downto 0);
	rdreq_i : in std_logic;
	pktend_i : in std_logic;
	flag_wr_i : in std_logic;
	flag_i : in std_logic_vector(7 downto 0);
	
	-- TODO
	pkt_size_i : in std_logic_vector(15 downto 0);
	
	data_o : out std_logic_vector(15 downto 0);
	flow_rdy_o: out std_logic;
	f_empty_o : out std_logic;
	fifos_f_o : out std_logic;
	
	clk_in_i : in std_logic;
	clk_out_i :in std_logic;
	
	rst_n_i :in std_logic 
    );
end component;

component synchronizer 
  generic (
	CDC_SYNC_FF_CHAIN_DEPTH: integer := 2 -- CDC Flip flop Chain depth
	);
  port(
	signal_i : in std_logic;
	signal_o : out std_logic;
	clk_i: in std_logic;
	clk_o: in std_logic
	);
end component;

component write_flow is
  generic (
  	PACKET_SIZE : integer := 256; 
	FLAGS_CODES : my_array_t := InitFlagCodes
    );
  port(

	data_i : in std_logic_vector(15 downto 0);
	fv_i: in std_logic;
	dv_i : in std_logic;
	enable_i : in std_logic;
	fifo_f_i : in std_logic;

	data_wr_o : out std_logic;
    data_o : out std_logic_vector(15 downto 0);
	pktend_o : out std_logic;
	flag_wr_o :out std_logic;
	flag_o :out  std_logic_vector(7 downto 0);
	
	clk_i :in std_logic;
	rst_n_i :in std_logic
    );
end component;
	
---------------------------------------------------------
--	SIGNALS FOR INTERCONNECT
---------------------------------------------------------
	signal fifo_f_s: std_logic:='0';
	signal data_wr_s: std_logic:='0';
	signal data_s: std_logic_vector(15 downto 0):=(others=>'0');
	signal pktend_s: std_logic:='0';
	signal pktend_resync_s: std_logic:='0';
	
	-- may add CDC component for flag
	signal flag_s: std_logic_vector(7 downto 0):=(others=>'0');
	signal flag_wr_s:std_logic:='0';
	
begin

-- port map 
WRFLOW_process : component write_flow
  generic map(
  PACKET_SIZE =>PACKET_SIZE,
  FLAGS_CODES => FLAGS_CODES)
  port map(
	data_i => data_i,
	fv_i => fv_i,
	dv_i => dv_i,
	enable_i => enable_i,
	fifo_f_i => fifo_f_s,
	data_wr_o => data_wr_s,
    data_o => data_s,
	pktend_o => pktend_s,
	flag_wr_o => flag_wr_s ,
	flag_o => flag_s,
	clk_i => clk_in_i,
	rst_n_i =>rst_n_i
 );
 
-- CDC Synchronizer
pktend_sync_inst : component synchronizer
 generic map (CDC_SYNC_FF_CHAIN_DEPTH=>2)
 port map(
	signal_i => pktend_s,
	signal_o => pktend_resync_s,
	clk_i => clk_in_i,
	clk_o => clk_out_i
 );
 
 
ComFlowFifoTX_inst : component com_flow_fifo_tx 
  generic map(
	FIFO_DEPTH => FIFO_DEPTH,
	FLOW_ID => FLOW_ID,
	PACKET_SIZE => PACKET_SIZE, 
	FLAGS_CODES => FLAGS_CODES
    )
  port map(
	data_wr_i => data_wr_s,
    data_i => data_s,
	rdreq_i => rdreq_i,
	pktend_i => pktend_resync_s,
	pkt_size_i => (others=>'0'),
	flag_wr_i => flag_wr_s,
	flag_i=> flag_s,
	data_o => data_o,
	flow_rdy_o => flow_rdy_o,
	f_empty_o => f_empty_o,
	fifos_f_o => fifo_f_s,  
	clk_in_i => clk_in_i,
	clk_out_i =>clk_out_i,
	rst_n_i  => rst_n_i or not(enable_i)
    );

end rtl;

