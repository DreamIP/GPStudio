library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity rgmii_rx_top_2 is
port(
    iEthClk : IN STD_LOGIC;
    iRst_n  : IN STD_LOGIC;
	
	 iMAC_HAL : IN STD_LOGIC_VECTOR(47 DOWNTO 0);
	
	 iEnetRxData : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    iEnetRxDv   : IN STD_LOGIC;
    iEnetRxErr  : IN STD_LOGIC;
	
	-- hardware checksum check
    iCheckSumIPCheck   : IN STD_LOGIC;
    iCheckSumTCPCheck  : IN STD_LOGIC;
    iCheckSumUDPCheck  : IN STD_LOGIC;
    iCheckSumICMPCheck : IN STD_LOGIC;
    

	--USR IF
	iUDP_rx_rdy		: IN STD_lOGIC;
	oData_rx			: OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
	oData_valid 	: OUT STD_LOGIC;
	oSOF				: OUT STD_LOGIC;
	oEOF				: OUT STD_LOGIC
	
	);
end entity;



architecture rtl of rgmii_rx_top_2 is

COMPONENT rgmii_rx IS

  PORT (
    iClk   : IN STD_LOGIC;
    iRst_n : IN STD_LOGIC;

    iRxData : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    iRxDV   : IN STD_LOGIC;
    iRxEr   : IN STD_LOGIC;

    -- these signals come from wishbone clock domian, NOT synchronized
    iCheckSumIPCheck   : IN STD_LOGIC;
    iCheckSumTCPCheck  : IN STD_LOGIC;
    iCheckSumUDPCheck  : IN STD_LOGIC;
    iCheckSumICMPCheck : IN STD_LOGIC;

    oEOF         : OUT STD_LOGIC;
    oSOF         : OUT STD_LOGIC;
    oCRCErr      : OUT STD_LOGIC;
    oRxErr       : OUT STD_LOGIC;
    oLenErr      : OUT STD_LOGIC;
    oCheckSumErr : OUT STD_LOGIC;

    iMyMAC : IN STD_LOGIC_VECTOR(47 DOWNTO 0);

    oGetARP  : OUT    STD_LOGIC;
    oGetIPv4 : BUFFER STD_LOGIC;
    oGetCtrl : OUT    STD_LOGIC;
    oGetRaw  : BUFFER STD_LOGIC;

    oTaged      : OUT    STD_LOGIC;
    oTagInfo    : OUT    STD_LOGIC_VECTOR(15 DOWNTO 0);
    oStackTaged : BUFFER STD_LOGIC;
    oTagInfo2   : OUT    STD_LOGIC_VECTOR(15 DOWNTO 0);

    oLink   : OUT STD_LOGIC;
    oSpeed  : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
    oDuplex : OUT STD_LOGIC;

    oPayloadLen : BUFFER UNSIGNED(15 DOWNTO 0);
    oRxData     : OUT    STD_LOGIC_VECTOR(7 DOWNTO 0);
    oRxDV       : OUT    STD_LOGIC
    );

END COMPONENT;

  SIGNAL cSOF        : STD_LOGIC;
  SIGNAL cEof        : STD_LOGIC;
  SIGNAL cErrCrc     : STD_LOGIC;
  SIGNAL cErrLen     : STD_LOGIC;
  SIGNAL cGetArp     : STD_LOGIC;
  SIGNAL cErrCheckSum : STD_LOGIC;
  SIGNAL cGetIPv4    : STD_LOGIC;
  SIGNAL cGetCtrl    : STD_LOGIC;
  SIGNAL cGetRaw     : STD_LOGIC;
  SIGNAL cPayloadLen : UNSIGNED(15 DOWNTO 0);
  SIGNAL cRxData     : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL cRxDV       : STD_LOGIC;


begin
  rgmii_rx_1 : ENTITY work.rgmii_rx
    PORT MAP (
      iClk               => iEthClk,
      iRst_n             => iRst_n,
      iRxData            => iEnetRxData,
      iRxDV              => iEnetRxDv,
      iRxEr              => iEnetRxErr,
      iCheckSumIPCheck   => iCheckSumIPCheck,
      iCheckSumTCPCheck  => iCheckSumTCPCheck,
      iCheckSumUDPCheck  => iCheckSumUDPCheck,
      iCheckSumICMPCheck => iCheckSumICMPCheck,
      oEOF               => cEof,
      oCRCErr            => cErrCrc,
      oRxErr             => OPEN,
      oLenErr            => cErrLen,
      oCheckSumErr       => cErrCheckSum,
      iMyMAC             => iMAC_HAL,--MY_MAC,
      oGetARP            => cGetArp,
      oGetIPv4           => cGetIPv4,
      oGetCtrl           => cGetCtrl,
      oGetRaw            => cGetRaw,
      oSOF               => cSOF,
      oTaged             => OPEN,
      oTagInfo           => OPEN,
      oStackTaged        => OPEN,
      oTagInfo2          => OPEN,
      oLink              => OPEN,
      oSpeed             => OPEN,
      oDuplex            => OPEN,
      oPayloadLen        => cPayloadLen,
      oRxData            => cRxData,
      oRxDV              => cRxDV
	  );
	  
--	rgmii_rx_2 : ENTITY work.rgmii_rx_2
--	PORT MAP(
--		iClk   		=> iEthClk,
--		iRst_n 		=> iRst_n,
--
--		iRxData 	=> iEnetRxData,
--		iRxDV   	=> iEnetRxDv,
--		iRxEr   	=> iEnetRxErr,
--	
--		iMAC_FPGA	=> iMAC_HAL,
--	
--		iUdpRdy		=> iUDP_rx_rdy,
--		oRxData     => cRxData,
--		oRxDV       => cRxDV,
--		oEOF		=>	cEof
--	);
	  
	oData_rx	<= cRxData;
	oData_valid <= cRxDV;
	oSOF		<= cSOF;
	oEOF		<= cEof;
	
end architecture;