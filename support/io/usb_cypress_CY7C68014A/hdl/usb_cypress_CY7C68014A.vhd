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

entity usb_cypress_CY7C68014A is
	generic (
		MASTER_ADDR_WIDTH   : integer;
		IN0_SIZE            : integer := 8;
		IN1_SIZE            : integer := 8;
		IN2_SIZE            : integer := 8;
		IN3_SIZE            : integer := 8;
		OUT0_SIZE           : integer := 16;
		OUT1_SIZE           : integer := 16;
		IN0_NBWORDS         : integer := 32768;
		IN1_NBWORDS         : integer := 32768;
		IN2_NBWORDS         : integer := 1280;
		IN3_NBWORDS         : integer := 1280;
		OUT0_NBWORDS        : integer := 1024;
		OUT1_NBWORDS        : integer := 1024;
		CLK_PROC_FREQ       : integer := 48000000
	);
	port (
		clk_proc        : in std_logic;
		clk_hal         : out std_logic;
		reset           : out std_logic;

		------ external ports ------
		rst             : in std_logic;
		ifclk           : in std_logic;
		flaga           : in std_logic;
		flagb           : in std_logic;
		flagc           : in std_logic;
		flagd           : in std_logic;
		fd_io           : inout std_logic_vector(15 downto 0);
		sloe            : out std_logic;
		slrd            : out std_logic;
		slwr            : out std_logic;
		pktend          : out std_logic;
		addr            : out std_logic_vector(1 downto 0);

		------ in0 flow ------
		in0_data        : in std_logic_vector(IN0_SIZE-1 downto 0);
		in0_fv          : in std_logic;
		in0_dv          : in std_logic;
		------ in1 flow ------
		in1_data        : in std_logic_vector(IN1_SIZE-1 downto 0);
		in1_fv          : in std_logic;
		in1_dv          : in std_logic;
		------ in2 flow ------
		in2_data        : in std_logic_vector(IN2_SIZE-1 downto 0);
		in2_fv          : in std_logic;
		in2_dv          : in std_logic;
		------ in3 flow ------
		in3_data        : in std_logic_vector(IN3_SIZE-1 downto 0);
		in3_fv          : in std_logic;
		in3_dv          : in std_logic;


		------ out0 flow ------
		out0_data       : out std_logic_vector(OUT0_SIZE-1 downto 0);
		out0_fv         : out std_logic;
		out0_dv         : out std_logic;

		------ out1 flow ------
		out1_data       : out std_logic_vector(OUT1_SIZE-1 downto 0);
		out1_fv         : out std_logic;
		out1_dv         : out std_logic;

		---- ===== Masters =====

		------ bus_master ------
		master_addr_o   : out std_logic_vector(MASTER_ADDR_WIDTH-1 downto 0);
		master_wr_o     : out std_logic;
		master_rd_o     : out std_logic;
		master_datawr_o : out std_logic_vector(31 downto 0);
		master_datard_i : in std_logic_vector(31 downto 0);

		---- ===== Slaves =====

		------ bus_sl ------
		addr_rel_i      : in std_logic_vector(3 downto 0);
		wr_i            : in std_logic;
		rd_i            : in std_logic;
		datawr_i        : in std_logic_vector(31 downto 0);
		datard_o        : out std_logic_vector(31 downto 0)
	);
end entity;


architecture rtl of usb_cypress_CY7C68014A is
-- SLAVE BUS
	signal enable_s             : std_logic := '0';
	signal enable_in0_s         : std_logic := '0';
	signal enable_in1_s         : std_logic := '0';
	signal enable_in2_s         : std_logic := '0';
	signal enable_in3_s         : std_logic := '0';

-- USBFLOW_IN
	signal flow_in1_data_s      : std_logic_vector(15 downto 0) := (others => '0');
	signal flow_in1_wr_s        : std_logic := '0';
	signal flow_in1_full_s      : std_logic := '0';
	signal flow_in1_pktend_s    : std_logic := '0';

