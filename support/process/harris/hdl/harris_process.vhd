-------------------------------------------------------------------------------
-- Copyright Institut Pascal Equipe Dream (19-10-2016)
-- Francois Berry, El Mehdi Abdali, Maxime Pelcat

-- This software is a computer program whose purpose is to manage dynamic 
-- partial reconfiguration.

-- This software is governed by the CeCILL-C license under French law and
-- abiding by the rules of distribution of free software.  You can  use, 
-- modify and/ or redistribute the software under the terms of the CeCILL-C
-- license as circulated by CEA, CNRS and INRIA at the following URL
-- "http://www.cecill.info". 

-- As a counterpart to the access to the source code and  rights to copy,
-- modify and redistribute granted by the license, users are provided only
-- with a limited warranty  and the software's author,  the holder of the
-- economic rights,  and the successive licensors  have only  limited
-- liability. 

-- In this respect, the user's attention is drawn to the risks associated
-- with loading,  using,  modifying and/or developing or reproducing the
-- software by the user in light of its specific status of free software,
-- that may mean  that it is complicated to manipulate,  and  that  also
-- therefore means  that it is reserved for developers  and  experienced
-- professionals having in-depth computer knowledge. Users are therefore
-- encouraged to load and test the software's suitability as regards their
-- requirements in conditions enabling the security of their systems and/or 
-- data to be ensured and,  more generally, to use and operate it in the 
-- same conditions as regards security. 

-- The fact that you are presently reading this means that you have had
-- knowledge of the CeCILL-C license and that you accept its terms.
-------------------------------------------------------------------------------

-- Doxygen Comments -----------------------------------------------------------
--! @file         harris_process.vhd
--
--! @brief        harris key points extractor
--! @author       Francois Berry, El Mehdi Abdali, Maxime Pelcat
--! @board        SoCKit from Arrow and Terasic
--! @version      1.0
--! @date         09/05/2017
-------------------------------------------------------------------------------

library ieee;
use     ieee.std_logic_1164.all;
use     ieee.numeric_std.all;
use     ieee.math_real.all;
library std;
library altera_mf;
use     altera_mf.altera_mf_components.all;
use     work.window_extractor_pkg.all;

entity harris_process is
generic 
   (
      line_width_max : integer;
      pix_width      : integer
   );		
port
   (
      clk_proc       : in  std_logic;
      reset_n        : in  std_logic;
	
      in_data        : in  std_logic_vector((pix_width-1) downto 0);
      in_fv          : in  std_logic;
      in_dv          : in  std_logic;
		
      out_data       : out std_logic_vector (pix_width-1 downto 0);
      out_fv         : out std_logic;
      out_dv         : out std_logic;

      enable_i       : in  std_logic;
      widthimg_i     : in  std_logic_vector(15 downto 0)
   );
end harris_process;


architecture arch of harris_process is

constant        result_length             : integer             := 52; 
constant        filter_result_length      : integer             := 28;
signal          fv_signal,
                out_clk_dv                : std_logic           := '0';
signal          pixel_window_computing    : generic_pixel_window(0 to 4, 0 to 4)(59 downto 0); 
signal          pixel_window_filtering    : generic_pixel_window(0 to 6, 0 to 6)((result_length-1) downto 0); 

signal          gradient_window_computing : generic_pixel_window(0 to 2, 0 to 2)((pix_width-1) downto 0);
signal          out_clk_fv                : std_logic;
signal          out_clk_dv_2              : std_logic;
signal          harris_threshold          : std_logic_vector((result_length-1) downto 0):= x"0000014DC9380";


signal          fv_signal_3               : std_logic;
signal          dv_signal_3               : std_logic;

shared variable conv_value_x,
                conv_value_y              : signed(17 downto 0) := to_signed(0,18);
shared variable Ixx,Ixy,Iyy               : signed(31 downto 0) := to_signed(0,32);

shared variable Ixx_connection,
                Ixy_connection,
					 Iyy_connection            : std_logic_vector(19 downto 0);

shared variable conv_value                : integer             := 0;  
shared variable cast_36_bits              : std_logic_vector(35 downto 0); 
shared variable Ixx_vec,Iyy_vec           : std_logic_vector(31 downto 0);
shared variable filtered_score            : std_logic_vector(result_length-1 downto 0);
shared variable comp_s                    : std_logic           := '0';
shared variable mult_a,mult_b,
                mult_2_a,mult_2_b,
                mult_3_a,mult_3_b         : std_logic_vector(31 downto 0);
