library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

entity harris_slave is
	port (
		clk_proc		: in std_logic;
		reset_n			: in std_logic;
		
		addr_rel_i		: in std_logic_vector(3 downto 0);
		wr_i			: in std_logic;
		rd_i			: in std_logic;
		datawr_i		: in std_logic_vector(31 downto 0);
		datard_o		: out std_logic_vector(31 downto 0);
		
		enable_o		: out std_logic;
		widthimg_o		: out std_logic_vector(15 downto 0)
	);

end harris_slave;

architecture rtl of harris_slave is

	constant ENABLE_REG_ADDR	        : natural := 0;
	constant WIDTHIMG_REG_ADDR	        : natural := 1;
	
	signal enable_reg 					: std_logic;
	signal widthimg_reg 		        : std_logic_vector(15 downto 0);

begin

	write_reg : process (clk_proc, reset_n)
	begin
		if(reset_n='0') then
			enable_reg 		<= '0';
			widthimg_reg	<= std_logic_vector(to_unsigned(320, 16));
			
		elsif(rising_edge(clk_proc)) then
			if(wr_i='1') then
			
			case addr_rel_i is
				when std_logic_vector(to_unsigned(ENABLE_REG_ADDR, 4))	=>	enable_reg 		<= datawr_i(0);                 
				when std_logic_vector(to_unsigned(WIDTHIMG_REG_ADDR, 4))=>	widthimg_reg 	  <= datawr_i(15 downto 0);
				when others=>
				end case;
			end if;
		end if;
	end process;
	
	enable_o 	<= enable_reg;
	widthimg_o 	<= widthimg_reg;

end rtl;