-- USBFLOW OUT
	signal flow_out_data_s      : std_logic_vector(15 downto 0) := (others => '0');
	signal flow_out_empty_s     : std_logic := '0';
	signal flow_out_rd_s        : std_logic := '0';
	signal flow_out_rdy_s       : std_logic := '0';

	signal flow_out_data_0_s    : std_logic_vector(15 downto 0) := (others => '0');
	signal flow_out_empty_0_s   : std_logic := '0';
	signal flow_out_rd_0_s      : std_logic := '0';
	signal flow_out_rdy_0_s     : std_logic := '0';

	signal flow_out_data_1_s    : std_logic_vector(15 downto 0) := (others => '0');
	signal flow_out_empty_1_s   : std_logic := '0';
	signal flow_out_rd_1_s      : std_logic := '0';
	signal flow_out_rdy_1_s     : std_logic := '0';

	signal flow_out_data_2_s    : std_logic_vector(15 downto 0) := (others => '0');
	signal flow_out_empty_2_s   : std_logic := '0';
	signal flow_out_rd_2_s      : std_logic := '0';
	signal flow_out_rdy_2_s     : std_logic := '0';

	signal flow_out_data_3_s    : std_logic_vector(15 downto 0) := (others => '0');
	signal flow_out_empty_3_s   : std_logic := '0';
	signal flow_out_rd_3_s      : std_logic := '0';
	signal flow_out_rdy_3_s     : std_logic := '0';

	-- FLOW_PARAMS
    signal update_port_s        : std_logic := '0';

    -- clock
    signal clk_hal_s            : std_logic := '0';

begin

reset <= rst;

-- USB HAL, control of USB cypress
USB_HAL_INST : usb_cypress_CY7C68014A_hal
port map (
    usb_ifclk           => ifclk,
    usb_flaga           => flaga,
    usb_flagb           => flagb,
    usb_flagc           => flagc,
    usb_flagd           => flagd,
    usb_fd_io           => fd_io,
    usb_sloe            => sloe,
    usb_slrd            => slrd,
    usb_slwr            => slwr,
    usb_pktend          => pktend,
    usb_rst		        => rst,
    usb_addr            => addr,

    flow_in_data_o      => flow_in1_data_s,
    flow_in_wr_o        => flow_in1_wr_s,
    flow_in_full_i      => flow_in1_full_s,
    flow_in_end_o       => flow_in1_pktend_s,

    flow_out_data_i     => flow_out_data_s,
    flow_out_rd_o       => flow_out_rd_s,
    flow_out_empty_i    => flow_out_empty_s,
    flow_out_rdy_i      => flow_out_rdy_s
);

-- 
SLAVE_BUS_INST : component usb_cypress_CY7C68014A_slave
generic map (
    CLK_PROC_FREQ  => CLK_PROC_FREQ
)
port map (
    clk_proc        => clk_proc,
    reset_n         => rst,
    addr_rel_i      => addr_rel_i,
    wr_i            => wr_i,
    datawr_i        => datawr_i,
    rd_i            => rd_i,
    datard_o        => datard_o,
    status_enable   => enable_s,
    flow_in0_enable => enable_in0_s,
    flow_in1_enable => enable_in1_s,
    flow_in2_enable => enable_in2_s,
    flow_in3_enable => enable_in3_s
);

--FLOW_IN 0
FI0_label0 : if OUT0_NBWORDS = 0 generate
	out0_data   <= (others => '0');
	out0_fv     <= '0';
	out0_dv     <= '0';
end generate FI0_label0;

FI0_label1 : if OUT0_NBWORDS > 0 generate
    USBFLOW_IN0: component com_to_flow
    generic map (
        FIFO_DEPTH  => OUT0_NBWORDS,
        FLOW_ID     => 1,
        FLAGS_CODES => InitFlagCodes,
        OUTPUT_SIZE => OUT0_SIZE
    )
    port map (
        clk_in      => clk_hal_s,
        clk_out     => clk_proc,
        rst_n       => rst,

        data_wr_i   => flow_in1_wr_s,
        data_i      => flow_in1_data_s,
        pktend_i    => flow_in1_pktend_s,
        enable_i    => enable_s,

        data_o      => out0_data,
        fv_o        => out0_fv,
        dv_o        => out0_dv,
        flow_full_o => open
    );
end generate FI0_label1;

--FLOW_IN 1
FI1_label0 : if OUT1_NBWORDS = 0 generate
	out1_data   <= (others => '0');
	out1_fv     <= '0';
	out1_dv     <= '0';
end generate FI1_label0;

