library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library work;
use work.all;

entity tb_comflow_fifo is
end tb_comflow_fifo;

architecture behavioral of tb_comflow_fifo is

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
	
	
component  com_flow_fifo_rx
generic (
	FIFO_DEPTH : POSITIVE := 1024;
	FLOW_ID : integer := 1
    );
  port(
	data_wr_i : in std_logic;
    data_i : in std_logic_vector(15 downto 0);
	rdreq_i : in std_logic;
	pktend_i : in std_logic;
	enable_i : in std_logic;

	data_o : out std_logic_vector(15 downto 0);
	flow_rdy_o: out std_logic;
	f_empty_o : out std_logic;
	fifos_f_o : out std_logic;
	flag_o : out std_logic_vector(7 downto 0);

	clk_in_i : in std_logic;
	clk_out_i :in std_logic;

	rst_n_i :in std_logic
    );
end component;

component read_flow
  generic (
	ReadingDelay : POSITIVE := 50
    );
  port(
  
    data_i : in std_logic_vector(15 downto 0);
	flow_rdy_i: in std_logic;
	f_empty_i : in std_logic;
	enable_i : in std_logic;
	flag_i :in  std_logic_vector(7 downto 0);
	
	read_data_o : out std_logic;
	data_o : out std_logic_vector(15 downto 0);
	fv_o: out std_logic;
	dv_o : out std_logic;

	clk_i :in std_logic;
	rst_n_i :in std_logic
    
    );
end component;

begin


ComFlowFifo_inst : component com_flow_fifo_rx
 generic map (FIFO_DEPTH => 1024, FLOW_ID => 0)
 port map (
	data_wr_i => wr,
    data_i => std_logic_vector(data_s),
	rdreq_i => read_data,
	pktend_i => pkt_end,
	enable_i =>'1',
	data_o => data_o_s,
	flag_o => flag_ss,
	flow_rdy_o => flow_rdy,
	f_empty_o => empty ,
	fifos_f_o => fifos_full_s,
	clk_in_i => clk_usb_s,
	clk_out_i => clk_s,
	rst_n_i => rst_s
);	
	
WR_process : process
variable counter : integer range 0 to 1024:= 0;
variable frame_number : integer range 0 to 100 := 0;
begin
	
	flag_s(0) <= X"00AA";
	flag_s(1) <= X"00BB";
	flag_s(2) <= X"00AB";
	flag_s(3) <= X"00BA";
	flag_s(4) <= X"00AA";
	flag_s(5) <= X"00BB";
	flag_s(6) <= X"00AB";
	flag_s(7) <= X"00BA";

	data_s <= X"0000";
	counter := 0 ;
	pkt_end <= '0';
	frame_number := 0;
	wr <= '0';
	wait until rst_s='1';
	wait for 8*CLOCK_USB_T_ON;
	
	while true loop
		
		if fifos_full_s ='0' then
			wait until rising_edge(clk_usb_s);
			data_s <= flag_s(frame_number);
			wr <= '1';
			
			wait until rising_edge(clk_usb_s);
			data_s <= to_unsigned(frame_number,16);
			
			wait until rising_edge(clk_usb_s);
			data_s <=  X"0000";
			
			for i in 1 to USB_PKT_SIZE-3 loop
				wait until rising_edge(clk_usb_s);	
				data_s <= data_s + X"0001";	
			end loop;
					
			pkt_end <= '1';
			wait until rising_edge(clk_usb_s);
			pkt_end <= '0';
			wr <= '0';
			
			counter := 0 ;
			frame_number := frame_number +1;
			data_s <= X"0000";
			wait for 40 us;
		else
			wait until rising_edge(clk_usb_s);
		end if;
		
	end loop;	
	wait;
end process;

RD_process : component read_flow
  generic map(ReadingDelay => 300)
  port map(
    data_i => data_o_s, 
	flow_rdy_i=>flow_rdy,
	f_empty_i => empty,
	enable_i => '1',	
	flag_i => flag_ss,
	read_data_o => read_data,
	data_o => data_output,
	fv_o => fv_s,
	dv_o => dv_s,
	clk_i =>clk_s,
	rst_n_i => rst_s
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
	

end behavioral;
