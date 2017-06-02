
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;


entity harris is
	generic (
		LINE_WIDTH_MAX 	: integer	:=	320;
		IN_SIZE 		: integer	:=	8;
		OUT_SIZE 		: integer	:=	8;
		CLK_PROC_FREQ 	: integer 	:=  48000000
	);
	port (
		clk_proc 	: in std_logic;
		reset_n 	: in std_logic;

		in_data 	: in std_logic_vector((IN_SIZE-1) downto 0);
		in_fv 		: in std_logic;
		in_dv 		: in std_logic;

		out_data 	: out std_logic_vector((OUT_SIZE-1) downto 0);
		out_fv 		: out std_logic;
		out_dv 		: out std_logic;

		addr_rel_i 	: in std_logic_vector(3 downto 0);
		wr_i 		: in std_logic;
		rd_i 		: in std_logic;
		datawr_i 	: in std_logic_vector(31 downto 0);
		datard_o 	: out std_logic_vector(31 downto 0)
	);
end harris;

architecture rtl of harris is

component harris_slave
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
end component;

component harris_process
	generic (
		LINE_WIDTH_MAX	: integer;
		PIX_WIDTH 		: integer
	);
	port (
		clk_proc	: in std_logic;
		reset_n		: in std_logic;
	
		in_data		: in std_logic_vector((PIX_WIDTH-1) downto 0);
		in_fv		: in std_logic;
		in_dv		: in std_logic;
		
		out_data	: out std_logic_vector((PIX_WIDTH-1) downto 0);
		out_fv		: out std_logic;
		out_dv		: out std_logic;	
		
		enable_i	: in std_logic;
		widthimg_i	: in std_logic_vector(15 downto 0)
	);
end component;

	signal 	enable_s    : std_logic;
	signal 	widthimg_s 	: std_logic_vector(15 downto 0);
	
begin

	harris_slave_inst : harris_slave
    port map (
		clk_proc	=>	clk_proc,
		reset_n		=>	reset_n,

		addr_rel_i	=>	addr_rel_i,
		wr_i		=>	wr_i,
		rd_i		=>	rd_i,
		datawr_i	=>	datawr_i,
		datard_o	=>	datard_o,
	
		enable_o	=>	enable_s,
		widthimg_o	=>	widthimg_s
	);	

	harris_process_inst : harris_process
    generic map (
    	LINE_WIDTH_MAX	=>	LINE_WIDTH_MAX,
    	PIX_WIDTH		=>	IN_SIZE
	)
    port map (
		clk_proc	=>	clk_proc,
		reset_n		=>	reset_n,

		in_data		=>	in_data,
		in_fv		=>	in_fv,
		in_dv		=>	in_dv,

		out_data	=>	out_data,
		out_fv		=>	out_fv,
		out_dv		=>	out_dv,

		enable_i	=>	enable_s,
		widthimg_i	=>	widthimg_s
	);

end rtl;
