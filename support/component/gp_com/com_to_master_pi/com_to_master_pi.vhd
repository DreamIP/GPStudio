-- **************************************************************************
--	READ FLOW to params
-- *************************************************************************
-- Ce composant est connecte a un com_flow_fifo_rx en entree et gere une zone de params
-- 25/11/2014 - creation
--------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.ComFlow_pkg.all;
use ieee.math_real.all;

entity com_to_master_pi is
    generic (
        FIFO_DEPTH          : POSITIVE := 64;
        FLOW_ID_SET         : INTEGER := 12;
        --FLOW_ID_GET       : integer := 13
        MASTER_ADDR_WIDTH   : POSITIVE := 4;
        DATA_HAL_SIZE       : POSITIVE := 16
    );
    port (
        clk_hal             : in std_logic; -- clk_usb
        clk_proc            : in std_logic; -- clk_design
        rst_n               : in std_logic;

        -- USB driver connexion
        data_wr_i           : in std_logic;
        data_i              : in std_logic_vector(DATA_HAL_SIZE-1 downto 0);
        -- rdreq_i          : in std_logic;
        pktend_i            : in std_logic;
        fifo_full_o         : out std_logic;

        -- signaux pour wishbone
        param_addr_o        : out std_logic_vector(MASTER_ADDR_WIDTH-1 DOWNTO 0);
        param_data_o        : out std_logic_vector(31 downto 0);
        param_wr_o          : out std_logic;

        -- may add RAM arbiter connexion
        -- tmp signal to trigger caph update reg
        tmp_update_port_o   : out std_logic
    );
end com_to_master_pi;

architecture rtl of com_to_master_pi is

component  com_flow_fifo_rx
    generic (
        FIFO_DEPTH  : POSITIVE := 64;
        FLOW_ID     : INTEGER := 12;
        IN_SIZE     : POSITIVE := 16;
        OUT_SIZE    : POSITIVE := 16
    );
    port (
        clk_hal     : in std_logic;
        clk_proc    : in std_logic;
        rst_n       : in std_logic;
        data_wr_i   : in std_logic;
        data_i      : in std_logic_vector(DATA_HAL_SIZE-1 downto 0);
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

-- GET PARAMS :: TODO
------------------------------------------
-- On peut aussi faire sans ce composant: handshake manuel avec le driver USB + prise de bus memoire + depilage manuel

-- component  com_flow_fifo_tx
  -- generic (
	-- FIFO_DEPTH : POSITIVE := 1024;
	-- FLOW_ID : integer := 1;
	-- PACKET_SIZE : integer := 512;
	-- FLAGS_CODES : my_array_t := InitFlagCodes
    -- );
  -- port(
	-- data_wr_i : in std_logic;
    -- data_i : in std_logic_vector(15 downto 0);
	-- rdreq_i : in std_logic;
	-- pktend_i : in std_logic;
	-- flag_i : in std_logic_vector(7 downto 0);
	-- data_o : out std_logic_vector(15 downto 0);
	-- flow_rdy_o: out std_logic;
	-- f_empty_o : out std_logic;
	-- fifos_f_o : out std_logic;
	-- clk_hal : in std_logic;
	-- clk_proc :in std_logic;
	-- rst_n :in std_logic
    -- );
-- end component;

component params_flow_decoder
	generic (
        MASTER_ADDR_WIDTH : POSITIVE := 10
    );
    port (
        clk             : in std_logic;
        rst_n           : in std_logic;

        data_i          : in std_logic_vector(15 downto 0);
        flow_rdy_i      : in std_logic;
        f_empty_i       : in std_logic;
        flag_i          : in std_logic_vector(7 downto 0);
        read_data_o     : out std_logic;

        -- signaux pour wishbone
        param_addr_o    : out std_logic_vector(MASTER_ADDR_WIDTH-1 DOWNTO 0);
        param_data_o    : out std_logic_vector(31 downto 0);
        param_wr_o      : out std_logic;
        update_port_o   : out std_logic
    );
end component;

-- SIGNAUX INTERNES POUR CONNEXION ENTRE COM_FLOW_FIFO_RW et READPARAMS
	signal data_s               : std_logic_vector(15 downto 0);
	signal flow_rdy_s           : std_logic := '0';
	signal f_empty_s            : std_logic := '0';
	signal flag_s               : std_logic_vector(7 downto 0);
	signal rdreq_s              : std_logic := '0';

	signal param_data_s         : std_logic_vector(31 downto 0);
begin

-- MAP COM_FLOW_FIFO_RX
COM_RX_PARAMS: component  com_flow_fifo_rx
generic map (
    FIFO_DEPTH    => FIFO_DEPTH,
    FLOW_ID       => FLOW_ID_SET,
    IN_SIZE       => DATA_HAL_SIZE,
    OUT_SIZE      => 16
)
port map (
    clk_hal       => clk_hal,
    clk_proc      => clk_proc,
    rst_n         => rst_n,
    data_wr_i     => data_wr_i,
    data_i        => data_i,
    rdreq_i       => rdreq_s,
    pktend_i      => pktend_i,
    enable_i      => '1',
    data_o        => data_s,
    flow_rdy_o    => flow_rdy_s,
    f_empty_o     => f_empty_s,
    fifos_f_o     => fifo_full_o,
    flag_o        => flag_s
);

-- MAP COMPONENT READFLOW TO params
-- pour le get params faire un flag particulier qui va declencher une reecriture sur le flow de sortie
decoder_inst :component params_flow_decoder
generic map (
    MASTER_ADDR_WIDTH => MASTER_ADDR_WIDTH
)
port map (
    clk             => clk_proc,
    rst_n           => rst_n,
    data_i          => data_s,
    flow_rdy_i      => flow_rdy_s,
    f_empty_i       => f_empty_s,
    flag_i          => flag_s,
    read_data_o     => rdreq_s,
    param_addr_o    => param_addr_o,
    param_data_o    => param_data_s,
    param_wr_o      => param_wr_o,
    update_port_o   => tmp_update_port_o
);

param_data_o <= param_data_s;

end rtl;

