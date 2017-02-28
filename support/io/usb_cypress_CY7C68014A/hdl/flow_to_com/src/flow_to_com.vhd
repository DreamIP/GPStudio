-- **************************************************************************
--	FLOW IN
-- **************************************************************************
-- This component is connected to USB Driver and generate FV/DV/data as outputs
-- 26/11/2014 - creation - C.Bourrasset
--------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.ComFlow_pkg.all;

entity flow_to_com is
    generic (
        FIFO_DEPTH          : integer := 1024;
        FLOW_ID             : integer := 1;
        PACKET_SIZE         : integer := 256;
        FLAGS_CODES         : my_array_t := InitFlagCodes
    );
    port (
        clk_in              : in std_logic;
        clk_out             : in std_logic;
        rst_n               : in std_logic;

        in_data             : in std_logic_vector(15 downto 0);
        in_fv               : in std_logic;
        in_dv               : in std_logic;
        rdreq_i             : in std_logic;
        enable_i            : in std_logic;

        data_o              : out std_logic_vector(15 downto 0);
        flow_rdy_o          : out std_logic;
        f_empty_o           : out std_logic
    );
end flow_to_com;

architecture rtl of flow_to_com is
---------------------------------------------------------
--	COMPONENT DECLARATION
---------------------------------------------------------
component com_flow_fifo_tx
    generic (
        FIFO_DEPTH          : integer := 1024;
        FLOW_ID             : integer := 1;
        PACKET_SIZE         : integer := 256;
        FLAGS_CODES         : my_array_t := InitFlagCodes
    );
    port (
        clk_in_i            : in std_logic;
        clk_out_i           : in std_logic;

        rst_n_i             : in std_logic;

        data_wr_i           : in std_logic;
        data_i              : in std_logic_vector(15 downto 0);
        rdreq_i             : in std_logic;
        flag_wr_i           : in std_logic;
        flag_i              : in std_logic_vector(7 downto 0);

        -- fifo pkt inputs
        fifo_pkt_wr_i       : in std_logic;
        fifo_pkt_data_i     : in std_logic_vector(15 downto 0);

        data_o              : out std_logic_vector(15 downto 0);
        flow_rdy_o          : out std_logic;
        f_empty_o           : out std_logic;
        fifos_f_o           : out std_logic
    );
end component;

component write_flow is
    generic (
        PACKET_SIZE         : integer := 256;
        FLAGS_CODES         : my_array_t := InitFlagCodes
    );
    port (
        clk                 : in std_logic;
        rst_n               : in std_logic;

        in_data             : in std_logic_vector(15 downto 0);
        in_fv               : in std_logic;
        in_dv               : in std_logic;
        enable_i            : in std_logic;
        fifo_f_i            : in std_logic;

        data_wr_o           : out std_logic;
        data_o              : out std_logic_vector(15 downto 0);
        flag_wr_o           : out std_logic;
        flag_o              : out  std_logic_vector(7 downto 0);

        fifo_pkt_wr_o       : out std_logic;
        fifo_pkt_data_o     : out std_logic_vector(15 downto 0)
    );
end component;

---------------------------------------------------------
--	SIGNALS FOR INTERCONNECT
---------------------------------------------------------
	signal fifo_f_s         : std_logic := '0';
	signal data_wr_s        : std_logic := '0';
	signal data_s           : std_logic_vector(15 downto 0) := (others=>'0');

	signal fifo_pkt_wr_s    : std_logic;
	signal fifo_pkt_data_s  : std_logic_vector(15 downto 0);

	-- may add CDC component for flag
	signal flag_s           : std_logic_vector(7 downto 0) := (others=>'0');
	signal flag_wr_s        : std_logic := '0';

begin

-- port map
WRFLOW_process : component write_flow
generic map (
    PACKET_SIZE             => PACKET_SIZE,
    FLAGS_CODES             => FLAGS_CODES
)
port map (
    clk                     => clk_in,
    rst_n                   => rst_n,
    in_data                 => in_data,
    in_fv                   => in_fv,
    in_dv                   => in_dv,
    enable_i                => enable_i,
    fifo_f_i                => fifo_f_s,
    data_wr_o               => data_wr_s,
    data_o                  => data_s,
    flag_wr_o               => flag_wr_s ,
    flag_o                  => flag_s,
    fifo_pkt_wr_o           => fifo_pkt_wr_s,
    fifo_pkt_data_o         => fifo_pkt_data_s
);

ComFlowFifoTX_inst : component com_flow_fifo_tx
generic map (
    FIFO_DEPTH              => FIFO_DEPTH,
    FLOW_ID                 => FLOW_ID,
    PACKET_SIZE             => PACKET_SIZE,
    FLAGS_CODES             => FLAGS_CODES
)
port map (
    data_wr_i               => data_wr_s,
    data_i                  => data_s,
    rdreq_i                 => rdreq_i,
    flag_wr_i               => flag_wr_s,
    flag_i                  => flag_s,
    fifo_pkt_wr_i           => fifo_pkt_wr_s,
    fifo_pkt_data_i         => fifo_pkt_data_s,
    data_o                  => data_o,
    flow_rdy_o              => flow_rdy_o,
    f_empty_o               => f_empty_o,
    fifos_f_o               => fifo_f_s,
    clk_in_i                => clk_in,
    clk_out_i               => clk_out,
    rst_n_i                 => rst_n and enable_i
);

end rtl;
