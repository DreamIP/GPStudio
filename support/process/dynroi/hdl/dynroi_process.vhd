library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity dynroi_process is
	generic (
		CLK_PROC_FREQ : integer;
		BINIMG_SIZE   : integer;
		IMG_SIZE      : integer;
		ROI_SIZE      : integer;
		COORD_SIZE    : integer
	);
	port (
		clk_proc                  : in std_logic;
		reset_n                   : in std_logic;

		---------------- dynamic parameters ports ---------------
		status_reg_enable_bit     : in std_logic;
		status_reg_bypass_bit     : in std_logic;
		status_reg_static_res_bit : in std_logic;
		inImg_size_reg_in_w_reg   : in std_logic_vector(11 downto 0);
		inImg_size_reg_in_h_reg   : in std_logic_vector(11 downto 0);
		BinImg_size_reg_in_w_reg  : in std_logic_vector(11 downto 0);
		BinImg_size_reg_in_h_reg  : in std_logic_vector(11 downto 0);
		out_size_reg_out_w_reg    : in std_logic_vector(11 downto 0);
		out_size_reg_out_h_reg    : in std_logic_vector(11 downto 0);

		----------------------- BinImg flow ---------------------
		BinImg_data               : in std_logic_vector(BINIMG_SIZE-1 downto 0);
		BinImg_fv                 : in std_logic;
		BinImg_dv                 : in std_logic;

		------------------------ Img flow -----------------------
		Img_data                  : in std_logic_vector(IMG_SIZE-1 downto 0);
		Img_fv                    : in std_logic;
		Img_dv                    : in std_logic;

		------------------------ roi flow -----------------------
		roi_data                  : out std_logic_vector(ROI_SIZE-1 downto 0);
		roi_fv                    : out std_logic;
		roi_dv                    : out std_logic;

		----------------------- coord flow ----------------------
		coord_data                : out std_logic_vector(COORD_SIZE-1 downto 0);
		coord_fv                  : out std_logic;
		coord_dv                  : out std_logic
	);
end dynroi_process;

architecture rtl of dynroi_process is

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

--conversion offset from binImg to img
signal conv_offset_x : unsigned(X_COUNTER_SIZE-1 downto 0);
signal conv_offset_y : unsigned(Y_COUNTER_SIZE-1 downto 0);

--Coord over serial line related vars
signal frame_buffer 		        : std_logic_vector(63 downto 0);
signal frame_buffer_has_been_filled : std_logic;
signal frame_buffer_has_been_sent	: std_logic;
signal frame_buffer_position        : unsigned(6 downto 0);



begin
------------------------------------------------------------------------
	 apply_roi : process(clk_proc, reset_n)
	--img reference coordinates
	variable x		: unsigned(X_COUNTER_SIZE-1 downto 0);
	variable y 		: unsigned(Y_COUNTER_SIZE-1 downto 0);

	variable w		: unsigned(X_COUNTER_SIZE-1 downto 0);
	variable h		: unsigned(Y_COUNTER_SIZE-1 downto 0);

	variable xImg_pos : unsigned(X_COUNTER_SIZE-1 downto 0);
	variable yImg_pos : unsigned(Y_COUNTER_SIZE-1 downto 0);	 
	begin
		 if(reset_n='0') then
			 -- reset pixel counters
			 xImg_pos := to_unsigned(0, X_COUNTER_SIZE);
			 yImg_pos := to_unsigned(0, Y_COUNTER_SIZE);
			 -- reset ROI coord : default is central frame with param.w/h values
			 x 	:= (others => '0');
			 y 	:= (others => '0');
			 w 	:= (others => '0');
			 h 	:= (others => '0');			
			 roi_data <= (others => '0');
			 roi_dv <= '0';
			 roi_fv <= '0';
		 elsif(rising_edge(clk_proc)) then
             if Img_fv = '1' and enabled = '1' then
                roi_fv <= '1';
             else
                roi_fv <= '0';
             end if;
			 roi_dv <= '0';
			 roi_data <= (others => '1');
			 	 
			 --Updating last frame coordinates
			 if frame_buffer_has_been_filled = '1' and enabled = '1' then
				 x := unsigned(frame_buffer(X_COUNTER_SIZE-1 downto 0));
				 y := unsigned(frame_buffer(Y_COUNTER_SIZE+15 downto 16));
				 w := unsigned(frame_buffer(X_COUNTER_SIZE+31 downto 32));
				 h := unsigned(frame_buffer(Y_COUNTER_SIZE+47 downto 48));
			 end if;
				 
			 -- ROI action	 
			 if Img_fv = '0' then
				 --reset pixel counters
				 xImg_pos 	 	   := to_unsigned(0, X_COUNTER_SIZE);
				 yImg_pos    	   := to_unsigned(0, Y_COUNTER_SIZE);
			 else
				 if Img_dv = '1' and enabled = '1' then
	 				 --ROI						
					 if(yBin_pos >= y
					 and yBin_pos < y + h
					 and xBin_pos >= x
					 and xBin_pos < x + w )then
						 roi_dv <= '1';								
						 roi_data <= Img_data;
					 end if;
					 					 
					 --Pixel counter in img
					 xImg_pos := xImg_pos + 1;
					 if(xImg_pos=unsigned(inImg_size_reg_in_w_reg)) then
						 yImg_pos := yImg_pos + 1;
						 xImg_pos := to_unsigned(0, X_COUNTER_SIZE);
					 end if;
				 end if;
			 end if;
			 
		 end if;
	 end process;
