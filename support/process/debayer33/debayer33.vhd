library ieee;
use     ieee.std_logic_1164.all;
use     ieee.numeric_std.all;
use 	ieee.math_real.all;

entity debayer33 is
generic( 	CLK_PROC_FREQ 	: integer;
			IM_WIDTH 		: integer := 1280;
			IM_HEIGHT		: integer := 960;
			COLOR_CHANNELS	: integer := 3;
			DATA_SIZE		: integer := 8
			);
port(
		clk_proc			: in std_logic;
		reset_n				: in std_logic;	
	
		------------------------- in flow -----------------------
		in_data				: in std_logic_vector(DATA_SIZE-1 downto 0);
		in_fv				: in std_logic;
		in_dv				: in std_logic;
		
		------------------------ out flow -----------------------
		out_fv				: out std_logic;
		out_dv				: out std_logic; 
		out_data			: out std_logic_vector((COLOR_CHANNELS*DATA_SIZE)-1 downto 0);

		------------------------- bus_sl ------------------------
		addr_rel_i          : in std_logic_vector(3 downto 0);
		wr_i                : in std_logic;
		rd_i                : in std_logic;
		datawr_i            : in std_logic_vector(31 downto 0);
		datard_o            : out std_logic_vector(31 downto 0);
		
		------------------------- for sim -------------------------
		sim_en				: in std_logic;
		bayer_code_sim		: in std_logic_vector(1 downto 0);
		test_count_x_out	: out std_logic_vector(integer(ceil(log2(real(IM_WIDTH))))-1 downto 0);
		test_count_y_out	: out std_logic_vector(integer(ceil(log2(real(IM_HEIGHT))))-1 downto 0)	
		
	);
end entity;



architecture structural of debayer33 is

component debayer33_slave is
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
end component;

component debayer33_process is
generic( 	CLK_PROC_FREQ 	: integer;
			IM_WIDTH 		: integer := 1280;
			IM_HEIGHT		: integer := 960;
			COLOR_CHANNELS	: integer := 3;
			DATA_SIZE		: integer := 8
			);
port(
		clk_proc			: in std_logic;
		reset_n				: in std_logic;	
		
		------------------------- from slave -------------------------
		bayer_code_slave	: in std_logic_vector(1 downto 0);	
	
		------------------------- in flow -----------------------
		in_data				: in std_logic_vector(DATA_SIZE-1 downto 0);
		in_fv				: in std_logic;
		in_dv				: in std_logic;
		
		------------------------ out flow -----------------------
		out_data			: out std_logic_vector((COLOR_CHANNELS*DATA_SIZE)-1 downto 0);
		out_fv				: out std_logic;
		out_dv				: out std_logic; 
		
		------------------------- for sim -------------------------
		sim_en				: in std_logic;
		bayer_code_sim		: in std_logic_vector(1 downto 0);
		test_count_x_out	: out std_logic_vector(integer(ceil(log2(real(IM_WIDTH))))-1 downto 0);
		test_count_y_out	: out std_logic_vector(integer(ceil(log2(real(IM_HEIGHT))))-1 downto 0)			
	);
end component;

signal bayer_code_slave_int : std_logic_vector(1 downto 0);	

begin

u0 : debayer33_slave
		generic map(
			CLK_PROC_FREQ 	=> CLK_PROC_FREQ
		)
		port map(
			clk_proc              => clk_proc,
			reset_n               => reset_n,

			---------------- dynamic parameters ports ---------------
			status_reg_enable_bit => open,
			bayer_code    		  => bayer_code_slave_int,

			--======================= Slaves ========================

			------------------------- bus_sl ------------------------
			addr_rel_i            => addr_rel_i,
			wr_i                  => wr_i,
			rd_i                  => rd_i,
			datawr_i              => datawr_i,
			datard_o              => datard_o
		);
	
u1 : debayer33_process 
		generic map(
			CLK_PROC_FREQ 	=> CLK_PROC_FREQ,
			IM_WIDTH		=> IM_WIDTH,
			IM_HEIGHT 		=> IM_HEIGHT,
			COLOR_CHANNELS	=> COLOR_CHANNELS,
			DATA_SIZE		=> DATA_SIZE		
		)
		port map(
			clk_proc		=> clk_proc,
			reset_n			=> reset_n,
	
			------------------------- from slave -------------------------
			bayer_code_slave=> bayer_code_slave_int,
			
			------------------------- in flow -----------------------
			in_data			=> in_data,
			in_fv			=> in_fv,
			in_dv			=> in_dv,
		
			------------------------ out flow -----------------------
			out_data		=> out_data,
			out_fv			=> out_fv,
			out_dv 			=> out_dv,
			
			------------------------- for sim -------------------------
			sim_en				=> sim_en,
			bayer_code_sim		=> bayer_code_sim,
			test_count_x_out	=> test_count_x_out,
			test_count_y_out	=> test_count_y_out
		);

end architecture;