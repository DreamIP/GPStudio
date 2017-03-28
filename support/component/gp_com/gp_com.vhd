library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.ComFlow_pkg.all;

-- Top level du driver USB
-- 4 output flows max
-- 2 input flow max

-- TODO
-- PASSER LES Identifiants de FLOW en générique du driver:
-- ca permettrait de specifier les valeurs des identifiants des trames par GPStudio

-- Header trame USB
-- TRAME USB 16 bits	: permier mot header:  FLOW ID/FLAG  (8b/8b)
--						: second mot header :  Packet number  (16b)

entity gp_com is
	generic (
		IN0_SIZE           : INTEGER := 8;
		IN1_SIZE           : INTEGER := 8;
		IN2_SIZE           : INTEGER := 8;
		IN3_SIZE           : INTEGER := 8;
		OUT0_SIZE          : INTEGER := 8;
		OUT1_SIZE          : INTEGER := 8;
		IN0_NBWORDS        : INTEGER := 1280;
		IN1_NBWORDS        : INTEGER := 1280;
		IN2_NBWORDS        : INTEGER := 1280;
		IN3_NBWORDS        : INTEGER := 1280;
		OUT0_NBWORDS       : INTEGER := 1024;
		OUT1_NBWORDS       : INTEGER := 1024;
		CLK_PROC_FREQ      : INTEGER;
		CLK_HAL_FREQ       : INTEGER;
		DATA_HAL_SIZE      : INTEGER;
        MASTER_ADDR_WIDTH  : INTEGER
	);
	port (
		clk_proc           : in std_logic;
		reset_n            : in std_logic;

		------ hal connections ------
		clk_hal            : in   std_logic;

        from_hal_data      : in   std_logic_vector(DATA_HAL_SIZE-1 downto 0);
        from_hal_wr	       : in   std_logic;
        from_hal_full      : out  std_logic;
        from_hal_pktend    : in   std_logic;

        to_hal_data        : out  std_logic_vector(DATA_HAL_SIZE-1 downto 0);
        to_hal_rd	       : in   std_logic;
        to_hal_empty       : out  std_logic;
        to_hal_rdy         : out  std_logic;
        to_hal_size_packet : out  std_logic_vector(15 downto 0);

        -------- slave -------
		status_enable      : in std_logic;
		flow_in0_enable    : in std_logic;
		flow_in1_enable    : in std_logic;
		flow_in2_enable    : in std_logic;
		flow_in3_enable    : in std_logic;
                           
		------ in0 flow ------
		in0_data           : in std_logic_vector(IN0_SIZE-1 downto 0);
		in0_fv             : in std_logic;
		in0_dv             : in std_logic;
		------ in1 flow ------
		in1_data           : in std_logic_vector(IN1_SIZE-1 downto 0);
		in1_fv             : in std_logic;
		in1_dv             : in std_logic;
		------ in2 flow ------
		in2_data           : in std_logic_vector(IN2_SIZE-1 downto 0);
		in2_fv             : in std_logic;
		in2_dv             : in std_logic;
		------ in3 flow ------
		in3_data           : in std_logic_vector(IN3_SIZE-1 downto 0);
		in3_fv             : in std_logic;
		in3_dv             : in std_logic;


		------ out0 flow ------
		out0_data          : out std_logic_vector(OUT0_SIZE-1 downto 0);
		out0_fv            : out std_logic;
		out0_dv            : out std_logic;
		------ out1 flow ------
		out1_data          : out std_logic_vector(OUT1_SIZE-1 downto 0);
		out1_fv            : out std_logic;
		out1_dv            : out std_logic;

		---- ===== Masters =====

		------ bus_master ------
		master_addr_o      : out std_logic_vector(MASTER_ADDR_WIDTH-1 downto 0);
		master_wr_o        : out std_logic;
		master_rd_o        : out std_logic;
		master_datawr_o    : out std_logic_vector(31 downto 0);
		master_datard_i    : in std_logic_vector(31 downto 0)
	);
end entity;


architecture rtl of gp_com is

