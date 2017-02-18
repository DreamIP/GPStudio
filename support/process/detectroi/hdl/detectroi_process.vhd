library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity detectroi_process is
	generic (
		CLK_PROC_FREQ : integer;
		IN_SIZE       : integer;
		COORD_SIZE    : integer
	);
	port (
		clk_proc              : in std_logic;
		reset_n               : in std_logic;

		---------------- dynamic parameters ports ---------------
		status_reg_enable_bit : in std_logic;
		in_size_reg_in_w_reg  : in std_logic_vector(11 downto 0);
		in_size_reg_in_h_reg  : in std_logic_vector(11 downto 0);

		------------------------- in flow -----------------------
		in_data               : in std_logic_vector(IN_SIZE-1 downto 0);
		in_fv                 : in std_logic;
		in_dv                 : in std_logic;

		----------------------- coord flow ----------------------
		coord_data            : out std_logic_vector(COORD_SIZE-1 downto 0);
		coord_fv              : out std_logic;
		coord_dv              : out std_logic
	);
end detectroi_process;

architecture rtl of detectroi_process is
constant X_COUNTER_SIZE : integer := 12;
constant Y_COUNTER_SIZE : integer := 12;

--process data_process vars
--bin image reference coordinates
signal xBin_pos : unsigned(X_COUNTER_SIZE-1 downto 0);
signal yBin_pos : unsigned(Y_COUNTER_SIZE-1 downto 0);

signal x_min	: unsigned(X_COUNTER_SIZE-1 downto 0);
signal x_max	: unsigned(X_COUNTER_SIZE-1 downto 0);

signal y_min	: unsigned(Y_COUNTER_SIZE-1 downto 0);
signal y_max	: unsigned(Y_COUNTER_SIZE-1 downto 0);

signal enabled  : std_logic;

signal frame_buffer 		        : std_logic_vector(63 downto 0);
signal frame_buffer_has_been_filled : std_logic;
signal frame_buffer_has_been_sent	: std_logic;
signal frame_buffer_position        : unsigned(6 downto 0);

begin

	send_roi : process (clk_proc, reset_n)
	-- Vars used to fill frame_buffer
	variable x_to_send		: unsigned(X_COUNTER_SIZE-1 downto 0);
	variable y_to_send 		: unsigned(Y_COUNTER_SIZE-1 downto 0);
	variable w_to_send		: unsigned(X_COUNTER_SIZE-1 downto 0);
	variable h_to_send		: unsigned(Y_COUNTER_SIZE-1 downto 0);
	begin
		if(reset_n='0') then
			xBin_pos 	 	   <= to_unsigned(0, X_COUNTER_SIZE);
            yBin_pos    	   <= to_unsigned(0, Y_COUNTER_SIZE);
			--Cleaning frame coordinates
			x_max 			<= (others=>'0');
			y_max 			<= (others=>'0');
			x_min 			<= unsigned(in_size_reg_in_w_reg);
			y_min 			<= unsigned(in_size_reg_in_h_reg);
			--Cleaning frame buffer
			frame_buffer    <= (others=>'0');
			--Cleaning signals used to fill buffer
			frame_buffer_has_been_filled <= '0';
			frame_buffer_has_been_sent	 <= '0';
			coord_fv 	<= '0';
			coord_dv 	<= '0';
			coord_data 	<= (others=>'0');
			enabled <= '0';
		elsif(rising_edge(clk_proc)) then
			coord_fv 	<= '0';
			coord_dv 	<= '0';
			coord_data 	<= (others=>'0');
            if(in_fv = '0') then
                xBin_pos <= to_unsigned(0, X_COUNTER_SIZE);
                yBin_pos <= to_unsigned(0, Y_COUNTER_SIZE);
				
				
				--
				if frame_buffer_has_been_filled = '0' then				

					--We send frame coordinates only if there is something to send
					if enabled = '1' and frame_buffer_has_been_sent	 = '0' then
						
						--something was detected
						if x_max > 0 then
							x_to_send := x_min; 
							y_to_send := y_min;
							w_to_send := x_max-x_min; 
							h_to_send := y_max-y_min;
						else
						--nothing found -> empty rectangle at image center
							x_to_send := unsigned(in_size_reg_in_w_reg)/2; 
							y_to_send := unsigned(in_size_reg_in_h_reg)/2;
							w_to_send := (others=>'0'); 
							h_to_send := (others=>'0');								
						end if;
						
						
						--filling buffer with matching coordinates
						frame_buffer(X_COUNTER_SIZE-1 downto 0)   <= std_logic_vector(x_to_send);
						frame_buffer(Y_COUNTER_SIZE+15 downto 16) <= std_logic_vector(y_to_send);
						frame_buffer(X_COUNTER_SIZE+31 downto 32) <= std_logic_vector(w_to_send);
						frame_buffer(Y_COUNTER_SIZE+47 downto 48) <= std_logic_vector(h_to_send);							
						--zero padding, each value has 16 bits but only uses XY_COUNTER_SIZE  bits
						frame_buffer(15 downto X_COUNTER_SIZE)    <= (others=>'0');
						frame_buffer(31 downto X_COUNTER_SIZE+16) <= (others=>'0');
						frame_buffer(47 downto X_COUNTER_SIZE+32) <= (others=>'0');
						frame_buffer(63 downto X_COUNTER_SIZE+48) <= (others=>'0');
						
						-- Get buffer ready to send
						frame_buffer_has_been_filled <= '1';
						frame_buffer_position		 <= (others=>'0') ;
								
					end if;
					
					--Cleaning frame coordinates
					x_max 		<= (others=>'0');
					y_max 		<= (others=>'0');
					x_min 		<= unsigned(in_size_reg_in_w_reg);
					y_min 		<= unsigned(in_size_reg_in_h_reg);
				else
					--send roi coord
					coord_fv <= '1';
					coord_dv <= '1';
					coord_data <= frame_buffer(to_integer(frame_buffer_position)+7 downto to_integer(frame_buffer_position));
					
					if frame_buffer_position >= 56 then
						frame_buffer_has_been_filled <= '0';
						frame_buffer_has_been_sent	 <= '1';

					else
						frame_buffer_position <= frame_buffer_position + to_unsigned(8, 7);
					end if;
					
				end if;
				
				enabled  <= status_reg_enable_bit;
                
            else
				coord_fv 	<= '0';
				coord_dv 	<= '0';
				coord_data 	<= (others=>'0');
				frame_buffer_has_been_sent	 <= '0';

				if status_reg_enable_bit = '1' and enabled = '1' then
					
					if(in_dv = '1' ) then
					
						--bin img pixel counter
						xBin_pos <= xBin_pos + 1;
						if(xBin_pos=unsigned(in_size_reg_in_w_reg)-1) then
							yBin_pos <= yBin_pos + 1;
							xBin_pos <= to_unsigned(0, X_COUNTER_SIZE);
						end if;
							
						-- This will give the smallest area including all non-black points
						if in_data /= (in_data'range => '0') then
							if xBin_pos < x_min then
								x_min <= xBin_pos;
							end if;
							if xBin_pos > x_max then
								x_max <= xBin_pos;
							end if;
							--
							if yBin_pos < y_min then
								y_min <= yBin_pos;
							end if;
							if yBin_pos > y_max then
								y_max <= yBin_pos;
							end if;				
						end if;
						
					end if;
				else 
					enabled <= '0';
				end if;
            end if;
		end if;
	end process;
end rtl;