signal          mult_s,mult_2_s,
                mult_3_s,comp_a,
                comp_a_2,comp_b,
                comp_b_2,add_a_1,
                add_b_1,add_b_2,
                add_s_inter,add_s         : std_logic_vector((result_length-1) downto 0);

type            keypoints_coord  is array (99 downto 0) of std_logic_vector(15 downto 0); 	-- PARAM TO CHANGE				 
signal          x_keypoint : keypoints_coord;        
signal          y_keypoint : keypoints_coord; 				 
					 

signal frame_buffer 		        : std_logic_vector(3199 downto 0); -- PARAM TO CHANGE
signal frame_buffer_has_been_filled : std_logic;
signal frame_buffer_has_been_sent	: std_logic;
signal frame_buffer_position        : unsigned(12 downto 0); -- PARAM TO CHANGE

					 
component generic_window_extractor is
generic 
   (
      line_width_max : integer;
      pix_width      : integer;
      matrix_width   : integer  	
   );
port 
   (
      clk            : in  std_logic;
      reset_n        : in  std_logic;

      in_data        : in  std_logic_vector((pix_width-1) downto 0);
      in_fv          : in  std_logic;
      in_dv          : in  std_logic;

      out_fv         : out std_logic;
      out_dv         : out std_logic;
		
      widthimg_i     : in  std_logic_vector(15 downto 0);
      pixel_window   : out generic_pixel_window(0 to matrix_width-1, 0 to matrix_width-1)(pix_width-1 downto 0)
   );
end component;

component LPM_MULT
generic 
   ( 
      LPM_WIDTHA         : natural;                
      LPM_WIDTHB         : natural;                
      LPM_WIDTHS         : natural:=1;            
      LPM_WIDTHP         : natural;                
      LPM_REPRESENTATION : string:="SIGNED";        
      LPM_PIPELINE       : natural:=0;         
      LPM_TYPE           : string;
      LPM_HINT           : string:="UNUSED"
   );
port    
   ( 
      DATAA              : in std_logic_vector(LPM_WIDTHA-1 downto 0);
      DATAB              : in std_logic_vector(LPM_WIDTHB-1 downto 0);
      ACLR               : in std_logic :='0';
      SUM                : in std_logic_vector(LPM_WIDTHS-1 downto 0):=(OTHERS=>'0');
      RESULT             : out std_logic_vector(LPM_WIDTHP-1 downto 0)
   );
end component;


component LPM_ADD_SUB
generic 
   ( 
      LPM_WIDTH          : natural:=44;
      LPM_DIRECTION      : string:="ADD";
      LPM_REPRESENTATION : string:="SIGNED";
      LPM_PIPELINE       : natural:=0;
      LPM_TYPE           : string:="LPM_ADD_SUB";
      LPM_HINT           : string:="UNUSED"
   );
port 
   (    
      DATAA              : in std_logic_vector(LPM_WIDTH-1 downto 0);
      DATAB              : in std_logic_vector(LPM_WIDTH-1 downto 0);
      ACLR               : in std_logic:='0';
      CLOCK              : in std_logic:='0';
      CLKEN              : in std_logic:='1';
      CIN                : in std_logic:='Z';
      ADD_SUB            : in std_logic:='1';
      RESULT             : out std_logic_vector(LPM_WIDTH-1 downto 0);
      COUT               : out std_logic;
      OVERFLOW           : out std_logic	
   );
end component;


component LPM_COMPARE
generic 
   ( 
      LPM_WIDTH          : natural:=44;
      LPM_REPRESENTATION : string:="SIGNED";
      LPM_PIPELINE       : natural:=0;
      LPM_TYPE           : string:="LPM_COMPARE";
      LPM_HINT           : string:="UNUSED"
   );
port 
   (    
      DATAA              : in std_logic_vector(LPM_WIDTH-1 downto 0);
      DATAB              : in std_logic_vector(LPM_WIDTH-1 downto 0);
      ACLR               : in std_logic :='0';
      AGB                : out std_logic;
      AGEB               : out std_logic;
      AEB                : out std_logic;
      ANEB               : out std_logic;
      ALB                : out std_logic;
      ALEB               : out std_logic
   );
