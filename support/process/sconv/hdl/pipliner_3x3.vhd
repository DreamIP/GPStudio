-- Author     : K. Abdelouahab
-- Company    : DREAM - Institut Pascal - Unviersite Clermont Auvergne

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;


entity pipliner_3x3 is
    generic (
        LINE_WIDTH_MAX  : integer;
        PIX_WIDTH       : integer
    );
        port (
        clk_proc        : in  std_logic;
        reset_n         : in  std_logic;
				enable_i        : in  std_logic;
				widthimg_i      : in  std_logic_vector(15 downto 0);
        in_data         : in  std_logic_vector((PIX_WIDTH-1) downto 0);
        in_fv           : in  std_logic;
        in_dv           : in  std_logic;
        out_fv          : out std_logic;
        out_dv          : out std_logic;
        p00, p01, p02   : out std_logic_vector((PIX_WIDTH-1) downto 0);
        p10, p11, p12   : out std_logic_vector((PIX_WIDTH-1) downto 0);
        p20, p21, p22   : out std_logic_vector((PIX_WIDTH-1) downto 0)
    );
end pipliner_3x3;



architecture structural of pipliner_3x3 is

    signal line0_pix_out        : std_logic_vector((PIX_WIDTH-1) downto 0);
    signal line1_pix_out        : std_logic_vector((PIX_WIDTH-1) downto 0);
    signal p00_s, p01_s, p02_s  : std_logic_vector((PIX_WIDTH-1) downto 0);
    signal p10_s, p11_s, p12_s  : std_logic_vector((PIX_WIDTH-1) downto 0);
    signal p20_s, p21_s, p22_s  : std_logic_vector((PIX_WIDTH-1) downto 0);
    signal all_valid,fvs,dvs    : std_logic;
    signal enable_reg           : std_logic;
    signal reset_st,reset_s     : std_logic;

    component pipline_gen
		generic (
        PIPLINE_LENGHT : integer;
        WORD_SIZE : integer
    );
    port (
        clk_proc :    in  std_logic;
        reset_n  :    in  std_logic;
        e        :    in  std_logic;
        in_data  :    in  std_logic_vector (WORD_SIZE-1 downto 0);
        i0,i1,i2 :    out std_logic_vector (WORD_SIZE-1 downto 0);
        out_data :    out std_logic_vector (WORD_SIZE-1 downto 0)
    );
    end component;

    begin
    enable_proc    :    process(enable_i,in_fv)
    begin
        if (enable_i = '0') then
            reset_st    <= '0';
        elsif(rising_edge(in_fv)) then
            reset_st    <= '1';
        end if;
    end process;

    all_valid <= in_dv and in_fv;
    reset_s   <= reset_st and reset_n;

    data_pipline0 : pipline_gen
        generic map (
        PIPLINE_LENGHT => LINE_WIDTH_MAX-1,
        WORD_SIZE => PIX_WIDTH
        )
        port map (
        clk_proc =>  clk_proc,
        reset_n  =>  reset_s,
        e        =>  all_valid,
        in_data  =>  in_data,
        i0       =>  p22,
        i1       =>  p21,
        i2       =>  p20,
        out_data =>  line0_pix_out
    );

    data_pipline1 : pipline_gen
        generic map (
        PIPLINE_LENGHT => LINE_WIDTH_MAX-1,
        WORD_SIZE => PIX_WIDTH
        )
        port map (
        clk_proc => clk_proc,
        reset_n  => reset_s,
        e        => all_valid,
        in_data  => line0_pix_out,
        i0       => p12,
        i1       => p11,
        i2       => p10,
        out_data => line1_pix_out
        );

    data_pipline2 : pipline_gen
        generic map (
        PIPLINE_LENGHT => 3,
        WORD_SIZE => PIX_WIDTH
        )
        port map (
        clk_proc => clk_proc,
        reset_n  => reset_s,
        e        => all_valid,
        in_data  => line1_pix_out,
        i0       => p02,
        i1       => p01,
        i2       => p00
    );

    fv_pipline : pipline_gen
     generic map (
        PIPLINE_LENGHT => LINE_WIDTH_MAX + LINE_WIDTH_MAX + 4 ,
        WORD_SIZE => 1
        )
        port map (
        clk_proc    => clk_proc,
        reset_n     => reset_s,
        e           => all_valid,
        in_data(0)  => in_fv,
        out_data(0) => fvs
        );

    dv_pipline : pipline_gen
     generic map (
        PIPLINE_LENGHT => LINE_WIDTH_MAX + LINE_WIDTH_MAX +4 ,
        WORD_SIZE => 1
        )
        port map (
        clk_proc    => clk_proc,
        reset_n     => reset_s,
        e           => all_valid,
        in_data(0)  => in_dv,
        out_data(0) => dvs
        );

    out_dv    <= dvs and in_dv;
    out_fv    <= fvs and in_fv;

    end structural;
