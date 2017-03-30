library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.axi.all;
use work.ipv4_types.all;
use work.arp_types.all;

entity UDP_MAC_GE is
port (
	--iRst_n 				: IN  STD_LOGIC;
    ---------------------------------------------------------------------------
    -- RGMII Interface
    ---------------------------------------------------------------------------
    gtx_clk    	: out std_logic;
    tx_en 		: out std_logic;
    tx_data     	: out std_logic_vector(3 downto 0);
    rx_clk    	: in  std_logic;
    rx_dv 		: in  std_logic;
    rx_data     	: in  std_logic_vector(3 downto 0);

	--phy
	phy_reset_l	: out std_logic;
	phy_mdc 		: out std_logic;
	phy_mdio 		: inout std_logic;

    ---------------------------------------------------------------------------
    -- user Interface
    ---------------------------------------------------------------------------
	udp_tx_start			: in std_logic;							-- indicates req to tx UDP
	udp_txi					: in udp_tx_type;						-- UDP tx cxns
	udp_tx_result			: out std_logic_vector (1 downto 0);    -- tx status (changes during transmission)
	udp_tx_data_out_ready	: out std_logic;						-- indicates udp_tx is ready to take data

	-- UDP RX signals
	udp_rx_start			: out std_logic;						-- indicates receipt of udp header
	udp_rxo					: out udp_rx_type;

	CLK_OUT				: OUT STD_LOGIC
);
end entity;


architecture rtl of UDP_MAC_GE is


COMPONENT UDP_Complete_nomac
	 generic (
			CLOCK_FREQ			: integer := 125000000;							-- freq of data_in_clk -- needed to timout cntr
			ARP_TIMEOUT			: integer := 60;									-- ARP response timeout (s)
			ARP_MAX_PKT_TMO	: integer := 5;									-- # wrong nwk pkts received before set error
			MAX_ARP_ENTRIES 	: integer := 255									-- max entries in the ARP store
			);
    Port (
			-- UDP TX signals
			udp_tx_start			: in std_logic;							-- indicates req to tx UDP
			udp_txi					: in udp_tx_type;							-- UDP tx cxns
			udp_tx_result			: out std_logic_vector (1 downto 0);-- tx status (changes during transmission)
			udp_tx_data_out_ready	: out std_logic;							-- indicates udp_tx is ready to take data
			-- UDP RX signals
			udp_rx_start			: out std_logic;							-- indicates receipt of udp header
			udp_rxo					: out udp_rx_type;
			-- IP RX signals
			ip_rx_hdr				: out ipv4_rx_header_type;
			-- system signals
			rx_clk					: in  STD_LOGIC;
			tx_clk					: in  STD_LOGIC;
			reset 					: in  STD_LOGIC;
			our_ip_address 			: in STD_LOGIC_VECTOR (31 downto 0);
			our_mac_address 		: in std_logic_vector (47 downto 0);
			control					: in udp_control_type;
			-- status signals
			arp_pkt_count			: out STD_LOGIC_VECTOR(7 downto 0);			-- count of arp pkts received
			ip_pkt_count			: out STD_LOGIC_VECTOR(7 downto 0);			-- number of IP pkts received for us
			-- MAC Transmitter
			mac_tx_tdata         : out  std_logic_vector(7 downto 0);	-- data byte to tx
			mac_tx_tvalid        : out  std_logic;							-- tdata is valid
			mac_tx_tready        : in std_logic;							-- mac is ready to accept data
			mac_tx_tfirst        : out  std_logic;							-- indicates first byte of frame
			mac_tx_tlast         : out  std_logic;							-- indicates last byte of frame
			-- MAC Receiver
			mac_rx_tdata         : in std_logic_vector(7 downto 0);	-- data byte received
			mac_rx_tvalid        : in std_logic;							-- indicates tdata is valid
			mac_rx_tready        : out  std_logic;							-- tells mac that we are ready to take data
			mac_rx_tfirst			: in std_logic;
			mac_rx_tlast         : in std_logic								-- indicates last byte of the trame
			);
END COMPONENT;



COMPONENT gbe_mac
port(
	iRst_n 				: IN  STD_LOGIC;
    ---------------------------------------------------------------------------
    -- RGMII Interface
    ---------------------------------------------------------------------------
    ENET1_GTX_CLK    	: OUT STD_LOGIC;
    ENET1_TX_EN 		: OUT STD_LOGIC;
    ENET1_TX_DATA     	: OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
    ENET1_RX_CLK    	: IN  STD_LOGIC;
    ENET1_RX_DV 		: IN  STD_LOGIC;
    ENET1_RX_DATA     	: IN  STD_LOGIC_VECTOR(3 DOWNTO 0);
	--PHY
	ENET1_PHY_RESET_L	: OUT std_logic;
	ENET1_PHY_MDC 		: OUT std_logic;
	ENET1_PHY_MDIO 		: INOUT std_logic;


	--TO UDP
	iMAC_HAL			: IN  STD_LOGIC_VECTOR(47 DOWNTO 0);

	iUDP_rx_rdy			:	IN STD_LOGIC;
	DATA_VALID_RX_OUT 	: OUT STD_LOGIC;
	DATA_RX_OUT 		: OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
	SOF_RX_OUT 			: OUT STD_LOGIC;
	EOF_RX_OUT 			: OUT STD_LOGIC;

	DATA_VALID_TX_IN 	: IN STD_LOGIC;
	DATA_TX_IN 			: IN STD_LOGIC_VECTOR(7 DOWNTO 0);
	SOF_TX_IN 			: IN STD_LOGIC;
	EOF_TX_IN 			: IN STD_LOGIC;
	MAC_RDY_IN			: OUT STD_LOGIC;

	CLK_OUT				: OUT STD_LOGIC
);
END COMPONENT;

