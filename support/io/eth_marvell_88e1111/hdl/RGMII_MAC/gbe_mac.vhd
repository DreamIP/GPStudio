library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity gbe_mac is
port(
	
	CLK50M				: IN STD_LOGIC;
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
	ENET1_PHY_RESET_L	: out std_logic;
	ENET1_PHY_MDC 		: out std_logic;
	ENET1_PHY_MDIO 		: inout std_logic;
	
	
	--TO UDP
	iMAC_HAL			: IN  STD_LOGIC_VECTOR(47 DOWNTO 0);
	
	
	--RX
	iUDP_rx_rdy			: IN  STD_LOGIC;
	DATA_VALID_RX_OUT 	: OUT STD_LOGIC;
	DATA_RX_OUT 		: OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
	SOF_RX_OUT 			: OUT STD_LOGIC;
	EOF_RX_OUT 			: OUT STD_LOGIC;
	--TX
	DATA_VALID_TX_IN 	: IN STD_LOGIC;
	DATA_TX_IN 			: IN STD_LOGIC_VECTOR(7 DOWNTO 0);
	SOF_TX_IN 			: IN STD_LOGIC;
	EOF_TX_IN 			: IN STD_LOGIC;
	MAC_RDY_IN			: OUT STD_LOGIC;

	CLK_OUT				: OUT STD_LOGIC
);
end entity;



architecture rtl of gbe_mac is

component rgmii1000_io is
PORT (
    iRst_n : IN  STD_LOGIC;	
    ---------------------------------------------------------------------------
    -- RGMII Interface
    ---------------------------------------------------------------------------
    TXC    : OUT STD_LOGIC;
    TX_CTL : OUT STD_LOGIC;
    TD     : OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
    RXC    : IN  STD_LOGIC;
    RX_CTL : IN  STD_LOGIC;
    RD     : IN  STD_LOGIC_VECTOR(3 DOWNTO 0);
    ---------------------------------------------------------------------------
    -- data to PHY 
    ---------------------------------------------------------------------------
    iTxData : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    iTxEn   : IN STD_LOGIC;
    iTxErr  : IN STD_LOGIC;
    ---------------------------------------------------------------------------
    -- data from PHY
    ---------------------------------------------------------------------------
    oRxData : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    oRxDV   : OUT STD_LOGIC;
    oRxErr  : OUT STD_LOGIC;
    ---------------------------------------------------------------------------
    -- clock for MAC controller
    ---------------------------------------------------------------------------
    oEthClk      : OUT STD_LOGIC
    );

end component;

component rgmii_rx_top_2 is
port(
    iEthClk 			: IN STD_LOGIC;
    iRst_n  			: IN STD_LOGIC;
	iMAC_HAL 			: IN STD_LOGIC_VECTOR(47 DOWNTO 0);
	iEnetRxData 		: IN STD_LOGIC_VECTOR(7 DOWNTO 0); 
    iEnetRxDv   		: IN STD_LOGIC;
    iEnetRxErr  		: IN STD_LOGIC;
    iCheckSumIPCheck   	: IN STD_LOGIC;
    iCheckSumTCPCheck  	: IN STD_LOGIC;
    iCheckSumUDPCheck  	: IN STD_LOGIC;
    iCheckSumICMPCheck 	: IN STD_LOGIC;  
	--USR IF
	iUDP_rx_rdy			: IN STD_lOGIC;
	oData_rx			: OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
	oData_valid 		: OUT STD_LOGIC;
	oSOF				: OUT STD_LOGIC;
	oEOF				: OUT STD_LOGIC	
	);
end component;

component rgmii_tx_top_2 IS
  PORT (
    iEthClk 			: IN STD_LOGIC;
    iRst_n  			: IN STD_LOGIC;
	
	oEnetTxData 		: OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    oEnetTxEn   		: OUT STD_LOGIC;
    oEnetTxErr  		: OUT STD_LOGIC;
	
	--USR IF FROM "UDP COMPLETE"
	iData_tx			: IN STD_LOGIC_VECTOR(7 DOWNTO 0);
	iTxDataValid		: IN STD_LOGIC;
	iSOF				: IN STD_LOGIC;
	iEOF				: IN STD_LOGIC;		
	oMACTxRdy			: OUT STD_LOGIC	
	);
end component;

component eth_mdio is 
Port(
	CLK         : in STD_LOGIC;
	RESET       : in STD_LOGIC;
	E_RST_L     : out STD_LOGIC;
	E_MDC       : out STD_LOGIC;
	E_MDIO      : inout STD_LOGIC);
end component;

signal cEthClk  : std_logic; 
signal  cEnetRxDV, cEnetRxErr : std_logic;
signal cEnetTxEn, cEnetTxErr : std_logic;  

signal cEnetRxData, cEnetTxData : std_logic_vector(7 downto 0);

begin

rgmii_rx_top_2_inst : rgmii_rx_top_2
	PORT MAP(
		iEthClk 			=> cEthClk, 
		iRst_n  			=> iRst_n,
		iMAC_HAL 			=> iMAC_HAL,
		iEnetRxData 		=> cEnetRxData,
		iEnetRxDv   		=> cEnetRxDV,
		iEnetRxErr  		=> cEnetRxErr,
		iCheckSumIPCheck   	=> '0',
		iCheckSumTCPCheck  	=> '0',
		iCheckSumUDPCheck  	=> '0',
		iCheckSumICMPCheck 	=> '0',
		--USR IF
		iUDP_rx_rdy		=> iUDP_rx_rdy,
		oData_rx		=> DATA_RX_OUT,
		oData_valid 	=> DATA_VALID_RX_OUT,
		oSOF			=> SOF_RX_OUT,
		oEOF			=> EOF_RX_OUT
	);
	
rgmii_tx_top_2_inst : rgmii_tx_top_2
	PORT MAP(
		iEthClk 			=> cEthClk,
		iRst_n  			=> iRst_n,
		
		oEnetTxData 		=> cEnetTxData,
		oEnetTxEn   		=> cEnetTxEn,
		oEnetTxErr  		=> cEnetTxErr,
		
		--USR IF FROM "UDP COMPLETE"
		iData_tx			=> DATA_TX_IN,
		iTxDataValid		=> DATA_VALID_TX_IN,
		iSOF				=> SOF_TX_IN,
		iEOF				=> EOF_TX_IN,
		oMACTxRdy			=> MAC_RDY_IN
		);
		

  rgmii_io_1 : rgmii1000_io
    PORT MAP (
      iRst_n  => iRst_n,
      TXC     => ENET1_GTX_CLK,
      TX_CTL  => ENET1_TX_EN,
      TD      => ENET1_TX_DATA,
      RXC     => ENET1_RX_CLK,
      RX_CTL  => ENET1_RX_DV,
      RD      => ENET1_RX_DATA,
      iTxData => cEnetTxData,
      iTxEn   => cEnetTxEn,
      iTxErr  => cEnetTxErr,
      oRxData => cEnetRxData,
      oRxDV   => cEnetRxDV,
      oRxErr  => cEnetRxErr,
      oEthClk => cEthClk);
	  
	  CLK_OUT <= cEthClk;

eth_mdio_inst : eth_mdio 
	PORT MAP(
	CLK         => cEthClk,
	RESET       => iRst_n,
	E_RST_L     => ENET1_PHY_RESET_L,
	E_MDC       => ENET1_PHY_MDC,
	E_MDIO      => ENET1_PHY_MDIO	
	);

end architecture;