library ieee;
use ieee.std_logic_1164.all;
use IEEE.std_logic_unsigned.all;
use work.axi.all;
use work.ipv4_types.all;

entity icmp is
port(
	ip_rx_start         : in std_logic;  -- indicates receipt of ip frame.
   ip_rx               : in ipv4_rx_type;
	
	ip_tx_start			   : out std_logic;
	ip_tx_data_out_rdy	: in std_logic;
	ip_tx_result			: in std_logic_vector(1 downto 0);
	
	icmp_tx				: out ipv4_tx_type;
	
	rx_clk				: in std_logic;
	tx_clk				: in std_logic;
	reset               : in std_logic;
	
	icmp_rx_count 		: out std_logic_vector(15 downto 0);
	icmp_tx_count		: out std_logic_vector(15 downto 0);
	
	req_ip_layer		: out std_logic;
	granted_ip_layer	: in std_logic
	
	);
end entity;


architecture rtl of icmp is


--RX side
type state_0 is(idle, echo_request, code, checksum_1, checksum_2, identifier_1, identifier_2, sequence_number_1, sequence_number_2, data_odd,
				data_even, last_data_odd, last_data_even, start_req_tx, wait_for_last);
signal state_rx : state_0;

signal data_to_crc, data_in 			: std_logic_vector(7 downto 0);
signal data_word 							: std_logic_vector(15 downto 0);
signal data_length, data_length_1 	: std_logic_vector(15 downto 0);
signal checksum_long 					: std_logic_vector(16 downto 0);
signal checksumInt, checksum 			: std_logic_vector(15 downto 0);
signal checksum_icmp 					: std_logic_vector(15 downto 0);
signal data_icmp 							: std_logic;
signal crc_init, crc_en, crc_en_2 	: std_logic;
signal odd_event 							: std_logic;
signal start_tx							: std_logic;


--FF
signal ff_reset_int, ff_rdreq_int, ff_wrreq_int, ff_empty_int, ff_full_int : std_logic;
signal ff_data_in_int, ff_data_out_int : std_logic_vector(7 downto 0);



--TX side
type state_1 is(idle_tx, echo_reply_tx, code_tx, checksum_1_tx, checksum_2_tx, identifier_1_tx, identifier_2_tx, sequence_number_1_tx, sequence_number_2_tx, data_tx, reload_ff_read_req);
signal state_tx : state_1;

signal ack_start_tx, transmit_done 	: std_logic;
signal checksum_tx_0, checksum_tx_1 : std_logic_vector(15 downto 0);
signal dst_ip, dst_ip_0, dst_ip_1 	: std_logic_vector(31 downto 0);


signal icmp_tx_data_tmp 						: std_logic_vector(7 downto 0);
signal icmp_tx_dv_tmp, icmp_tx_first_tmp 	: std_logic;

signal icmp_tx_last, icmp_tx_dv, icmp_tx_first 	: std_logic;
signal icmp_tx_data 										: std_logic_vector(7 downto 0);
signal icmp_data_length 								: std_logic_vector(15 downto 0);

--test purpose
signal en_count_rx_int, en_count_tx_int : std_logic;
signal icmp_rx_count_int, icmp_tx_count_int : std_logic_vector(15 downto 0);

begin

data_in <= ip_rx.data.data_in;

-----FIFO SYNC clock domains & store data icmp
ff_icmp_inst : entity work.ff_icmp PORT MAP (
		aclr	 	=> ff_reset_int,
		data	 	=> data_in,
		rdclk	 	=> tx_clk,
		rdreq	 	=> ff_rdreq_int,
		wrclk	 	=> rx_clk,
		wrreq	 	=> ff_wrreq_int,
		q	 		=> ff_data_out_int,
		rdempty	 	=> ff_empty_int,
		wrfull	 	=> ff_full_int
	);