constant MAC_FPGA : std_logic_vector(47 downto 0) := x"11223344AABB";
constant IP_FPGA : std_logic_vector(31 downto 0) := x"AC1B0A05";--172.27.10.5

signal clk_int : std_logic;
signal control_int : udp_control_type;

signal mac_rx_valid_int, mac_sof_rx_int, mac_eof_rx_int, mac_rx_rdy_int : std_logic;
signal mac_rx_data_int : std_logic_vector(7 downto 0);

signal mac_tx_dv_int, mac_tx_ready_int, mac_tx_sof_int, mac_tx_eof_int : std_logic;
signal mac_tx_data_int : std_logic_vector(7 downto 0);

signal count : unsigned(31 downto 0);
signal iRst_n_debounced_int : std_logic;

type state is (idle, s0, s1, s2, s3, s4);
signal state_tx : state;

begin
process(clk_int)
begin
	if clk_int'event and clk_int='1' then
		if count < x"3D090" then
			iRst_n_debounced_int	<= '0';
			count <= count +1;
		else
			iRst_n_debounced_int	<= '1';
		end if;
	end if;
end process;


control_int.ip_controls.arp_controls.clear_cache <= '0';

UDP_INST : UDP_Complete_nomac
generic map (
     CLOCK_FREQ			=> 125000000,						-- artificially low count to enable pragmatic testing
     ARP_TIMEOUT		=> 20,
     ARP_MAX_PKT_TMO	=> 10,									-- # wrong nwk pkts received before set error
     MAX_ARP_ENTRIES 	=> 255									-- max entries in the ARP store
)
PORT MAP (
    udp_tx_start => udp_tx_start,
    udp_txi => udp_txi,
    udp_tx_result => udp_tx_result,
    udp_tx_data_out_ready => udp_tx_data_out_ready,

    udp_rx_start => udp_rx_start,
    udp_rxo => udp_rxo,
    ip_rx_hdr => open,--ip_rx_hdr,

    rx_clk => clk_int,
    tx_clk => clk_int,
    reset => not iRst_n_debounced_int,
    our_ip_address => IP_FPGA,
    our_mac_address => MAC_FPGA,
    control => control_int,
    arp_pkt_count => open,--arp_pkt_count,
    ip_pkt_count => open,--ip_pkt_count,

    mac_tx_tdata => mac_tx_data_int,
    mac_tx_tvalid => mac_tx_dv_int,
    mac_tx_tready => mac_tx_ready_int,
    mac_tx_tfirst => mac_tx_sof_int,
    mac_tx_tlast => mac_tx_eof_int,

    mac_rx_tdata => mac_rx_data_int,
    mac_rx_tvalid => mac_rx_valid_int,
    mac_rx_tready => mac_rx_rdy_int,--mac_rx_tready, --TEST
    mac_rx_tfirst => mac_sof_rx_int,
    mac_rx_tlast => mac_eof_rx_int
);

GBE_MAC_INST : gbe_mac
PORT MAP (

    iRst_n 				=> iRst_n_debounced_int,
    ---------------------------------------------------------------------------
    -- RGMII Interface
    ---------------------------------------------------------------------------
    ENET1_GTX_CLK    	=> gtx_clk,
    ENET1_TX_EN 		=> tx_en,
    ENET1_TX_DATA     	=> tx_data,
    ENET1_RX_CLK    	=> rx_clk,
    ENET1_RX_DV 		=> rx_dv,
    ENET1_RX_DATA     	=> rx_data,
    --PHY
    ENET1_PHY_RESET_L	=> phy_reset_l,
    ENET1_PHY_MDC 		=> phy_mdc,
    ENET1_PHY_MDIO 		=> phy_mdio,

    --TO UDP
    iMAC_HAL			=> MAC_FPGA,

    iUDP_rx_rdy			=> mac_rx_rdy_int,
    DATA_VALID_RX_OUT 	=> mac_rx_valid_int,
    DATA_RX_OUT 		=> mac_rx_data_int,
    SOF_RX_OUT 			=> mac_sof_rx_int,
    EOF_RX_OUT 			=> mac_eof_rx_int,

    DATA_VALID_TX_IN 	=> mac_tx_dv_int,
    DATA_TX_IN 			=> mac_tx_data_int,
    SOF_TX_IN 			=> mac_tx_sof_int,
    EOF_TX_IN 			=> mac_tx_eof_int,
    MAC_RDY_IN			=> mac_tx_ready_int,

    CLK_OUT				=> clk_int
);

CLK_OUT <= clk_int;

end architecture;
