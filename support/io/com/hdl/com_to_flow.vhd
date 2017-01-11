-- This block extracts the GPStudio header and gets flags Start of Frame and End of Frame.
-- It outputs a flow depending on this header (generate Flow valid and Data valid according to the flags).

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;
use work.com_package.all;

entity com_to_flow is
    generic (
        ID_FIFO         : std_logic_vector(5 downto 0):="000001";
        FLOW_OUT_SIZE   : integer := 8;
        DATA_WIDTH      : integer := 8;
        FIFO_DEPTH      : integer := 2048
    );
    port (
        clk_hal         : in std_logic;
        clk_proc        : in std_logic;
        reset_n         : in std_logic;
        enable          : in std_logic;
        flow_out_data   : out std_logic_vector(FLOW_OUT_SIZE-1 downto 0);
        flow_out_fv     : out std_logic;
        flow_out_dv     : out std_logic;
        write_data      : in std_logic;
        data_in         : in std_logic_vector(7 downto 0)
    );
end com_to_flow;

architecture RTL of com_to_flow is

    type fsm_com_to_flow is (idle, header, wr_data_st);
    signal state                : fsm_com_to_flow;
    signal reset                : std_logic;
    signal empty_fifo           : std_logic;
    signal empty_fifo_dl        : std_logic;
    signal wrreq                : std_logic;
    signal set_wrreq            : std_logic;
    signal rdreq,rdreq_dl       : std_logic;
    signal sof                  : std_logic;
    signal eof                  : std_logic;
    signal write_data_dl        : std_logic;
    signal id_detected          : std_logic_vector(5 downto 0);

begin

reset       <= not reset_n;

--- Fifo that synchronizes data of the flow on clk_proc
fifo_data_inst : entity work.gp_dcfifo
    generic map (DATA_WIDTH  => 8,
                 FIFO_DEPTH  => FIFO_DEPTH)
    port map(
        aclr        => reset,
        data        => data_in,
        rdclk       => clk_proc,
        rdreq       => rdreq,
        wrclk       => clk_hal,
        wrreq       => wrreq,
        q           => flow_out_data,
        rdempty     => empty_fifo
    );


--- Filter and remove GPStudio header to get flags and write only the data in the FIFO
process(clk_hal, reset_n)
begin
    if reset_n='0' then
        sof             <= '0';
        eof             <= '0';
        id_detected     <= "000000";

    elsif clk_hal'event and clk_hal='1' then
        write_data_dl       <= write_data;

        case(state) is

            when idle =>
                set_wrreq       <= '0';
                if write_data='1' and write_data_dl='0' then
                    sof             <= data_in(1);
                    eof             <= data_in(0);
                    id_detected     <= data_in(7 downto 2);
                    state           <= header;
                end if;

            when header =>
                if write_data='1' then
                    if id_detected=ID_FIFO then
                        state       <= wr_data_st;
                        set_wrreq   <= '1';
                    else
                        state       <= idle;
                        set_wrreq   <= '0';
                    end if;
                end if;

            when wr_data_st =>
                if write_data='0' and write_data_dl='0' then
                    state           <= idle;
                end if;

            when others =>
                state               <= idle;
        end case;
    end if;

end process;

wrreq       <=  set_wrreq when state=idle or state=header else
                write_data;

--- Set data valid, flow valid and control the read request of the fifo
process(clk_proc, reset_n)
begin
    if reset_n='0' then
        rdreq           <= '0';
        flow_out_fv     <= '0';

    elsif clk_proc'event and clk_proc='1' then
        rdreq           <= not empty_fifo;
        rdreq_dl        <= rdreq;
        empty_fifo_dl   <= empty_fifo;
        if rdreq='1' and rdreq_dl='0' then
            if sof='1' then
                flow_out_fv     <= '1';
            end if;
        elsif empty_fifo='1' and empty_fifo_dl='0' then
            if eof='1' then
                flow_out_fv     <= '0';
            end if;
        end if;
    end if;
end process;
flow_out_dv     <= rdreq_dl and not empty_fifo;

end RTL;