-----CHECKSUM COMPUTE
checksumInt <= 	checksum_long(15 downto 0) + checksum_long(16);
checksum <= NOT checksumInt;
process(rx_clk)
begin
if rising_edge(rx_clk) then
	crc_en_2 <= crc_en;
	if crc_init = '1' then
		checksum_long	<= (others => '0');
	elsif crc_en_2 = '1' then
		checksum_long <= ('0' & checkSumInt) + ('0' & data_word);
	end if;
end if;
end process;

process(tx_clk)--sync stage
begin
if rising_edge(tx_clk) then
	checksum_tx_0 		<= checksum;
	checksum_tx_1 		<= checksum_tx_0;
	dst_ip_0	 			<= dst_ip;--ip_rx.hdr.src_ip_addr;
	dst_ip_1				<= dst_ip_0;
	data_length_1		<= data_length;
	icmp_data_length	<= data_length_1;	
end if;
end process;

icmp_rx_count	<= icmp_rx_count_int;

-----STATES RX
process(rx_clk, reset)
begin
if reset = '1' then
	
	en_count_rx_int<= '0';
	icmp_rx_count_int<= (others => '0');
	dst_ip			<= (others => '0');
	data_word		<= (others => '0');
	data_length		<= (others => '0');
	ff_reset_int	<= '0';
	ff_wrreq_int	<= '0';
	ff_data_in_int	<= (others => '0');
	crc_init			<= '1';
	crc_en			<= '0';
	odd_event		<= '0';
	start_tx			<= '0';
	state_rx			<= idle;

