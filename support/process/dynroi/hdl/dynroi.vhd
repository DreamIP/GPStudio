library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity dynroi is
	generic (
		CLK_PROC_FREQ : integer;
		BINIMG_SIZE   : integer;
		IMG_SIZE      : integer;
		ROI_SIZE      : integer;
		COORD_SIZE    : integer
	);
	port (
		clk_proc    : in std_logic;
		reset_n     : in std_logic;

		----------------------- BinImg flow ---------------------
		BinImg_data : in std_logic_vector(BINIMG_SIZE-1 downto 0);
		BinImg_fv   : in std_logic;
		BinImg_dv   : in std_logic;

		------------------------ Img flow -----------------------
		Img_data    : in std_logic_vector(IMG_SIZE-1 downto 0);
		Img_fv      : in std_logic;
		Img_dv      : in std_logic;

		------------------------ roi flow -----------------------
		roi_data    : out std_logic_vector(ROI_SIZE-1 downto 0);
		roi_fv      : out std_logic;
		roi_dv      : out std_logic;

		----------------------- coord flow ----------------------
		coord_data  : out std_logic_vector(COORD_SIZE-1 downto 0);
		coord_fv    : out std_logic;
		coord_dv    : out std_logic;

		--======================= Slaves ========================

		------------------------- bus_sl ------------------------
		addr_rel_i  : in std_logic_vector(1 downto 0);
		wr_i        : in std_logic;
		rd_i        : in std_logic;
		datawr_i    : in std_logic_vector(31 downto 0);
		datard_o    : out std_logic_vector(31 downto 0)
	);
end dynroi;

architecture rtl of dynroi is
component dynroi_process
	generic (
		CLK_PROC_FREQ : integer;
		BINIMG_SIZE   : integer;
		IMG_SIZE      : integer;
		ROI_SIZE      : integer;
		COORD_SIZE    : integer
	);
	port (
		clk_proc                  : in std_logic;
		reset_n                   : in std_logic;

		---------------- dynamic parameters ports ---------------
		status_reg_enable_bit     : in std_logic;
		status_reg_bypass_bit     : in std_logic;
		status_reg_static_res_bit : in std_logic;
		inImg_size_reg_in_w_reg   : in std_logic_vector(11 downto 0);
		inImg_size_reg_in_h_reg   : in std_logic_vector(11 downto 0);
		BinImg_size_reg_in_w_reg  : in std_logic_vector(11 downto 0);
		BinImg_size_reg_in_h_reg  : in std_logic_vector(11 downto 0);
		out_size_reg_out_w_reg    : in std_logic_vector(11 downto 0);
		out_size_reg_out_h_reg    : in std_logic_vector(11 downto 0);

		----------------------- BinImg flow ---------------------
		BinImg_data               : in std_logic_vector(BINIMG_SIZE-1 downto 0);
		BinImg_fv                 : in std_logic;
		BinImg_dv                 : in std_logic;

		------------------------ Img flow -----------------------
		Img_data                  : in std_logic_vector(IMG_SIZE-1 downto 0);
		Img_fv                    : in std_logic;
		Img_dv                    : in std_logic;

		------------------------ roi flow -----------------------
		roi_data                  : out std_logic_vector(ROI_SIZE-1 downto 0);
		roi_fv                    : out std_logic;
		roi_dv                    : out std_logic;

		----------------------- coord flow ----------------------
		coord_data                : out std_logic_vector(COORD_SIZE-1 downto 0);
		coord_fv                  : out std_logic;
		coord_dv                  : out std_logic
	);
end component;