end component;

	
begin
	
	
gradient_window_extractor : generic_window_extractor 
generic map
   (
      line_width_max   => line_width_max,
      pix_width        => 8,
      matrix_width     => 3 	
   )	
port map
   (
      clk              => clk_proc,
      reset_n          => reset_n,

      in_data          => in_data,
      in_fv            => in_fv,
      in_dv            => in_dv,
		
      out_fv           => fv_signal_3,
      out_dv           => dv_signal_3, 

      widthimg_i       => widthimg_i,
      pixel_window     => gradient_window_computing
   );
	
	
	
computing_window_extractor : generic_window_extractor 
generic map
   (
      line_width_max   => line_width_max,
      pix_width        => 60,--pix_width,
      matrix_width     => 5 	
   )	
port map
   (
      clk              => clk_proc,
      reset_n          => reset_n,

      in_data          => Ixx_connection & Ixy_connection & Iyy_connection,
      in_fv            => fv_signal_3,--in_fv,
      in_dv            => dv_signal_3,--in_dv,
		
      out_fv           => fv_signal,
      out_dv           => out_clk_dv, 

      widthimg_i       => widthimg_i,
      pixel_window     => pixel_window_computing
   );
	

filtering_window_extractor : generic_window_extractor 
generic map
   (
      line_width_max   => line_width_max,
      pix_width        => result_length,
      matrix_width     => 7 	
   )	
port map
   (
      clk              => clk_proc,
      reset_n          => reset_n,

      in_data          => filtered_score,
      in_fv            => in_fv,
      in_dv            => out_clk_dv,
		
      out_fv           => out_clk_fv,
      out_dv           => out_clk_dv_2, 

      widthimg_i       => widthimg_i,
      pixel_window     => pixel_window_filtering
   );

mult_inst:LPM_MULT
generic map 
   ( 
      LPM_WIDTHA         => result_length/2,
      LPM_WIDTHB         => result_length/2,
      LPM_WIDTHS         => 1,           
      LPM_WIDTHP         => result_length,
      LPM_REPRESENTATION => "SIGNED",        
      LPM_PIPELINE       => 0,         
      LPM_TYPE           => "LPM_MULT",
      LPM_HINT           => "UNUSED"
   )
port  map   
   ( 
      DATAA              => mult_a((result_length/2)-1 downto 0),
      DATAB              => mult_b((result_length/2)-1 downto 0),
      ACLR               => '0',
      RESULT             => mult_s(result_length-1 downto 0)
   );
	
mult_inst_2:LPM_MULT
generic map 
   ( 
      LPM_WIDTHA         => result_length/2,
      LPM_WIDTHB         => result_length/2,
      LPM_WIDTHS         => 1,           
      LPM_WIDTHP         => result_length,
      LPM_REPRESENTATION => "SIGNED",        
      LPM_PIPELINE       => 0,         
      LPM_TYPE           => "LPM_MULT",
      LPM_HINT           => "UNUSED"
   )
port  map  
   (    
      DATAA              => mult_2_a((result_length/2)-1 downto 0),
      DATAB              => mult_2_b((result_length/2)-1 downto 0),
      ACLR               => '0',
      RESULT             => mult_2_s(result_length-1 downto 0)
   );

mult_inst_3:LPM_MULT
generic map 
   ( 
      LPM_WIDTHA         => result_length/2,
      LPM_WIDTHB         => result_length/2,
      LPM_WIDTHS         => 1,           
      LPM_WIDTHP         => result_length,
      LPM_REPRESENTATION => "SIGNED",        
      LPM_PIPELINE       => 0,         
      LPM_TYPE           => "LPM_MULT",
      LPM_HINT           => "UNUSED"
   )
port map   
   ( 
      DATAA              => mult_3_a((result_length/2)-1 downto 0),
      DATAB              => mult_3_b((result_length/2)-1 downto 0),
      ACLR               => '0',
      RESULT             => mult_3_s(result_length-1 downto 0)    
   );


comp_inst_1: LPM_COMPARE
generic map 
   (
      LPM_WIDTH          => result_length,
      LPM_REPRESENTATION => "SIGNED",
      LPM_PIPELINE       => 0,
      LPM_TYPE           => "LPM_COMPARE",
      LPM_HINT           => "UNUSED"
   )
port map
   (      
      DATAA              => add_s,
      DATAB              => harris_threshold,
      ACLR               => '0',
      AGB                => comp_s
   );


