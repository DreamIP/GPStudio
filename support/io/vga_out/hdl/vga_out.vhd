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
component vga_out_process
	generic (
		CLK_PROC_FREQ : integer;
		IN_SIZE       : integer
	);
	port (
		clk_proc              : in std_logic;
		reset_n               : in std_logic;

		---------------- dynamic parameters ports ---------------
		status_reg_enable_bit : in std_logic;

		------------------------- in flow -----------------------
		in_data               : in std_logic_vector(IN_SIZE-1 downto 0);
		in_fv                 : in std_logic;
		in_dv                 : in std_logic
	);
end component;

component vga_out_slave
	generic (
		CLK_PROC_FREQ : integer
	);
	port (
		clk_proc              : in std_logic;
		reset_n               : in std_logic;

		---------------- dynamic parameters ports ---------------
		status_reg_enable_bit : out std_logic;

		--======================= Slaves ========================

		------------------------- bus_sl ------------------------
		addr_rel_i            : in std_logic_vector(3 downto 0);
		wr_i                  : in std_logic;
		rd_i                  : in std_logic;
		datawr_i              : in std_logic_vector(31 downto 0);
		datard_o              : out std_logic_vector(31 downto 0)
	);
end component;

	signal status_reg_enable_bit : std_logic;

begin
	vga_out_process_inst : vga_out_process
    generic map (
		CLK_PROC_FREQ => CLK_PROC_FREQ,
		IN_SIZE       => IN_SIZE
	)
    port map (
		clk_proc              => clk_proc,
		reset_n               => reset_n,
		status_reg_enable_bit => status_reg_enable_bit,
		in_data               => in_data,
		in_fv                 => in_fv,
		in_dv                 => in_dv
	);

	vga_out_slave_inst : vga_out_slave
    generic map (
		CLK_PROC_FREQ => CLK_PROC_FREQ
	)
    port map (
		clk_proc              => clk_proc,
		reset_n               => reset_n,
		status_reg_enable_bit => status_reg_enable_bit,
		addr_rel_i            => addr_rel_i,
		wr_i                  => wr_i,
		rd_i                  => rd_i,
		datawr_i              => datawr_i,
		datard_o              => datard_o
	);


end rtl;
