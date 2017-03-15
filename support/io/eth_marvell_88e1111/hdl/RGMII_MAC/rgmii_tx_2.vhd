-------------------------------------------------------------------------------
-- Title      : 
-- Project    : 
-------------------------------------------------------------------------------
-- File       : rgmii_tx_2.vhd
-- Author     : liyi  <alxiuyain@foxmail.com>
-- Company    : OE@HUST
-- Created    : 2012-11-15
-- Last update: 2013-05-07
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: 
-------------------------------------------------------------------------------
-- Copyright (c) 2012 OE@HUST
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2012-11-15  1.0      root    Created
-------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;
-------------------------------------------------------------------------------
ENTITY rgmii_tx_2 IS

  PORT (
    iClk   : IN STD_LOGIC;
    iRst_n : IN STD_LOGIC;

    -- from fifo
    iTxData     : IN  STD_LOGIC_VECTOR(7 DOWNTO 0);
	--iTxDv		: IN  STD_LOGIC;
    --iSOF        : IN  STD_LOGIC;
    --iEOF        : IN  STD_LOGIC;
	iFFempty 	: IN  STD_LOGIC;
	oTRANSMIT_DONE 	: OUT STD_LOGIC;
	iReadReq	: OUT STD_LOGIC;
	
	
	
    --iGenFrame   : IN  STD_LOGIC;
    --oGenFrameAck : OUT STD_LOGIC;

    -- signals TO PHY
    oTxData : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    oTxEn   : OUT STD_LOGIC;
    oTxErr  : OUT STD_LOGIC
    );

END ENTITY rgmii_tx_2;
-------------------------------------------------------------------------------
ARCHITECTURE rtl OF rgmii_tx_2 IS

  TYPE state_t IS (IDLE, PREAMBLE, SEND_DATA, WAIT_1_CYCLE_PAD, PAD, SEND_CRC, IPG);
  SIGNAL state                      : state_t;
  ATTRIBUTE syn_encoding            : STRING;
  ATTRIBUTE syn_encoding OF state_t : TYPE IS "safe,onehot";

  SIGNAL byteCnt : UNSIGNED(15 DOWNTO 0);

  SIGNAL crcInit : STD_LOGIC;
  SIGNAL crcEn   : STD_LOGIC;
  SIGNAL crc     : STD_LOGIC_VECTOR(31 DOWNTO 0);
  
  SIGNAL data_to_crc_int : STD_LOGIC_VECTOR(7 DOWNTO 0);
  
BEGIN  -- ARCHITECTURE rtl

  crcCalc : ENTITY work.eth_crc32
    PORT MAP (
      iClk    => iClk,
      iRst_n  => iRst_n,
      iInit   => crcInit,
      iCalcEn => crcEn,
      iData   => data_to_crc_int,--iTxData,
      oCRC    => crc,
      oCRCErr => OPEN);

  oTxErr <= '0';
  
  data_to_crc_int <= (others => '0') when ((state = WAIT_1_CYCLE_PAD) or (state = PAD)) else iTxData;

  PROCESS (iClk, iRst_n) IS
  BEGIN
    IF iRst_n = '0' THEN
      state        <= IDLE;
	  iReadReq	   <= '0';
      --oSOF         <= '0';
      byteCnt      <= (OTHERS => '0');
      --oGenFrameAck <= '0';
      crcInit      <= '0';
      crcEn        <= '0';
      oTxData      <= (OTHERS => '0');
      oTxEn        <= '0';
    ELSIF rising_edge(iClk) THEN
      --oGenFrameAck <= '0';
      crcInit      <= '0';
      --oSOF         <= '0';
	  oTRANSMIT_DONE <= '0';
	  iReadReq	   <= '0';
      byteCnt      <= byteCnt + 1;
      CASE state IS
        WHEN IDLE =>
          byteCnt <= (OTHERS => '0');
          --IF iGenFrame = '1' THEN
		  IF iFFempty = '0' then
            crcInit      <= '1';
            --oGenFrameAck <= '1';
            state        <= PREAMBLE;
          END IF;
        -----------------------------------------------------------------------
        WHEN PREAMBLE =>
          oTxEn   <= '1';
          oTxData <= X"55";
          CASE byteCnt(2 DOWNTO 0) IS
            --WHEN B"101" => oSOF <= '1';
			WHEN B"110" =>
			  iReadReq<= '1';
            WHEN B"111" =>
			  iReadReq<= '1';
              oTxData <= X"D5";
              crcEn   <= '1';
              state   <= SEND_DATA;
              byteCnt <= (OTHERS => '0');
            WHEN OTHERS => NULL;
          END CASE;
        -----------------------------------------------------------------------
        WHEN SEND_DATA =>--ajout data_valid from UDP
			oTxData 	<= iTxData;
			IF iFFempty = '0' then--
				iReadReq	<= '1';
				state		<= SEND_DATA;
			ELSE				
				iReadReq	<= '0';
				IF byteCnt < X"003B" THEN
					state <= WAIT_1_CYCLE_PAD;
				ELSE
				  state <= SEND_CRC;
				  crcEn <= '0';
				  byteCnt <= (OTHERS => '0');
				END IF;
			END IF;
					
			
			WHEN WAIT_1_CYCLE_PAD =>
				oTxData 	<= (OTHERS => '0');
				state		<= PAD;
			
			
			  -- oTxData <= iTxData;
			  -- IF iEOF = '1' THEN
				-- IF byteCnt < X"003B" THEN
				  -- state <= PAD;
				-- ELSE
				  -- state <= SEND_CRC;
				  -- crcEn <= '0';
				  -- byteCnt <= (OTHERS => '0');
				-- END IF;
			  -- END IF;
			-- END IF;
        -----------------------------------------------------------------------
        WHEN PAD =>
			 oTxData	<= (OTHERS => '0');
          IF byteCnt(7 DOWNTO 0) = X"3B" THEN
				oTxData <= (OTHERS => '0');
            crcEn   <= '0';
            state   <= SEND_CRC;
            byteCnt <= (OTHERS => '0');
          END IF;
        -----------------------------------------------------------------------
        WHEN SEND_CRC =>
          CASE byteCnt(1 DOWNTO 0) IS
            WHEN B"00" => oTxData <= crc(31 DOWNTO 24);
            WHEN B"01" => oTxData <= crc(23 DOWNTO 16);
            WHEN B"10" => oTxData <= crc(15 DOWNTO 8);
            WHEN B"11" =>
              oTxData <= crc(7 DOWNTO 0);
              state   <= IPG;
              byteCnt <= (OTHERS => '0');
            WHEN OTHERS => NULL;
          END CASE;
        -----------------------------------------------------------------------
        WHEN IPG =>                     -- 96 bits(12 Bytes) time
          oTxEn <= '0';
          IF byteCnt(3 DOWNTO 0) = X"B" THEN
			oTRANSMIT_DONE <= '1';
            state   <= IDLE;
            byteCnt <= (OTHERS => '0');
          END IF;
        -----------------------------------------------------------------------
        WHEN OTHERS => NULL;
      END CASE;
    END IF;
  END PROCESS;

END ARCHITECTURE rtl;
