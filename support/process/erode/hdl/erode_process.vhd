library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity erode_process is
	generic (
		LINE_WIDTH_MAX : integer;
		CLK_PROC_FREQ  : integer;
		IN_SIZE        : integer;
		OUT_SIZE       : integer
	);
	port (
		clk_proc              : in std_logic;
		reset_n               : in std_logic;

		---------------- dynamic parameters ports ---------------
		status_reg_enable_bit : in std_logic;
		widthimg_reg_value    : in std_logic_vector(15 downto 0);
		heigtimg_reg_value    : in std_logic_vector(15 downto 0);
		er00_reg_m00          : in std_logic_vector(7 downto 0);
		er01_reg_m01          : in std_logic_vector(7 downto 0);
		er02_reg_m02          : in std_logic_vector(7 downto 0);
		er10_reg_m10          : in std_logic_vector(7 downto 0);
		er11_reg_m11          : in std_logic_vector(7 downto 0);
		er12_reg_m12          : in std_logic_vector(7 downto 0);
		er20_reg_m20          : in std_logic_vector(7 downto 0);
		er21_reg_m21          : in std_logic_vector(7 downto 0);
		er22_reg_m22          : in std_logic_vector(7 downto 0);

		------------------------- in flow -----------------------
		in_data               : in std_logic_vector(IN_SIZE-1 downto 0);
		in_fv                 : in std_logic;
		in_dv                 : in std_logic;

		------------------------ out flow -----------------------
		out_data              : out std_logic_vector(OUT_SIZE-1 downto 0);
		out_fv                : out std_logic;
		out_dv                : out std_logic
	);
end erode_process;

architecture rtl of erode_process is

component matrix_extractor
	generic (
		LINE_WIDTH_MAX : integer;
		PIX_WIDTH : integer;
		OUTVALUE_WIDTH : integer
	);
	port (
		clk_proc : in std_logic;
		reset_n : in std_logic;

		------------------------- in flow -----------------------
		in_data : in std_logic_vector((PIX_WIDTH-1) downto 0);
		in_fv : in std_logic;
		in_dv : in std_logic;

		------------------------ out flow -----------------------
		out_data : out std_logic_vector((PIX_WIDTH-1) downto 0);
		out_fv : out std_logic;
		out_dv : out std_logic;
		
		------------------------ matrix out ---------------------
		p00, p01, p02 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		p10, p11, p12 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		p20, p21, p22 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		matrix_dv : out std_logic;
		
		---------------------- computed value -------------------
		value_data : in std_logic_vector((PIX_WIDTH-1) downto 0);
		value_dv : in std_logic;

		------------------------- params ------------------------
		enable_i : in std_logic;
		widthimg_i : in std_logic_vector(15 downto 0)
	);
end component;

-- neighbors extraction
signal p00, p01, p02 : std_logic_vector((IN_SIZE-1) downto 0);
signal p10, p11, p12 : std_logic_vector((IN_SIZE-1) downto 0);
signal p20, p21, p22 : std_logic_vector((IN_SIZE-1) downto 0);
signal matrix_dv : std_logic;

-- Erosion matrix
-- 0 1 0	e00 e01 e02
-- 1 1 1	e10 e11 e12
-- 0 1 0	e20 e21 e22
signal e00, e01, e02 : unsigned((IN_SIZE-1) downto 0);
signal e10, e11, e12 : unsigned((IN_SIZE-1) downto 0);
signal e20, e21, e22 : unsigned((IN_SIZE-1) downto 0);


-- parsing back from value to flow
signal value_data : std_logic_vector((IN_SIZE-1) downto 0);
signal value_dv : std_logic;
signal out_fv_s : std_logic;
signal enable_s : std_logic;
signal erode_dv  : std_logic;


begin
	matrix_extractor_inst : matrix_extractor
    generic map (
    	LINE_WIDTH_MAX		=> LINE_WIDTH_MAX,
    	PIX_WIDTH			=> IN_SIZE,
    	OUTVALUE_WIDTH		=> IN_SIZE
	)
    port map (
		clk_proc => clk_proc,
		reset_n => reset_n,
		in_data => in_data,
		in_fv => in_fv,
		in_dv => in_dv,
		p00 => p00, p01 => p01, p02 => p02,
		p10 => p10, p11 => p11, p12 => p12,
		p20 => p20, p21 => p21, p22 => p22,
		matrix_dv => matrix_dv,
		value_data => value_data,
		value_dv => value_dv,
		out_data => out_data,
		out_fv => out_fv_s,
		out_dv => out_dv,
		enable_i => status_reg_enable_bit,
		widthimg_i => widthimg_reg_value
    );



	data_process : process (clk_proc, reset_n, matrix_dv)
        variable val   : unsigned((IN_SIZE-1) downto 0);

	begin
		if(reset_n='0') then
			enable_s <= '0';
			erode_dv <= '0';
            		value_dv <= '0';
		elsif(rising_edge(clk_proc)) then
			-- Waiting for an image flow
			if(in_fv = '0') then
				-- Update params here, between two images processing
				enable_s <= status_reg_enable_bit;

				e00 <= unsigned(er00_reg_m00);
				e01 <= unsigned(er01_reg_m01);
				e02 <= unsigned(er02_reg_m02);

				e10 <= unsigned(er10_reg_m10);
				e11 <= unsigned(er11_reg_m11);
				e12 <= unsigned(er12_reg_m12);

				e20 <= unsigned(er20_reg_m20);
				e21 <= unsigned(er21_reg_m21);
				e22 <= unsigned(er22_reg_m22);

				value_dv <= '0';
            		end if;
			-- A matrix is available to process
			erode_dv <= '0';
			if(matrix_dv = '1' and enable_s = '1') then
				val := (others => '1');
				--
				if(e00 /=0) then
					if(val>unsigned(p00)) then
						val := unsigned(p00);
					end if;
				end if;
				--
				if(e01 /=0) then
					if(val>unsigned(p01)) then
						val := unsigned(p01);
					end if;
				end if;
				--
				if(e02 /=0) then
					if(val>unsigned(p02)) then
						val := unsigned(p02);
					end if;
				end if;


				--
				if(e10 /=0) then
					if(val>unsigned(p10)) then
						val := unsigned(p10);
					end if;
				end if;
				--
				if(e11 /=0) then
					if(val>unsigned(p11)) then
						val := unsigned(p11);
					end if;
				end if;
				--
				if(e12 /=0) then
					if(val>unsigned(p12)) then
						val := unsigned(p12);
					end if;
				end if;


				--
				if(e20 /=0) then
					if(val>unsigned(p20)) then
						val := unsigned(p20);
					end if;
				end if;
				--
				if(e21 /=0) then
					if(val>unsigned(p21)) then
						val := unsigned(p21);
					end if;
				end if;
				--
				if(e22 /=0) then
					if(val>unsigned(p22)) then
						val := unsigned(p22);
					end if;
				end if;

				erode_dv <= '1';
			end if;
			-- Matrix process has ended
			if(enable_s = '1' and erode_dv = '1') then
				value_data <= std_logic_vector(val)(OUT_SIZE -1 downto 0);
				value_dv <= '1';
			else
				value_dv <= '0';
			end if;
		end if;
	end process;
	out_fv <= enable_s and out_fv_s;
end rtl;