-- flow signals
	signal flow_out_data_0_s        : std_logic_vector(DATA_HAL_SIZE-1 downto 0) := (others => '0');
	signal flow_out_empty_0_s       : std_logic := '0';
	signal flow_out_rd_0_s          : std_logic := '0';
	signal flow_out_rdy_0_s         : std_logic := '0';
	signal flow_out_size_0_packet_s : std_logic_vector(15 downto 0) := (others => '0');

	signal flow_out_data_1_s        : std_logic_vector(DATA_HAL_SIZE-1 downto 0) := (others => '0');
	signal flow_out_empty_1_s       : std_logic := '0';
	signal flow_out_rd_1_s          : std_logic := '0';
	signal flow_out_rdy_1_s         : std_logic := '0';
	signal flow_out_size_1_packet_s : std_logic_vector(15 downto 0) := (others => '0');

	signal flow_out_data_2_s        : std_logic_vector(DATA_HAL_SIZE-1 downto 0) := (others => '0');
	signal flow_out_empty_2_s       : std_logic := '0';
	signal flow_out_rd_2_s          : std_logic := '0';
	signal flow_out_rdy_2_s         : std_logic := '0';
	signal flow_out_size_2_packet_s : std_logic_vector(15 downto 0) := (others => '0');

	signal flow_out_data_3_s        : std_logic_vector(DATA_HAL_SIZE-1 downto 0) := (others => '0');
	signal flow_out_empty_3_s       : std_logic := '0';
	signal flow_out_rd_3_s          : std_logic := '0';
	signal flow_out_rdy_3_s         : std_logic := '0';
	signal flow_out_size_3_packet_s : std_logic_vector(15 downto 0) := (others => '0');

	-- FLOW_PARAMS
    signal update_port_s            : std_logic := '0';

begin

--FLOW_OUT out0
FO0_disabled : if OUT0_NBWORDS = 0 generate
	out0_data   <= (others => '0');
	out0_fv     <= '0';
	out0_dv     <= '0';
end generate FO0_disabled;

FO0_enabled : if OUT0_NBWORDS > 0 generate
    FLOW_OUT0: component com_to_flow
    generic map (
        FIFO_DEPTH  => OUT0_NBWORDS,
        FLOW_ID     => 1,
        FLAGS_CODES => InitFlagCodes,
        FLOW_SIZE   => OUT0_SIZE
    )
    port map (
        clk_hal     => clk_hal,
        clk_proc    => clk_proc,
        rst_n       => reset_n,

        data_wr_i   => from_hal_wr,
        data_i      => from_hal_data,
        pktend_i    => from_hal_pktend,
        enable_i    => status_enable,

        data_o      => out0_data,
        fv_o        => out0_fv,
        dv_o        => out0_dv,
        flow_full_o => open
    );
end generate FO0_enabled;

--FLOW_OUT out1
FO1_disabled : if OUT1_NBWORDS = 0 generate
	out1_data   <= (others => '0');
	out1_fv     <= '0';
	out1_dv     <= '0';
end generate FO1_disabled;

FO1_enabled : if OUT1_NBWORDS > 0 generate
    FLOW_OUT1: component com_to_flow
    generic map (
        FIFO_DEPTH  => OUT1_NBWORDS,
        FLOW_ID     => 2,
        FLAGS_CODES => InitFlagCodes,
        FLOW_SIZE   => OUT1_SIZE
    )
    port map (
        clk_hal     => clk_hal,
        clk_proc    => clk_proc,
        rst_n       => reset_n,

        data_wr_i   => from_hal_wr,
        data_i      => from_hal_data,
        pktend_i    => from_hal_pktend,
        enable_i    => status_enable,

        data_o      => out1_data,
        fv_o        => out1_fv,
        dv_o        => out1_dv,
        flow_full_o => open
    );
end generate FO1_enabled;

------------------------------------------------------------
--FLOW IN in0
--Disable flow if not used
FI0_disabled : if IN0_NBWORDS = 0 generate
	flow_out_rdy_0_s <= '0';
	flow_out_empty_0_s <= '0';
	flow_out_data_0_s <= (others => '0');