FI1_label1 : if OUT1_NBWORDS > 0 generate
    USBFLOW_IN1: component com_to_flow
    generic map (
        FIFO_DEPTH  => OUT1_NBWORDS,
        FLOW_ID     => 2,
        FLAGS_CODES => InitFlagCodes,
        OUTPUT_SIZE => OUT1_SIZE
    )
    port map (
        clk_in      => clk_hal_s,
        clk_out     => clk_proc,
        rst_n       => rst,

        data_wr_i   => flow_in1_wr_s,
        data_i      => flow_in1_data_s,
        pktend_i    => flow_in1_pktend_s,
        enable_i    => enable_s,

        data_o      => out1_data,
        fv_o        => out1_fv,
        dv_o        => out1_dv,
        flow_full_o => open
    );
end generate FI1_label1;
------------------------------------------------------------
--FLOW OUT 0
--Disable flow if not used
FO0_label3 : if IN0_NBWORDS = 0 generate
	flow_out_rdy_0_s <= '0';
	flow_out_empty_0_s <= '0';
	flow_out_data_0_s <= (others => '0');
end generate FO0_label3;

FO0_label4 : if IN0_NBWORDS > 0 generate
    USBFLOW_OUT0: component flow_to_com
    generic map (
        INPUT_SIZE      => IN0_SIZE,
        FIFO_DEPTH      => IN0_NBWORDS,
        OUTPUT_SIZE     => 16,
        FLOW_ID         => 128,
        PACKET_SIZE     => 256, -- header inclus
        FLAGS_CODES     => InitFlagCodes
    )
    port map (
        clk_in          => clk_proc,
        clk_out         => clk_hal_s,
        rst_n           => rst,

        in_data         => in0_data,
        in_fv           => in0_fv,
        in_dv           => in0_dv,

        enable_flow_i   => enable_in0_s,
        enable_global_i => enable_s,

        rdreq_i         => flow_out_rd_0_s, -- to arb
        data_o          => flow_out_data_0_s, -- to arb
        flow_rdy_o      => flow_out_rdy_0_s, -- to arb
        f_empty_o       => flow_out_empty_0_s -- to arb
    );
end generate FO0_label4;
------------------------------------------------------------

------------------------------------------------------------
--FLOW OUT 1
FO1_label3 : if IN1_NBWORDS = 0 generate
	flow_out_rdy_1_s    <= '0';
	flow_out_empty_1_s  <= '0';
	flow_out_data_1_s   <= (others => '0');
end generate FO1_label3;

FO1_label4 : if IN1_NBWORDS > 0 generate
    USBFLOW_OUT1: component flow_to_com
    generic map (
        INPUT_SIZE      => IN1_SIZE,
        FIFO_DEPTH      => IN1_NBWORDS,
        OUTPUT_SIZE     => 16,
        FLOW_ID         => 129,
        PACKET_SIZE     => 256, -- header inclus
        FLAGS_CODES     => InitFlagCodes
    )
    port map (
        clk_in          => clk_proc,
        clk_out         => clk_hal_s,
        rst_n           => rst,

        in_data         => in1_data,
        in_fv           => in1_fv,
        in_dv           => in1_dv,

        enable_flow_i   => enable_in1_s,
        enable_global_i => enable_s,

        rdreq_i         => flow_out_rd_1_s, -- to arb
        data_o          => flow_out_data_1_s, -- to arb
        flow_rdy_o      => flow_out_rdy_1_s, -- to arb
        f_empty_o       => flow_out_empty_1_s -- to arb
    );
end generate FO1_label4;
------------------------------------------------------------

------------------------------------------------------------
--FLOW OUT 2
FO2_label3 : if IN2_NBWORDS = 0 generate
	flow_out_rdy_2_s <= '0';
	flow_out_empty_2_s <= '0';
	flow_out_data_2_s <= (others => '0');
end generate FO2_label3;

FO2_label4 : if IN2_NBWORDS > 0 generate
    USBFLOW_OUT2: component flow_to_com
    generic map (
        INPUT_SIZE      => IN2_SIZE,
        FIFO_DEPTH      => IN2_NBWORDS,
        OUTPUT_SIZE     => 16,
        FLOW_ID         => 130,
        PACKET_SIZE     => 256, -- header inclus
        FLAGS_CODES     => InitFlagCodes
    )
    port map (
        clk_in          => clk_proc,
        clk_out         => clk_hal_s,
        rst_n           => rst,

        in_data         => in2_data,
        in_fv           => in2_fv,
        in_dv           => in2_dv,

        enable_flow_i   => enable_in2_s,
        enable_global_i => enable_s,

        rdreq_i         => flow_out_rd_2_s, -- to arb
        data_o          => flow_out_data_2_s, -- to arb
        flow_rdy_o      => flow_out_rdy_2_s, -- to arb
        f_empty_o       => flow_out_empty_2_s -- to arb
    );
