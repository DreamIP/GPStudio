library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity draw_slave is
	generic (
		CLK_PROC_FREQ : integer
	);
	port (
		clk_proc                  : in std_logic;
		reset_n                   : in std_logic;

		---------------- dynamic parameters ports ---------------
		status_reg_enable_bit     : out std_logic;
		inImg_size_reg_in_w_reg   : out std_logic_vector(11 downto 0);
		inImg_size_reg_in_h_reg   : out std_logic_vector(11 downto 0);

		--======================= Slaves ========================

		------------------------- bus_sl ------------------------
		addr_rel_i                : in std_logic_vector(1 downto 0);
		wr_i                      : in std_logic;
		rd_i                      : in std_logic;
		datawr_i                  : in std_logic_vector(31 downto 0);
		datard_o                  : out std_logic_vector(31 downto 0)
	);
end draw_slave;

architecture rtl of draw_slave is

	-- Registers address      
	constant STATUS_REG_REG_ADDR      : natural := 0;
	constant INIMG_SIZE_REG_REG_ADDR : natural := 1;

	-- Internal registers          
	signal status_reg_enable_bit_reg     : std_logic;
	signal inImg_size_reg_in_w_reg_reg   : std_logic_vector (11 downto 0);
	signal inImg_size_reg_in_h_reg_reg   : std_logic_vector (11 downto 0);

begin
	write_reg : process (clk_proc, reset_n)
	begin
		if(reset_n='0') then
			status_reg_enable_bit_reg <= '0';
		elsif(rising_edge(clk_proc)) then
			if(wr_i='1') then
				case to_integer(unsigned(addr_rel_i)) is
					when STATUS_REG_REG_ADDR =>
						status_reg_enable_bit_reg <= datawr_i(0);
					when INIMG_SIZE_REG_REG_ADDR =>
						inImg_size_reg_in_w_reg_reg <= datawr_i(11) & datawr_i(10) & datawr_i(9) & datawr_i(8) & datawr_i(7) & datawr_i(6) & datawr_i(5) & datawr_i(4) & datawr_i(3) & datawr_i(2) & datawr_i(1) & datawr_i(0);
						inImg_size_reg_in_h_reg_reg <= datawr_i(27) & datawr_i(26) & datawr_i(25) & datawr_i(24) & datawr_i(23) & datawr_i(22) & datawr_i(21) & datawr_i(20) & datawr_i(19) & datawr_i(18) & datawr_i(17) & datawr_i(16);
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
				case to_integer(unsigned(addr_rel_i)) is
					when STATUS_REG_REG_ADDR =>
						datard_o <= "00000000000000000000000000000" & '0' & '0' & status_reg_enable_bit_reg;
					when INIMG_SIZE_REG_REG_ADDR =>
						datard_o <= "0000" & inImg_size_reg_in_h_reg_reg(11) & inImg_size_reg_in_h_reg_reg(10) & inImg_size_reg_in_h_reg_reg(9) & inImg_size_reg_in_h_reg_reg(8) & inImg_size_reg_in_h_reg_reg(7) & inImg_size_reg_in_h_reg_reg(6) & inImg_size_reg_in_h_reg_reg(5) & inImg_size_reg_in_h_reg_reg(4) & inImg_size_reg_in_h_reg_reg(3) & inImg_size_reg_in_h_reg_reg(2) & inImg_size_reg_in_h_reg_reg(1) & inImg_size_reg_in_h_reg_reg(0) & "0000" & inImg_size_reg_in_w_reg_reg(11) & inImg_size_reg_in_w_reg_reg(10) & inImg_size_reg_in_w_reg_reg(9) & inImg_size_reg_in_w_reg_reg(8) & inImg_size_reg_in_w_reg_reg(7) & inImg_size_reg_in_w_reg_reg(6) & inImg_size_reg_in_w_reg_reg(5) & inImg_size_reg_in_w_reg_reg(4) & inImg_size_reg_in_w_reg_reg(3) & inImg_size_reg_in_w_reg_reg(2) & inImg_size_reg_in_w_reg_reg(1) & inImg_size_reg_in_w_reg_reg(0);
					when others=>
						datard_o <= (others => '0');
				end case;
			end if;
		end if;
	end process;

	status_reg_enable_bit <= status_reg_enable_bit_reg;
	inImg_size_reg_in_w_reg <= inImg_size_reg_in_w_reg_reg;
	inImg_size_reg_in_h_reg <= inImg_size_reg_in_h_reg_reg;

end rtl;
