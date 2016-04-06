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
	constant W11_ADDR			: natural := 2;
	constant W12_ADDR			: natural := 3;
	constant W13_ADDR			: natural := 4;
	constant W21_ADDR			: natural := 5;
	constant W22_ADDR			: natural := 6;
	constant W23_ADDR			: natural := 7;
	constant W31_ADDR			: natural := 8;
	constant W32_ADDR			: natural := 9;
	constant W33_ADDR			: natural := 10;
	constant NORM_ADDR			: natural := 11;

        constant HRES_ADDR			: natural := 12;
	

	signal enable_reg 			: std_logic;
	signal widthimg_reg 		        : std_logic_vector(15 downto 0);
	signal w11_reg				: std_logic_vector(7 downto 0);
	signal w12_reg				: std_logic_vector(7 downto 0);
	signal w13_reg				: std_logic_vector(7 downto 0);
	signal w21_reg				: std_logic_vector(7 downto 0);
	signal w22_reg				: std_logic_vector(7 downto 0);
	signal w23_reg				: std_logic_vector(7 downto 0);
	signal w31_reg				: std_logic_vector(7 downto 0);
	signal w32_reg				: std_logic_vector(7 downto 0);
	signal w33_reg				: std_logic_vector(7 downto 0);
	signal norm_reg				: std_logic_vector(7 downto 0);
	

begin

	write_reg : process (clk_proc, reset_n)
	begin
		if(reset_n='0') then
			enable_reg <= '0';
			widthimg_reg <= std_logic_vector(to_unsigned(320, 16));
			
			
		elsif(rising_edge(clk_proc)) then
			if(wr_i='1') then
			case addr_rel_i is
				when std_logic_vector(to_unsigned(ENABLE_REG_ADDR, 4))	=>	enable_reg 		<= datawr_i(0);
                    
				when std_logic_vector(to_unsigned(WIDTHIMG_REG_ADDR, 4))=>	widthimg_reg 	  <= datawr_i(15 downto 0);

				when std_logic_vector(to_unsigned(W11_ADDR, 4))			=>	w11_reg	  <= datawr_i (7 downto 0);	
				when std_logic_vector(to_unsigned(W12_ADDR, 4))			=>	w12_reg	  <= datawr_i (7 downto 0);
				when std_logic_vector(to_unsigned(W13_ADDR, 4))			=>	w13_reg	  <= datawr_i (7 downto 0);
				when std_logic_vector(to_unsigned(W21_ADDR, 4))			=>	w21_reg	  <= datawr_i (7 downto 0);
				when std_logic_vector(to_unsigned(W22_ADDR, 4))			=>	w22_reg	  <= datawr_i (7 downto 0);
				when std_logic_vector(to_unsigned(W23_ADDR, 4))			=>	w23_reg	  <= datawr_i (7 downto 0);
				when std_logic_vector(to_unsigned(W31_ADDR, 4))			=>	w31_reg	  <= datawr_i (7 downto 0);
				when std_logic_vector(to_unsigned(W32_ADDR, 4))			=>	w32_reg	  <= datawr_i (7 downto 0);
				when std_logic_vector(to_unsigned(W33_ADDR, 4))			=>	w33_reg	  <= datawr_i (7 downto 0);
				when std_logic_vector(to_unsigned(NORM_ADDR, 4))		=>	norm_reg  <= datawr_i (7 downto 0);
                          
					
				when others=>
				end case;
			end if;
		end if;
	end process;
	
	enable_o 	<= enable_reg;
	widthimg_o 	<= widthimg_reg;

	
	
	
end rtl;
