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
signal static_res_enabled : std_logic;

signal current_image_data_send : std_logic;

--process apply_roi vars
--img reference coordinates
signal x		: unsigned(X_COUNTER_SIZE-1 downto 0);
signal y 		: unsigned(Y_COUNTER_SIZE-1 downto 0);

signal w		: unsigned(X_COUNTER_SIZE-1 downto 0);
signal h		: unsigned(Y_COUNTER_SIZE-1 downto 0);

signal xImg_pos : unsigned(X_COUNTER_SIZE-1 downto 0);
signal yImg_pos : unsigned(Y_COUNTER_SIZE-1 downto 0);



--process send roi vars
signal frame_buffer 		        : std_logic_vector(63 downto 0);
signal frame_buffer_position        : unsigned(6 downto 0);
signal frame_buffer_has_been_filled : std_logic;

begin
------------------------------------------------------------------------
	send_roi : process(clk_proc, reset_n)
	begin
	if reset_n='0' then
	--Cleaning frame_buffer
	frame_buffer 			   <= (others=>'0');
	frame_buffer_position      <= (others=>'0') ;
	--Cleaning signals used to fill buffer
	frame_buffer_has_been_filled <= '0';
	-- 	Cleaning output
	coord_data 	<= (others=>'0');
	coord_fv <= '0';
	coord_dv <= '0';
	else
	--coord are sent once for each img
	coord_fv <= '0';
	coord_dv <= '0';
	--bin img and original img processing times are differents:
	--here we want the coord processed with bin img to be sent
	--when the original image processing is finished
	if Img_fv = '0' then
		--Bypass case is managed by data_process	
		if frame_buffer_has_been_filled = '1' then
			if enabled = '1' then
				coord_fv <= '1';
				coord_dv <= '1';
				coord_data <= frame_buffer(to_integer(frame_buffer_position)+7 downto to_integer(frame_buffer_position));
				
				if frame_buffer_position >= 56 then
					frame_buffer_has_been_filled <= '0';
				else
					frame_buffer_position <= frame_buffer_position + to_unsigned(8, 7);
				end if;
			end if;
		end if;
	end if;
	end process;
------------------------------------------------------------------------

