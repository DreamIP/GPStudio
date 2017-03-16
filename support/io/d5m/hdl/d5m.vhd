library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity d5m is
	generic (
		CLK_PROC_FREQ : integer;
		OUT_SIZE      : integer
	);
	port (
		clk_proc    : in std_logic;
		reset_n     : in std_logic;

		--------------------- external ports --------------------
		ccd_pixclk  : in std_logic;
		ccd_data    : in std_logic_vector(11 downto 0);
		ccd_xclkin  : out std_logic;
		ccd_reset   : out std_logic;
		ccd_trigger : out std_logic;
		ccd_lval    : in std_logic;
		ccd_fval    : in std_logic;
		i2c_sdata   : inout std_logic;
		i2c_sclk    : out std_logic;

		------------------------ out flow -----------------------
		out_data    : out std_logic_vector(OUT_SIZE-1 downto 0);
		out_fv      : out std_logic;
		out_dv      : out std_logic;

		--======================= Slaves ========================

		------------------------- bus_sl ------------------------
		addr_rel_i  : in std_logic_vector(3 downto 0);
		wr_i        : in std_logic;
		rd_i        : in std_logic;
		datawr_i    : in std_logic_vector(31 downto 0);
		datard_o    : out std_logic_vector(31 downto 0)
	);
end d5m;

architecture rtl of d5m is

--/*D5M controller */--
  component d5m_controller
    generic
      (
        pixel_address_width : integer
        );
    port
      (
        clk         : in    std_logic;
        reset_n     : in    std_logic;
        ccd_trigger : out   std_logic;
        ccd_xclkin  : out   std_logic;
        ccd_reset   : out   std_logic;
        ccd_data    : in std_logic_vector(11 downto 0);
        ccd_fval    : in std_logic;
        ccd_lval    : in std_logic;
        ccd_pixclk  : in std_logic;
        i2c_sclk    : out std_logic;
		  
        i_exposure_adj : in std_logic;
		  
        i2c_sdata   : inout std_logic;
        pix_address : out   std_logic_vector(pixel_address_width-1 downto 0);
        oRed,
        oGreen,
        oBlue,
        data       : out   std_logic_vector(7 downto 0);
        dv,
        fv       : out   std_logic
        );
  end component d5m_controller;

	signal status_reg_enable_bit : std_logic;
        signal pixel_address : std_logic_vector(31 downto 0) := (others => '0');

begin
	  d5m_controller_inst : d5m_controller
    generic map
    (
      pixel_address_width => 19
      )
    port map
    (
      -- External I/Os
      clk             => clk_proc,
      ccd_xclkin      => ccd_xclkin,
      ccd_trigger     => ccd_trigger,
      ccd_reset       => ccd_reset,
      ccd_data        => ccd_data,
      ccd_fval        => ccd_fval,
      ccd_lval        => ccd_lval,
      ccd_pixclk      => ccd_pixclk,
		
      i_exposure_adj   => '0',
		
      reset_n         => reset_n,
      i2c_sclk        => i2c_sclk,
      i2c_sdata       => i2c_sdata,
      pix_address     => pixel_address(18 downto 0),

      -- Output flow
      data => out_data,
      dv => out_dv,
      fv => out_fv
      );


end rtl;
