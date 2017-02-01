library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity usb3_ft600x is
	generic (
		CLK_PROC_FREQ : integer;
		OUT0_SIZE     : integer;
		OUT1_SIZE     : integer;
		IN0_SIZE      : integer;
		IN1_SIZE      : integer
	);
	port (
		clk_proc  : in std_logic;
		reset_n   : in std_logic;

		--------------------- external ports --------------------
		ftreset_n : out std_logic;
		ftclk     : in std_logic;
		be        : inout std_logic_vector(1 downto 0);
		data      : inout std_logic_vector(15 downto 0);
		txe_n     : in std_logic;
		rxf_n     : in std_logic;
		siwu_n    : out std_logic;
		wr_n      : out std_logic;
		rd_n      : out std_logic;
		oe_n      : out std_logic;

		------------------------ out0 flow ----------------------
		out0_data : out std_logic_vector(OUT0_SIZE-1 downto 0);
		out0_fv   : out std_logic;
		out0_dv   : out std_logic;

		------------------------ out1 flow ----------------------
		out1_data : out std_logic_vector(OUT1_SIZE-1 downto 0);
		out1_fv   : out std_logic;
		out1_dv   : out std_logic;

		------------------------ in0 flow -----------------------
		in0_data  : in std_logic_vector(IN0_SIZE-1 downto 0);
		in0_fv    : in std_logic;
		in0_dv    : in std_logic;

		------------------------ in1 flow -----------------------
		in1_data  : in std_logic_vector(IN1_SIZE-1 downto 0);
		in1_fv    : in std_logic;
		in1_dv    : in std_logic
	);
end usb3_ft600x;

architecture rtl of usb3_ft600x is

begin
	
    ftreset_n <= '0';

end rtl;