------------------------------------------------------------------------
	apply_roi : process(clk_proc, reset_n)
	begin
		if(reset_n='0') then
			-- reset pixel counters
			xImg_pos <= to_unsigned(0, X_COUNTER_SIZE);
			yImg_pos <= to_unsigned(0, Y_COUNTER_SIZE);
			-- reset ROI coord : default is central frame with param.w/h values
			x 	<= ('0'&BinImg_size_reg_in_w_reg(10 downto 0))-('0'&out_size_reg_out_w_reg(10 downto 0));
			y 	<= ('0'&BinImg_size_reg_in_h_reg(10 downto 0))-('0'&out_size_reg_out_h_reg(10 downto 0));
			w 	<= unsigned(BinImg_size_reg_in_w_reg);
			h 	<= unsigned(BinImg_size_reg_in_h_reg);			
			roi_data <= (others => '0');
			roi_dv <= '0';
			roi_fv <= '0';
		else
			if Img_fv = '0' then
				roi_fv <= '0';					
				roi_dv <= '0';
				roi_data <= (others => '0');
				-- todo update coord x,y,w,h with img reference
				xBin_pos 	 	   <= to_unsigned(0, X_COUNTER_SIZE);
				yBin_pos    	   <= to_unsigned(0, Y_COUNTER_SIZE);
				--Bypass case is managed by data_process	
				--Updating last frame coordinates
				x <= frame_buffer(X_COUNTER_SIZE-1 downto 0);
				y <= frame_buffer(Y_COUNTER_SIZE+15 downto 16);
				w <= frame_buffer(X_COUNTER_SIZE+31 downto 32);
				h <= frame_buffer(Y_COUNTER_SIZE+47 downto 48);
			else
				if enabled =  '1' then
					roi_fv =  '1';						
					roi_dv <= '0';
					--Pixel counter in img
					xImg_pos <= xImg_pos + 1;
					if(xImg_pos=unsigned(in_size_reg_in_w_reg)-1) then
						yImg_pos <= yImg_pos + 1;
						xImg_pos <= to_unsigned(0, X_COUNTER_SIZE);
					end if;
					
					if Img_dv = '1' then
						--ROI						
						if(yBin_pos >= y
						and yBin_pos < y + h
						and xBin_pos >= x
						and xBin_pos < x + w )then
							roi_dv <= '1';								
							roi_data <= Img_data;
						end if;
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
			x_max 		<= (others=>'0');
			y_max 		<= (others=>'0');
			x_min 		<= unsigned(BinImg_size_reg_in_w_reg);
			y_min 		<= unsigned(BinImg_size_reg_in_h_reg);
			current_image_data_send <= '1';
			
		elsif(rising_edge(clk_proc)) then

            if(BinImg_fv = '0') then
                xBin_pos <= to_unsigned(0, X_COUNTER_SIZE);
                yBin_pos <= to_unsigned(0, Y_COUNTER_SIZE);
				
				
				--
				if frame_buffer_has_been_filled = '0' then				

					--We send frame coordinates only if there is something to send
					if enabled = '1' and current_image_data_send = '0' then
						
						if status_reg_bypass_bit = '1' then
							x_to_send := 0; 
							y_to_send := 0;
							w_to_send := inImg_size_reg_in_w_reg; 
							h_to_send := inImg_size_reg_in_h_reg;
						else
							--roi resolution fixed by user
							if status_reg_static_res_bit = '1' then
							
								--something was detected
								if x_max > 0 then
									--checking top left corner position to ensure frame width is matching static_res 
									if x_min > (unsigned(inImg_size_reg_in_w_reg)-unsigned(out_size_reg_out_w_reg)) then
										x_to_send := (unsigned(inImg_size_reg_in_w_reg)-unsigned(out_size_reg_out_w_reg));
									else
										x_to_send := x_min;										
									end if;									
									if y_min > (unsigned(inImg_size_reg_in_h_reg)-unsigned(out_size_reg_out_h_reg)) then
										y_to_send := (unsigned(inImg_size_reg_in_h_reg)-unsigned(out_size_reg_out_h_reg));
									else
										y_to_send := y_min;										
									end if;
									w_to_send := unsigned(out_size_reg_out_w_reg) ; 
									h_to_send := unsigned(out_size_reg_out_h_reg);
								else
								--nothing found
									x_to_send := (unsigned(inImg_size_reg_in_w_reg)-unsigned(BinImg_size_reg_in_w_reg))/2 ; 
									y_to_send := (unsigned(inImg_size_reg_in_h_reg)-unsigned(BinImg_size_reg_in_h_reg))/2;
									w_to_send := inImg_size_reg_in_w_reg ; 
									h_to_send := inImg_size_reg_in_h_reg;								
								end if;
								
							--dynamic resolution for roi
							else
							
								--something was detected
								if x_max > 0 then
									x_to_send := x_min; 
									y_to_send := y_min;
									w_to_send := x_max-x_min; 
									h_to_send := y_max-y_min;
								else
								--nothing found -> empty rectangle at image center
									x_to_send := inImg_size_reg_in_w_reg/2; 
									y_to_send := inImg_size_reg_in_h_reg/2;
									w_to_send := 0; 
									h_to_send := 0;								
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
						--To fill buffer once for each image
						current_image_data_send <= '1';
						
					end if;
					
					--Cleaning frame coordinates
					x_max 		<= (others=>'0');
					y_max 		<= (others=>'0');
					x_min 		<= unsigned(BinImg_size_reg_in_w_reg);
					y_min 		<= unsigned(BinImg_size_reg_in_h_reg);
				end if;
				--
	
                enabled  <= status_reg_enable_bit;

                
            else
       			current_image_data_send <= '0';

				if status_reg_enable_bit = '1' and enabled = '1' then
					
					if(BinImg_dv = '1' ) then
					
						--bin img pixel counter
						xBin_pos <= xBin_pos + 1;
						if(xBin_pos=unsigned(in_size_reg_in_w_reg)-1) then
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

