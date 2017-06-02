library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity draw_process is
	generic (
		CLK_PROC_FREQ : integer;
		IMG_SIZE      : integer;
		COORD_SIZE    : integer
	);
	port (
		clk_proc                  : in std_logic;
		reset_n                   : in std_logic;

		---------------- dynamic parameters ports ---------------
		status_reg_enable_bit     : in std_logic;
		inImg_size_reg_in_w_reg   : in std_logic_vector(11 downto 0);
		inImg_size_reg_in_h_reg   : in std_logic_vector(11 downto 0);

		------------------------ Img flow -----------------------
		Img_data                  : in std_logic_vector(IMG_SIZE-1 downto 0);
		Img_fv                    : in std_logic;
		Img_dv                    : in std_logic;

		----------------------- coord flow ----------------------
		coord_data                : out std_logic_vector(COORD_SIZE-1 downto 0);
		coord_fv                  : out std_logic;
		coord_dv                  : out std_logic
	);
end draw_process;

architecture rtl of draw_process is

--process data_process vars
signal enabled  : std_logic;

type            keypoints_coord  is array (49 downto 0) of std_logic_vector(15 downto 0); 					 
signal          x_keypoint : keypoints_coord;
signal          y_keypoint : keypoints_coord;   

--Coord over serial line related vars
signal frame_buffer 		        : std_logic_vector(1599 downto 0); -- PARAM TO CHANGE
signal frame_buffer_has_been_filled : std_logic;
signal frame_buffer_has_been_sent	: std_logic;
signal frame_buffer_position        : unsigned(12 downto 0); -- PARAM TO CHANGE

