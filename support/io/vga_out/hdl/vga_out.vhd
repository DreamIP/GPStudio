library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity vga_out is
	generic (
		CLK_PROC_FREQ : integer;
		IN_SIZE       : integer
	);
	port (
		clk_proc    : in std_logic;
		reset_n     : in std_logic;

		--------------------- external ports --------------------
		vga_blank_n : out std_logic;
		vga_r       : out std_logic_vector(7 downto 0);
		vga_g       : out std_logic_vector(7 downto 0);
		vga_b       : out std_logic_vector(7 downto 0);
		vga_clk     : out std_logic;
		vga_hs      : out std_logic;
		vga_vs      : out std_logic;
		vga_sync_n  : out std_logic;

		------------------------- in flow -----------------------
		in_data     : in std_logic_vector(IN_SIZE-1 downto 0);
		in_fv       : in std_logic;
		in_dv       : in std_logic;

		--======================= Slaves ========================

		------------------------- bus_sl ------------------------
		addr_rel_i  : in std_logic_vector(3 downto 0);
		wr_i        : in std_logic;
		rd_i        : in std_logic;
		datawr_i    : in std_logic_vector(31 downto 0);
		datard_o    : out std_logic_vector(31 downto 0)
	);
end vga_out;

architecture rtl of vga_out is

  --/*D5M controller */--
  component vga_controller
    port
      (
        OSC_50                       : in  std_logic;
        RESET_N                        : in  std_logic;
        VGA_HS, VGA_VS               : out std_logic;
        VGA_SYNC, VGA_BLANK          : out std_logic;
        VGA_RED, VGA_GREEN, VGA_BLUE : out std_logic_vector(7 downto 0);
        CLOCK108                     : out std_logic;

		  x_offset : in integer range 0 to 1280; -- offset of the upper left pixel
        y_offset : in integer range 0 to 1024;
	 
        data : in std_logic_vector(7 downto 0);
        fv   : in std_logic;            -- frame valid
        dv   : in std_logic             -- data valid
        );
  end component vga_controller;


	signal status_reg_enable_bit : std_logic;
	signal vga_clk108 : std_logic;

begin

  vga_controller_inst : vga_controller
    port map
    (
      -- External I/Os
      OSC_50    => clk_proc,
      RESET_N     => reset_n,
      VGA_HS    => vga_hs,
      VGA_VS    => vga_vs,
      VGA_SYNC  => vga_sync_n,
      VGA_BLANK => vga_blank_n,
      VGA_RED   => vga_r,
      VGA_GREEN => vga_g,
      VGA_BLUE  => vga_b,
      CLOCK108  => vga_clk108,

      x_offset  => to_integer(X"0190"),
      y_offset  => to_integer(X"0190"),
		
      -- Input flow
      data => in_data,
      fv   => in_fv,
      dv   => in_dv
      );
      
      vga_clk <= vga_clk108;

end rtl;
