library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity conv_process is
	generic (
		LINE_WIDTH_MAX : integer;
		CLK_PROC_FREQ  : integer;
		IN_SIZE        : integer;
		OUT_SIZE       : integer;
		WEIGHT_SIZE    : integer := 8
	);
	port (
		clk_proc               : in std_logic;
		reset_n                : in std_logic;

		---------------- dynamic parameters ports ---------------
		status_reg_enable_bit  : in std_logic;
		widthimg_reg_width     : in std_logic_vector(15 downto 0);
		w00_reg_m00            : in std_logic_vector(7 downto 0);
		w01_reg_m01            : in std_logic_vector(7 downto 0);
		w02_reg_m02            : in std_logic_vector(7 downto 0);
		w10_reg_m10            : in std_logic_vector(7 downto 0);
		w11_reg_m11            : in std_logic_vector(7 downto 0);
		w12_reg_m12            : in std_logic_vector(7 downto 0);
		w20_reg_m20            : in std_logic_vector(7 downto 0);
		w21_reg_m21            : in std_logic_vector(7 downto 0);
		w22_reg_m22            : in std_logic_vector(7 downto 0);
		norm_reg_norm          : in std_logic_vector(3 downto 0);

		------------------------- in flow -----------------------
		in_data                : in std_logic_vector(IN_SIZE-1 downto 0);
		in_fv                  : in std_logic;
		in_dv                  : in std_logic;

		------------------------ out flow -----------------------
		out_data               : out std_logic_vector(OUT_SIZE-1 downto 0);
		out_fv                 : out std_logic;
		out_dv                 : out std_logic
	);
end conv_process;

architecture rtl of conv_process is

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

-- buffered kernell weights and norm
signal w00, w01, w02 : signed((IN_SIZE-1) downto 0);
signal w10, w11, w12 : signed((IN_SIZE-1) downto 0);
signal w20, w21, w22 : signed((IN_SIZE-1) downto 0);
signal norm : std_logic_vector(3 downto 0);

-- neighbors extraction
signal p00, p01, p02 : std_logic_vector((IN_SIZE-1) downto 0);
signal p10, p11, p12 : std_logic_vector((IN_SIZE-1) downto 0);
signal p20, p21, p22 : std_logic_vector((IN_SIZE-1) downto 0);
signal matrix_dv : std_logic;

-- products calculation
signal prod00, prod01, prod02 : signed((WEIGHT_SIZE + IN_SIZE) downto 0);
signal prod10, prod11, prod12 : signed((WEIGHT_SIZE + IN_SIZE) downto 0);
signal prod20, prod21, prod22 : signed((WEIGHT_SIZE + IN_SIZE) downto 0);
signal prod_dv : std_logic;

signal value_data : std_logic_vector((IN_SIZE-1) downto 0);
signal value_dv : std_logic;
signal out_fv_s : std_logic;

signal enable_s : std_logic;

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
		widthimg_i => widthimg_reg_width
    );

	process (clk_proc, reset_n, matrix_dv)
        variable sum   : signed((WEIGHT_SIZE + IN_SIZE) downto 0);

	begin
		if(reset_n='0') then
			enable_s <= '0';
            prod_dv <= '0';
            value_dv <= '0';

		elsif(rising_edge(clk_proc)) then
            if(in_fv = '0') then
                w00 <= signed(w00_reg_m00);
                w01 <= signed(w01_reg_m01);
                w02 <= signed(w02_reg_m02);
                w10 <= signed(w10_reg_m10);
                w11 <= signed(w11_reg_m11);
                w12 <= signed(w12_reg_m12);
                w20 <= signed(w20_reg_m20);
                w21 <= signed(w21_reg_m21);
                w22 <= signed(w22_reg_m22);
                norm <= norm_reg_norm;
                
                enable_s <= status_reg_enable_bit;
                prod_dv <= '0';
                value_dv <= '0';
            end if;

            -- product calculation pipeline stage
			prod_dv <= '0';
			if(matrix_dv = '1' and enable_s = '1') then
                prod00 <= w00 * signed('0' & p00);
                prod01 <= w01 * signed('0' & p01);
                prod02 <= w02 * signed('0' & p02);
                prod10 <= w10 * signed('0' & p10);
                prod11 <= w11 * signed('0' & p11);
                prod12 <= w12 * signed('0' & p12);
                prod20 <= w20 * signed('0' & p20);
                prod21 <= w21 * signed('0' & p21);
                prod22 <= w22 * signed('0' & p22);

				prod_dv <= '1';
			end if;
            
			if(prod_dv='1' and enable_s = '1') then
                sum := prod00 + prod01 + prod02 +
                       prod10 + prod11 + prod12 +
                       prod20 + prod21 + prod22;

                if (sum(sum'left) = '1') then
                    sum := (others => '0');
                end if;
                value_data <= std_logic_vector(shift_right(unsigned(sum), to_integer(unsigned(norm)))(OUT_SIZE -1 downto 0));
				value_dv <= '1';
			else
				value_dv <= '0';
			end if;
		end if;
	end process;

    out_fv <= enable_s and out_fv_s;
end rtl;
