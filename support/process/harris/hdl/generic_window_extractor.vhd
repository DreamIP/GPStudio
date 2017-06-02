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
--! @file         generic_window_extractor.vhd
--
--! @brief        generic window extractor
--! @author       Francois Berry, El Mehdi Abdali, Maxime Pelcat
--! @board        SoCKit from Arrow and Terasic
--! @version      1.0
--! @date         11/01/2017
-------------------------------------------------------------------------------

library ieee;
use     ieee.std_logic_1164.all;
use     ieee.numeric_std.all;
use     ieee.math_real.all;
library std;
library altera_mf;
use     work.window_extractor_pkg.all;

entity generic_window_extractor is
generic 
   (
      line_width_max   : integer;
      pix_width        : integer;
      matrix_width     : integer  	
   );
	
port 
   (
      clk              : in  std_logic;
      reset_n          : in  std_logic;
      --/* input flow */
      in_data          : in  std_logic_vector((pix_width-1) downto 0);
      in_fv            : in  std_logic;
      in_dv            : in  std_logic;

      out_fv           : out std_logic;
      out_dv           : out std_logic;

      widthimg_i       : in  std_logic_vector(15 downto 0);
      pixel_window     : out generic_pixel_window(0 to matrix_width-1, 0 to matrix_width-1)(pix_width-1 downto 0)
   );
end generic_window_extractor;


architecture arch of generic_window_extractor is

type             pix_out_signal         is array (0 to matrix_width-2) of std_logic_vector((pix_width-1) downto 0);   
constant         FIFO_LENGHT          : integer   := line_width_max;      
constant         FIFO_LENGHT_WIDTH    : integer   := integer(ceil(log2(real(FIFO_LENGHT))));
signal           widthimg_temp        : std_logic_vector(15 downto 0):=widthimg_i;
signal           sig_rdreq            : std_logic := '0';
signal           line_pix_out         : generic_pixel_line(0 to matrix_width-1)(pix_width-1 downto 0);
shared variable  param_changing_reset : std_logic := '0';      
shared variable  aclr                 : std_logic := '0';
shared variable  pixel_matrix_kernel  : generic_pixel_window(0 to matrix_width-1, 0 to matrix_width-1)(pix_width-1 downto 0);


component scfifo
generic
   (        
      LPM_WIDTH               : positive;
      LPM_WIDTHU              : positive;
      LPM_NUMWORDS            : positive;
      LPM_SHOWAHEAD           : string := "OFF";
      ALLOW_RWCYCLE_WHEN_FULL : string := "OFF";
      OVERFLOW_CHECKING       : string := "ON";
      UNDERFLOW_CHECKING      : string := "ON"
   );
      port
   (
      data                    : in std_logic_vector(LPM_WIDTH-1 downto 0);
      clock,
      wrreq,
      rdreq,
      aclr                    : in std_logic;
      full,
      empty,
      almost_full,
      almost_empty            : out std_logic;
      q                       : out std_logic_vector(LPM_WIDTH-1 downto 0);
      usedw                   : out std_logic_vector(LPM_WIDTHU-1 downto 0)
   );
end component;

begin

--/* generating the matrix_width-1 line buffers */
G_1 : for i in 0 to matrix_width-2 generate
line_fifo_inst : scfifo
generic map
   (
      LPM_WIDTH    => pix_width,
      LPM_WIDTHU   => FIFO_LENGHT_WIDTH,
      LPM_NUMWORDS => FIFO_LENGHT
   )
port map
   (
      data         => pixel_matrix_kernel(i+1,0),
      clock        => clk,
      wrreq        => in_dv,
      q            => line_pix_out(i),
      rdreq        => sig_rdreq and in_dv,
      aclr         => param_changing_reset or(not(reset_n))
   );
end generate;


process (clk, reset_n)
   variable counter :integer:=0;
begin          
   if(reset_n='0') then 
   elsif(rising_edge(clk)) then
	   out_fv <= in_fv;
		out_dv <= in_dv;
      if(in_fv='0') then

      elsif(in_dv='1') then

         counter:=counter+1;
			 		 
         if(counter=(unsigned(widthimg_i)-matrix_width-1)) then 
            sig_rdreq <= '1';
         end if;
        
         --/* updating the matrix */					 
         for o in 0 to matrix_width-1 loop    
            for p in 0 to matrix_width-2 loop  
               pixel_matrix_kernel(o,p):=pixel_matrix_kernel(o,p+1);
            end loop;

            if (o<matrix_width-1) then
               pixel_matrix_kernel(o,matrix_width-1):=line_pix_out(o);
            end if;
         end loop; 
         pixel_matrix_kernel(matrix_width-1,matrix_width-1):=in_data;
                                      
      else
      end if;

      --/* fifo reset when widthimg_i changes */
      if (unsigned(widthimg_i)=unsigned(widthimg_temp)) then
         param_changing_reset := '0';                          
      else
         param_changing_reset := '1';
         counter              := 0;
         sig_rdreq            <= '0';
      end if;
      widthimg_temp<=widthimg_i;                        

   else
   end if;
end process;

   pixel_window <= pixel_matrix_kernel;

end arch;
