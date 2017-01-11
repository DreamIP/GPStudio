-- This block receives a flow from a GPStudio block.
-- Depending on the data valid and flow valid, it add the GPStudio header (Start of Frame and End of Frame flags and the number of the packet)
-- before sending the data to the HAL block.

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;
use work.com_package.all;

entity flow_to_com is
    generic (
        ID_FIFO         : std_logic_vector(5 downto 0) := "000001";
        FLOW_IN_SIZE    : integer := 8;
        DATA_WIDTH      : integer := 8;
        FIFO_DEPTH      : integer := 2048;
        ONE_PACKET      : integer := 1450
    );
    port (
        clk_hal         : in std_logic;
        clk_proc        : in std_logic;
        reset_n         : in std_logic;
        enable          : in std_logic;
        flow_in_data    : in std_logic_vector(FLOW_IN_SIZE-1 downto 0);
        flow_in_fv      : in std_logic;
        flow_in_dv      : in std_logic;
        read_data       : in std_logic;
        ready           : out std_logic;
        data_size       : out std_logic_vector(15 downto 0);
        data_out        : out std_logic_vector(7 downto 0)
    );
end flow_to_com;

architecture RTL of flow_to_com is

    type flow_to_com_fsm is (idle, gps_header, data_state);
    signal state                    : flow_to_com_fsm; --Flow to hal FSM
    signal count_fifo               : std_logic_vector(15 downto 0);
    signal one_packet_size          : std_logic_vector(15 downto 0);
    signal num_packet_s             : std_logic_vector(7 downto 0);
    signal ready_s                  : std_logic;
    signal wrreq,rdreq              : std_logic;
    signal reset                    : std_logic;
    signal flow_fv_dl               : std_logic;
    signal empty_fifo               : std_logic;
    signal rd_flags,wr_flags        : std_logic;
    signal empty_flags              : std_logic;
    signal data_flags               : std_logic_vector(17 downto 0);
    signal count_wr,count_wr_b      : std_logic_vector(15 downto 0);
    signal count_rd                 : std_logic_vector(15 downto 0);
    signal pkt_size                 : std_logic_vector(15 downto 0);
    signal sof,eof                  : std_logic;
    signal data,data_out_s          : std_logic_vector(7 downto 0);
    signal flags_out                : std_logic_vector(17 downto 0);
    signal count_fsm                : std_logic_vector(3 downto 0);
    signal data_size_s              : std_logic_vector(15 downto 0);
    signal read_data_dl             : std_logic;
    signal rdreq_s                  : std_logic;

begin
reset               <= not reset_n;

rd_flags            <= '1' when read_data='1' and read_data_dl='0' and state=idle else '0';
ready               <= '0' when empty_fifo='1' else ready_s;
one_packet_size     <= std_logic_vector(to_unsigned(ONE_PACKET,16));
data_size           <= data_size_s+x"2";

--- Write fifo only when enable
wrreq           <= flow_in_dv when enable='1' else '0';

--- Fifo that contains data
fifo_data_inst : entity work.gp_dcfifo
    generic map (DATA_WIDTH  => 8,
                 FIFO_DEPTH  => FIFO_DEPTH)
    port map(
        aclr        => reset,
        data        => flow_in_data,
        rdclk       => clk_hal,
        rdreq       => rdreq,
        wrclk       => clk_proc,
        wrreq       => wrreq,
        q           => data,
        rdempty     => empty_fifo
    );

--- Fifo that contains flags of the packets and their size
fifo_flags_inst : entity work.gp_dcfifo
    generic map (DATA_WIDTH  => 18,
                 FIFO_DEPTH  => 256)
    port map(
        aclr        => reset,
        data        => data_flags,
        rdclk       => clk_hal,
        rdreq       => rd_flags,
        wrclk       => clk_proc,
        wrreq       => wr_flags,
        q           => flags_out,
        rdempty     => empty_flags
    );

