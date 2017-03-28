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
-- TRAME USB 16 bits    : permier mot header:  FLOW ID/FLAG  (8b/8b)
--                      : second mot header :  Packet number  (16b)

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
        CLK_PROC_FREQ       : integer
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
	constant DATA_HAL_SIZE        : INTEGER := 16;
	constant CLK_HAL_FREQ         : INTEGER := 48000000;

-- SLAVE BUS
    signal status_enable          : std_logic := '0';
    signal flow_in0_enable        : std_logic := '0';
    signal flow_in1_enable        : std_logic := '0';
    signal flow_in2_enable        : std_logic := '0';
    signal flow_in3_enable        : std_logic := '0';

-- USBFLOW_IN
    signal hal_out_data           : std_logic_vector(DATA_HAL_SIZE-1 downto 0) := (others => '0');
    signal hal_out_data_wr        : std_logic := '0';
    signal hal_out_data_full      : std_logic := '0';
    signal hal_out_data_end       : std_logic := '0';

-- USBFLOW OUT
    signal hal_in_data            : std_logic_vector(DATA_HAL_SIZE-1 downto 0) := (others => '0');
    signal hal_in_empty           : std_logic := '0';
    signal hal_in_rd              : std_logic := '0';
    signal hal_in_rdy             : std_logic := '0';
    signal hal_in_size_packet     : std_logic_vector(15 downto 0) := (others => '0');

    -- FLOW_PARAMS
    signal update_port_s          : std_logic := '0';

    -- clock
    signal clk_hal_s              : std_logic := '0';

    component usb_cypress_CY7C68014A_hal
        port (
            usb_ifclk       : in    std_logic;
            usb_flaga       : in    std_logic;
            usb_flagb       : in    std_logic;
            usb_flagc       : in    std_logic;
            usb_flagd       : in    std_logic;
            usb_fd_io       : inout std_logic_vector(15 downto 0);
            usb_sloe        : out   std_logic;
            usb_slrd        : out   std_logic;
            usb_slwr        : out   std_logic;
            usb_pktend      : out   std_logic;
            usb_addr        : out   std_logic_vector(1 downto 0);

            usb_rst         : in    std_logic;

            out_data_o      : out   std_logic_vector(15 downto 0);
            out_data_wr_o   : out   std_logic;
            out_data_full_i : in    std_logic;
            out_data_end_o  : out   std_logic;

            in_data_i       : in    std_logic_vector(15 downto 0);
            in_data_rd_o    : out   std_logic;
            in_data_empty_i : in    std_logic;
            in_data_rdy_i   : in    std_logic
        );
    end component;

    component usb_cypress_CY7C68014A_slave
        generic (
            CLK_PROC_FREQ : INTEGER
        );
        port (
            clk_proc        : in std_logic;
            reset_n         : in std_logic;

            ---------------- dynamic parameters ports ---------------
            status_enable   : out std_logic;
            flow_in0_enable : out std_logic;
            flow_in1_enable : out std_logic;
            flow_in2_enable : out std_logic;
            flow_in3_enable : out std_logic;

            --======================= Slaves ========================

            ------------------------- bus_sl ------------------------
            addr_rel_i      : in std_logic_vector(3 downto 0);
            wr_i            : in std_logic;
            rd_i            : in std_logic;
            datawr_i        : in std_logic_vector(31 downto 0);
            datard_o        : out std_logic_vector(31 downto 0)
        );
    end component;

begin

reset <= rst;

-- USB HAL, control of USB cypress
usb_hal_inst : usb_cypress_CY7C68014A_hal
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
    usb_rst             => rst,
    usb_addr            => addr,

    out_data_o          => hal_out_data,
    out_data_wr_o       => hal_out_data_wr,
    out_data_full_i     => hal_out_data_full,
    out_data_end_o      => hal_out_data_end,

    in_data_i           => hal_in_data,
    in_data_rd_o        => hal_in_rd,
    in_data_empty_i     => hal_in_empty,
    in_data_rdy_i       => hal_in_rdy
);

-- slave
usb_slave_inst : component usb_cypress_CY7C68014A_slave
generic map (
    CLK_PROC_FREQ   => CLK_PROC_FREQ
)
port map (
    clk_proc        => clk_proc,
    reset_n         => rst,
    addr_rel_i      => addr_rel_i,
    wr_i            => wr_i,
    datawr_i        => datawr_i,
    rd_i            => rd_i,
    datard_o        => datard_o,
    status_enable   => status_enable,
    flow_in0_enable => flow_in0_enable,
    flow_in1_enable => flow_in1_enable,
    flow_in2_enable => flow_in2_enable,
    flow_in3_enable => flow_in3_enable
);

-- com controler
gp_com_inst : component gp_com
generic map (
    IN0_SIZE          => IN0_SIZE,
    IN1_SIZE          => IN1_SIZE,
    IN2_SIZE          => IN2_SIZE,
    IN3_SIZE          => IN3_SIZE,
    OUT0_SIZE         => OUT0_SIZE,
    OUT1_SIZE         => OUT1_SIZE,
    IN0_NBWORDS       => IN0_NBWORDS,
    IN1_NBWORDS       => IN1_NBWORDS,
    IN2_NBWORDS       => IN2_NBWORDS,
    IN3_NBWORDS       => IN3_NBWORDS,
    OUT0_NBWORDS      => OUT0_NBWORDS,
    OUT1_NBWORDS      => OUT1_NBWORDS,
    CLK_PROC_FREQ     => CLK_PROC_FREQ,
    CLK_HAL_FREQ      => 48000000,
    DATA_HAL_SIZE     => DATA_HAL_SIZE,
    MASTER_ADDR_WIDTH => MASTER_ADDR_WIDTH
)
port map (
    clk_proc          => clk_proc,
    reset_n           => rst,

    clk_hal           => clk_hal_s,

    from_hal_data      => hal_out_data,
    from_hal_wr        => hal_out_data_wr,
    from_hal_full      => hal_out_data_full,
    from_hal_pktend    => hal_out_data_end,
    
    to_hal_data        => hal_in_data,
    to_hal_rd          => hal_in_rd,
    to_hal_empty       => hal_in_empty,
    to_hal_rdy         => hal_in_rdy,
    to_hal_size_packet => hal_in_size_packet,

    status_enable     => status_enable,
    flow_in0_enable   => flow_in0_enable,
    flow_in1_enable   => flow_in1_enable,
    flow_in2_enable   => flow_in2_enable,
    flow_in3_enable   => flow_in3_enable,

    in0_data          => in0_data,
    in0_fv            => in0_fv,
    in0_dv            => in0_dv,

    in1_data          => in1_data,
    in1_fv            => in1_fv,
    in1_dv            => in1_dv,

    in2_data          => in2_data,
    in2_fv            => in2_fv,
    in2_dv            => in2_dv,

    in3_data          => in3_data,
    in3_fv            => in3_fv,
    in3_dv            => in3_dv,

    out0_data         => out0_data,
    out0_fv           => out0_fv,
    out0_dv           => out0_dv,

    out1_data         => out1_data,
    out1_fv           => out1_fv,
    out1_dv           => out1_dv,

    master_addr_o     => master_addr_o,
    master_wr_o       => master_wr_o,
    master_rd_o       => master_rd_o,
    master_datawr_o   => master_datawr_o,
    master_datard_i   => master_datard_i
);

    clk_hal_s <= ifclk;
    clk_hal <= clk_hal_s;

end rtl;
