--||================================================================||--
--||-------  VHDL code for a 3x3 kernel image convolution --------- ||--                                                       
--||----------------------------------------------------------------||--
--|| Author	: 	Kamel Eddine ABDELOUAHAB - PhD Student              ||--
--|| Institution :	Institut Pascal - DREAM team                    ||--                                          
--||                Université Blaise Pascal - Clermont Ferrand     ||--                                         
--|| Contact:	ke.abdelouahab(at) gmail.com                        ||--                                                  
--||================================================================||--

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;


entity sconv is
	generic (
		LINE_WIDTH_MAX 	: integer	:=	320;
		IN_SIZE 		: integer	:=	8;
		OUT_SIZE 		: integer	:=	8;
		CLK_PROC_FREQ 	: integer 	:=  48000000
	);
	port (
		clk_proc 	: in std_logic;
		reset_n 	: in std_logic;

--============================ IN FLOW =================================
		in_data 	: in std_logic_vector((IN_SIZE-1) downto 0);
		in_fv 		: in std_logic;
		in_dv 		: in std_logic;

--=========================== OUT FLOW =================================
		out_data 	: out std_logic_vector((OUT_SIZE-1) downto 0);
		out_fv 		: out std_logic;
		out_dv 		: out std_logic;

--============================ Slaves =================================
		addr_rel_i 	: in std_logic_vector(3 downto 0);
		wr_i 		: in std_logic;
		rd_i 		: in std_logic;
		datawr_i 	: in std_logic_vector(31 downto 0);
		datard_o 	: out std_logic_vector(31 downto 0)
	);
end sconv;

architecture rtl of sconv is

component conv_slave
	port (
		clk_proc		: in std_logic;
		reset_n			: in std_logic;
		
		------------------------- bus_sl ------------------------
		addr_rel_i		: in std_logic_vector(3 downto 0);
		wr_i			: in std_logic;
		rd_i			: in std_logic;
		datawr_i		: in std_logic_vector(31 downto 0);
		datard_o		: out std_logic_vector(31 downto 0);
		
		--------------- connections to conv module--------------------
		enable_o		: out std_logic;
		widthimg_o		: out std_logic_vector(15 downto 0);
		
		------------------------ weights----------------------------
		w11_o			: out std_logic_vector (7 downto 0);
		w12_o			: out std_logic_vector (7 downto 0);
		w13_o			: out std_logic_vector (7 downto 0);
		w21_o			: out std_logic_vector (7 downto 0);
		w22_o			: out std_logic_vector (7 downto 0);
		w23_o			: out std_logic_vector (7 downto 0);
		w31_o			: out std_logic_vector (7 downto 0);
		w32_o			: out std_logic_vector (7 downto 0);
		w33_o			: out std_logic_vector (7 downto 0);
		norm_o			: out std_logic_vector (7 downto 0)
	);
end component;

component conv_process
	generic (
		LINE_WIDTH_MAX	: integer;
		PIX_WIDTH 		: integer
	);
	port (
		clk_proc	: in std_logic;
		reset_n		: in std_logic;

		------------------------- in flow -----------------------
		in_data		: in std_logic_vector((PIX_WIDTH-1) downto 0);
		in_fv		: in std_logic;
		in_dv		: in std_logic;

     --------------------------- in kernel-----------------------------
		w11,w12,w13	:	in std_logic_vector ((PIX_WIDTH-1) downto 0);
		w21,w22,w23	:	in std_logic_vector ((PIX_WIDTH-1) downto 0);
		w31,w32,w33	:	in std_logic_vector ((PIX_WIDTH-1) downto 0);
		norm		: 	in std_logic_vector ((PIX_WIDTH-1) downto 0);		

		------------------------ out flow -----------------------
		out_data	: out std_logic_vector((PIX_WIDTH-1) downto 0);
		out_fv		: out std_logic;
		out_dv		: out std_logic;

		
		------------------------- params ------------------------
		enable_i	: in std_logic;
		widthimg_i	: in std_logic_vector(15 downto 0)
	);
end component;

	
-- signals part
	signal 	enable_s 		: std_logic;
	signal 	widthimg_s 		: std_logic_vector(15 downto 0);
	signal 	w11s,w12s,w13s	: std_logic_vector (IN_SIZE-1 downto 0);
	signal	w21s,w22s,w23s	: std_logic_vector (IN_SIZE-1 downto 0);
	signal	w31s,w32s,w33s	: std_logic_vector (IN_SIZE-1 downto 0);
	signal	norms			: std_logic_vector (IN_SIZE-1 downto 0);
	
begin

	conv_slave_inst : conv_slave
    port map (
		clk_proc	=>	clk_proc,
		reset_n		=>	reset_n,

		-- bus_sl
		addr_rel_i	=>	addr_rel_i,
		wr_i		=>	wr_i,
		rd_i		=>	rd_i,
		datawr_i	=>	datawr_i,
		datard_o	=>	datard_o,

		-- connections to conv_process
		enable_o	=>	enable_s,
		widthimg_o	=>	widthimg_s,
		
		-- kernel value
		w11_o		=> w11s,
		w12_o		=> w12s,
		w13_o		=> w13s,
		w21_o		=> w21s,
		w22_o		=> w22s,
		w23_o		=> w23s,
		w31_o		=> w31s,
		w32_o		=> w32s,
		w33_o		=> w33s,
		norm_o		=> norms
	);

	conv_process_inst : conv_process
    generic map (
    	LINE_WIDTH_MAX	=>	LINE_WIDTH_MAX,
    	PIX_WIDTH		=>	IN_SIZE
	)
    port map (
		clk_proc	=>	clk_proc,
		reset_n		=>	reset_n,
--============================ IN FLOW =============================
		in_data		=>	in_data,
		in_fv		=>	in_fv,
		in_dv		=>	in_dv,

--============================ IN KERNEL ===========================
		w11 => w11s	,	w12 => w12s	,	w13 => w13s,	
		w21 => w21s	,	w22 => w22s	,	w23 => w23s,	
		w31 => w31s	,	w32 => w32s	,	w33 => w33s,

--============================ OUT FLOW ============================
		out_data	=>	out_data,
		out_fv		=>	out_fv,
		out_dv		=>	out_dv,
		norm		=>	norms,

--============================ PARAMS ==============================
		enable_i	=>	enable_s,
		widthimg_i	=>	widthimg_s
	);

end rtl;