end generate FI0_disabled;

FI0_enabled : if IN0_NBWORDS > 0 generate
    FLOW_IN0: component flow_to_com
    generic map (
        FLOW_SIZE       => IN0_SIZE,
        FIFO_DEPTH      => IN0_NBWORDS,
        OUTPUT_SIZE     => 16,
        FLOW_ID         => 128,
        PACKET_SIZE     => 256, -- header inclus
        FLAGS_CODES     => InitFlagCodes
    )
    port map (
        clk_proc        => clk_proc,
        clk_hal         => clk_hal,
        rst_n           => reset_n,

        in_data         => in0_data,
        in_fv           => in0_fv,
        in_dv           => in0_dv,

        enable_flow_i   => flow_in0_enable,
        enable_global_i => status_enable,

        -- to arbitrer
        rdreq_i         => flow_out_rd_0_s,
        data_o          => flow_out_data_0_s,
        flow_rdy_o      => flow_out_rdy_0_s,
        f_empty_o       => flow_out_empty_0_s,
        size_packet_o   => flow_out_size_0_packet_s
    );
end generate FI0_enabled;
------------------------------------------------------------

------------------------------------------------------------
--FLOW IN in1
FI1_disabled : if IN1_NBWORDS = 0 generate
	flow_out_rdy_1_s    <= '0';
	flow_out_empty_1_s  <= '0';
	flow_out_data_1_s   <= (others => '0');
end generate FI1_disabled;

FI1_enabled : if IN1_NBWORDS > 0 generate
    FLOW_IN1: component flow_to_com
    generic map (
        FLOW_SIZE       => IN1_SIZE,
        FIFO_DEPTH      => IN1_NBWORDS,
        OUTPUT_SIZE     => 16,
        FLOW_ID         => 129,
        PACKET_SIZE     => 256, -- header inclus
        FLAGS_CODES     => InitFlagCodes
    )
    port map (
        clk_proc        => clk_proc,
        clk_hal         => clk_hal,
        rst_n           => reset_n,

        in_data         => in1_data,
        in_fv           => in1_fv,
        in_dv           => in1_dv,

        enable_flow_i   => flow_in1_enable,
        enable_global_i => status_enable,

        -- to arbitrer
        rdreq_i         => flow_out_rd_1_s,
        data_o          => flow_out_data_1_s,
        flow_rdy_o      => flow_out_rdy_1_s,
        f_empty_o       => flow_out_empty_1_s,
        size_packet_o   => flow_out_size_1_packet_s
    );
end generate FI1_enabled;
------------------------------------------------------------

------------------------------------------------------------
--FLOW IN in2
FI2_disabled : if IN2_NBWORDS = 0 generate
	flow_out_rdy_2_s <= '0';
	flow_out_empty_2_s <= '0';
	flow_out_data_2_s <= (others => '0');
end generate FI2_disabled;

FI2_enabled : if IN2_NBWORDS > 0 generate
    FLOW_IN2: component flow_to_com
    generic map (
        FLOW_SIZE       => IN2_SIZE,
        FIFO_DEPTH      => IN2_NBWORDS,
        OUTPUT_SIZE     => 16,
        FLOW_ID         => 130,
        PACKET_SIZE     => 256, -- header inclus
        FLAGS_CODES     => InitFlagCodes
    )
    port map (
        clk_proc        => clk_proc,
        clk_hal         => clk_hal,
        rst_n           => reset_n,

        in_data         => in2_data,
        in_fv           => in2_fv,
        in_dv           => in2_dv,

        enable_flow_i   => flow_in2_enable,
        enable_global_i => status_enable,

        -- to arbitrer
        rdreq_i         => flow_out_rd_2_s,
        data_o          => flow_out_data_2_s,
        flow_rdy_o      => flow_out_rdy_2_s,
        f_empty_o       => flow_out_empty_2_s,
        size_packet_o   => flow_out_size_2_packet_s
    );
end generate FI2_enabled;
------------------------------------------------------------

