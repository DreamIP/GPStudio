-------------------------------------------------------------------------------
-- Copyright Institut Pascal Equipe Dream (19-10-2016)
-- Francois Berry, El Mehdi Abdali, Maxime Pelcat

-- This software is a computer program whose purpose is to manage dynamic 
-- partial reconfiguration.

-- This software is governed by the CeCILL-C license under French law and
-- abiding by the rules of distribution of free software.  You can  use, 
-- modify and/ or redistribute the software under the terms of the CeCILL-C
-- license as circulated by CEA, CNRS and INRIA at the following URL
-- "http://www.cecill.info". 

-- As a counterpart to the access to the source code and  rights to copy,
-- modify and redistribute granted by the license, users are provided only
-- with a limited warranty  and the software's author,  the holder of the
-- economic rights,  and the successive licensors  have only  limited
-- liability. 

-- In this respect, the user's attention is drawn to the risks associated
-- with loading,  using,  modifying and/or developing or reproducing the
-- software by the user in light of its specific status of free software,
-- that may mean  that it is complicated to manipulate,  and  that  also
-- therefore means  that it is reserved for developers  and  experienced
-- professionals having in-depth computer knowledge. Users are therefore
-- encouraged to load and test the software's suitability as regards their
-- requirements in conditions enabling the security of their systems and/or 
-- data to be ensured and,  more generally, to use and operate it in the 
-- same conditions as regards security. 

-- The fact that you are presently reading this means that you have had
-- knowledge of the CeCILL-C license and that you accept its terms.
-------------------------------------------------------------------------------

-- Doxygen Comments -----------------------------------------------------------
--! @file d5m_controller.vhd
--
--! @brief        D5M CMOS Image sensor controller
--! @author       Francois Berry, El Mehdi Abdali, Maxime Pelcat
--! @board        SoCKit from Arrow and Terasic
--! @version      1.0
--! @date         16/11/2016
-------------------------------------------------------------------------------


library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity d5m_controller is
  generic
    (
      pixel_address_width : integer
      );

  port
    (
	   -----
		-- Board I/Os
      clk             : in  std_logic; -- Input clock for processing and sent to CCD_XCLKIN
      reset_n         : in  std_logic;
		
      ccd_trigger     : out std_logic; -- Enable 
      ccd_reset       : out std_logic; -- Reset sensor
      ccd_xclkin      : out std_logic;
      ccd_data        : in  std_logic_vector(11 downto 0);
      ccd_fval        : in  std_logic;
      ccd_lval        : in  std_logic;
      ccd_pixclk      : in  std_logic;
		
      i_exposure_adj  : in  std_logic; -- Adjusting exposure
		
      i2c_sclk        : out std_logic;
      i2c_sdata       : inout std_logic;
		
		-----
		-- Unused I/Os (for future extensions)
      pix_address     : out   std_logic_vector(pixel_address_width-1 downto 0);
      oRed            : out   std_logic_vector(7 downto 0);
      oGreen          : out   std_logic_vector(7 downto 0);
      oBlue           : out   std_logic_vector(7 downto 0);
		
      -----
		-- GPStudio i/os
      data           : out   std_logic_vector(7 downto 0); -- gray data output
      dv             : out   std_logic; -- data valid
      fv             : out   std_logic  -- flow valid
      );
end d5m_controller;
architecture arch of d5m_controller is

  component CCD_Capture
    port
      (
        oDATA       : out   std_logic_vector(11 downto 0);
        oDVAL       : out   std_logic;
        oX_Cont     : out   std_logic_vector(15 downto 0);
        oY_Cont     : out   std_logic_vector(15 downto 0);
        oFrame_Cont : out   std_logic_vector(31 downto 0);
        iDATA       : in    std_logic_vector(11 downto 0);
        iFVAL       : in    std_logic;
        iLVAL       : in    std_logic;
        iSTART      : in    std_logic;
        iEND        : in    std_logic;
        iCLK        : in    std_logic;
        iRST        : in    std_logic;
        oADDRESS    : out   std_logic_vector(23 downto 0);
        oLVAL       : out   std_logic
        );
  end component CCD_Capture;



  component I2C_CCD_Config
    port
      (
        iCLK            : in    std_logic;
        iRST_N          : in    std_logic;
        iZOOM_MODE_SW   : in    std_logic;
        iEXPOSURE_ADJ   : in    std_logic;
        iEXPOSURE_DEC_p : in    std_logic;
        I2C_SCLK        : out   std_logic;
        I2C_SDAT        : inout std_logic
        );

  end component I2C_CCD_Config;


  component RAW2RGB
    port
      (
        iCLK    : in  std_logic;
        iRST    : in  std_logic;
        iDATA   : in  std_logic_vector(11 downto 0);
        iDVAL   : in  std_logic;
        oRed    : out std_logic_vector(11 downto 0);
        oGreen  : out std_logic_vector(11 downto 0);
        oBlue   : out std_logic_vector(11 downto 0);
        oDVAL   : out std_logic;
        iX_Cont : in  std_logic_vector(15 downto 0);
        iY_Cont : in  std_logic_vector(15 downto 0)
        );

  end component RAW2RGB;

  component RGB2GRY
    port
      (
        clk       : in  std_logic;
        reset     : in  std_logic;
        src_CCD_R : in  std_logic_vector(11 downto 0);
        src_CCD_G : in  std_logic_vector(11 downto 0);
        src_CCD_B : in  std_logic_vector(11 downto 0);
        oCCD_GRY  : out std_logic_vector(7 downto 0)
        );
  end component RGB2GRY;

  component VideoSampler
	generic(
		DATA_WIDTH			: integer;
		PIXEL_WIDTH			: integer;
		FIFO_DEPTH			: integer;
		DEFAULT_SCR			: integer;
		DEFAULT_FLOWLENGHT	: integer;
    	HREF_POLARITY		: string;
    	VSYNC_POLARITY		: string
	);
	port(
		-- input from CLOCK50 domain
		clk_i 				: in	std_logic;
		reset_n_i 			: in	std_logic;

		-- inputs from camera
		pclk_i				: in	std_logic;
		href_i				: in	std_logic;
		vsync_i				: in	std_logic;
		pixel_i				: in std_logic_vector(7 downto 0);

		-- params from slave
		enable_i			: in std_logic;
		flowlength_i		: in std_logic_vector(31 downto 0);

		-- Stream interface
		data_o				: out std_logic_vector(7 downto 0);
		dv_o				: out std_logic;
		fv_o				: out std_logic
	);
