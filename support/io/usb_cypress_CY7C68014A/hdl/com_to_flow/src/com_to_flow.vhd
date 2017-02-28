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

entity com_to_flow is
    generic (
        FIFO_DEPTH  : POSITIVE := 1024;
        FLOW_ID     : INTEGER := 1;
        FLAGS_CODES : my_array_t := InitFlagCodes;
        OUTPUT_SIZE : INTEGER := 16
    );
    port (
        clk_in      : in std_logic;
        clk_out     : in std_logic;
        rst_n       : in std_logic;
        
        data_wr_i   : in std_logic;
        data_i      : in std_logic_vector(15 downto 0);
        pktend_i    : in std_logic;
        enable_i    : in std_logic;

        data_o      : out std_logic_vector(OUTPUT_SIZE-1 downto 0);
        fv_o        : out std_logic;
        dv_o        : out std_logic;
        flow_full_o : out std_logic
    );
end com_to_flow;

architecture rtl of com_to_flow is

---------------------------------------------------------
--	COMPONENT DECLARATION
---------------------------------------------------------
    component com_flow_fifo_rx
        generic (
            FIFO_DEPTH  : POSITIVE := 1024;
            FLOW_ID     : integer := 1
        );
        port (
            clk_in      : in std_logic;
            clk_out     : in std_logic;

            rst_n       : in std_logic;

            data_wr_i   : in std_logic;
            data_i      : in std_logic_vector(15 downto 0);
            rdreq_i     : in std_logic;
            pktend_i    : in std_logic;
            enable_i    : in std_logic;

            data_o      : out std_logic_vector(15 downto 0);
            flow_rdy_o  : out std_logic;
            f_empty_o   : out std_logic;
            fifos_f_o   : out std_logic;
            flag_o      : out std_logic_vector(7 downto 0)
        );
    end component;

    component synchronizer
        generic (
            CDC_SYNC_FF_CHAIN_DEPTH: integer := 2 -- CDC Flip flop Chain depth
        );
        port (
            signal_i    : in std_logic;
            signal_o    : out std_logic;
            clk_i       : in std_logic;
            clk_o       : in std_logic
        );
    end component;

    component read_flow_nbits is
        generic (
            FLAGS_CODES : my_array_t := InitFlagCodes;
            DATA_SZ     : integer
        );
        port (
            clk         : in std_logic;
            rst_n       : in std_logic;

            data_i      : in std_logic_vector(15 downto 0);
            flow_rdy_i  : in std_logic;
            f_empty_i   : in std_logic;
            enable_i    : in std_logic;
            flag_i      : in  std_logic_vector(7 downto 0);

            read_data_o : out std_logic;
            data_o      : out std_logic_vector(DATA_SZ-1 downto 0);
            fv_o        : out std_logic;
            dv_o        : out std_logic
        );
    end component;

---------------------------------------------------------
--	SIGNALS FOR INTERCONNECT
---------------------------------------------------------
	signal flow_rdy_s           : std_logic := '0';
	signal flow_rdy_resync_s    : std_logic := '0';
	signal read_data_s          : std_logic := '0';
	signal empty_s              : std_logic := '0';
	signal data_o_s             : std_logic_vector(15 downto 0) := (others=>'0');
	signal fifo_rx_end_s        : std_logic := '0';
	signal flag_s               : std_logic_vector(7 downto 0) := (others=>'0');
begin

-- port map
ComFlowFifoRx_inst : component com_flow_fifo_rx
generic map (
    FIFO_DEPTH  => FIFO_DEPTH,
    FLOW_ID     => FLOW_ID
)
port map (
    clk_in      => clk_in,
    clk_out     => clk_out,
    rst_n       => rst_n,

    data_wr_i   => data_wr_i,
    data_i      => data_i,
    rdreq_i     => read_data_s,
    pktend_i    => pktend_i,
    enable_i    => enable_i,

    data_o      => data_o_s,
    flow_rdy_o  => flow_rdy_s,
    flag_o      => flag_s,
    f_empty_o   => empty_s,
    fifos_f_o   => flow_full_o
);

-- CDC Synchronizer
Sync_inst : component synchronizer
generic map (
    CDC_SYNC_FF_CHAIN_DEPTH => 2
)
port map (
    clk_i       => clk_in,
    clk_o       => clk_out,
    signal_i    => flow_rdy_s,
    signal_o    => flow_rdy_resync_s
);

RD_process : component read_flow_nbits
generic map (
    FLAGS_CODES => FLAGS_CODES,
    DATA_SZ     => OUTPUT_SIZE
)
port map (
    clk         => clk_out,
    rst_n       => rst_n,
    data_i      => data_o_s,
    flow_rdy_i  => flow_rdy_resync_s,
    f_empty_i   => empty_s,
    enable_i    => enable_i,
    flag_i      => flag_s,
    read_data_o => read_data_s,
    data_o      => data_o,
    fv_o        => fv_o,
    dv_o        => dv_o
);

end rtl;