------------------------------------------------------------
--FLOW IN in3
FI3_disabled : if IN3_NBWORDS = 0 generate
	flow_out_rdy_3_s <= '0';
	flow_out_empty_3_s <= '0';
	flow_out_data_3_s <= (others => '0');
end generate FI3_disabled;

FI3_enabled : if IN3_NBWORDS > 0 generate
    FLOW_IN3: component flow_to_com
    generic map (
        FLOW_SIZE       => IN3_SIZE,
        FIFO_DEPTH      => IN3_NBWORDS,
        OUTPUT_SIZE     => 16,
        FLOW_ID         => 131,
        PACKET_SIZE     => 256, -- header inclus
        FLAGS_CODES     => InitFlagCodes
    )
    port map (
        clk_proc        => clk_proc,
        clk_hal         => clk_hal,
        rst_n           => reset_n,

        in_data         => in3_data,
        in_fv           => in3_fv,
        in_dv           => in3_dv,

        enable_flow_i   => flow_in3_enable,
        enable_global_i => status_enable,

        -- to arbitrer
        rdreq_i         => flow_out_rd_3_s,
        data_o          => flow_out_data_3_s,
        flow_rdy_o      => flow_out_rdy_3_s,
        f_empty_o       => flow_out_empty_3_s,
        size_packet_o   => flow_out_size_3_packet_s
    );
end generate FI3_enabled;
------------------------------------------------------------

-- component flow_to_com_arb4
FLOW_ARB : component flow_to_com_arb4
    port map (
		clk             => clk_hal,
		rst_n           => reset_n,

		-- fv 0 signals
		rdreq_0_o       => flow_out_rd_0_s,
		data_0_i        => flow_out_data_0_s,
		flow_rdy_0_i    => flow_out_rdy_0_s,
		f_empty_0_i     => flow_out_empty_0_s,
		size_packet_0_i => flow_out_size_0_packet_s,

		-- fv 1 signals
		rdreq_1_o       => flow_out_rd_1_s,
		data_1_i        => flow_out_data_1_s,
		flow_rdy_1_i    => flow_out_rdy_1_s,
		f_empty_1_i     => flow_out_empty_1_s,
		size_packet_1_i => flow_out_size_1_packet_s,

		-- fv 2 signals
		rdreq_2_o       => flow_out_rd_2_s,
		data_2_i        => flow_out_data_2_s,
		flow_rdy_2_i    => flow_out_rdy_2_s,
		f_empty_2_i     => flow_out_empty_2_s,
		size_packet_2_i => flow_out_size_2_packet_s,

		-- fv 3 signals
		rdreq_3_o       => flow_out_rd_3_s,
		data_3_i        => flow_out_data_3_s,
		flow_rdy_3_i    => flow_out_rdy_3_s,
		f_empty_3_i     => flow_out_empty_3_s,
		size_packet_3_i => flow_out_size_3_packet_s,

		-- fv usb signals
		rdreq_usb_i     => to_hal_rd,
		data_usb_o      => to_hal_data,
		flow_rdy_usb_o  => to_hal_rdy,
		f_empty_usb_o   => to_hal_empty,
		size_packet_o   => to_hal_size_packet
	);

--  FLOW_PARAMS module --> Parameter Interconnect Master
FLOW_PARAMS : component com_to_master_pi
    generic map (
        FIFO_DEPTH          => 64,
        FLOW_ID_SET         => 15,
        MASTER_ADDR_WIDTH   => MASTER_ADDR_WIDTH
    )
    port map (
        clk_hal             => clk_hal,
        clk_proc            => clk_proc,
        rst_n               => reset_n,
        data_wr_i           => from_hal_wr,
        data_i              => from_hal_data,
        pktend_i            => from_hal_pktend,
        fifo_full_o         => from_hal_full,
        param_addr_o        => master_addr_o,
        param_data_o        => master_datawr_o,
        param_wr_o          => master_wr_o
        -- rajouter fin d'ecriture dans la memoire...
        --~ tmp_update_port_o => update_port_s
    );

end rtl;
