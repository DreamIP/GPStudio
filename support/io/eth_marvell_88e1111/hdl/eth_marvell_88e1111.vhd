library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;
use work.axi.all;
use work.ipv4_types.all;
use work.arp_types.all;

entity eth_marvell_88e1111 is
    generic (
        IN0_NBWORDS       : integer;
        IN1_NBWORDS       : integer;
        IN2_NBWORDS       : integer;
        IN3_NBWORDS       : integer;
        OUT0_NBWORDS      : integer;
        OUT1_NBWORDS      : integer;
        CLK_PROC_FREQ     : integer;
        IN0_SIZE          : integer;
        IN1_SIZE          : integer;
        IN2_SIZE          : integer;
        IN3_SIZE          : integer;
        OUT0_SIZE         : integer;
        OUT1_SIZE         : integer;
        MASTER_ADDR_WIDTH : integer
    );
    port (
        clk_proc    : in std_logic;
        clk_hal     : out std_logic;
        reset_n     : in std_logic;

        --------------------- external ports --------------------
        gtx_clk     : out std_logic;
        tx_en       : out std_logic;
        tx_data     : out std_logic_vector(3 downto 0);
        rx_clk      : in std_logic;
        rx_dv       : in std_logic;
        rx_data     : in std_logic_vector(3 downto 0);
        phy_reset_l : out std_logic;
        phy_mdc     : out std_logic;
        phy_mdio    : inout std_logic;

        ------------------------ in0 flow -----------------------
        in0_data    : in std_logic_vector(IN0_SIZE-1 downto 0);
        in0_fv      : in std_logic;
        in0_dv      : in std_logic;

        ------------------------ in1 flow -----------------------
        in1_data    : in std_logic_vector(IN1_SIZE-1 downto 0);
        in1_fv      : in std_logic;
        in1_dv      : in std_logic;

        ------------------------ in2 flow -----------------------
        in2_data    : in std_logic_vector(IN2_SIZE-1 downto 0);
        in2_fv      : in std_logic;
        in2_dv      : in std_logic;

        ------------------------ in3 flow -----------------------
        in3_data    : in std_logic_vector(IN3_SIZE-1 downto 0);
        in3_fv      : in std_logic;
        in3_dv      : in std_logic;

        ------------------------ out0 flow ----------------------
        out0_data   : out std_logic_vector(OUT0_SIZE-1 downto 0);
        out0_fv     : out std_logic;
        out0_dv     : out std_logic;

        ------------------------ out1 flow ----------------------
        out1_data   : out std_logic_vector(OUT1_SIZE-1 downto 0);
        out1_fv     : out std_logic;
        out1_dv     : out std_logic;

        ---- ===== Masters =====

        ------ bus_master ------
        master_addr_o   : out std_logic_vector(MASTER_ADDR_WIDTH-1 downto 0);
        master_wr_o     : out std_logic;
        master_rd_o     : out std_logic;
        master_datawr_o : out std_logic_vector(31 downto 0);
        master_datard_i : in std_logic_vector(31 downto 0);

        --======================= Slaves ========================
        ------------------------- bus_sl ------------------------
        addr_rel_i  : in std_logic_vector(3 downto 0);
        wr_i        : in std_logic;
        rd_i        : in std_logic;
        datawr_i    : in std_logic_vector(31 downto 0);
        datard_o    : out std_logic_vector(31 downto 0)
    );
end eth_marvell_88e1111;

architecture rtl of eth_marvell_88e1111 is
    constant DATA_HAL_SIZE        : INTEGER := 8;
    constant CLK_HAL_FREQ         : INTEGER := 125000000;

component eth_marvell_88e1111_slave
    generic (
        CLK_PROC_FREQ   : integer
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

component UDP_MAC_GE
    port (
        ---------------------------------------------------------------------------
        -- RGMII Interface
        ---------------------------------------------------------------------------
        gtx_clk     : out std_logic;
        tx_en       : out std_logic;
        tx_data     : out std_logic_vector(3 downto 0);
        rx_clk      : in  std_logic;
        rx_dv       : in  std_logic;
        rx_data         : in  std_logic_vector(3 downto 0);

        --phy
        phy_reset_l : out std_logic;
        phy_mdc     : out std_logic;
        phy_mdio    : inout std_logic;

        ---------------------------------------------------------------------------
        -- user Interface
        ---------------------------------------------------------------------------
        udp_tx_start            : in std_logic;                         -- indicates req to tx UDP
        udp_txi                 : in udp_tx_type;                       -- UDP tx cxns
        udp_tx_result           : out std_logic_vector (1 downto 0);    -- tx status (changes during transmission)
        udp_tx_data_out_ready   : out std_logic;                        -- indicates udp_tx is ready to take data
        -- UDP RX signals
        udp_rx_start            : out std_logic;                        -- indicates receipt of udp header
        udp_rxo                 : out udp_rx_type
    );
end component;

    signal clk_hal_s             : std_logic;

    signal status_enable         : std_logic;
    signal flow_in0_enable       : std_logic;
    signal flow_in1_enable       : std_logic;
    signal flow_in2_enable       : std_logic;
    signal flow_in3_enable       : std_logic;

    signal udp_tx_start          : std_logic;
    signal udp_txi               : udp_tx_type;
    signal udp_tx_result         : std_logic_vector (1 downto 0);
    signal udp_tx_data_out_ready : std_logic;
    signal udp_rx_start          : std_logic;
    signal udp_rxo               : udp_rx_type;

begin

    eth_marvell_88e1111_slave_inst : eth_marvell_88e1111_slave
    generic map (
        CLK_PROC_FREQ => CLK_PROC_FREQ
    )
    port map (
        clk_proc        => clk_proc,
        reset_n         => reset_n,
        status_enable   => status_enable,
        flow_in0_enable => flow_in0_enable,
        flow_in1_enable => flow_in1_enable,
        flow_in2_enable => flow_in2_enable,
        flow_in3_enable => flow_in3_enable,
        addr_rel_i      => addr_rel_i,
        wr_i            => wr_i,
        rd_i            => rd_i,
        datawr_i        => datawr_i,
        datard_o        => datard_o
    );
    
    udp_mac_ge_inst : UDP_MAC_GE
    port map (
        ---------------------------------------------------------------------------
        -- RGMII Interface
        ---------------------------------------------------------------------------
        gtx_clk     => gtx_clk,
        tx_en       => tx_en,
        tx_data     => tx_data,
        rx_clk      => rx_clk,
        rx_dv       => rx_dv,
        rx_data     => rx_data,

        --phy
        phy_reset_l => phy_reset_l,
        phy_mdc     => phy_mdc,
        phy_mdio    => phy_mdio,

        ---------------------------------------------------------------------------
        -- user Interface
        ---------------------------------------------------------------------------
        udp_tx_start            => udp_tx_start,
        udp_txi                 => udp_txi,
        udp_tx_result           => open,
        udp_tx_data_out_ready   => udp_tx_data_out_ready,
        -- UDP RX signals
        udp_rx_start            => udp_rx_start,
        udp_rxo                 => udp_rxo
    );
    
    clk_hal <= rx_clk;
    
end rtl;
