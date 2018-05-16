-- Author     : K. Abdelouahab
-- Company    : DREAM - Institut Pascal - Unviersite Clermont Auvergne

library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;
    use ieee.math_real.all;

entity kernel_3x3 is
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
end kernel_3x3;


architecture BHV of kernel_3x3 is

    signal    p11u,p12u,p13u        : signed (PIX_WIDTH downto 0);
    signal    p21u,p22u,p23u        : signed (PIX_WIDTH downto 0);
    signal    p31u,p32u,p33u        : signed (PIX_WIDTH downto 0);
    signal    ker11u,ker12u,ker13u  : signed (PIX_WIDTH downto 0);
    signal    ker21u,ker22u,ker23u  : signed (PIX_WIDTH downto 0);
    signal    ker31u,ker32u,ker33u  : signed (PIX_WIDTH downto 0);
    signal    normi                 : integer range 0 to 8;

    begin

    p11u     <=  signed( '0' & (p11));
    p12u     <=  signed( '0' & (p12));
    p13u     <=  signed( '0' & (p13));
    p21u     <=  signed( '0' & (p21));
    p22u     <=  signed( '0' & (p22));
    p23u     <=  signed( '0' & (p23));
    p31u     <=  signed( '0' & (p31));
    p32u     <=  signed( '0' & (p32));
    p33u     <=  signed( '0' & (p33));

    ker11u     <=  signed(ker11(ker11'LEFT) & (ker11));
    ker12u     <=  signed(ker12(ker12'LEFT) & (ker12));
    ker13u     <=  signed(ker13(ker13'LEFT) & (ker13));
    ker21u     <=  signed(ker21(ker21'LEFT) & (ker21));
    ker22u     <=  signed(ker22(ker22'LEFT) & (ker22));
    ker23u     <=  signed(ker23(ker23'LEFT) & (ker23));
    ker31u     <=  signed(ker31(ker31'LEFT) & (ker31));
    ker32u     <=  signed(ker32(ker32'LEFT) & (ker32));
    ker33u     <=  signed(ker33(ker33'LEFT) & (ker33));

    process    (clk_proc)

        variable    m11    :     signed (PIX_WIDTH + PIX_WIDTH + 1  downto 0);
        variable    m12    :     signed (PIX_WIDTH + PIX_WIDTH + 1  downto 0);
        variable    m13    :     signed (PIX_WIDTH + PIX_WIDTH + 1  downto 0);
        variable    m21    :     signed (PIX_WIDTH + PIX_WIDTH + 1  downto 0);
        variable    m22    :     signed (PIX_WIDTH + PIX_WIDTH + 1  downto 0);
        variable    m23    :     signed (PIX_WIDTH + PIX_WIDTH + 1  downto 0);
        variable    m31    :     signed (PIX_WIDTH + PIX_WIDTH + 1  downto 0);
        variable    m32    :     signed (PIX_WIDTH + PIX_WIDTH + 1  downto 0);
        variable    m33    :     signed (PIX_WIDTH + PIX_WIDTH + 1  downto 0);
        variable    sum    :     signed (PIX_WIDTH + PIX_WIDTH + 1  downto 0);
        variable    res    :     signed (PIX_WIDTH + PIX_WIDTH + 1  downto 0);

    begin

      if (reset_n = '0')     then
              sum :=    (others => '0');

      elsif    (RISING_EDGE(clk_proc)) then
          if ((in_fv and in_dv and enable_i) = '1') then

              m11    :=    ker11u * p11u  ;
              m12    :=    ker12u * p12u  ;
              m13    :=    ker13u * p13u  ;
              m21    :=    ker21u * p21u  ;
              m22    :=    ker22u * p22u  ;
              m23    :=    ker23u * p23u  ;
              m31    :=    ker31u * p31u  ;
              m32    :=    ker32u * p32u  ;
              m33    :=    ker33u * p33u  ;

              sum := m11 + m12 + m13 + m21 + m22 + m23 + m31 + m32 + m33;

              if (sum(sum'left) = '1')    then
                  sum := (others => '0');
              end if;

              normi    <=  to_integer (unsigned(norm));
              res        :=    sum srl normi;
              out_data <= std_logic_vector (res(PIX_WIDTH -1  downto 0));

            end if;
            out_dv    <= in_dv;
            out_fv    <= in_fv;
        end if;
    end process;
end BHV;
