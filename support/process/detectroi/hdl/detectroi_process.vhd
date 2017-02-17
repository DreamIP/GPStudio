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

signal x_pos    : unsigned(X_COUNTER_SIZE-1 downto 0);
signal y_pos    : unsigned(Y_COUNTER_SIZE-1 downto 0);

signal x_min	: unsigned(X_COUNTER_SIZE-1 downto 0);
signal x_max	: unsigned(X_COUNTER_SIZE-1 downto 0);
signal x		: unsigned(X_COUNTER_SIZE-1 downto 0);

signal y_min	: unsigned(Y_COUNTER_SIZE-1 downto 0);
signal y_max	: unsigned(Y_COUNTER_SIZE-1 downto 0);
signal y 		: unsigned(Y_COUNTER_SIZE-1 downto 0);

signal w		: unsigned(X_COUNTER_SIZE-1 downto 0);
signal h		: unsigned(Y_COUNTER_SIZE-1 downto 0);

signal frame_buffer 		        : std_logic_vector(63 downto 0);
signal frame_buffer_position        : unsigned(6 downto 0);
signal frame_buffer_has_been_filled : std_logic;

signal enabled : std_logic ;

begin
	data_process : process (clk_proc, reset_n)
	begin
		if(reset_n='0') then
			enabled <= '0';				 
			x_pos 	 	   <= to_unsigned(0, X_COUNTER_SIZE);
            y_pos    	   <= to_unsigned(0, Y_COUNTER_SIZE);
            --Cleaning frame_buffer
            frame_buffer 			   <= (others=>'0');
            frame_buffer_position      <= (others=>'0') ;
			--Cleaning signals used to fill buffer
			frame_buffer_has_been_filled <= '0';
			x_max 		<= (others=>'0');
			y_max 		<= (others=>'0');
			x_min 		<= unsigned(in_size_reg_in_w_reg);
			y_min 		<= unsigned(in_size_reg_in_h_reg);
			-- 	Cleaning output
			coord_data 	<= (others=>'0');
			coord_fv 	<= '0';
			coord_dv 	<= '0';
		elsif(rising_edge(clk_proc)) then
			coord_fv 	<= '0';
			coord_data 	<= (others=>'0');
			coord_dv 	<= '0';
			if in_fv = '0' then
				x_pos 	 	   <= to_unsigned(0, X_COUNTER_SIZE);
				y_pos    	   <= to_unsigned(0, Y_COUNTER_SIZE);
				-- Sending coordinates at the end of the last image
				-- GUI takes this order [x0 lsb, x0 msb, y0 lsb, y0 msb, w0 lsb, w0 msb, h0 lsb, h0 msb]
				-- Filling buffer
				if frame_buffer_has_been_filled = '0' then
					--We send frame coordinates only if there is something to send
					if x_max>0 and enabled = '1' then
						frame_buffer(X_COUNTER_SIZE-1 downto 0)   <= std_logic_vector(x_min);
						frame_buffer(15 downto X_COUNTER_SIZE)    <= (others=>'0');
											
						frame_buffer(Y_COUNTER_SIZE+15 downto 16) <= std_logic_vector(y_min);
						frame_buffer(31 downto X_COUNTER_SIZE+16) <= (others=>'0');
											
						frame_buffer(X_COUNTER_SIZE+31 downto 32) <= std_logic_vector(x_max-x_min);
						frame_buffer(47 downto X_COUNTER_SIZE+32) <= (others=>'0');
						
						frame_buffer(Y_COUNTER_SIZE+47 downto 48) <= std_logic_vector(y_max-y_min);
						frame_buffer(63 downto X_COUNTER_SIZE+48) <= (others=>'0');
						
						-- Get buffer ready to send
						frame_buffer_has_been_filled <= '1';
						frame_buffer_position		 <= (others=>'0') ;
					end if;
					
					--Clearing signals used to fill buffer
					x_max <= (others=>'0');
					y_max <= (others=>'0');
					x_min <= unsigned(in_size_reg_in_w_reg);
					y_min <= unsigned(in_size_reg_in_h_reg);
							
				-- Sending buffer (only one time)	
				else
					if enabled = '1' then
						coord_fv <= '1';
						coord_dv <= '1';
					end if;
					
					coord_data <= frame_buffer(to_integer(frame_buffer_position)+7 downto to_integer(frame_buffer_position));
					
					if frame_buffer_position >= 56 then
						frame_buffer_has_been_filled <= '0';
					else
						frame_buffer_position <= frame_buffer_position + to_unsigned(8, 7);
					end if;

				end if;				
				
				-- If processing has been disabled before the end of an image,
				-- x and y values are reset and nothing will be sent for this image
				enabled <= status_reg_enable_bit;
				
			else
				if  enabled = '1' and status_reg_enable_bit = '1' then					
					
					if in_dv='1' then
						-- Following current line
						x_pos <= x_pos + 1;
						-- End of line
						if x_pos=unsigned(in_size_reg_in_w_reg)-1 then
							y_pos <= y_pos + 1;
							x_pos <= to_unsigned(0, X_COUNTER_SIZE);
						end if;
						
						-- This will give the smallest area including all non-black points
						if in_data /= (in_data'range => '0') then
							if x_pos < x_min then
								x_min <= x_pos;
							end if;
							if x_pos > x_max then
								x_max <= x_pos;
							end if;
							--							
							if y_pos > x_max then
								y_max <= y_pos;
							end if;		
							if y_pos < y_min then
								y_min <= y_pos;
							end if;
			
						end if;
					end if;
				else
					-- Disabled, may change at next image beginning
					enabled <= '0';
				end if;
			end if;
		end if;
	end process;
end rtl;
