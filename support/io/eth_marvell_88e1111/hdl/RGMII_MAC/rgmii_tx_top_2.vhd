LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ENTITY rgmii_tx_top_2 IS
  PORT (
    iEthClk : IN STD_LOGIC;
    iRst_n  : IN STD_LOGIC;
	
	oEnetTxData : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    oEnetTxEn   : OUT STD_LOGIC;
    oEnetTxErr  : OUT STD_LOGIC;

    -- iCheckSumIPGen   : IN STD_LOGIC;
    -- iCheckSumTCPGen  : IN STD_LOGIC;
    -- iCheckSumUDPGen  : IN STD_LOGIC;
    -- iCheckSumICMPGen : IN STD_LOGIC;
	
	--USR IF FROM "UDP COMPLETE"
	iData_tx		: IN STD_LOGIC_VECTOR(7 DOWNTO 0);
	iTxDataValid	: IN STD_LOGIC;
	iSOF			: IN STD_LOGIC;
	iEOF			: IN STD_LOGIC;		
	oMACTxRdy		: OUT STD_LOGIC
	);
END ENTITY;



ARCHITECTURE RTL OF rgmii_tx_top_2 is

COMPONENT rgmii_tx_2 IS
  PORT (
    iClk   : IN STD_LOGIC;
    iRst_n : IN STD_LOGIC;

    -- from fifo
    iTxData     : IN  STD_LOGIC_VECTOR(7 DOWNTO 0);
    --iSOF        : IN STD_LOGIC;
    --iEOF        : IN  STD_LOGIC;	
	iFFempty 	: IN  STD_LOGIC;
	oTRANSMIT_DONE : OUT STD_LOGIC;
	iReadReq	: OUT STD_LOGIC;

    -- signals TO PHY
    oTxData : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    oTxEn   : OUT STD_LOGIC;
    oTxErr  : OUT STD_LOGIC
    );
END COMPONENT;

COMPONENT fifo_tx_udp IS
	PORT
	(
		aclr		: IN STD_LOGIC ;
		clock		: IN STD_LOGIC ;
		data		: IN STD_LOGIC_VECTOR (7 DOWNTO 0);
		rdreq		: IN STD_LOGIC ;
		wrreq		: IN STD_LOGIC ;
		empty		: OUT STD_LOGIC ;
		full		: OUT STD_LOGIC ;
		q			: OUT STD_LOGIC_VECTOR (7 DOWNTO 0)
	);
END COMPONENT;

signal rst_ff_int, ff_rdreq_int, ff_wrreq_int, ff_empty_int, ff_full_int, oTRANSMIT_DONE_int : std_logic;

signal iData_tx_delayed_int, cTxData : std_logic_vector(7 downto 0);

type state_ff is (init, s0, s1, s2, s3);
signal state : state_ff;

begin

ff_tx_udp_inst : fifo_tx_udp
	PORT MAP(
		aclr		=> rst_ff_int,
		clock		=> iEthClk,
		data		=> iData_tx_delayed_int,--iData_tx,
		rdreq		=> ff_rdreq_int,
		wrreq		=> ff_wrreq_int,
		empty		=> ff_empty_int,
		full		=> ff_full_int,
		q			=> cTxData
	);

  rgmii_tx_inst : rgmii_tx_2
    PORT MAP (
      iClk         => iEthClk,
      iRst_n       => iRst_n,
      iTxData      => cTxData,
      iFFempty     => ff_empty_int,
	  oTRANSMIT_DONE=> oTRANSMIT_DONE_int,
	  iReadReq		=> ff_rdreq_int,
      --iEOF         => eof
      oTxData      => oEnetTxData,--oEnetTxData,
      oTxEn        => oEnetTxEn,--oEnetTxEn,
      oTxErr       => oEnetTxErr);
		
process(iEthClk, iRst_n)
begin
if iRst_n = '0' then

	iData_tx_delayed_int	<= (others => '0');
elsif rising_edge(iEthClk) then

	iData_tx_delayed_int	<= iData_tx;
end if;
end process;	
		
	  
process(iEthClk, iRst_n)
begin
if iRst_n = '0' then
	
	rst_ff_int 		<= '1';
	oMACTxRdy 		<= '0';
	ff_wrreq_int	<= '0';
	state 			<= init;

elsif rising_edge(iEthClk) then

	case state is
	
	when init =>
		rst_ff_int 		<= '1';
		oMACTxRdy 		<= '0';
		ff_wrreq_int	<= '0';
		state 			<= s0;
		
	when s0 =>--MAC IS READY
		rst_ff_int 		<= '0';
		ff_wrreq_int	<= '0';
		if ff_full_int = '0' then
			oMACTxRdy 		<= '1';
			state 			<= s1;
		else
			oMACTxRdy 		<= '0';
			state 			<= s0;			
		end if;
	
	when s1 =>--SOF STARTING
		rst_ff_int 		<= '0';
		if ff_full_int = '0' then
			oMACTxRdy 		<= '1';
			if iTxDataValid = '1' then
				if iSOF = '1' then -- signal de start
					ff_wrreq_int 		<= '1';
					state 				<= s2;
				--else--sinon on attend eof sans ecrire
				end if;
			else
				ff_wrreq_int 		<= '0';
				state 				<= s1;
			end if;
		else
			oMACTxRdy 		<= '0';
			ff_wrreq_int	<= '0';
			state 			<= s1;		
		end if;
		
	when s2 =>--DATA TO FF
		rst_ff_int 		<= '0';
		if ff_full_int = '0' then
			oMACTxRdy 		<= '1';
			if iTxDataValid = '1' then
				if iEOF = '1' then	--signal de fin d'écriture
					ff_wrreq_int 		<= '1';
					state 				<= s3;
				else
					ff_wrreq_int 		<= '1';--on continue d'écrire dans FF
					state 				<= s2;
				end if;
			else
				ff_wrreq_int 		<= '0';
				state 				<= s2;				
			end if;
		else
			oMACTxRdy 		<= '0';
			ff_wrreq_int	<= '0';
			state 			<= s3;		
		end if;
	
	when s3 =>	
		ff_wrreq_int	<= '0';
		if oTRANSMIT_DONE_int = '1' then
			rst_ff_int 		<= '1';
			oMACTxRdy 		<= '0';
			state 			<= s0;
		end if;
	
	when others => 
	
		rst_ff_int 		<= '1';
		oMACTxRdy 		<= '0';
		ff_wrreq_int	<= '0';
		state 			<= init;
		
	end case;

end if;
end process;


END ARCHITECTURE;