end component;

  signal sCCD_R, sCCD_G, sCCD_B, mCCD_DATA : std_logic_vector(11 downto 0);
  signal mCCD_DVAL, sCCD_DVAL              : std_logic;
  signal X_Cont, Y_Cont                    : std_logic_vector(15 downto 0);
  signal frame_count                       : std_logic_vector(31 downto 0);
  signal temp_pix_address                  : std_logic_vector(23 downto 0);
  signal sig_LVAL                          : std_logic;
  signal m_DATA                            : std_logic_vector(7 downto 0);
  constant DEFAULT_FLOWLENGTH              : std_logic_vector(31 downto 0) := x"00140000"; -- 1280*1024

begin

-- Preparing debayering
  CCD_Capture_inst : CCD_Capture
    port map
    (
      iDATA       => ccd_data,
      iFVAL       => ccd_fval,
      iLVAL       => ccd_lval,
      iSTART      => '1', -- always activating
      iEND        => '0',
      iCLK        => ccd_pixclk,
      iRST        => reset_n,
      oDATA       => mCCD_DATA,
      oDVAL       => mCCD_DVAL,
      oX_Cont     => X_Cont,
      oY_Cont     => Y_Cont,
      oFrame_Cont => frame_count,
      oADDRESS    => temp_pix_address,
      oLVAL       => sig_LVAL
      );


-- Debayering
  RAW2RGB_inst : RAW2RGB
    port map
    (
      iCLK    => ccd_pixclk,
      iRST    => reset_n,
      iDATA   => mCCD_DATA,
      iDVAL   => mCCD_DVAL,
      oRed    => sCCD_R,
      oGreen  => sCCD_G,
      oBlue   => sCCD_B,
      oDVAL   => sCCD_DVAL,
      iX_Cont => X_Cont,
      iY_Cont => Y_Cont
      );

  -- Converting to grayscale
  RGB2GRY_int : RGB2GRY
    port map
    (
      clk       => clk,
      reset     => not(reset_n),
      src_CCD_R => sCCD_R(11 downto 0),
      src_CCD_G => sCCD_G(11 downto 0),
      src_CCD_B => sCCD_B(11 downto 0),
      oCCD_GRY  => m_DATA
      );



  I2C_CCD_Config_inst : I2C_CCD_Config
    port map
    (
      iCLK            => clk,
      iRST_N          => reset_n,
      iZOOM_MODE_SW   => '0',
      iEXPOSURE_ADJ   => i_exposure_adj,
      iEXPOSURE_DEC_p => '0',
      I2C_SCLK        => i2c_sclk,
      I2C_SDAT        => i2c_sdata
      );

		
  -- Resampling
	VideoSampler_inst : VideoSampler
    generic map (
    	PIXEL_WIDTH	=>	8,
    	DATA_WIDTH	=>	32,
    	FIFO_DEPTH	=>	4096*4,
    	DEFAULT_SCR	=>	0,
    	DEFAULT_FLOWLENGHT => 1280*1024,
    	HREF_POLARITY => "high",
    	VSYNC_POLARITY => "high"
	)
    port map (
    	reset_n_i	  => reset_n,
    	clk_i	        => clk,
    	pclk_i	     => ccd_pixclk,
    	href_i	     => ccd_lval,
    	vsync_i	     => ccd_fval,
    	pixel_i	     => m_DATA,

    	enable_i	     => '1',
    	flowlength_i  => DEFAULT_FLOWLENGTH,

		data_o	 => data,
		dv_o	    => dv,
		fv_o	    => fv
	);
	
  oRed        <= sCCD_R(11 downto 4);
  oGreen      <= sCCD_G(11 downto 4);
  oBlue       <= sCCD_B(11 downto 4);
  ccd_xclkin  <= clk;
  ccd_trigger <= '1';
  ccd_reset   <= '1';
  pix_address <= temp_pix_address(pixel_address_width-1 downto 0);

end arch;