------------------------------------------------------------------------
	
------------------------------------------------------------------------
	data_process : process (clk_proc, reset_n)
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
			x_min 			<= (others => '0');
			y_min 			<= (others => '0');
			--Cleaning frame buffer
			frame_buffer    <= (others=>'0');
			--Cleaning signals used to fill buffer
			frame_buffer_has_been_filled <= '0';
			frame_buffer_has_been_sent	 <= '0';
			coord_fv 	<= '0';
			coord_dv 	<= '0';
			coord_data 	<= (others=>'0');
			--Cleaning flags
			enabled 	<= '0';
			--Cleaning conv offset
			conv_offset_x <= (others => '0');
			conv_offset_y <= (others => '0');
		elsif(rising_edge(clk_proc)) then
			coord_fv 	<= '0';
			coord_dv 	<= '0';
			coord_data 	<= (others=>'0');
			--offset calculation
			conv_offset_x <= (unsigned(inImg_size_reg_in_w_reg)-unsigned(BinImg_size_reg_in_w_reg))/2;
			conv_offset_y <= (unsigned(inImg_size_reg_in_h_reg)-unsigned(BinImg_size_reg_in_h_reg))/2;
			
            if(BinImg_fv = '0') then
                xBin_pos <= to_unsigned(0, X_COUNTER_SIZE);
                yBin_pos <= to_unsigned(0, Y_COUNTER_SIZE);
				
				
				--
				if frame_buffer_has_been_filled = '0' then				

					--We send frame coordinates only if there is something to send
					if enabled = '1' and frame_buffer_has_been_sent	 = '0' then
						
						if status_reg_bypass_bit = '1' then
							x_to_send := (others=>'0'); 
							y_to_send := (others=>'0');
							w_to_send := unsigned(inImg_size_reg_in_w_reg); 
							h_to_send := unsigned(inImg_size_reg_in_h_reg);
						else
							----roi resolution fixed by user
							if status_reg_static_res_bit = '1' then
							
								--something was detected
								if x_max > 0 then
									--checking top left corner position to ensure frame width is matching static_res 
									if x_min + conv_offset_x > (unsigned(inImg_size_reg_in_w_reg)-unsigned(out_size_reg_out_w_reg)) then
										x_to_send := unsigned(inImg_size_reg_in_w_reg) - unsigned(out_size_reg_out_w_reg);
									else
										x_to_send := x_min + conv_offset_x;										
									end if;									
									if y_min + conv_offset_y > (unsigned(inImg_size_reg_in_h_reg)-unsigned(out_size_reg_out_h_reg)) then
										y_to_send := (unsigned(inImg_size_reg_in_h_reg)-unsigned(out_size_reg_out_h_reg));
									else
										y_to_send := y_min + conv_offset_y;										
									end if;
									w_to_send := unsigned(out_size_reg_out_w_reg) ; 
									h_to_send := unsigned(out_size_reg_out_h_reg);
								else
								--nothing found
									x_to_send := (unsigned(inImg_size_reg_in_w_reg)-unsigned(out_size_reg_out_w_reg))/2 ; 
									y_to_send := (unsigned(inImg_size_reg_in_h_reg)-unsigned(out_size_reg_out_h_reg))/2;
									w_to_send := unsigned(out_size_reg_out_w_reg); 
									h_to_send := unsigned(out_size_reg_out_h_reg);								
								end if;
								
							----dynamic resolution for roi
							else
							
								--something was detected
								if x_max > 0 then
									x_to_send := x_min + conv_offset_x; 
									y_to_send := y_min + conv_offset_y;
									w_to_send := x_max-x_min; 
									h_to_send := y_max-y_min;
								else
								--nothing found -> empty rectangle at image center
									x_to_send := unsigned(inImg_size_reg_in_w_reg)/2; 
									y_to_send := unsigned(inImg_size_reg_in_h_reg)/2;
									w_to_send := (others=>'0'); 
									h_to_send := (others=>'0');								
								end if;
							
							end if;
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
					x_min 		<= unsigned(BinImg_size_reg_in_w_reg);
					y_min 		<= unsigned(BinImg_size_reg_in_h_reg);
					--To prevent sending coord after reset of x/y_min/max
					frame_buffer_has_been_sent <= '1';
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
				
				enabled  	<= status_reg_enable_bit;

            else
				coord_fv 	<= '0';
				coord_dv 	<= '0';
				coord_data 	<= (others=>'0');
				
				frame_buffer_has_been_sent	 <= '0';
                
				if status_reg_enable_bit = '1' and enabled = '1' then
					
					if(BinImg_dv = '1' ) then
					
						--bin img pixel counter
						xBin_pos <= xBin_pos + 1;
						if(xBin_pos=unsigned(BinImg_size_reg_in_w_reg)-1) then
							yBin_pos <= yBin_pos + 1;
							xBin_pos <= to_unsigned(0, X_COUNTER_SIZE);
						end if;
							
						-- This will give the smallest area including all non-black points
						if BinImg_data /= (BinImg_data'range => '0') then
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


