library ieee;
use     ieee.std_logic_1164.all;
use     ieee.numeric_std.all;
use 	ieee.math_real.all;




entity debayer33_process is
generic( 	CLK_PROC_FREQ	: integer;
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
		out_fv				: out std_logic;
		out_dv				: out std_logic;
		out_data			: out std_logic_vector((COLOR_CHANNELS*DATA_SIZE)-1 downto 0);

		------------------------- for sim -------------------------
		sim_en				: in std_logic;
		bayer_code_sim		: in std_logic_vector(1 downto 0);
		test_count_x_out	: out std_logic_vector(integer(ceil(log2(real(IM_WIDTH))))-1 downto 0);
		test_count_y_out	: out std_logic_vector(integer(ceil(log2(real(IM_HEIGHT))))-1 downto 0)

	);
end entity;



architecture rtl of debayer33_process is

component div_5 is
	port
	(
		denom		: in std_logic_vector (2 downto 0);
		numer		: in std_logic_vector (10 downto 0);
		quotient	: out std_logic_vector (10 downto 0);
		remain		: out std_logic_vector (2 downto 0)
	);
end component;

----------
signal test_count_x_out_int : unsigned(integer(ceil(log2(real(IM_WIDTH))))-1 downto 0);
signal test_count_y_out_int : unsigned(integer(ceil(log2(real(IM_HEIGHT))))-1 downto 0);
type state_count_line_out_enum is(s0,s1);
signal state_count_line_out	: state_count_line_out_enum;


----------
type state_demosaic_enum is(s0,s1,s2);
signal state_demosaic : state_demosaic_enum;

type ramshift_array is array(IM_WIDTH-1 downto 0) of std_logic_vector(DATA_SIZE-1 downto 0);
signal ramshift_0, ramshift_1, ramshift_2 : ramshift_array;
--type ramshift_2_array is array(2 downto 0) of std_logic_vector(DATA_SIZE-1 downto 0);
--signal ramshift_2 : ramshift_2_array;

signal bayer_code_int, code_demosaic_int : std_logic_vector(1 downto 0);

signal line_dv_int, enable_debayer_int : std_logic;


signal pixel_count_int, line_count_int : unsigned(31 downto 0);


signal red_tmp, blue_tmp : unsigned(DATA_SIZE+1 downto 0);
signal green_tmp : unsigned(DATA_SIZE+2 downto 0);


signal enable_debayer_latched_1_int, enable_debayer_latched_2_int, line_dv_tmp : std_logic;
signal path_1_int, path_2_int : std_logic_vector(1 downto 0);
signal denom_int, rem_int, threshold_int : std_logic_vector(2 downto 0);
signal red_tmp_1_latched_int, blue_tmp_1_latched_int : unsigned(DATA_SIZE+1 downto 0);
signal red_tmp_2_latched_int, blue_tmp_2_latched_int : unsigned(DATA_SIZE-1 downto 0);
signal green_tmp_1_latched_int, green_out_tmp : unsigned(DATA_SIZE+2 downto 0);
signal green_tmp_2_latched_int : std_logic_vector(DATA_SIZE+2 downto 0);

begin


bayer_code_int <= bayer_code_slave when sim_en = '0' else bayer_code_sim;


--test count pour x et y du demosaic
test_count_x_out 	<= std_logic_vector(test_count_x_out_int);
test_count_y_out	<= std_logic_vector(test_count_y_out_int);
process(clk_proc, reset_n)
begin
if reset_n = '0' then
	test_count_x_out_int	<= (others => '0');
	test_count_y_out_int	<= (others => '0');
	state_count_line_out	<= s0;
elsif rising_edge(clk_proc) then
	if line_dv_tmp = '1' then
		test_count_x_out_int		<= test_count_x_out_int + 1;
	else
		test_count_x_out_int		<= (others => '0');
	end if;

	case state_count_line_out is

		when s0 =>
			if in_fv = '1' then
				if line_dv_tmp = '1' then
					test_count_y_out_int	<= test_count_y_out_int;
					state_count_line_out	<= s1;
				end if;
			else
				test_count_y_out_int	<= (others => '0');
				state_count_line_out	<= s0;
			end if;


		when s1 =>
			if in_fv = '1' then
				if line_dv_tmp = '0' then
					test_count_y_out_int	<= test_count_y_out_int + 1;
					state_count_line_out	<= s0;
				end if;
			else
				test_count_y_out_int	<= (others => '0');
				state_count_line_out	<= s0;
			end if;

		when others => NULL;

	end case;

