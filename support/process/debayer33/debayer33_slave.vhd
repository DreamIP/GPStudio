library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity debayer33_slave is
	generic (
		CLK_PROC_FREQ : integer
	);
	port (
		clk_proc              : in std_logic;
		reset_n               : in std_logic;

		---------------- dynamic parameters ports ---------------
		status_reg_enable_bit : out std_logic;
		bayer_code    		  : out std_logic_vector(1 downto 0);

		--======================= Slaves ========================

		------------------------- bus_sl ------------------------
		addr_rel_i            : in std_logic_vector(3 downto 0);
		wr_i                  : in std_logic;
		rd_i                  : in std_logic;
		datawr_i              : in std_logic_vector(31 downto 0);
		datard_o              : out std_logic_vector(31 downto 0)
	);
end debayer33_slave;

architecture rtl of debayer33_slave is

	-- Registers address   
	constant STATUS_REG_REG_ADDR   : natural := 0;
	constant BAYER_CODE_REG_REG_ADDR : natural := 1;

	-- Internal registers      
	signal status_reg_enable_bit_reg : std_logic;
	signal bayer_code_reg    : std_logic_vector (1 downto 0);

begin
	write_reg : process (clk_proc, reset_n)
	begin
		if(reset_n='0') then
			status_reg_enable_bit_reg <= '0';
			bayer_code_reg <= (others => '0');
		elsif(rising_edge(clk_proc)) then
			if(wr_i='1') then
				case addr_rel_i is
					when std_logic_vector(to_unsigned(STATUS_REG_REG_ADDR, 4))=>
						status_reg_enable_bit_reg <= datawr_i(0);
					when std_logic_vector(to_unsigned(BAYER_CODE_REG_REG_ADDR, 4))=>
						bayer_code_reg <= datawr_i(1) & datawr_i(0);
					when others=>
				end case;
			end if;
		end if;
	end process;

	read_reg : process (clk_proc, reset_n)
	begin
		if(reset_n='0') then
			datard_o <= (others => '0');
		elsif(rising_edge(clk_proc)) then
			if(rd_i='1') then
				case addr_rel_i is
					when std_logic_vector(to_unsigned(STATUS_REG_REG_ADDR, 4))=>
						datard_o <= (0 => status_reg_enable_bit_reg, others => '0');
					when std_logic_vector(to_unsigned(BAYER_CODE_REG_REG_ADDR, 4))=>
						datard_o <= (1 => bayer_code_reg(1), 0 => bayer_code_reg(0), others => '0');
					when others=>
						datard_o <= (others => '0');
				end case;
			end if;
		end if;
	end process;

	status_reg_enable_bit <= status_reg_enable_bit_reg;
	bayer_code <= bayer_code_reg;

end rtl;