lpm_add_sub_inst_1 : LPM_ADD_SUB
generic map 
   ( 
      LPM_WIDTH          => result_length,
      LPM_DIRECTION      => "DEFAULT",
      LPM_REPRESENTATION => "SIGNED",
      LPM_PIPELINE       => 0,
      LPM_TYPE           => "LPM_ADD_SUB",
      LPM_HINT           => "UNUSED"
   )
port map 
   (
      DATAA              => mult_s,
      DATAB              => mult_3_s,
      ACLR               => '0',
      CIN                => '0',
      ADD_SUB            => '0',
      RESULT             => add_s_inter
   );


lpm_add_sub_inst_2 : LPM_ADD_SUB
generic map 
   ( 
      LPM_WIDTH          => result_length,
      LPM_DIRECTION      => "DEFAULT",
      LPM_REPRESENTATION => "SIGNED",
      LPM_PIPELINE       => 0,
      LPM_TYPE           => "LPM_ADD_SUB",
      LPM_HINT           => "UNUSED"
	)
port map 
   (
      DATAA              => add_s_inter,
      DATAB              => mult_2_s,
      ACLR               => '0',
      CIN                => '0',
      ADD_SUB            => '0',
      RESULT             => add_s
   );


process (clk_proc, reset_n)

   variable x_pos,y_pos       : unsigned(15 downto 0);                                 	 
   variable zero_number       : integer   := 0;
   variable max_i             : integer   := 0;
   variable max_j             : integer   := 0;
   variable temp_max          : std_logic_vector(filter_result_length-1 downto 0);
   variable conv_x_std_vector : std_logic_vector(17 downto 0);
   variable conv_y_std_vector : std_logic_vector(17 downto 0);
   variable keypoint_index    : integer   :=0;
   variable c				  : integer :=0;
   