end if;
end process;

--on compte les lignes
process(clk_proc,reset_n)
begin
if reset_n = '0' then
	line_count_int	<= (others => '0');
	state_demosaic	<= s0;
elsif rising_edge(clk_proc) then
	case state_demosaic is
		when s0 =>--attente
			if in_fv = '1' then
				line_count_int	<= line_count_int;
				if in_dv = '1' then
					state_demosaic	<= s1;
				else
					state_demosaic	<= s0;
				end if;
			else
				line_count_int	<= (others => '0');
				state_demosaic	<= s0;
			end if;
		when s1 =>--attente fin d'image et/ou ligne
			if in_fv = '1' then
				if in_dv = '0' then --la ligne vient de se terminer
					if line_count_int < IM_HEIGHT-1 then--on continue
						line_count_int	<= line_count_int + 1;
						state_demosaic	<= s0;
					else--image
						line_count_int	<= line_count_int;
						state_demosaic	<= s2;
					end if;
				else
					line_count_int	<= line_count_int;
					state_demosaic	<= s1;
				end if;
			end if;
		when s2 => --fin de l'image, on attend que frame valid retombe
			if in_fv = '0' then
				line_count_int	<= (others => '0');
				state_demosaic	<= s0;
			else
				line_count_int	<= line_count_int;
				state_demosaic	<= s2;
			end if;
		when others => NULL;
	end case;
end if;
end process;

--pixel pipeline for 3 lines
--evaluate when debayer should be activated
process(clk_proc,reset_n)
begin
if reset_n = '0' then

	pixel_count_int 	<= (others => '0');
	enable_debayer_int	<= '0';
	code_demosaic_int	<= (others => '0');
	ramshift_0			<= (others => (others => '0'));
	ramshift_1			<= (others => (others => '0'));
	ramshift_2			<= (others => (others => '0'));

elsif rising_edge(clk_proc) then
	if in_fv = '1' then
		if in_dv = '1' then
			ramshift_0	<= ramshift_1(0) & ramshift_0(IM_WIDTH-1 downto 1);-- a changer si la taille du kernel change
			ramshift_1	<= ramshift_2(0) & ramshift_1(IM_WIDTH-1 downto 1);
			ramshift_2	<= in_data & ramshift_2(IM_WIDTH-1 downto 1);--ramshift_2(2 downto 1);
		end if;
	end if;

	if in_fv = '1' then
		if in_dv = '1' then
			pixel_count_int <= pixel_count_int + 1;
		else
			pixel_count_int	<= (others => '0');
		end if;
	else
		pixel_count_int	<= (others => '0');
	end if;

	if line_count_int > 1 and line_count_int <= IM_HEIGHT-1 then
		if pixel_count_int > 1  and pixel_count_int <= IM_WIDTH-1 then
			enable_debayer_int	<= '1';
			code_demosaic_int 	<= line_count_int(0) & pixel_count_int(0);
		else
			enable_debayer_int	<= '0';
			code_demosaic_int 	<= (others => '0');
		end if;
	else
		enable_debayer_int 	<= '0';
		code_demosaic_int 	<= (others => '0');
	end if;



end if;
end process;


--debayering
process(clk_proc, reset_n)
begin
if reset_n  = '0' then
	enable_debayer_latched_1_int	<= '0';
	path_1_int					<= (others => '0');
	red_tmp						<= (others => '0');
	green_tmp					<= (others => '0');
	blue_tmp					<= (others => '0');