begin
	
	data_process : process (clk_proc, reset_n)
	
	variable c : integer :=0;

	begin
		  if(reset_n='0') then			
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
			 x_keypoint <= ((others =>(others =>'0')));
			 y_keypoint <= ((others =>(others =>'0')));
			
			
		  elsif(rising_edge(clk_proc)) then
		  
		  
		  if(Img_dv='1')then
		  
			x_keypoint(0) <= std_logic_vector(to_unsigned(10,16)); -- x0
			y_keypoint(0) <= std_logic_vector(to_unsigned(10,16)); -- x0
			
			x_keypoint(1) <= std_logic_vector(to_unsigned(12,16)); -- 
			y_keypoint(1) <= std_logic_vector(to_unsigned(12,16)); -- 
			
			x_keypoint(2) <= std_logic_vector(to_unsigned(14,16)); -- 
			y_keypoint(2) <= std_logic_vector(to_unsigned(14,16)); -- 
			
			x_keypoint(3) <= std_logic_vector(to_unsigned(16,16)); -- 
			y_keypoint(3) <= std_logic_vector(to_unsigned(16,16)); -- 
			
			x_keypoint(4) <= std_logic_vector(to_unsigned(18,16)); -- 
			y_keypoint(4) <= std_logic_vector(to_unsigned(18,16)); -- 
			
			x_keypoint(5) <= std_logic_vector(to_unsigned(20,16)); -- 
			y_keypoint(5) <= std_logic_vector(to_unsigned(20,16)); -- 
			
			x_keypoint(6) <= std_logic_vector(to_unsigned(22,16)); -- 
			y_keypoint(6) <= std_logic_vector(to_unsigned(22,16)); -- 
			
			x_keypoint(7) <= std_logic_vector(to_unsigned(24,16)); -- 
			y_keypoint(7) <= std_logic_vector(to_unsigned(24,16)); -- 
			
			x_keypoint(8) <= std_logic_vector(to_unsigned(26,16)); -- 
			y_keypoint(8) <= std_logic_vector(to_unsigned(26,16)); -- 
			
			x_keypoint(9) <= std_logic_vector(to_unsigned(28,16)); -- 
			y_keypoint(9) <= std_logic_vector(to_unsigned(28,16)); -- 
			
			---------------------------------------------------------
			
			x_keypoint(10) <= std_logic_vector(to_unsigned(30,16)); -- x10
			y_keypoint(10) <= std_logic_vector(to_unsigned(30,16)); -- x10
			
			x_keypoint(11) <= std_logic_vector(to_unsigned(32,16)); -- 
			y_keypoint(11) <= std_logic_vector(to_unsigned(32,16)); -- 
			
			x_keypoint(12) <= std_logic_vector(to_unsigned(34,16)); -- 
			y_keypoint(12) <= std_logic_vector(to_unsigned(34,16)); -- 
			
			x_keypoint(13) <= std_logic_vector(to_unsigned(36,16)); -- 
			y_keypoint(13) <= std_logic_vector(to_unsigned(36,16)); -- 
			
			x_keypoint(14) <= std_logic_vector(to_unsigned(38,16)); -- 
			y_keypoint(14) <= std_logic_vector(to_unsigned(38,16)); -- 
			
			x_keypoint(15) <= std_logic_vector(to_unsigned(40,16)); -- 
			y_keypoint(15) <= std_logic_vector(to_unsigned(40,16)); -- 
			
			x_keypoint(16) <= std_logic_vector(to_unsigned(42,16)); -- 
			y_keypoint(16) <= std_logic_vector(to_unsigned(42,16)); -- 
			
			x_keypoint(17) <= std_logic_vector(to_unsigned(44,16)); -- 
			y_keypoint(17) <= std_logic_vector(to_unsigned(44,16)); -- 
			
			x_keypoint(18) <= std_logic_vector(to_unsigned(46,16)); -- 
			y_keypoint(18) <= std_logic_vector(to_unsigned(46,16)); -- 
			
			x_keypoint(19) <= std_logic_vector(to_unsigned(48,16)); -- 
			y_keypoint(19) <= std_logic_vector(to_unsigned(48,16)); -- 
			---------------------------------------------------------
			x_keypoint(20) <= std_logic_vector(to_unsigned(50,16)); -- x20
			y_keypoint(20) <= std_logic_vector(to_unsigned(50,16)); -- x20
			
			x_keypoint(21) <= std_logic_vector(to_unsigned(52,16)); -- 
			y_keypoint(21) <= std_logic_vector(to_unsigned(52,16)); -- 
			
			x_keypoint(22) <= std_logic_vector(to_unsigned(54,16)); -- 
			y_keypoint(22) <= std_logic_vector(to_unsigned(54,16)); -- 
			
			x_keypoint(23) <= std_logic_vector(to_unsigned(56,16)); -- 
			y_keypoint(23) <= std_logic_vector(to_unsigned(56,16)); -- 
			
			x_keypoint(24) <= std_logic_vector(to_unsigned(58,16)); -- 
			y_keypoint(24) <= std_logic_vector(to_unsigned(58,16)); -- 
			
			 x_keypoint(25) <= std_logic_vector(to_unsigned(60,16)); -- 
			 y_keypoint(25) <= std_logic_vector(to_unsigned(60,16)); -- 
						 
			x_keypoint(26) <= std_logic_vector(to_unsigned(62,16)); -- 
			 y_keypoint(26) <= std_logic_vector(to_unsigned(62,16)); -- 
			
			 x_keypoint(27) <= std_logic_vector(to_unsigned(64,16)); -- 
			 y_keypoint(27) <= std_logic_vector(to_unsigned(64,16)); -- 
			
			 x_keypoint(28) <= std_logic_vector(to_unsigned(66,16)); -- 
			 y_keypoint(28) <= std_logic_vector(to_unsigned(66,16)); -- 
			
			 x_keypoint(29) <= std_logic_vector(to_unsigned(68,16)); -- 
			 y_keypoint(29) <= std_logic_vector(to_unsigned(68,16)); -- 
			-- ---------------------------------------------------------
			 x_keypoint(30) <= std_logic_vector(to_unsigned(70,16)); -- x30
			 y_keypoint(30) <= std_logic_vector(to_unsigned(70,16)); -- x30
			
			 x_keypoint(31) <= std_logic_vector(to_unsigned(72,16)); -- 
			 y_keypoint(31) <= std_logic_vector(to_unsigned(72,16)); -- 
			
			 x_keypoint(32) <= std_logic_vector(to_unsigned(74,16)); -- 
			 y_keypoint(32) <= std_logic_vector(to_unsigned(74,16)); -- 
			
			 x_keypoint(33) <= std_logic_vector(to_unsigned(76,16)); -- 
			 y_keypoint(33) <= std_logic_vector(to_unsigned(76,16)); -- 
			
			 x_keypoint(34) <= std_logic_vector(to_unsigned(78,16)); -- 
			y_keypoint(34) <= std_logic_vector(to_unsigned(78,16)); -- 
			
			 x_keypoint(35) <= std_logic_vector(to_unsigned(80,16)); -- 
			 y_keypoint(35) <= std_logic_vector(to_unsigned(80,16)); -- 
			
			 x_keypoint(36) <= std_logic_vector(to_unsigned(82,16)); -- 
			 y_keypoint(36) <= std_logic_vector(to_unsigned(82,16)); -- 
			
			 x_keypoint(37) <= std_logic_vector(to_unsigned(84,16)); -- 
			 y_keypoint(37) <= std_logic_vector(to_unsigned(84,16)); -- 
			
			 x_keypoint(38) <= std_logic_vector(to_unsigned(86,16)); -- 
			 y_keypoint(38) <= std_logic_vector(to_unsigned(86,16)); -- 
			
			 x_keypoint(39) <= std_logic_vector(to_unsigned(88,16)); -- 
			 y_keypoint(39) <= std_logic_vector(to_unsigned(88,16)); --
			-- ---------------------------------------------------------
			 x_keypoint(40) <= std_logic_vector(to_unsigned(90,16)); -- x40
			 y_keypoint(40) <= std_logic_vector(to_unsigned(90,16)); -- x40
			
			 x_keypoint(41) <= std_logic_vector(to_unsigned(92,16)); -- 
			 y_keypoint(41) <= std_logic_vector(to_unsigned(92,16)); -- 
			
			 x_keypoint(42) <= std_logic_vector(to_unsigned(94,16)); -- 
			 y_keypoint(42) <= std_logic_vector(to_unsigned(94,16)); -- 
			
			 x_keypoint(43) <= std_logic_vector(to_unsigned(96,16)); -- 
			 y_keypoint(43) <= std_logic_vector(to_unsigned(96,16)); -- 
			
			 x_keypoint(44) <= std_logic_vector(to_unsigned(98,16)); -- 
			 y_keypoint(44) <= std_logic_vector(to_unsigned(98,16)); -- 
			
			 x_keypoint(45) <= std_logic_vector(to_unsigned(100,16)); -- 
			 y_keypoint(45) <= std_logic_vector(to_unsigned(100,16)); -- 
			
			 x_keypoint(46) <= std_logic_vector(to_unsigned(102,16)); -- 
			 y_keypoint(46) <= std_logic_vector(to_unsigned(102,16)); -- 
			
			 x_keypoint(47) <= std_logic_vector(to_unsigned(104,16)); -- 
			 y_keypoint(47) <= std_logic_vector(to_unsigned(104,16)); -- 
			
			 x_keypoint(48) <= std_logic_vector(to_unsigned(106,16)); -- 
			 y_keypoint(48) <= std_logic_vector(to_unsigned(106,16)); -- 
			
			 x_keypoint(49) <= std_logic_vector(to_unsigned(108,16)); -- 
			 y_keypoint(49) <= std_logic_vector(to_unsigned(108,16)); -- 
			-- ---------------------------------------------------------
			
			
		  
		  end if; 
		  
			 coord_fv  	<= '0';
			 coord_dv 	<= '0';
			 coord_data 	<= (others=>'0');
		
             if(Img_fv = '0') then

				 --
				 if(frame_buffer_has_been_filled = '0')then				

					 --We send frame coordinates only if there is something to send
					 if(enabled = '1' and frame_buffer_has_been_sent = '0')then	
						
						 frame_buffer <= (others => '0');
						 c := 0;
						
						  
						  for i in 0 to 49 loop
						  
							frame_buffer(c+15 downto c) <= x_keypoint(i);
							frame_buffer(c+31 downto c+16) <= y_keypoint(i);
							c := c+32;
							
						  end loop;
						 
					     -- Get buffer ready to send
						 frame_buffer_has_been_filled <= '1';
						 frame_buffer_position		  <= (others=>'0');
								
					 end if;

				 else
					 --send coord
					 coord_fv <= '1';
					 coord_dv <= '1';
					 coord_data <= frame_buffer(to_integer(frame_buffer_position)+7 downto to_integer(frame_buffer_position));
					
					
					-- PARAM TO CHANGE
					 if(frame_buffer_position >= 1601)then -- Value = 32*number_of_points + 1
						 frame_buffer_has_been_filled <= '0';
						 frame_buffer_has_been_sent	 <= '1';
						 c := 0;
					 else
						 frame_buffer_position <= frame_buffer_position + to_unsigned(8,9);
					 end if;
					
				 end if;
				 enabled  	<= status_reg_enable_bit;

             else
			
				 coord_fv 	<= '0';
				 coord_dv 	<= '0';
				 coord_data 	<= (others=>'0');
				 frame_buffer_has_been_sent	 <= '0';

             end if;
		 end if;
	 end process;	
	

end rtl;