elsif rising_edge(rx_clk) then

	en_count_rx_int <= '0';
	
	data_word	<= data_word(7 downto 0) & ff_data_in_int;
	
	if en_count_rx_int = '1' then
		icmp_rx_count_int	<= icmp_rx_count_int + 1;
	end if;
	
	case state_rx is
	
	-------------------------------------------
	when idle =>
		odd_event		<= '0';
		start_tx		<= '0';
		if transmit_done = '1' then
			dst_ip			<= (others => '0');
			crc_init		<= '1';
			crc_en			<= '0';
			data_length		<= (others => '0');
			ff_reset_int	<= '1';
			ff_wrreq_int	<= '0';
			ff_data_in_int	<= (others => '0');
			state_rx		<= echo_request;	
		else
			dst_ip			<= dst_ip;
			crc_init		<= '0';
			crc_en			<= '0';
			data_length		<= data_length;
			ff_reset_int	<= '0';
			ff_wrreq_int	<= '0';
			ff_data_in_int	<= (others => '0');
			state_rx		<= idle;	
		end if;
			
	
	-------------------------------------------
	when echo_request =>
		ff_reset_int	<= '0';
		ff_wrreq_int	<= '0';
		crc_init		<= '0';
		odd_event		<= '0';
		start_tx		<= '0';
		if ip_rx.data.data_in_valid = '1' and ip_rx_start = '1' then
			if ip_rx.hdr.protocol = x"01" and ip_rx.hdr.is_valid = '1' then
				dst_ip			<=	ip_rx.hdr.src_ip_addr;
				if data_in = x"08" then --only take echo request
					ff_data_in_int	<= (others => '0');--reply for crc
					crc_en			<= '0';
					data_length		<= ip_rx.hdr.data_length;	
					state_rx		<= code;
				else
					crc_en			<= '0';
					ff_data_in_int	<= (others => '0');	
					data_length		<= (others => '0');
					state_rx		<= wait_for_last;						
				end if;
			else
				crc_en		<= '0';
				ff_data_in_int	<= (others => '0');	
				data_length	<= (others => '0');
				state_rx	<= wait_for_last;			
			end if;
		else
			crc_en		<= '0';
			ff_data_in_int	<= (others => '0');	
			data_length	<= (others => '0');
			state_rx	<= echo_request;--idle;
		end if;
	
	-------------------------------------------
	when code =>
		ff_reset_int	<= '0';
		ff_wrreq_int	<= '0';
		ff_data_in_int	<= (others => '0');	
		crc_init		<= '0';
		odd_event		<= '0';
		start_tx		<= '0';
		if data_length > 504 then --can not handle such big data 504
			crc_en		<= '0';
			data_length	<= (others => '0');
			state_rx	<= wait_for_last;
		else
			data_length	<= data_length;
			if ip_rx.data.data_in_valid = '1' then
				crc_en		<= '1';				
				state_rx	<= checksum_1;
			else
				crc_en		<= '0';
				state_rx	<= code;
			end if;
		end if;
	
	-------------------------------------------
	when checksum_1 =>
		ff_reset_int	<= '0';
		ff_wrreq_int	<= '0';
		ff_data_in_int	<= (others => '0');	
		crc_init		<= '0';
		data_length		<= data_length;
		--odd_event		<= '0';
		start_tx		<= '0';
		if ip_rx.data.data_in_valid = '1' then
			crc_en		<= '0';
			odd_event	<= '1';--checksum non pris en compte dans le calcul
			state_rx	<= checksum_2;
		else
			crc_en		<= '0';
			odd_event 	<= '0';
			state_rx	<= checksum_1;
		end if;
	
	-------------------------------------------
	when checksum_2 =>
		ff_reset_int	<= '0';
		ff_wrreq_int	<= '0';
		ff_data_in_int	<= (others => '0');	
		crc_init		<= '0';
		data_length	<= data_length;
		--odd_event		<= '0';
		start_tx		<= '0';
		if ip_rx.data.data_in_valid = '1' then
			crc_en		<= '1';
			odd_event	<= '1';--checksum non pris en compte dans le calcul
			state_rx	<= identifier_1;
		else
			crc_en		<= '0';
			odd_event	<= '0';
			state_rx	<= checksum_2;
		end if;
	
	-------------------------------------------
	when identifier_1 =>
		ff_reset_int	<= '0';
		ff_data_in_int	<= data_in;	
		crc_init		<= '0';
		data_length		<= data_length;
		odd_event		<= '0';
		start_tx		<= '0';
		if ip_rx.data.data_in_valid = '1' then
			ff_wrreq_int<= '1';
			crc_en		<= '0';
			data_length	<= data_length;
			state_rx	<= identifier_2;
		else
			ff_wrreq_int<= '0';
			crc_en		<= '0';
			data_length	<= data_length;
			state_rx	<= identifier_1;
		end if;
	
	-------------------------------------------
	when identifier_2 =>
		ff_reset_int	<= '0';
		ff_data_in_int	<= data_in;	
		crc_init		<= '0';
		data_length		<= data_length;
		odd_event		<= '0';
		start_tx		<= '0';
		if ip_rx.data.data_in_valid = '1' then
			ff_wrreq_int<= '1';
			crc_en		<= '1';
			data_length	<= data_length;
			state_rx	<= sequence_number_1;
		else
			ff_wrreq_int<= '0';
			crc_en		<= '0';
			data_length	<= data_length;
			state_rx	<= identifier_2;
		end if;
	
	-------------------------------------------
	when sequence_number_1 =>
		ff_reset_int	<= '0';
		ff_data_in_int	<= data_in;	
		crc_init		<= '0';
		data_length		<= data_length;
		odd_event		<= '0';
		start_tx		<= '0';
		if ip_rx.data.data_in_valid = '1' then
			ff_wrreq_int<= '1';
			crc_en		<= '0';
			data_length	<= data_length;
			state_rx	<= sequence_number_2;
		else
			ff_wrreq_int<= '0';
			crc_en		<= '0';
			data_length	<= data_length;
			state_rx	<= sequence_number_1;
		end if;
	
	-------------------------------------------
	when sequence_number_2 =>
		ff_reset_int	<= '0';
		ff_data_in_int	<= data_in;	
		crc_init		<= '0';
		data_length		<= data_length;
		odd_event		<= '0';
		start_tx		<= '0';
		if ip_rx.data.data_in_valid = '1' then
			crc_en		<= '1';
			ff_wrreq_int<= '1';
			data_length	<= data_length;
			state_rx	<= data_odd;
		else
			crc_en		<= '0';
			ff_wrreq_int<= '0';
			data_length	<= data_length;
			state_rx	<= sequence_number_2;
		end if;
	
	-------------------------------------------
	when data_odd =>
		ff_reset_int	<= '0';
		ff_data_in_int	<= data_in;	
		crc_init		<= '0';
		data_length		<= data_length;
		start_tx		<= '0';
		if ip_rx.data.data_in_valid = '1' then
			if ip_rx.data.data_in_last = '1' then
				crc_en		<= '0';
				ff_wrreq_int<= '1';
				data_length	<= data_length;
				odd_event	<= '1';
				state_rx	<= last_data_odd;--ajouter un octet à zero pour le checksum	
			else
				crc_en		<= '0';
				ff_wrreq_int<= '1';
				data_length	<= data_length;
				odd_event	<= '0';
				state_rx	<= data_even;
			end if;
		else
			crc_en		<= '0';
			ff_wrreq_int<= '0';
			data_length	<= data_length;
			odd_event	<= '0';
			state_rx	<= data_odd;
		end if;
	
	-------------------------------------------
	when data_even =>
		ff_reset_int	<= '0';
		ff_data_in_int	<= data_in;		
		crc_init		<= '0';
		data_length		<= data_length;
		odd_event		<= '0';
		start_tx		<= '0';
		if ip_rx.data.data_in_valid = '1' then
			if ip_rx.data.data_in_last = '1' then
				crc_en		<= '1';
				ff_wrreq_int<= '1';
				data_length	<= data_length;
				state_rx	<= last_data_even;
			else
				crc_en		<= '1';
				ff_wrreq_int<= '1';
				data_length	<= data_length;
				state_rx	<= data_odd;
			end if;
		else
			crc_en		<= '0';
			ff_wrreq_int<= '0';
			data_length	<= data_length;
			state_rx	<= data_even;
		end if;
	
	-------------------------------------------
	when last_data_odd =>
		ff_reset_int	<= '0';
		ff_wrreq_int	<= '0';
		ff_data_in_int	<= (others => '0');	
		crc_en			<= '1';
		crc_init		<= '0';
		data_length		<=	data_length;
		odd_event		<= '0';
		start_tx		<= '0';
		state_rx		<= start_req_tx;
	
	-------------------------------------------
	when last_data_even =>
		ff_reset_int	<= '0';
		ff_wrreq_int	<= '0';
		ff_data_in_int	<= data_in;	
		crc_en			<= '0';
		crc_init		<= '0';
		data_length		<=	data_length;
		odd_event		<= '0';
		start_tx		<= '0';
		state_rx		<= start_req_tx;
	
	-------------------------------------------
	when start_req_tx =>
		ff_reset_int	<= '0';
		ff_wrreq_int	<= '0';
		--ff_data_in_int	<= (others => '0');	
		crc_en			<= '0';
		crc_init		<= '0';
		data_length		<=	data_length;
		odd_event		<= '0';
		if ack_start_tx = '1' then
			en_count_rx_int	<= '1';
			start_tx		<= '0';
			state_rx		<= idle;
		else
			start_tx		<= '1';
			state_rx		<= start_req_tx;			
		end if;
	
				
			
	-------------------------------------------		
	when wait_for_last =>
		ff_reset_int	<= '1';
		ff_wrreq_int	<= '0';
		crc_en			<= '0';
		crc_init		<= '1';
		data_length		<=	(others => '0');
		odd_event		<= '0';
		start_tx		<= '0';
		if ip_rx.data.data_in_last = '1' then
			state_rx		<= idle;
		else
			state_rx		<= wait_for_last;
		end if;
	
	when others =>
	
	end case;
	