elsif rising_edge(clk_proc) then
	if enable_debayer_int = '1' then
		enable_debayer_latched_1_int	<= '1';
		path_1_int					<= code_demosaic_int;
		if code_demosaic_int =  bayer_code_int(1)&bayer_code_int(0) then --"00"
			red_tmp			<= ("00"&unsigned(ramshift_0(IM_WIDTH-2))) + ("00"&unsigned(ramshift_2(IM_WIDTH-2)));
			green_tmp 		<= ("000"&unsigned(ramshift_0(IM_WIDTH-1))) + ("000"&unsigned(ramshift_0(IM_WIDTH-3))) + ("000"&unsigned(ramshift_1(IM_WIDTH-2))) + ("000"&unsigned(ramshift_2(IM_WIDTH-1))) + ("000"&unsigned(ramshift_2(IM_WIDTH-3)));
			blue_tmp 		<= ("00"&unsigned(ramshift_1(IM_WIDTH-1))) + ("00"&unsigned(ramshift_1(IM_WIDTH-3)));
		elsif code_demosaic_int = bayer_code_int(1)&not(bayer_code_int(0)) then --"01"
			red_tmp			<= ("00"&unsigned(ramshift_0(IM_WIDTH-1))) + ("00"&unsigned(ramshift_0(IM_WIDTH-3))) + ("00"&unsigned(ramshift_2(IM_WIDTH-1))) + ("00"&unsigned(ramshift_2(IM_WIDTH-3)));
			green_tmp 		<= ("000"&unsigned(ramshift_0(IM_WIDTH-2))) + ("000"&unsigned(ramshift_1(IM_WIDTH-1))) + ("000"&unsigned(ramshift_1(IM_WIDTH-3))) + ("000"&unsigned(ramshift_2(IM_WIDTH-2)));
			blue_tmp 		<= ("00"&unsigned(ramshift_1(IM_WIDTH-2)));
		elsif code_demosaic_int =  not(bayer_code_int(1))& bayer_code_int(0) then --"01" then
			red_tmp			<= ("00"&unsigned(ramshift_1(IM_WIDTH-2)));
			green_tmp 		<= ("000"&unsigned(ramshift_0(IM_WIDTH-2))) + ("000"&unsigned(ramshift_1(IM_WIDTH-1))) + ("000"&unsigned(ramshift_1(IM_WIDTH-3))) + ("000"&unsigned(ramshift_2(IM_WIDTH-2)));
			blue_tmp 		<= ("00"&unsigned(ramshift_0(IM_WIDTH-2))) + ("00"&unsigned(ramshift_1(IM_WIDTH-1))) + ("00"&unsigned(ramshift_1(IM_WIDTH-3))) + ("00"&unsigned(ramshift_2(IM_WIDTH-2)));
		else
			red_tmp			<= ("00"&unsigned(ramshift_1(IM_WIDTH-1))) + ("00"&unsigned(ramshift_1(IM_WIDTH-3)));
			green_tmp 		<= ("000"&unsigned(ramshift_0(IM_WIDTH-1))) + ("000"&unsigned(ramshift_0(IM_WIDTH-3))) + ("000"&unsigned(ramshift_1(IM_WIDTH-2))) + ("000"&unsigned(ramshift_2(IM_WIDTH-1))) + ("000"&unsigned(ramshift_2(IM_WIDTH-3)));
			blue_tmp 		<= ("00"&unsigned(ramshift_0(IM_WIDTH-2))) + ("00"&unsigned(ramshift_2(IM_WIDTH-2)));
		end if;
	else
		enable_debayer_latched_1_int	<= '0';
		path_1_int					<= (others => '0');
		red_tmp						<= (others => '0');
		green_tmp					<= (others => '0');
		blue_tmp					<= (others => '0');
	end if;


end if;
end process;

--process feed divider
process(clk_proc, reset_n)
begin
if reset_n = '0' then

	enable_debayer_latched_2_int	<= '0';
	path_2_int						<= (others => '0');
	red_tmp_1_latched_int			<= (others => '0');
	green_tmp_1_latched_int			<= (others => '0');
	denom_int						<= "001";
	blue_tmp_1_latched_int			<= (others => '0');

elsif rising_edge(clk_proc) then
	if enable_debayer_latched_1_int	= '1' then
		enable_debayer_latched_2_int	<= '1';
		path_2_int						<= path_1_int;
		red_tmp_1_latched_int			<= red_tmp;
		green_tmp_1_latched_int			<= green_tmp;
		blue_tmp_1_latched_int			<= blue_tmp;
		if path_1_int = "00" or path_1_int = "11" then
			denom_int		<= "101";
		else
			denom_int		<= "100";
		end if;
	else
		enable_debayer_latched_2_int	<= '0';
		path_2_int						<= (others => '0');
		red_tmp_1_latched_int			<= (others => '0');
		green_tmp_1_latched_int			<= (others => '0');
		denom_int						<= (others => '0');
		blue_tmp_1_latched_int			<= (others => '0');
	end if;
