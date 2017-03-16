
library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.all;

library altera_mf;
use altera_mf.all;

entity fifo_com_rx is
	generic
	(
		depth       : POSITIVE := 1024
	);
	port
	(
		aclr		: in std_logic  := '0';
		data		: in std_logic_vector (15 downto 0);
		rdclk		: in std_logic ;
		rdreq		: in std_logic ;
		wrclk		: in std_logic ;
		wrreq		: in std_logic ;
		q		    : out std_logic_vector (15 downto 0);
		rdempty		: out std_logic ;
		wrfull		: out std_logic
	);
END fifo_com_rx;


ARCHITECTURE syn OF fifo_com_rx IS

	SIGNAL sub_wire0	: STD_LOGIC;
	SIGNAL sub_wire1	: STD_LOGIC_VECTOR (15 DOWNTO 0);
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
        data	: in std_logic_vector (15 downto 0);
        rdclk	: in std_logic ;
        rdreq	: in std_logic ;
        wrfull	: out std_logic ;
        q	    : out std_logic_vector (15 downto 0);
        rdempty	: out std_logic ;
        wrclk	: in std_logic ;
        wrreq	: in std_logic ;
        aclr	: in std_logic 
	);
	end component;

begin
	wrfull  <= sub_wire0;
	q       <= sub_wire1(15 downto 0);
	rdempty <= sub_wire2;

	dcfifo_component : dcfifo
	generic map (
		intended_device_family => "Cyclone III",
		lpm_numwords => DEPTH,
		lpm_showahead => "OFF",
		lpm_type => "dcfifo",
		lpm_width => 16,
		lpm_widthu =>  integer(ceil(log2(real(DEPTH)))),
		overflow_checking => "ON",
		rdsync_delaypipe => 4,
		read_aclr_synch => "OFF",
		underflow_checking => "ON",
		write_aclr_synch => "OFF",
		use_eab => "ON",
		wrsync_delaypipe => 4
	)
	PORT MAP (
		data => data,
		rdclk => rdclk,
		rdreq => rdreq,
		wrclk => wrclk,
		wrreq => wrreq,
		wrfull => sub_wire0,
		q => sub_wire1,
		rdempty => sub_wire2,
		aclr => aclr
	);
end syn;
