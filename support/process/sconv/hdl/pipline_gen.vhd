-- Author     : K. Abdelouahab
-- Company    : DREAM - Institut Pascal - Unviersite Clermont Auvergne

library ieee;
use ieee.std_logic_1164.all;

entity pipline_gen  is
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
    end pipline_gen;

    architecture bhv of pipline_gen is

    type cell_t is array (0 to (PIPLINE_LENGHT-1)) of std_logic_vector ( (WORD_SIZE-1) downto 0);
    signal cell    :    cell_t;

    begin
    process(clk_proc)
    variable i        :    integer := 0;
        begin

        if ( reset_n = '0' )    then
            cell <= (others =>(others => '0'));

        elsif    (rising_edge(clk_proc))    then
            if    (e='1') then
                cell(0)     <=     in_data;
                for i in 1 to  (PIPLINE_LENGHT-1) loop
                    cell(i)    <=    cell(i-1);
                end loop;

                out_data<=    cell(PIPLINE_LENGHT - 1);
                i0    <=    cell(0);
                i1    <=    cell(1);
                i2    <=    cell(2);
            end if;
        end if;
end process;
end bhv;
