
library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.all;

library altera_mf;
use altera_mf.all;

entity fifo_com_tx is
    generic
    (
        DEPTH : positive
    );
    port
    (
        aclr        : in  std_logic;
        data        : in  std_logic_vector (15 downto 0);
        rdclk       : in  std_logic;
        rdreq       : in  std_logic;
        wrclk       : in  std_logic;
        wrreq       : in  std_logic;
        q           : out std_logic_vector (15 downto 0);
        rdempty     : out std_logic;
        rdusedw     : out std_logic_vector (integer(ceil(log2(real(depth))))-1 downto 0);
        wrfull      : out std_logic;
        wrusedw     : out std_logic_vector (integer(ceil(log2(real(depth))))-1 downto 0)
    );
END fifo_com_tx;

architecture syn of fifo_com_tx is

    signal sub_wire0    : std_logic;
    signal sub_wire1    : std_logic_vector (15 downto 0);
    signal sub_wire2    : std_logic;
    signal sub_wire3    : std_logic_vector (integer(ceil(log2(real(depth))))-1 downto 0);
    signal sub_wire4    : std_logic_vector (integer(ceil(log2(real(depth))))-1 downto 0);

    component dcfifo
    generic (
        intended_device_family  : STRING;
        lpm_numwords            : NATURAL;
        lpm_showahead           : STRING;
        lpm_type                : STRING;
        lpm_width               : NATURAL;
        lpm_widthu              : NATURAL;
        overflow_checking       : STRING;
        rdsync_delaypipe        : NATURAL;
        read_aclr_synch         : STRING;
        underflow_checking      : STRING;
        use_eab                 : STRING;
        write_aclr_synch        : STRING;
        wrsync_delaypipe        : NATURAL
    );
    port (
        rdclk   : in std_logic;
        wrfull  : out std_logic;
        q       : out std_logic_vector (15 downto 0);
        rdempty : out std_logic;
        wrclk   : in std_logic;
        wrreq   : in std_logic;
        wrusedw : out std_logic_vector (integer(ceil(log2(real(depth))))-1 downto 0);
        aclr    : in std_logic;
        data    : in std_logic_vector (15 downto 0);
        rdreq   : in std_logic;
        rdusedw : out std_logic_vector (integer(ceil(log2(real(depth))))-1 downto 0)
    );
    end component;

begin
    wrfull    <= sub_wire0;
    q    <= sub_wire1(15 downto 0);
    rdempty    <= sub_wire2;
    wrusedw    <= sub_wire3(integer(ceil(log2(real(DEPTH))))-1 downto 0);
    rdusedw    <= sub_wire4(integer(ceil(log2(real(DEPTH))))-1 downto 0);

    dcfifo_component : dcfifo
    generic map (
        intended_device_family => "Cyclone III",
        lpm_numwords => DEPTH,
        lpm_showahead => "OFF",
        lpm_type => "dcfifo",
        lpm_width => 16,
        lpm_widthu => integer(ceil(log2(real(DEPTH)))),
        overflow_checking => "ON",
        rdsync_delaypipe => 4,
        read_aclr_synch => "OFF",
        underflow_checking => "ON",
        use_eab => "ON",
        write_aclr_synch => "OFF",
        wrsync_delaypipe => 4
    )
    port map (
        rdclk => rdclk,
        wrclk => wrclk,
        wrreq => wrreq,
        aclr => aclr,
        data => data,
        rdreq => rdreq,
        wrfull => sub_wire0,
        q => sub_wire1,
        rdempty => sub_wire2,
        wrusedw => sub_wire3,
        rdusedw => sub_wire4
    );
end syn;
