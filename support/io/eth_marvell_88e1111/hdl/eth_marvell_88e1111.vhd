library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity eth_marvell_88e1111 is
	generic (
		IN0_NBWORDS   : integer;
		IN1_NBWORDS   : integer;
		IN2_NBWORDS   : integer;
		IN3_NBWORDS   : integer;
		OUT0_NBWORDS  : integer;
		OUT1_NBWORDS  : integer;
		CLK_PROC_FREQ : integer;
		IN0_SIZE      : integer;
		IN1_SIZE      : integer;
		IN2_SIZE      : integer;
		IN3_SIZE      : integer;
		OUT0_SIZE     : integer;
		OUT1_SIZE     : integer
	);
	port (
		clk_proc    : in std_logic;
		clk_hal     : out std_logic;
		reset       : out std_logic;

		--------------------- external ports --------------------
		gtx_clk     : out std_logic;
		tx_en       : out std_logic;
		tx_data     : out std_logic_vector(3 downto 0);
		rx_clk      : in std_logic;
		rx_dv       : in std_logic;
		rx_data     : in std_logic_vector(3 downto 0);
		phy_reset_l : out std_logic;
		phy_mdc     : out std_logic;
		phy_mdio    : inout std_logic;

		------------------------ in0 flow -----------------------
		in0_data    : in std_logic_vector(IN0_SIZE-1 downto 0);
		in0_fv      : in std_logic;
		in0_dv      : in std_logic;

		------------------------ in1 flow -----------------------
		in1_data    : in std_logic_vector(IN1_SIZE-1 downto 0);
		in1_fv      : in std_logic;
		in1_dv      : in std_logic;

		------------------------ in2 flow -----------------------
		in2_data    : in std_logic_vector(IN2_SIZE-1 downto 0);
		in2_fv      : in std_logic;
		in2_dv      : in std_logic;

		------------------------ in3 flow -----------------------
		in3_data    : in std_logic_vector(IN3_SIZE-1 downto 0);
		in3_fv      : in std_logic;
		in3_dv      : in std_logic;

		------------------------ out0 flow ----------------------
		out0_data   : out std_logic_vector(OUT0_SIZE-1 downto 0);
		out0_fv     : out std_logic;
		out0_dv     : out std_logic;

		------------------------ out1 flow ----------------------
		out1_data   : out std_logic_vector(OUT1_SIZE-1 downto 0);
		out1_fv     : out std_logic;
		out1_dv     : out std_logic;

		--======================= Slaves ========================

		------------------------- bus_sl ------------------------
		addr_rel_i  : in std_logic_vector(3 downto 0);
		wr_i        : in std_logic;
		rd_i        : in std_logic;
		datawr_i    : in std_logic_vector(31 downto 0);
		datard_o    : out std_logic_vector(31 downto 0)
	);
end eth_marvell_88e1111;

architecture rtl of eth_marvell_88e1111 is

component eth_marvell_88e1111_slave
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
end component;

	signal status_enable   : std_logic;
	signal flow_in0_enable : std_logic;
	signal flow_in1_enable : std_logic;
	signal flow_in2_enable : std_logic;
	signal flow_in3_enable : std_logic;

begin
	eth_marvell_88e1111_process_inst : eth_marvell_88e1111_process
    generic map (
		CLK_PROC_FREQ => CLK_PROC_FREQ,
		CLK_HAL_FREQ  => CLK_HAL_FREQ,
		IN0_NBWORDS   => IN0_NBWORDS,
		IN1_NBWORDS   => IN1_NBWORDS,
		IN2_NBWORDS   => IN2_NBWORDS,
		IN3_NBWORDS   => IN3_NBWORDS,
		OUT0_NBWORDS  => OUT0_NBWORDS,
		OUT1_NBWORDS  => OUT1_NBWORDS,
		IN0_SIZE      => IN0_SIZE,
		IN1_SIZE      => IN1_SIZE,
		IN2_SIZE      => IN2_SIZE,
		IN3_SIZE      => IN3_SIZE,
		OUT0_SIZE     => OUT0_SIZE,
		OUT1_SIZE     => OUT1_SIZE
	)
    port map (
		clk_proc        => clk_proc,
		clk_hal         => clk_hal,
		reset           => reset,
		status_enable   => status_enable,
		flow_in0_enable => flow_in0_enable,
		flow_in1_enable => flow_in1_enable,
		flow_in2_enable => flow_in2_enable,
		flow_in3_enable => flow_in3_enable,
		in0_data        => in0_data,
		in0_fv          => in0_fv,
		in0_dv          => in0_dv,
		in1_data        => in1_data,
		in1_fv          => in1_fv,
		in1_dv          => in1_dv,
		in2_data        => in2_data,
		in2_fv          => in2_fv,
		in2_dv          => in2_dv,
		in3_data        => in3_data,
		in3_fv          => in3_fv,
		in3_dv          => in3_dv,
		out0_data       => out0_data,
		out0_fv         => out0_fv,
		out0_dv         => out0_dv,
		out1_data       => out1_data,
		out1_fv         => out1_fv,
		out1_dv         => out1_dv
	);

	eth_marvell_88e1111_slave_inst : eth_marvell_88e1111_slave
    generic map (
		CLK_PROC_FREQ => CLK_PROC_FREQ,
		CLK_HAL_FREQ  => CLK_HAL_FREQ
	)
    port map (
		clk_proc        => clk_proc,
		clk_hal         => clk_hal,
		reset           => reset,
		status_enable   => status_enable,
		flow_in0_enable => flow_in0_enable,
		flow_in1_enable => flow_in1_enable,
		flow_in2_enable => flow_in2_enable,
		flow_in3_enable => flow_in3_enable,
		addr_rel_i      => addr_rel_i,
		wr_i            => wr_i,
		rd_i            => rd_i,
		datawr_i        => datawr_i,
		datard_o        => datard_o
	);


end rtl;
