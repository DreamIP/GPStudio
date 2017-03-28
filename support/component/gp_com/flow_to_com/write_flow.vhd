-- **************************************************************************
--	READ FLOW
-- **************************************************************************
-- Ce composant est connecte a un com_flow_fifo en entree et a un processing (FV/LV/Data) en sortie
-- 
-- 16/10/2014 - creation
--------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.ComFlow_pkg.all;

-- Transform FV/DV/Data en flow 

entity write_flow is
    generic (
        PACKET_SIZE     : integer := 255; 
        FLAGS_CODES     : my_array_t := InitFlagCodes
    );
    port (
        clk             : in std_logic;
        rst_n           : in std_logic;

        in_data         : in std_logic_vector(15 downto 0);
        in_fv           : in std_logic;
        in_dv           : in std_logic;
        enable_i        : in std_logic;
        fifo_f_i        : in std_logic;

        data_wr_o       : out std_logic;
        data_o          : out std_logic_vector(15 downto 0);
        flag_wr_o       : out std_logic;
        flag_o          : out std_logic_vector(7 downto 0);

        fifo_pkt_wr_o   : out std_logic;
        fifo_pkt_data_o : out std_logic_vector(15 downto 0)
    );
end write_flow;

architecture rtl of write_flow is
---------------------------------------------------------
--	SIGNALS
---------------------------------------------------------
	signal fv_r : std_logic := '0';
	signal dv_r : std_logic := '0';
	signal skip : std_logic := '0'; 
	
	signal flag_wr_cpt_s    : std_logic := '0';
	signal flag_wr_cpt_s_r  : std_logic := '0';
	signal flag_wr_fv_s     : std_logic := '0';

	signal fifo_pkt_data_s  : std_logic_vector(15 downto 0) := (others=>'0');

begin

FSM:process (clk, rst_n) 
variable cpt : integer range 0 to PACKET_SIZE:=0;
begin
	if (rst_n = '0') then			
		fv_r <='0';
		dv_r <='0';	
		flag_o <= (others=>'0');
		skip <= '0';
		cpt:=0;
		flag_wr_fv_s<='0';
		flag_wr_cpt_s<='0';
		flag_wr_cpt_s_r <='0';
		
	elsif rising_edge(clk) then	
		fv_r <= in_fv;
		dv_r <= in_dv;
		flag_wr_fv_s <= '0';
		flag_wr_cpt_s <= '0';
		flag_wr_cpt_s_r <= flag_wr_cpt_s;
		
		if(enable_i = '1') then
		
			if (in_dv = '1' and fifo_f_i = '0') then
				cpt := cpt + 1;
			end if;
		
			if (fv_r = '1' and in_fv = '0') then
				flag_wr_fv_s <= '1';
				flag_o <= FLAGS_CODES(EoF);
				skip <= '0';
				fifo_pkt_data_s <= std_logic_vector(to_unsigned(cpt,16));
				cpt := 0;

			elsif (cpt = (PACKET_SIZE-2) and skip = '1') then
				flag_wr_cpt_s <= '1';
				flag_o <= FLAGS_CODES(EoL);
				fifo_pkt_data_s <= std_logic_vector(to_unsigned(cpt,16));
				cpt := 0;

			elsif (cpt = (PACKET_SIZE-2) and skip = '0') then
				skip <='1';
				flag_wr_cpt_s <= '1';
				flag_o <= FLAGS_CODES(SoF);
				fifo_pkt_data_s <= std_logic_vector(to_unsigned(cpt,16));
				cpt := 0;
			end if;
		end if;
	end if;
end process;

--	data_wr_o <= in_dv and enable_i and not(fifo_f_i); -- Add in_fv in condition to avoid data write if fv = 0 
	data_wr_o <= in_dv and enable_i and not(fifo_f_i) and in_fv;
	data_o <= in_data;
	
	flag_wr_o <= flag_wr_fv_s or (flag_wr_cpt_s_r and in_fv);
	fifo_pkt_wr_o <= flag_wr_fv_s or (flag_wr_cpt_s_r and in_fv);

	fifo_pkt_data_o <= fifo_pkt_data_s;	

end rtl;