component dynroi_slave
	generic (
		CLK_PROC_FREQ : integer
	);
	port (
		clk_proc                  : in std_logic;
		reset_n                   : in std_logic;

		---------------- dynamic parameters ports ---------------
		status_reg_enable_bit     : out std_logic;
		status_reg_bypass_bit     : out std_logic;
		status_reg_static_res_bit : out std_logic;
		inImg_size_reg_in_w_reg   : out std_logic_vector(11 downto 0);
		inImg_size_reg_in_h_reg   : out std_logic_vector(11 downto 0);
		BinImg_size_reg_in_w_reg  : out std_logic_vector(11 downto 0);
		BinImg_size_reg_in_h_reg  : out std_logic_vector(11 downto 0);
		out_size_reg_out_w_reg    : out std_logic_vector(11 downto 0);
		out_size_reg_out_h_reg    : out std_logic_vector(11 downto 0);

		--======================= Slaves ========================

		------------------------- bus_sl ------------------------
		addr_rel_i                : in std_logic_vector(1 downto 0);
		wr_i                      : in std_logic;
		rd_i                      : in std_logic;
		datawr_i                  : in std_logic_vector(31 downto 0);
		datard_o                  : out std_logic_vector(31 downto 0)
	);
end component;

	signal status_reg_enable_bit     : std_logic;
	signal status_reg_bypass_bit     : std_logic;
	signal status_reg_static_res_bit : std_logic;
	signal inImg_size_reg_in_w_reg   : std_logic_vector (11 downto 0);
	signal inImg_size_reg_in_h_reg   : std_logic_vector (11 downto 0);
	signal BinImg_size_reg_in_w_reg  : std_logic_vector (11 downto 0);
	signal BinImg_size_reg_in_h_reg  : std_logic_vector (11 downto 0);
	signal out_size_reg_out_w_reg    : std_logic_vector (11 downto 0);
	signal out_size_reg_out_h_reg    : std_logic_vector (11 downto 0);

begin
	dynroi_process_inst : dynroi_process
    generic map (
		CLK_PROC_FREQ => CLK_PROC_FREQ,
		BINIMG_SIZE   => BINIMG_SIZE,
		IMG_SIZE      => IMG_SIZE,
		ROI_SIZE      => ROI_SIZE,
		COORD_SIZE    => COORD_SIZE
	)
    port map (
		clk_proc                  => clk_proc,
		reset_n                   => reset_n,
		status_reg_enable_bit     => status_reg_enable_bit,
		status_reg_bypass_bit     => status_reg_bypass_bit,
		status_reg_static_res_bit => status_reg_static_res_bit,
		inImg_size_reg_in_w_reg   => inImg_size_reg_in_w_reg,
		inImg_size_reg_in_h_reg   => inImg_size_reg_in_h_reg,
		BinImg_size_reg_in_w_reg  => BinImg_size_reg_in_w_reg,
		BinImg_size_reg_in_h_reg  => BinImg_size_reg_in_h_reg,
		out_size_reg_out_w_reg    => out_size_reg_out_w_reg,
		out_size_reg_out_h_reg    => out_size_reg_out_h_reg,
		BinImg_data               => BinImg_data,
		BinImg_fv                 => BinImg_fv,
		BinImg_dv                 => BinImg_dv,
		Img_data                  => Img_data,
		Img_fv                    => Img_fv,
		Img_dv                    => Img_dv,
		roi_data                  => roi_data,
		roi_fv                    => roi_fv,
		roi_dv                    => roi_dv,
		coord_data                => coord_data,
		coord_fv                  => coord_fv,
		coord_dv                  => coord_dv
	);

	dynroi_slave_inst : dynroi_slave
    generic map (
		CLK_PROC_FREQ => CLK_PROC_FREQ
	)
    port map (
		clk_proc                  => clk_proc,
		reset_n                   => reset_n,
		status_reg_enable_bit     => status_reg_enable_bit,
		status_reg_bypass_bit     => status_reg_bypass_bit,
		status_reg_static_res_bit => status_reg_static_res_bit,
		inImg_size_reg_in_w_reg   => inImg_size_reg_in_w_reg,
		inImg_size_reg_in_h_reg   => inImg_size_reg_in_h_reg,
		BinImg_size_reg_in_w_reg  => BinImg_size_reg_in_w_reg,
		BinImg_size_reg_in_h_reg  => BinImg_size_reg_in_h_reg,
		out_size_reg_out_w_reg    => out_size_reg_out_w_reg,
		out_size_reg_out_h_reg    => out_size_reg_out_h_reg,
		addr_rel_i                => addr_rel_i,
		wr_i                      => wr_i,
		rd_i                      => rd_i,
		datawr_i                  => datawr_i,
		datard_o                  => datard_o
	);


end rtl;