end if;

end process;









-----STATES TX

icmp_tx.data.data_out <= icmp_tx_data;
icmp_tx.data.data_out_valid <= icmp_tx_dv;
icmp_tx.data.data_out_last <= icmp_tx_last;
icmp_tx.hdr.protocol <= x"01";
icmp_tx.hdr.data_length <= icmp_data_length;
icmp_tx.hdr.dst_ip_addr <= dst_ip_1;



icmp_tx_count <= icmp_tx_count_int;

process(tx_clk, reset)
begin
if reset = '1' then
	
	en_count_tx_int		<= '0';
	icmp_tx_count_int		<= (others => '0');
	ip_tx_start				<= '0';
	ff_rdreq_int			<= '0';
	ack_start_tx			<= '0';
	req_ip_layer			<= '0';
	transmit_done			<= '1';
	icmp_tx_data_tmp		<= (others => '0');
	icmp_tx_data			<= (others => '0');
	icmp_tx_dv_tmp			<= '0';
	icmp_tx_dv				<= '0';
	icmp_tx_first_tmp		<= '0';
	icmp_tx_first			<= '0';
	icmp_tx_last			<= '0';
	state_tx 				<= idle_tx;

elsif rising_edge(tx_clk) then
	
	en_count_tx_int	<= '0';

	if en_count_tx_int = '1' then
		icmp_tx_count_int	<= icmp_tx_count_int + 1;
	end if;

	icmp_tx_data	<= icmp_tx_data_tmp;
	icmp_tx_dv		<= icmp_tx_dv_tmp;
	icmp_tx_first	<= icmp_tx_first_tmp;

	case state_tx is
	
	when idle_tx =>
		ff_rdreq_int		<= '0';
		--req_ip_layer		<= '0';
		if start_tx = '1' then
			--ack_start_tx		<= '1';--acknowledge to rx, rx can go idle
			--transmit_done		<= '0';			
			if granted_ip_layer = '1' then--arbiter granted us access to ip_tx layer
				ip_tx_start			<= '1';--tell ip layer to start fsm
				req_ip_layer		<= '1';
				ack_start_tx		<= '1';--acknowledge to rx, rx can go idle
				transmit_done		<= '0';
				icmp_tx_data_tmp		<= (others => '0');
				icmp_tx_dv_tmp			<= '0';
				icmp_tx_first_tmp		<= '0';
				icmp_tx_last		<= '0';
				state_tx 			<= echo_reply_tx;
			else
				ip_tx_start			<= '0';
				req_ip_layer		<= '1';
				ack_start_tx		<= '0';--acknowledge to rx, rx can go idle
				transmit_done		<= '1';
				icmp_tx_data_tmp		<= (others => '0');
				icmp_tx_dv_tmp			<= '0';
				icmp_tx_first_tmp		<= '0';
				icmp_tx_last		<= '0';
				state_tx 			<= idle_tx;
			end if;
		else
			ip_tx_start			<= '0';
			req_ip_layer		<= '0';
			ack_start_tx		<= '0';
			transmit_done		<= '1';
			icmp_tx_data_tmp		<= (others => '0');
			icmp_tx_dv_tmp			<= '0';
			icmp_tx_first_tmp		<= '0';
			icmp_tx_last		<= '0';
			state_tx 			<= idle_tx;		
		end if;
		
	--when WAIT_GRANTED =>
		
			
	
	when echo_reply_tx =>
		ip_tx_start			<= '0';
		if ip_tx_data_out_rdy = '1' then
			ff_rdreq_int		<= '0';
			ack_start_tx		<= '0';
			req_ip_layer		<= '1';
			transmit_done		<= '0';
			icmp_tx_data_tmp		<= (others => '0');--reply 
			icmp_tx_dv_tmp			<= '1';
			icmp_tx_first_tmp		<= '1';--first byte
			icmp_tx_last		<= '0';
			state_tx			<= code_tx;
		end if;
		
	
	when code_tx =>
		if ip_tx_data_out_rdy = '1' then
			ip_tx_start			<= '0';
			ff_rdreq_int		<= '0';
			ack_start_tx		<= '0';
			req_ip_layer		<= '1';
			transmit_done		<= '0';
			icmp_tx_data_tmp		<= (others => '0');--code = 0 
			icmp_tx_dv_tmp			<= '1';
			icmp_tx_first_tmp		<= '0';--
			icmp_tx_last		<= '0';
			state_tx			<= checksum_1_tx;
		end if;
		
	
	when checksum_1_tx =>
		if ip_tx_data_out_rdy = '1' then
			ip_tx_start			<= '0';
			ff_rdreq_int		<= '0';
			ack_start_tx		<= '0';
			req_ip_layer		<= '1';
			transmit_done		<= '0';
			icmp_tx_data_tmp		<= checksum_tx_1(15 downto 8);--checksum MSB
			icmp_tx_dv_tmp			<= '1';
			icmp_tx_first_tmp		<= '0';--
			icmp_tx_last		<= '0';
			state_tx			<= checksum_2_tx;
		end if;
	
	when checksum_2_tx =>
		if ip_tx_data_out_rdy = '1' then
			ip_tx_start			<= '0';
			ff_rdreq_int		<= '1';
			ack_start_tx		<= '0';
			req_ip_layer		<= '1';
			transmit_done		<= '0';
			icmp_tx_data_tmp		<= checksum_tx_1(7 downto 0);--checksum LSB
			icmp_tx_dv_tmp			<= '1';
			icmp_tx_first_tmp		<= '0';
			icmp_tx_last		<= '0';
			state_tx			<= data_tx;
		end if;
	
	when data_tx =>
		ip_tx_start			<= '0';
		if ip_tx_data_out_rdy = '1' then
			if ff_empty_int = '1' then
				en_count_tx_int	<= '1';
				ff_rdreq_int		<= '0';
				ack_start_tx		<= '0';
				req_ip_layer		<= '0';
				transmit_done		<= '1';
				icmp_tx_data_tmp		<= ff_data_out_int;--checksum LSB
				icmp_tx_dv_tmp			<= '0';
				icmp_tx_first_tmp		<= '0';
				icmp_tx_last		<= '1';
				state_tx			<= idle_tx;
			else
				ff_rdreq_int		<= '1';
				ack_start_tx		<= '0';
				req_ip_layer		<= '1';
				transmit_done		<= '0';
				icmp_tx_data_tmp		<= ff_data_out_int;--checksum LSB
				icmp_tx_dv_tmp			<= '1';
				icmp_tx_first_tmp		<= '0';
				icmp_tx_last		<= '0';
				state_tx			<= data_tx;
			end if;
		else
			ff_rdreq_int		<= '0';
			ack_start_tx		<= '0';
			req_ip_layer		<= '1';
			transmit_done		<= '0';
			icmp_tx_data_tmp		<= ff_data_out_int;--checksum LSB
			icmp_tx_dv_tmp			<= '0';
			icmp_tx_first_tmp		<= '0';
			icmp_tx_last		<= '0';
			state_tx			<= reload_ff_read_req;
		end if;
	
	when reload_ff_read_req =>--pas certain
		if ip_tx_data_out_rdy = '1' then
			ip_tx_start			<= '0';
			ff_rdreq_int		<= '1';
			ack_start_tx		<= '0';
			req_ip_layer		<= '1';
			transmit_done		<= '0';
			icmp_tx_data_tmp		<= ff_data_out_int;--checksum LSB
			icmp_tx_dv_tmp			<= '0';
			icmp_tx_first_tmp		<= '0';
			icmp_tx_last		<= '0';
			state_tx			<= data_tx;
		end if;
			
		
	
	when others =>
		ip_tx_start			<= '0';	
		ff_rdreq_int		<= '0';
		ack_start_tx		<= '0';
		req_ip_layer		<= '0';
		transmit_done		<= '1';
		icmp_tx_data_tmp		<= (others => '0');
		icmp_tx_dv_tmp			<= '0';
		icmp_tx_first_tmp		<= '0';
		icmp_tx_last		<= '0';
		state_tx 			<= idle_tx;
	end case;


end if;
end process;


end architecture;