end if;
end process;


-- compute pixels
div_5_inst : div_5 PORT MAP (
		denom	 => denom_int,-- a changer si la taille du kernel change
		numer	 => std_logic_vector(green_tmp_1_latched_int),
		quotient => green_tmp_2_latched_int,
		remain	 => rem_int
	);

threshold_int <= "011";
--cut process
process(clk_proc, reset_n)
begin
if reset_n = '0' then

	red_tmp_2_latched_int		<= (others => '0');
	green_out_tmp				<= (others => '0');
	blue_tmp_2_latched_int		<= (others => '0');
	line_dv_tmp					<= '0';

elsif rising_edge(clk_proc) then

	if enable_debayer_latched_2_int = '1' then
		line_dv_tmp	<= '1';
		if path_2_int = "00" then
			red_tmp_2_latched_int	<= red_tmp_1_latched_int(DATA_SIZE downto 1);
			blue_tmp_2_latched_int	<= blue_tmp_1_latched_int(DATA_SIZE downto 1);
			if rem_int > threshold_int then
				green_out_tmp	<= unsigned(green_tmp_2_latched_int) + "00000000001";
			else
				green_out_tmp	<= unsigned(green_tmp_2_latched_int);
			end if;
		elsif path_2_int = "01" then
			red_tmp_2_latched_int	<= red_tmp_1_latched_int(DATA_SIZE+1 downto 2);
			blue_tmp_2_latched_int	<= blue_tmp_1_latched_int(DATA_SIZE downto 1);
			green_out_tmp			<= unsigned(green_tmp_2_latched_int);
		elsif path_2_int = "10" then
			red_tmp_2_latched_int	<= red_tmp_1_latched_int(DATA_SIZE-1 downto 0);
			blue_tmp_2_latched_int	<= blue_tmp_1_latched_int(DATA_SIZE+1 downto 2);
			green_out_tmp			<= unsigned(green_tmp_2_latched_int);
		else--"11"
			red_tmp_2_latched_int	<= red_tmp_1_latched_int(DATA_SIZE downto 1);
			blue_tmp_2_latched_int	<= blue_tmp_1_latched_int(DATA_SIZE downto 1);
			if rem_int > threshold_int then
				green_out_tmp	<= unsigned(green_tmp_2_latched_int) + "00000000001";
			else
				green_out_tmp	<= unsigned(green_tmp_2_latched_int);
			end if;
		end if;
	else
		red_tmp_2_latched_int		<= (others => '0');
		green_out_tmp				<= (others => '0');
		blue_tmp_2_latched_int		<= (others => '0');
		line_dv_tmp					<= '0';
	end if;
end if;
end process;

--final process
process(clk_proc, reset_n)
begin
if reset_n = '0' then
	out_dv 	<= '0';
	out_data	<= (others => '0');
elsif rising_edge(clk_proc) then
	if line_dv_tmp = '1' then
		out_dv 	<= '1';
		out_data((COLOR_CHANNELS*DATA_SIZE)-1 downto ((COLOR_CHANNELS-1)*DATA_SIZE)) 		<= std_logic_vector(red_tmp_2_latched_int);
		out_data(((COLOR_CHANNELS-1)*DATA_SIZE)-1 downto ((COLOR_CHANNELS-2)*DATA_SIZE)) 	<= std_logic_vector(green_out_tmp(DATA_SIZE-1 downto 0));
		out_data(((COLOR_CHANNELS-2)*DATA_SIZE)-1 downto ((COLOR_CHANNELS-3)*DATA_SIZE)) 	<= std_logic_vector(blue_tmp_2_latched_int);
		-- red_plane	<= std_logic_vector(red_tmp_2_latched_int);
		-- green_plane	<= std_logic_vector(green_out_tmp(DATA_SIZE-1 downto 0));
		-- blue_plane	<= std_logic_vector(blue_tmp_2_latched_int);
	else
		out_dv 		<= '0';
		out_data	<= (others => '0');
		-- red_plane	<= (others => '0');
		-- green_plane	<= (others => '0');
		-- blue_plane	<= (others => '0');
	end if;
end if;
end process;


out_fv <= in_fv;

end architecture;