data_flags      <= sof & eof & pkt_size;
count_wr_b      <= std_logic_vector(to_unsigned(to_integer(unsigned(count_wr)) rem ONE_PACKET,16));
pkt_size        <= count_wr when count_wr<one_packet_size else
                   count_wr_b when eof='1' else
                   one_packet_size;

----- Set the write signals for fifo flags
process(clk_proc, reset_n)
begin
    if reset_n='0' then
        count_wr        <= x"0000";
        wr_flags        <= '0';
        sof             <= '0';
        eof             <= '0';

    elsif clk_proc'event and clk_proc='1' then
        flow_fv_dl          <= flow_in_fv;
        --- Counting data write
        if wrreq='1' then
            count_wr    <= count_wr+1;
        elsif eof='1' then
            count_wr    <= x"0000";
        end if;

        --- Control fifo flags write
        if (flow_in_fv='0' and flow_fv_dl='1') or (to_integer(unsigned(count_wr)) rem ONE_PACKET=0 and count_wr>=one_packet_size) then
            wr_flags        <= '1';
        else
            wr_flags        <= '0';
        end if;

        --- Set flags for each packet
            --- Set Start of Frame when a flow starts
        if flow_in_fv='1' and flow_fv_dl='0' then
            sof                 <= '1';
        elsif eof='1' or  count_wr>one_packet_size then
            sof                 <= '0';
        end if;
            --- Set End of Frame when a flow ends
        if flow_in_fv='0' and flow_fv_dl='1' then
            eof             <= '1';
        elsif flow_in_fv='1' and flow_fv_dl='0' then
            eof             <= '0';
        end if;

    end if;
end process;


----- Set the read signals for fifo flags and place gps header when HAL starts to read
process(clk_hal, reset_n)
begin
    if reset_n='0' then
        count_rd        <= x"0000";
        data_size_s     <= x"0000";
        num_packet_s    <= x"00";

    elsif clk_hal'event and clk_hal='1' then
        read_data_dl        <= read_data;
        --- Control fifo flags read
        if rdreq='1' then
            count_rd        <= count_rd +1;
        elsif state=idle then
            count_rd        <= x"0000";
        end if;
        --- Place gps header then read data from fifo
        case state is
            when idle =>
                rdreq_s     <= '0';
                if rd_flags='1' then
                    state     <= gps_header;
                    count_fsm   <= x"0";
                end if;

            when gps_header =>
                if read_data='1' then
                    count_fsm   <= count_fsm+1;
                    if count_fsm=x"0" then
                        data_out_s    <= ID_FIFO & flags_out(17 downto 16);
                        if flags_out(17)='1' then       --- Increment numero of the packet only when SoF=0
                            num_packet_s    <= x"00";
                        else
                            num_packet_s    <= num_packet_s+1;
                        end if;
                    elsif count_fsm=x"1" then
                        data_out_s      <= num_packet_s;
                        rdreq_s         <= '1';
                        data_size_s     <= flags_out(15 downto 0);
                    elsif count_fsm=x"2" then
                        state           <= data_state;
                        data_out_s      <= data;
                    end if;
                end if;

            when data_state =>
                if read_data='1' then
                    rdreq_s           <= '1';
                    if count_rd=data_size_s-x"1" or empty_fifo='1' then
                        state         <= idle;
                        rdreq_s       <= '0';
                    end if;
                end if;
            when others =>
                state     <= idle;
            end case;
    end if;
end process;

data_out    <= data_out_s when state=gps_header else data;
rdreq       <= '0' when (ready_s='0' or empty_fifo='1') else rdreq_s;

----- Set the ready signal
process(clk_hal, reset_n)
begin
    if reset_n='0' then
        ready_s         <= '0';
    elsif clk_hal'event and clk_hal='1' then
        if empty_flags='0' and ready_s='0' then
            ready_s        <= '1';
        elsif (state=data_state and count_rd=data_size_s-x"1") or (empty_fifo='1') then --- Not ready when a full packet has been read or when it's the last packet
            ready_s        <= '0';
        end if;
    end if;
end process;



end RTL;
