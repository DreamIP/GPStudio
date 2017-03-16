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
--! @file RGB2GRY.vhd
--
--! @brief        Converting RGB values into an 8-bit gray value 
--! @author       Francois Berry, El Mehdi Abdali, Maxime Pelcat
--! @board        SoCKit from Arrow and Terasic
--! @version      1.0
--! @date         16/11/2016
-------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;


entity RGB2GRY is

  port(
    clk       : in std_logic;
    reset     : in std_logic;
    ---------------------------------------
    src_CCD_R : in std_logic_vector(11 downto 0); -- Inputing 12-bit RGB
    src_CCD_G : in std_logic_vector(11 downto 0);
    src_CCD_B : in std_logic_vector(11 downto 0);

    oCCD_GRY : out std_logic_vector(7 downto 0) -- Keeping the 8 most significant bits of gray scale
    );
end entity RGB2GRY;


architecture arch of RGB2GRY is

begin

  process(clk)

	variable gray  : unsigned(13 downto 0); -- Variable for computing the sum of RGB, dividing by 3 and keeping the 8 MSBs
	variable vector_gray : std_logic_vector(13 downto 0) := (others => '0');
  begin
    if(clk'event and clk = '1') then

		-- summing over 14 bitsthe 3 components
      gray := unsigned("00" & src_CCD_R)+unsigned("00" & src_CCD_G)+unsigned("00" & src_CCD_B);
		
		-- x*1/3 about = x/4 + x/16 + x/64 + x/256 + x/1024 + x/4096
		--gray := (gray/4) + (gray/16) + (gray/64) + (gray/256) + (gray/1024);
		gray := ("00" & gray(13 downto 2)) + ("0000" & gray(13 downto 4));-- + ("000000" & gray(13 downto 6)) + ("00000000" & gray(13 downto 8)) + ("0000000000" & gray(13 downto 10));

		if(gray > "00111111110000") then
			gray := "00111111110000";
		end if;
      vector_gray := std_logic_vector(gray);
		
		oCCD_GRY <= vector_gray(11 downto 4);
    end if;
  end process;

end arch;