end generate FO2_label4;
------------------------------------------------------------
------------------------------------------------------------
--FLOW OUT 3

FO3_label3 : if IN3_NBWORDS = 0 generate
	flow_out_rdy_3_s <= '0';
	flow_out_empty_3_s <= '0';
	flow_out_data_3_s <= (others => '0');
end generate FO3_label3;

FO3_label4 : if IN3_NBWORDS > 0 generate
    USBFLOW_OUT3: component flow_to_com
    generic map (
        INPUT_SIZE      => IN3_SIZE,
        FIFO_DEPTH      => IN3_NBWORDS,
        OUTPUT_SIZE     => 16,
        FLOW_ID         => 131,
        PACKET_SIZE     => 256, -- header inclus
        FLAGS_CODES     => InitFlagCodes
    )
    port map (
        clk_in          => clk_proc,
        clk_out         => clk_hal_s,
        rst_n           => rst,

        in_data         => in3_data,
        in_fv           => in3_fv,
        in_dv           => in3_dv,

        enable_flow_i   => enable_in3_s,
        enable_global_i => enable_s,

        rdreq_i         => flow_out_rd_3_s, -- to arb
        data_o          => flow_out_data_3_s, -- to arb
        flow_rdy_o      => flow_out_rdy_3_s, -- to arb
        f_empty_o       => flow_out_empty_3_s -- to arb
    );
end generate FO3_label4;
------------------------------------------------------------

-- component flow_out_arbiter

USBFLOW_ARB : component flow_to_com_arb4
    port map (
		clk             => clk_hal_s,
		rst_n           => rst,

		-- fv 0 signals
		rdreq_0_o       => flow_out_rd_0_s,
		data_0_i        => flow_out_data_0_s,
		flow_rdy_0_i    => flow_out_rdy_0_s,
		f_empty_0_i     => flow_out_empty_0_s,

		-- fv 1 signals
		rdreq_1_o       => flow_out_rd_1_s,
		data_1_i        => flow_out_data_1_s,
		flow_rdy_1_i    => flow_out_rdy_1_s,
		f_empty_1_i     => flow_out_empty_1_s,

		-- fv 2 signals
		rdreq_2_o       => flow_out_rd_2_s,
		data_2_i        => flow_out_data_2_s,
		flow_rdy_2_i    => flow_out_rdy_2_s,
		f_empty_2_i     => flow_out_empty_2_s,

		-- fv 3 signals
		rdreq_3_o       => flow_out_rd_3_s,
		data_3_i        => flow_out_data_3_s,
		flow_rdy_3_i    => flow_out_rdy_3_s,
		f_empty_3_i     => flow_out_empty_3_s,

		-- fv usb signals
		rdreq_usb_i     => flow_out_rd_s,
		data_usb_o      => flow_out_data_s,
		flow_rdy_usb_o  => flow_out_rdy_s,
		f_empty_usb_o   => flow_out_empty_s
	);

--  FLOW_PARAMS module --> Bus Interconnect Master
FLOW_PARAMS : component com_to_master_pi
    generic map (
        FIFO_DEPTH          => 64,
        FLOW_ID_SET         => 15,
        MASTER_ADDR_WIDTH   => MASTER_ADDR_WIDTH
    )
    port map (
        clk_in              => clk_hal_s,
        clk_out             => clk_proc,
        rst_n               => rst,
        data_wr_i           => flow_in1_wr_s,
        data_i              => flow_in1_data_s,
        pktend_i            => flow_in1_pktend_s,
        fifo_full_o         => flow_in1_full_s,
        param_addr_o        => master_addr_o,
        param_data_o        => master_datawr_o,
        param_wr_o          => master_wr_o
        -- rajouter fin d'ecriture dans la memoire...
        --~ tmp_update_port_o => update_port_s
    );
    
    clk_hal_s <= ifclk;
    clk_hal <= clk_hal_s;

end rtl;