begin
             
   if(reset_n='0') then
    x_pos          := to_unsigned(0, 16);
    y_pos          := to_unsigned(0, 16);
    keypoint_index := 0;	
	x_keypoint <= ((others =>(others =>'0')));
	y_keypoint <= ((others =>(others =>'0')));
	 c :=0; 
	--Cleaning frame buffer
	frame_buffer    <= (others=>'0');
	--Cleaning signals used to fill buffer
	frame_buffer_has_been_filled <= '0';
	frame_buffer_has_been_sent	 <= '0';
	out_fv 	<= '0';
	out_dv 	<= '0';
	out_data 	<= (others=>'0');
                                     
    elsif(rising_edge(clk_proc)) then
                
        if(in_dv='1') then                      
					
			--/* compute positions */--
			if (x_pos = (unsigned(widthimg_i) - 1)) then
				x_pos := to_unsigned(0, 16);
				y_pos := y_pos + 1;
			else
				x_pos := x_pos + 1;
			end if;
				
			conv_value   := to_integer(unsigned(gradient_window_computing(1,1)));
				
			Ixx          := to_signed(+0,32);
			Ixy          := to_signed(+0,32);
			Iyy          := to_signed(+0,32);
			 
			--/* computing the gradient over x and y */--
			conv_value_x := to_signed(0,18);
			conv_value_y := to_signed(0,18);							 
			conv_value_x := conv_value_x + signed('0' & gradient_window_computing(0,0)) - signed('0' & gradient_window_computing(0,2))
										   + signed('0' & gradient_window_computing(1,0)) - signed('0' & gradient_window_computing(1,2))
										   + signed('0' & gradient_window_computing(2,0)) - signed('0' & gradient_window_computing(2,2));
																		  
			conv_value_y := conv_value_y + signed('0' & gradient_window_computing(0,0)) - signed('0' & gradient_window_computing(2,0))
										   + signed('0' & gradient_window_computing(0,1)) - signed('0' & gradient_window_computing(2,1))
										   + signed('0' & gradient_window_computing(0,2)) - signed('0' & gradient_window_computing(2,2)); 					

				
			cast_36_bits   := std_logic_vector(conv_value_x*conv_value_x);                                      
			Ixx_connection := cast_36_bits(19 downto 0);  
			cast_36_bits   := std_logic_vector(conv_value_x*conv_value_y); 
			Ixy_connection := cast_36_bits(19 downto 0);
			cast_36_bits   := std_logic_vector(conv_value_y*conv_value_y);
			Iyy_connection := cast_36_bits(19 downto 0); 
				
			--/* computing the harris score */--
			for i in 0 to 4 loop 
				for j in 0 to 4 loop									                                   
					Ixx := Ixx + signed(pixel_window_computing(i,j)(59 downto 40));
				    Ixy := Ixy + signed(pixel_window_computing(i,j)(39 downto 20));
				    Iyy := Iyy + signed(pixel_window_computing(i,j)(19 downto 0));  					
				end loop;
			end loop;

			mult_a   := std_logic_vector(Ixx);
			mult_b   := std_logic_vector(Iyy);
			Ixx_vec  := std_logic_vector(Ixx+Iyy);
			Iyy_vec  := Ixx_vec(31) & Ixx_vec(31) & Ixx_vec(31) & Ixx_vec(31) & Ixx_vec(31 downto 4);
			mult_2_a := Ixx_vec;
			mult_2_b := Iyy_vec;               
			Iyy_vec  := std_logic_vector(Ixy);
			mult_3_a := Iyy_vec;
			mult_3_b := Iyy_vec;  
			  
			if(comp_s='1') then
				filtered_score := add_s;
			else
				filtered_score := (others=>'0');			
			end if;
				
			--/* filtering part */--
			zero_number := 0;
			max_i       := 0;
			max_j       := 0;
			temp_max    := (others => '0');

			for i in 0 to 6 loop 
				for j in 0 to 6 loop
					
				    if (signed(pixel_window_filtering(i,j)(result_length-1 downto result_length-filter_result_length))=0) then
						zero_number := zero_number + 1;
				    else 
						if signed(pixel_window_filtering(i,j)(result_length-1 downto result_length-filter_result_length)) > signed(temp_max) then
							temp_max := pixel_window_filtering(i,j)(result_length-1 downto result_length-filter_result_length);
							max_i    := i;
							max_j    := j;					
						end if;
				    end if;
				end loop;
			end loop;
				
			if (max_i=3) and 	(max_j=3) and (zero_number < 25) then
				-- conv_value := 0;
					x_keypoint(keypoint_index) <= std_logic_vector(x_pos-to_unsigned(7,16));
					y_keypoint(keypoint_index) <= std_logic_vector(y_pos-to_unsigned(7,16));
					keypoint_index             := keypoint_index+1;

			end if;
		end if;																	
			
		out_fv  <= '0';
		out_dv 	<= '0';
		out_data 	<= (others=>'0');	
		
		if(in_fv ='0')then
				
				x_pos          := to_unsigned(0, 16);
				y_pos          := to_unsigned(0, 16);
				keypoint_index := 0;
				
				 if(frame_buffer_has_been_filled = '0')then				

					 --We send frame coordinates only if there is something to send
					 if(frame_buffer_has_been_sent = '0')then	
							
						 frame_buffer <= (others => '0');
						 c:=0;							
						
						  
						  for l in 0 to 99 loop
						  
							frame_buffer(c+15 downto c) <= x_keypoint(l);
							frame_buffer(c+31 downto c+16) <= y_keypoint(l);
							c := c+32;
							
						  end loop; 
						
												
						 -- Get buffer ready to send
						 frame_buffer_has_been_filled <= '1';
						 frame_buffer_position		  <= (others=>'0');
									
					 end if;

				 else
					 --send coord
					 out_fv <= '1';
					 out_dv <= '1';
					 out_data <= frame_buffer(to_integer(frame_buffer_position)+7 downto to_integer(frame_buffer_position));
					
						
					 -- PARAM TO CHANGE
					 if(frame_buffer_position >= 3201)then -- Value = 32*number_of_points + 1
						 frame_buffer_has_been_filled <= '0';
						 frame_buffer_has_been_sent	 <= '1';
						 x_pos          := to_unsigned(0, 16);
						 y_pos          := to_unsigned(0, 16);
						 keypoint_index := 0;
						 c:=0;
						 x_keypoint <= ((others =>(others =>'0')));
						 y_keypoint <= ((others =>(others =>'0')));
					 else
						 frame_buffer_position <= frame_buffer_position + to_unsigned(8,9);
					 end if;
					
				 end if;
			
		
		else
			out_fv 	<= '0';
			out_dv 	<= '0';
			out_data 	<= (others=>'0');
			frame_buffer_has_been_sent	 <= '0';
		end if;

	else
	end if;
end process;

end arch;
