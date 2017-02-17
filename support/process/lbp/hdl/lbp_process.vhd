library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
use IEEE.math_real.all;
library std;

entity lbp_process is
	generic (
		LINE_WIDTH_MAX : integer;
		PIX_WIDTH : integer
	);
	port (
		clk_proc : in std_logic;
		reset_n : in std_logic;

		------------------------- in flow -----------------------
		in_data : in std_logic_vector((PIX_WIDTH-1) downto 0);
		in_fv : in std_logic;
		in_dv : in std_logic;

		------------------------ out flow -----------------------
		out_data : out std_logic_vector((PIX_WIDTH-1) downto 0);
		out_fv : out std_logic;
		out_dv : out std_logic;

		------------------------- params ------------------------
		enable_i : in std_logic;
		widthimg_i : in std_logic_vector(15 downto 0);
		theshold_i	: in std_logic_vector(7 downto 0)
	);
end lbp_process;

architecture rtl of lbp_process is

component matrix_extractor
	generic (
		LINE_WIDTH_MAX : integer;
		PIX_WIDTH : integer;
		OUTVALUE_WIDTH : integer
	);
	port (
		clk_proc : in std_logic;
		reset_n : in std_logic;

		------------------------- in flow -----------------------
		in_data : in std_logic_vector((PIX_WIDTH-1) downto 0);
		in_fv : in std_logic;
		in_dv : in std_logic;

		------------------------ out flow -----------------------
		out_data : out std_logic_vector((OUTVALUE_WIDTH-1) downto 0);
		out_fv : out std_logic;
		out_dv : out std_logic;
		
		------------------------ matrix out ---------------------
		p00, p01, p02 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		p10, p11, p12 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		p20, p21, p22 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		matrix_dv : out std_logic;
		
		---------------------- computed value -------------------
		value_data : in std_logic_vector((OUTVALUE_WIDTH-1) downto 0);
		value_dv : in std_logic;

		------------------------- params ------------------------
		enable_i : in std_logic;
		widthimg_i : in std_logic_vector(15 downto 0)
	);
end component;

signal p00, p01, p02 : std_logic_vector((PIX_WIDTH-1) downto 0);
signal p10, p11, p12 : std_logic_vector((PIX_WIDTH-1) downto 0);
signal p20, p21, p22 : std_logic_vector((PIX_WIDTH-1) downto 0);
signal matrix_dv : std_logic;

signal value_data : std_logic_vector((PIX_WIDTH-1) downto 0);
signal value_dv : std_logic;

begin

	matrix_extractor_inst : matrix_extractor
    generic map (
    	LINE_WIDTH_MAX		=> LINE_WIDTH_MAX,
    	PIX_WIDTH			=> PIX_WIDTH,
    	OUTVALUE_WIDTH		=> PIX_WIDTH
	)
    port map (
		clk_proc => clk_proc,
		reset_n => reset_n,
		in_data => in_data,
		in_fv => in_fv,
		in_dv => in_dv,
		p00 => p00, p01 => p01, p02 => p02,
		p10 => p10, p11 => p11, p12 => p12,
		p20 => p20, p21 => p21, p22 => p22,
		matrix_dv => matrix_dv,
		value_data => value_data,
		value_dv => value_dv,
		out_data => out_data,
		out_fv => out_fv,
		out_dv => out_dv,
		enable_i => enable_i,
		widthimg_i => widthimg_i
    );
	
	process (clk_proc, reset_n, matrix_dv)
	begin
		if(reset_n='0') then
				
		elsif(rising_edge(clk_proc)) then
			value_dv<='0';
			if(matrix_dv='1') then
			
				if(unsigned(p11)>unsigned(p00)+unsigned(theshold_i)) then value_data(0) <= '1'; else value_data(0) <= '0'; end if;
				if(unsigned(p11)>unsigned(p01)+unsigned(theshold_i)) then value_data(1) <= '1'; else value_data(1) <= '0'; end if;
				if(unsigned(p11)>unsigned(p02)+unsigned(theshold_i)) then value_data(2) <= '1'; else value_data(2) <= '0'; end if;
				if(unsigned(p11)>unsigned(p10)+unsigned(theshold_i)) then value_data(3) <= '1'; else value_data(3) <= '0'; end if;
				if(unsigned(p11)>unsigned(p12)+unsigned(theshold_i)) then value_data(4) <= '1'; else value_data(4) <= '0'; end if;
				if(unsigned(p11)>unsigned(p20)+unsigned(theshold_i)) then value_data(5) <= '1'; else value_data(5) <= '0'; end if;
				if(unsigned(p11)>unsigned(p21)+unsigned(theshold_i)) then value_data(6) <= '1'; else value_data(6) <= '0'; end if;
				if(unsigned(p11)>unsigned(p22)+unsigned(theshold_i)) then value_data(7) <= '1'; else value_data(7) <= '0'; end if;
				
				--~ value_data<=p11;
				value_dv<='1';
			end if;
		end if;
	end process;

end rtl;
