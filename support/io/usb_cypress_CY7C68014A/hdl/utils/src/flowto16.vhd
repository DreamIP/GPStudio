
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- N TO 16 BITS Adaptateur
-- valeurs possibles entrée 8,16,32 
-- tester avec 1, 2, 4 avec le code en 8to16, devrait fonctionner ou presque ...

-- ALtera libray used for 32 to 16 bits scfifo component
LIBRARY altera_mf;
USE altera_mf.altera_mf_components.all;

use ieee.math_real.all;

entity flowto16 is
    generic (
        INPUT_SIZE : integer;
        FIFO_DEPTH : integer := 32
    );
    port (
        rst_n  	    	: in  std_logic;
        clk  			: in  std_logic;

        in_data         : in  std_logic_vector(INPUT_SIZE-1 downto 0);
        in_fv           : in  std_logic;
        in_dv           : in  std_logic;

        out_data        : out std_logic_vector(15 downto 0);
        out_fv          : out std_logic;
        out_dv          : out std_logic
    );
end flowto16;

architecture rtl of flowto16 is

-- signaux pour inf a 16bits
constant CPT_MAX : integer := 16/INPUT_SIZE;
type state_t is (Initial, WaitSd);
signal state : state_t := Initial;
signal tmp8bits : std_logic_vector(7 downto 0) := (others=>'0');

-- signaux pour fonctionnement 32 to 16
type state_32t is (Initial, SendLSB, DumpLastMSB, DumpLastLSB, SyncSignal);
signal state_32b : state_32t := Initial;

signal tmp16bits    : std_logic_vector(15 downto 0) := (others=>'0');
signal fifo_empty_s : std_logic := '0';
signal databuf      : std_logic_vector(31 downto 0) := (others=>'0');
signal aclr_s       : std_logic := '0';
signal rdreq_s      : std_logic := '0';
signal usedw_s      : std_logic_vector(integer(ceil(log2(real(FIFO_DEPTH))))-1 downto 0) := (others=>'0');
signal fifo_empty_r : std_logic :='0';


begin

label_16bits : if (INPUT_SIZE=16) generate
	out_fv <= in_fv;
	out_dv <= in_dv;
	out_data <= in_data;
end generate label_16bits;


label_32bits : if (INPUT_SIZE=32) generate

aclr_s <= not(rst_n);

with state_32b select
rdreq_s <=  not(fifo_empty_s) when Initial,
			'0' when SendLSB ,
			'0' when others;
			
FIFO : component scfifo
	generic map(
		intended_device_family => "Cyclone III",
		lpm_numwords => FIFO_DEPTH,
		lpm_showahead => "OFF",
		lpm_type => "scfifo",
		lpm_width => 32,
		lpm_widthu => integer(ceil(log2(real(FIFO_DEPTH)))),
		overflow_checking => "ON",
		underflow_checking => "ON",
		use_eab => "ON"
)
	port map 
	(
		data		=> in_data,
		rdreq		=> rdreq_s,
		clock		=> clk,
		wrreq		=> in_dv,
		aclr 		=> aclr_s ,
		q			=> databuf,
		empty       => fifo_empty_s,
		usedw       => usedw_s,
		full	    => open
	);
	
process(clk, rst_n)

begin

	if (rst_n = '0') then 
		state_32b <= Initial;
		out_fv <= '0';
		out_dv <='0';
		out_data <=  (others=>'0');
		tmp16bits <=  (others=>'0');
		fifo_empty_r <= '1';
--		rdreq_s <= '0';

	elsif rising_edge(clk) then
	--	out_fv <=  in_fv;
		out_dv <='0';
		fifo_empty_r <= fifo_empty_s;

		case state_32b is
			when Initial =>
				if (in_fv = '1') then
					out_fv <='1';
				end if;

				if( fifo_empty_r = '0' ) then

					out_data <= databuf(31 downto 16);
					out_dv <='1';

					tmp16bits <= databuf(15 downto 0);
					state_32b <= SendLSB;
				end if;

			when SendLSB =>
					out_data <= tmp16bits ;
					out_dv <='1';
					state_32b <= Initial;

					-- Dernière donnée : cas particulier
					if(fifo_empty_s = '1') then
						state_32b <= DumpLastMSB;
					end if;

			when DumpLastMSB =>
				out_dv <='1';
				out_data <= databuf(31 downto 16);
				state_32b <= DumpLastLSB;

			when DumpLastLSB =>
				out_dv <='1';
				out_data <= databuf(15 downto 0);
				state_32b <= SyncSignal;

			when SyncSignal =>
				if (in_fv = '0') then
					out_fv <='0';
					out_dv <='0';
				end if;

				state_32b <= Initial;
		end case;
	end if;
end process;

end generate label_32bits;

-- Fonctionnement 8 TO 16bits
label_8bits : if INPUT_SIZE = 8 generate
process(clk,rst_n)
begin

	if (rst_n = '0') then 
		state <= Initial;
		out_fv <= '0';
		out_dv <='0';
		out_data <= (others=>'0');
		tmp8bits <= (others=>'0');

	elsif rising_edge(clk) then
		out_fv <=  in_fv;
		out_dv <='0';
		
		case state is
			when Initial =>
				if (in_dv ='1' and in_fv='1') then
					out_dv <='0';
					tmp8bits <= in_data;
					state <= WaitSd;
				end if;
			
			when WaitSd =>
				if (in_dv ='1' and in_fv='1') then
					out_data <= tmp8bits & in_data;
					out_dv <='1';
					state <= Initial;
				end if;
		end case;
	end if;
end process;
end generate label_8bits;

-- Fonctionnement non verifie pour INPUT_SIZE < 8
-- label_inf8bits : if INPUT_SIZE < 8 generate
-- process(clk,rst_n)
-- variable cpt : integer range 0 to CPT_MAX := 0;
-- begin

	-- if (rst_n = '0') then 
		-- state <= Initial;
		-- out_fv <= '0';
		-- out_dv <='0';
		-- out_data <=  (others=>'0');
		-- tmp <=  (others=>'0');
		-- cpt = 0;
	-- elsif rising_edge(clk) then
		-- out_fv <=  in_fv;
		-- out_dv <='0';
		
		-- case state is
		
			-- when Initial =>
				-- if in_dv ='1' then
					-- tmp (INPUT_SIZE-1 downto 0) <= in_data;
					-- tmp sll INPUT_SIZE;
					-- cpt = cpt + 1;
					-- if (cpt = CPT_MAX) then
						-- state <= WaitSd;
						-- cpt = 0;
					-- end if;
				-- end if;
				
			
			-- when WaitSd =>
				-- if in_dv ='1' then
					-- out_data <= tmp ;
					-- out_dv <='1';
					-- state <= Initial;
				-- end if;
		-- end case;
	-- end if;
-- end process;
-- end generate label_inf8bits;

end rtl;
