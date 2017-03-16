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
--! @file top_levelr.vhd
--
--! @brief        Testing D5M CMOS Image sensor with VGA display
--! @from         http://tinyvga.com/vga-timing/1280x1024@60Hz
--! @author       Francois Berry, El Mehdi Abdali, Maxime Pelcat
--! @board        SoCKit from Arrow and Terasic
--! @version      1.0
--! @date         16/11/2016
-------------------------------------------------------------------------------

--
--                      General timing
--      Screen refresh rate     60 Hz
--      Vertical refresh                63.981042654028 kHz
--      Pixel freq.                             108.0 MHz
--
--              Horizontal timing (line)
--      Scanline part   Pixels  Time [µs]
--      Visible area    1280            11.851851851852
--      Front porch             48                      0.44444444444444
--      Sync pulse              112             1.037037037037
--      Back porch              248             2.2962962962963
--      Whole line              1688            15.62962962963
--
--                      Vertical timing (frame)
--      Frame part              Lines           Time [ms]
--      Visible area    1024            16.004740740741
--      Front porch             1                       0.01562962962963
--      Sync pulse              3                       0.046888888888889
--      Back porch              38                      0.59392592592593
--      Whole frame             1066            16.661185185185
--
--

library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;

entity vga_generate is
  port(
    CLOCK            : in  std_logic;
    PIX_IN           : in  std_logic_vector(7 downto 0);
    RESET            : in  std_logic;
    HSYNC, VSYNC     : out std_logic;
    SYNC, BLANK      : out std_logic               := '1';
    RED, GREEN, BLUE : out std_logic_vector(7 downto 0);
    DISPLAY          : out std_logic;
    X                : out integer range 0 to 1280 := 0;
    Y                : out integer range 0 to 1024 := 0
    );
end vga_generate;

architecture MAIN of vga_generate is

  signal HPOS  : integer range 0 to 1688 := 0;
  signal VPOS  : integer range 0 to 1066 := 0;
  signal GRAY  : integer range 0 to 255  := 0;
  signal GRAYV : std_logic_vector(7 downto 0);

  constant HZ_SYNC        : integer   := 112;
  constant HZ_BACK_PORCH  : integer   := 248;
  constant HZ_DISP        : integer   := 1280;
  constant HZ_FRONT_PORCH : integer   := 48;
  constant HZ_SCAN_WIDTH  : integer   := 1688;
  constant HS_POLARITY    : std_logic := '0';

  constant VT_SYNC        : integer   := 3;
  constant VT_BACK_PORCH  : integer   := 38;
  constant VT_DISP        : integer   := 1024;
  constant VT_FRONT_PORCH : integer   := 1;
  constant VT_SCAN_WIDTH  : integer   := 1066;
  constant VT_POLARITY    : std_logic := '0';

begin
  process(CLOCK, RESET)
  begin
    if (RESET = '1') then
      DISPLAY <= '0';
      X       <= 0;
      Y       <= 0;
      RED     <= (others => '0');
      GREEN   <= (others => '0');
      BLUE    <= (others => '0');
    else
      if (rising_edge(CLOCK)) then
        if (HPOS < HZ_SCAN_WIDTH) then
          HPOS <= HPOS + 1;
        else
          HPOS <= 0;

          if (VPOS < VT_SCAN_WIDTH) then
            VPOS <= VPOS + 1;
          else
            VPOS <= 0;
          end if;
        end if;

        if (HPOS > HZ_FRONT_PORCH and HPOS < (HZ_FRONT_PORCH + HZ_SYNC)) then
          HSYNC <= HS_POLARITY;
        else
          HSYNC <= not HS_POLARITY;
        end if;

        if (VPOS > VT_FRONT_PORCH and VPOS < (VT_FRONT_PORCH + VT_SYNC)) then
          VSYNC <= VT_POLARITY;
        else
          VSYNC <= not VT_POLARITY;
        end if;

        if (HPOS > (HZ_FRONT_PORCH + HZ_SYNC + HZ_BACK_PORCH) and VPOS > (VT_FRONT_PORCH + VT_SYNC + VT_BACK_PORCH)) then
          DISPLAY <= '1';

          X <= HPOS - (HZ_FRONT_PORCH + HZ_SYNC + HZ_BACK_PORCH - 1);
          Y <= VPOS - (VT_FRONT_PORCH + VT_SYNC + VT_BACK_PORCH - 1);

          RED   <= PIX_IN;              -- 255
          GREEN <= PIX_IN;              -- 79
          BLUE  <= PIX_IN;              -- 0

        else
          DISPLAY <= '0';
          X       <= 0;
          Y       <= 0;
          RED     <= (others => '0');
          GREEN   <= (others => '0');
          BLUE    <= (others => '0');
        end if;
      end if;
    end if;
  end process;

  BLANK <= '1';
  SYNC  <= '1';
end MAIN;
