-- Author     : K. Abdelouahab
-- Company    : DREAM - Institut Pascal - Unviersite Clermont Auvergne

library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;

entity conv_process is
generic (
    LINE_WIDTH_MAX     : integer;
    PIX_WIDTH     : integer
);
port(
    clk_proc    : in  std_logic;
    reset_n     : in  std_logic;
    in_data     : in  std_logic_vector((PIX_WIDTH-1) downto 0);
    in_fv       : in  std_logic;
    in_dv       : in  std_logic;
    w11,w12,w13 : in  std_logic_vector ((PIX_WIDTH-1) downto 0);
    w21,w22,w23 : in  std_logic_vector ((PIX_WIDTH-1) downto 0);
    w31,w32,w33 : in  std_logic_vector ((PIX_WIDTH-1) downto 0);
    norm        : in  std_logic_vector ((PIX_WIDTH-1) downto 0);
    enable_i    : in  std_logic;
    widthimg_i  : in  std_logic_vector(15 downto 0);
    out_data    : out std_logic_vector (PIX_WIDTH-1 downto 0);
    out_fv      : out std_logic;
    out_dv      : out std_logic
    );
end conv_process;


architecture DataFlow of conv_process is
    signal i00, i01, i02 : std_logic_vector (PIX_WIDTH-1 downto 0);
    signal i10, i11, i12 : std_logic_vector (PIX_WIDTH-1 downto 0);
    signal i20, i21, i22 : std_logic_vector (PIX_WIDTH-1 downto 0);
    signal out_dvp       : std_logic;
    signal out_fvp       : std_logic;

    component kernel_3x3
    generic(
        PIX_WIDTH :  integer
    );
    port (
        reset_n           : in  std_logic;
        clk_proc          : in  std_logic;
        in_dv             : in  std_logic;
        in_dv             : in  std_logic;
        enable_i          : in  std_logic;
        p11,p12,p13       : in  std_logic_vector(PIX_WIDTH-1 downto 0);
        p21,p22,p23       : in  std_logic_vector(PIX_WIDTH-1 downto 0);
        p31,p32,p33       : in  std_logic_vector(PIX_WIDTH-1 downto 0);
        ker11,ker12,ker13 : in  std_logic_vector(PIX_WIDTH-1 downto 0);
        ker21,ker22,ker23 : in  std_logic_vector(PIX_WIDTH-1 downto 0);
        ker31,ker32,ker33 : in  std_logic_vector(PIX_WIDTH-1 downto 0);
        norm              : in  std_logic_vector(PIX_WIDTH-1 downto 0);
        out_data          : out std_logic_vector(PIX_WIDTH-1 downto 0);
        out_dv            : out std_logic
        out_fv            : out std_logic
        );
    end component;

    component pipliner_3x3
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
    end component;

    ------------------------------------------------------------------------------
    begin
    inst_pipliner    :    pipliner_3x3
        generic map(
        LINE_WIDTH_MAX => LINE_WIDTH_MAX,
        PIX_WIDTH => PIX_WIDTH
        )

        port map (
        clk_proc   => clk_proc,
        reset_n    => reset_n,
        in_data    => in_data,
        in_fv      => in_fv,
        in_dv      => in_dv,
        out_fv     => out_fvp,
        out_dv     => out_dvp,
        enable_i   => enable_i ,
        widthimg_i => widthimg_i,
        p00 => i00, p01 => i01, p02 => i02,
        p10 => i10, p11 => i11, p12 => i12,
        p20 => i20, p21 => i21, p22 => i22
        );


    inst_ker        :    kernel_3x3
        generic map(
        PIX_WIDTH => PIX_WIDTH
        )
        port map(
        clk_proc => clk_proc,
        reset_n  => reset_n,
        in_fv    => out_fvp,
        in_dv    => out_dvp,
        enable_i => enable_i,
        norm     => norm,
        out_data => out_data,
        out_fv   => out_fv,
        out_dv   => out_dv,
        p11   => i00, p12   => i01, p13   => i02,
        p21   => i10, p22   => i11, p23   => i12,
        p31   => i20, p32   => i21, p33   => i22,
        ker11 => w11, ker12 => w12, ker13 => w13,
        ker21 => w21, ker22 => w22, ker23 => w23,
        ker31 => w31, ker32 => w32, ker33 => w33
        );

    end DataFlow;
