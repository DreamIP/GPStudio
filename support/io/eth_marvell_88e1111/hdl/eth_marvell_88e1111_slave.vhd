library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity eth_marvell_88e1111_slave is
	generic (
		CLK_PROC_FREQ : integer;
		CLK_HAL_FREQ  : integer
	);
	port (
		clk_proc        : in std_logic;
		clk_hal         : in std_logic;
		reset           : in std_logic;

		---------------- dynamic parameters ports ---------------
		status_enable   : out std_logic;
		flow_in0_enable : out std_logic;
		flow_in1_enable : out std_logic;
		flow_in2_enable : out std_logic;
		flow_in3_enable : out std_logic;

		--======================= Slaves ========================

		------------------------- bus_sl ------------------------
		addr_rel_i      : in std_logic_vector(3 downto 0);
		wr_i            : in std_logic;
		rd_i            : in std_logic;
		datawr_i        : in std_logic_vector(31 downto 0);
		datard_o        : out std_logic_vector(31 downto 0)
	);
end eth_marvell_88e1111_slave;

architecture rtl of eth_marvell_88e1111_slave is

	-- Registers address
	constant STATUS_REG_ADDR   : natural := 0;
	constant FLOW_IN0_REG_ADDR : natural := 1;
	constant FLOW_IN1_REG_ADDR : natural := 2;
	constant FLOW_IN2_REG_ADDR : natural := 3;
	constant FLOW_IN3_REG_ADDR : natural := 4;

	-- Internal registers
	signal status_enable_reg   : std_logic;
	signal flow_in0_enable_reg : std_logic;
	signal flow_in1_enable_reg : std_logic;
	signal flow_in2_enable_reg : std_logic;
	signal flow_in3_enable_reg : std_logic;

begin
	write_reg : process (clk_proc, reset_n)
	begin
		if(reset_n='0') then
			status_enable_reg <= '0';
			flow_in0_enable_reg <= '0';
			flow_in1_enable_reg <= '0';
			flow_in2_enable_reg <= '0';
			flow_in3_enable_reg <= '0';
		elsif(rising_edge(clk_proc)) then
			if(wr_i='1') then
				case to_integer(unsigned(addr_rel_i)) is
					when STATUS_REG_ADDR =>
						status_enable_reg <= datawr_i(0);
					when FLOW_IN0_REG_ADDR =>
						flow_in0_enable_reg <= datawr_i(0);
					when FLOW_IN1_REG_ADDR =>
						flow_in1_enable_reg <= datawr_i(0);
					when FLOW_IN2_REG_ADDR =>
						flow_in2_enable_reg <= datawr_i(0);
					when FLOW_IN3_REG_ADDR =>
						flow_in3_enable_reg <= datawr_i(0);
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
					when STATUS_REG_ADDR =>
						datard_o <= "0000000000000000000000000000000" & status_enable_reg;
					when FLOW_IN0_REG_ADDR =>
						datard_o <= "0000000000000000000000000000000" & flow_in0_enable_reg;
					when FLOW_IN1_REG_ADDR =>
						datard_o <= "0000000000000000000000000000000" & flow_in1_enable_reg;
					when FLOW_IN2_REG_ADDR =>
						datard_o <= "0000000000000000000000000000000" & flow_in2_enable_reg;
					when FLOW_IN3_REG_ADDR =>
						datard_o <= "0000000000000000000000000000000" & flow_in3_enable_reg;
					when others=>
						datard_o <= (others => '0');
				end case;
			end if;
		end if;
	end process;

	status_enable <= status_enable_reg;
	flow_in0_enable <= flow_in0_enable_reg;
	flow_in1_enable <= flow_in1_enable_reg;
	flow_in2_enable <= flow_in2_enable_reg;
	flow_in3_enable <= flow_in3_enable_reg;

end rtl;
