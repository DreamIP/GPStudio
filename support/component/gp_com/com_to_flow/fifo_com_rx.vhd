
library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.all;

library altera_mf;
use altera_mf.all;

entity fifo_com_rx is
	generic
	(
        DEPTH       : POSITIVE := 1024;
        IN_SIZE     : POSITIVE;
        OUT_SIZE    : POSITIVE
	);
	port
	(
		aclr		: in std_logic  := '0';
		data		: in std_logic_vector (IN_SIZE-1 downto 0);
		rdclk		: in std_logic ;
		rdreq		: in std_logic ;
		wrclk		: in std_logic ;
		wrreq		: in std_logic ;
		q		    : out std_logic_vector (OUT_SIZE-1 downto 0);
		rdempty		: out std_logic ;
		wrfull		: out std_logic
	);
END fifo_com_rx;


ARCHITECTURE syn OF fifo_com_rx IS

	SIGNAL sub_wire0	: STD_LOGIC;
	SIGNAL sub_wire1	: STD_LOGIC_VECTOR (OUT_SIZE-1 DOWNTO 0);
	SIGNAL sub_wire2	: STD_LOGIC;

	COMPONENT dcfifo
	GENERIC (
		intended_device_family		: STRING;
		lpm_numwords		        : NATURAL;
		lpm_showahead		        : STRING;
		lpm_type		            : STRING;
		lpm_width		            : NATURAL;
		lpm_widthu		            : NATURAL;
		overflow_checking		    : STRING;
		rdsync_delaypipe		    : NATURAL;
		read_aclr_synch		        : STRING;
		underflow_checking		    : STRING;
		write_aclr_synch		    : STRING;
		use_eab		                : STRING;
		wrsync_delaypipe		    : NATURAL
	);
	port (
        data	: in std_logic_vector (IN_SIZE-1 downto 0);
        rdclk	: in std_logic ;
        rdreq	: in std_logic ;
        wrfull	: out std_logic ;
        q	    : out std_logic_vector (OUT_SIZE-1 downto 0);
        rdempty	: out std_logic ;
        wrclk	: in std_logic ;
        wrreq	: in std_logic ;
        aclr	: in std_logic 
	);
	end component;

    component dcfifo_mixed_widths
    generic (
        intended_device_family  : STRING;
        lpm_numwords            : NATURAL;
        lpm_showahead           : STRING;
        lpm_type                : STRING;
        lpm_width               : NATURAL;
        lpm_widthu              : NATURAL;
        lpm_widthu_r            : NATURAL;
        lpm_width_r             : NATURAL;
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
        q       : out std_logic_vector (OUT_SIZE-1 downto 0);
        rdempty : out std_logic;
        wrclk   : in std_logic;
        wrreq   : in std_logic;
        aclr    : in std_logic;
        data    : in std_logic_vector (IN_SIZE-1 downto 0);
        rdreq   : in std_logic
    );
    end component;

begin
	wrfull  <= sub_wire0;
	rdempty <= sub_wire2;
    
    FIFO_GEN_SAME_WIDTH : if (IN_SIZE = OUT_SIZE) generate
        dcfifo_component : dcfifo
        generic map (
            intended_device_family  => "Cyclone III",
            lpm_numwords            => DEPTH,
            lpm_showahead           => "OFF",
            lpm_type                => "dcfifo",
            lpm_width               => IN_SIZE,
            lpm_widthu              => integer(ceil(log2(real(DEPTH)))),
            overflow_checking       => "ON",
            rdsync_delaypipe        => 4,
            read_aclr_synch         => "OFF",
            underflow_checking      => "ON",
            use_eab                 => "ON",
            write_aclr_synch        => "OFF",
            wrsync_delaypipe        => 4
        )
        port map (
            rdclk   => rdclk,
            wrclk   => wrclk,
            wrreq   => wrreq,
            aclr    => aclr,
            data    => data,
            rdreq   => rdreq,
            wrfull  => sub_wire0,
            q       => sub_wire1,
            rdempty => sub_wire2
        );

        q          <= sub_wire1;
    end generate;

    FIFO_GEN_MIXED_WIDTH : if (IN_SIZE /= OUT_SIZE) generate
        dcfifo_component : dcfifo_mixed_widths
        generic map (
            intended_device_family  => "Cyclone III",
            lpm_numwords            => DEPTH,
            lpm_showahead           => "OFF",
            lpm_type                => "dcfifo_mixed_widths",
            lpm_width               => IN_SIZE,
            lpm_widthu              => integer(ceil(log2(real(DEPTH)))),
            lpm_widthu_r            => integer(ceil(log2(real(DEPTH))*(real(IN_SIZE)/real(OUT_SIZE)))),
            lpm_width_r             => OUT_SIZE,
            overflow_checking       => "ON",
            rdsync_delaypipe        => 4,
            read_aclr_synch         => "OFF",
            underflow_checking      => "ON",
            use_eab                 => "ON",
            write_aclr_synch        => "OFF",
            wrsync_delaypipe        => 4
        )
        port map (
            rdclk   => rdclk,
            wrclk   => wrclk,
            wrreq   => wrreq,
            aclr    => aclr,
            data    => data,
            rdreq   => rdreq,
            wrfull  => sub_wire0,
            q       => sub_wire1,
            rdempty => sub_wire2
        );

        q       <= sub_wire1(7 downto 0) & sub_wire1(15 downto 8); -- inverse bytes
    end generate;
end syn;
