library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library work;
use work.ComFlow_pkg.all;

entity tb_flow_out is
end tb_flow_out;

architecture behavioral of tb_flow_out is

	constant IM_WIDTH	 : positive := 10;
	constant IM_HEIGHT 	 : positive := 1;

	constant PACKET_SIZE : positive := IM_WIDTH;
	constant INTERPACKET_TIME : positive := 10;

	constant INTERFRAME_TIME : positive := IM_WIDTH+4;

	
	constant CLOCK_T_ON  : time := 10 ns;
	constant CLOCK_T_OFF : time := 10 ns;

	constant CLOCK_USB_T_ON  : time := 10.4 ns;
	constant CLOCK_USB_T_OFF : time := 10.4 ns;
	
	constant RESET_T_ON  : time := 400 ns;
	
	constant USB_PKT_SIZE  : positive := 512 ;
	constant DELAY_RD : positive := 20 ;
	

	signal clk_s  			: std_logic;
	signal clk_usb_s 		: std_logic;
	signal rst_s  			: std_logic;

----------------------------------------------
	signal wr : std_logic;
	signal data_s : unsigned(15 downto 0);
	signal pkt_end : std_logic;
----------------------------------------------
	signal flow_rdy : std_logic;
	signal read_data : std_logic;
	signal empty : std_logic;
	signal q_s : std_logic_vector(15 downto 0);
	
	type flags_array is array (7 downto 0) of unsigned(15 downto 0);
	signal flag_s : flags_array := (others=>(others=>'0'));

	signal flag_ss : std_logic_vector(7 downto 0);
	signal fifos_full_s : std_logic;
	signal data_o_s : std_logic_vector(15 downto 0);
	signal data_output : std_logic_vector(15 downto 0);
	
	signal empty_r: std_logic;
	
	signal dv_s: std_logic;
	signal fv_s: std_logic;
	signal data_i_s : std_logic_vector(15 downto 0);
	
	-- Connexion entre FLOW OUT et USB
	signal flow_out1_data_s : std_logic_vector(15 downto 0):=(others=>'0');
	signal flow_out1_empty_s : std_logic:='0';
	signal flow_out1_rd_s : std_logic:='0';
	signal flow_out1_rdy_s : std_logic:='0';

	
component flow_out is
  generic (
	FIFO_DEPTH : POSITIVE := 1024;
	FLOW_ID : integer := 1;
	PACKET_SIZE : integer := 256; 
	FLAGS_CODES : my_array_t := InitFlagCodes
    );
  port(

	data_i : in std_logic_vector(15 downto 0);
	fv_i: in std_logic;
	dv_i : in std_logic;
	rdreq_i : in std_logic;
	enable_i : in std_logic;
	
	data_o : out std_logic_vector(15 downto 0);
	flow_rdy_o: out std_logic;	
	f_empty_o : out std_logic;
	
--	fifos_f_o : out std_logic;
	
	clk_in_i : in std_logic;
	clk_out_i :in std_logic;
	rst_n_i :in std_logic 
    );
end component;

begin


FLOW_OUT_128: component flow_out 
  generic map (
	FIFO_DEPTH => IM_WIDTH*16,
	FLOW_ID => 128,
	PACKET_SIZE => IM_WIDTH, -- must match cypress packet size ... 
	FLAGS_CODES => InitFlagCodes
    )
  port map(
	data_i => data_i_s,
	fv_i => fv_s,
	dv_i => dv_s,
	rdreq_i => flow_out1_rd_s,
	enable_i => '1',
	data_o => flow_out1_data_s,
	flow_rdy_o=> flow_out1_rdy_s,
	f_empty_o => flow_out1_empty_s,
	
	clk_in_i => clk_s, 
	clk_out_i => clk_usb_s,
	rst_n_i=> rst_s
);


------------------------------------------------------------------------------
	CLOCK_PROC : process 
	begin
		while true loop
			clk_s <= '0';
			wait for CLOCK_T_OFF;
			clk_s <= '1';
			wait for CLOCK_T_ON;
		end loop;
		wait;
	end process;
	
------------------------------------------------------------------------------
	CLOCK_USB_PROC : process 
	begin
		while true loop
			clk_usb_s <= '0';
			wait for CLOCK_USB_T_OFF;
			clk_usb_s <= '1';
			wait for CLOCK_USB_T_ON;
		end loop;
		wait;
	end process;
	

	RESET_PROC : process 
	begin
		rst_s <= '0';
		wait for RESET_T_ON;
		rst_s <= '1';
		wait;
	end process;	
	
	FRAME_VALID_PROC : process
		begin
			fv_s 	<= '0';
			wait until rst_s = '1';
			wait for 8*CLOCK_T_ON;
			while true loop
			
				for i in 1 to IM_WIDTH*(IM_WIDTH+INTERPACKET_TIME) loop
					wait until rising_edge(clk_s);
						fv_s 	<= '1';
				end loop;

				fv_s 	<= '0';
				wait for 12*CLOCK_T_ON;
			end loop;	
			wait;
		end process;
		
	DATA_VALID_PROC : process
		begin
			dv_s 	<= '0';
			data_i_s	<= (others => '0');

			wait until fv_s='1';
			while true loop
				if (fv_s='1') then
					wait for 4*CLOCK_T_ON;
						
						for i in 0 to IM_WIDTH loop
								dv_s <= '1';	
								data_i_s <= std_logic_vector(unsigned(data_i_s) + 5);
								wait until rising_edge(clk_s);		
						end loop;
						dv_s <= '0';					
						for i in 0 to INTERPACKET_TIME loop
							wait until rising_edge(clk_s);										
						end loop;
				else
					wait until rising_edge(clk_s);										
				end if;
			end loop;	
			wait;
		end process;
		

end behavioral;
