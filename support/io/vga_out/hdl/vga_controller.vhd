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
--! @file vga_controller.vhd
--
--! @brief        Displaying image on VGA from internal memory
--! @author       Francois Berry, El Mehdi Abdali, Maxime Pelcat
--! @board        DE1-SoC from Terasic
--! @version      1.0
--! @date         03/02/2017
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;

entity vga_controller is
  port(
  
    -----
    -- Board I/Os
    OSC_50                       : in  std_logic;
    RESET_N                        : in  std_logic;
    VGA_HS, VGA_VS               : out std_logic;
    VGA_SYNC, VGA_BLANK          : out std_logic;
    VGA_RED, VGA_GREEN, VGA_BLUE : out std_logic_vector(7 downto 0);
    CLOCK108                     : out std_logic;

    x_offset : in integer range 0 to 1280; -- offset of the upper left pixel
    y_offset : in integer range 0 to 1024;
	 
    -----
    -- GPStudio i/os
    data : in std_logic_vector(7 downto 0);
    fv   : in std_logic;                -- frame valid
    dv   : in std_logic                 -- data valid
    );
end vga_controller;

architecture hdl of vga_controller is

		
component PLL108
  port (
    refclk   : in  std_logic := '0'; --  refclk.clk
    rst      : in  std_logic := '0'; --   reset.reset
    outclk_0 : out std_logic         -- outclk0.clk
  );
end component PLL108;

  component vga_generate
    port(
      CLOCK            : in  std_logic;
      PIX_IN           : in  std_logic_vector(7 downto 0);
      RESET            : in  std_logic;
      HSYNC, VSYNC     : out std_logic;
      SYNC, BLANK      : out std_logic;
      RED, GREEN, BLUE : out std_logic_vector(7 downto 0);
      DISPLAY          : out std_logic;
      X                : out integer range 0 to 1280 := 0;
      Y                : out integer range 0 to 1024 := 0
      );
  end component vga_generate;

  component FrameBuffer
    port
      (
        data      : in  std_logic_vector (7 downto 0);
        rdaddress : in  std_logic_vector (14 downto 0);
        rdclock   : in  std_logic;
        wraddress : in  std_logic_vector (14 downto 0);
        wrclock   : in  std_logic := '1';
        wren      : in  std_logic := '0';
        q         : out std_logic_vector (7 downto 0)
        );
  end component FrameBuffer;

  signal rdaddress  : std_logic_vector (14 downto 0);
  signal PIX_IN_fb1 : std_logic_vector(7 downto 0);  -- VGA input signal when using frame buffer 1
  signal PIX_IN_fb2 : std_logic_vector(7 downto 0);  -- VGA input signal when using frame buffer 2
  signal PIX_IN     : std_logic_vector(7 downto 0);  -- VGA input signal
  signal X          : integer range 0 to 1280 := 0;
  signal Y          : integer range 0 to 1024 := 0;

  signal wrdata    : std_logic_vector (7 downto 0);  -- data to write in the frame buffer
  signal wraddress : std_logic_vector (14 downto 0); -- address to write to in the frame buffer
  signal wren      : std_logic;                      -- enabling writing to memory
  signal fb_index  : std_logic;                      -- 0 to write on fb1, read on fb2; 1 to write on fb2, readon fb1

  signal OSC_108_own   : std_logic;

begin

  -- Bug fix:ignoring GPStudio clock andd generating own 108MHz
  PLL108_inst : PLL108
  port map
  (
    refclk => OSC_50,
    rst => not RESET_N,
    outclk_0 => OSC_108_own
  );

  VGA_inst : vga_generate
    port map
    (
      CLOCK  => OSC_108_own,
      PIX_IN => PIX_IN,
      RESET  => not RESET_N,
      HSYNC  => VGA_HS,
      VSYNC  => VGA_VS,
      SYNC   => VGA_SYNC,
      BLANK  => VGA_BLANK,
      RED    => VGA_RED,
      GREEN  => VGA_GREEN,
      BLUE   => VGA_BLUE,
      X      => X,
      Y      => Y
      );

  -- Frame buffer with 2 different clock domain
  -- 2 frame buffers are instanciated and flipped
  FrameBuffer_inst1 : FrameBuffer
    port map
    (
      data      => wrdata,              -- input data
      wraddress => wraddress,
      wrclock   => OSC_50,
      wren      => wren and (not fb_index),
      rdaddress => rdaddress,
      rdclock   => OSC_108_own,
      q         => PIX_IN_fb1           -- output data
      );

  FrameBuffer_inst2 : FrameBuffer
    port map
    (
      data      => wrdata,              -- input data
      wraddress => wraddress,
      wrclock   => OSC_50,
      wren      => wren and fb_index,
      rdaddress => rdaddress,
      rdclock   => OSC_108_own,
      q         => PIX_IN_fb2           -- output data
      );

  PIX_IN <= PIX_IN_fb2 when (fb_index = '0') else PIX_IN_fb1;

  -- Generating address for reading the image from the frame buffer
  process(OSC_108_own, RESET_N)
    variable line_offset  : integer range 0 to 176*144 := 0;  -- pixel offset due to previous lines
    variable pixel_offset : integer range 0 to 176     := 0;  -- pixel offset due to previous pixels in the line
  begin
    if(RESET_N = '0') then
      rdaddress <= (others => '0');
    else
      if (OSC_108_own' event and OSC_108_own = '1') then
        if(X < 176*2 and Y < 144*2) then
          line_offset  := Y/2; -- Doubling the apparent size of the image
          line_offset  := line_offset * 176;
          pixel_offset := X/2; -- Doubling the apparent size of the image
          rdaddress    <= std_logic_vector(to_unsigned(pixel_offset, 15) + to_unsigned(line_offset, 15));
        else
          rdaddress <= std_logic_vector(to_unsigned(176*144, 15));
        end if;
      end if;
    end if;
  end process;

  -- Generating address for writing the image in the frame buffer
  process(OSC_50, RESET_N)
    variable line_nr      : integer range 0 to 144     := 0;  -- current line number
    variable pixel_nr     : integer range 0 to 177     := 0;  -- current pixel number in the row
    variable line_offset  : integer range 0 to 176*144 := 0;  -- pixel offset due to previous lines
    variable pixel_offset : integer range 0 to 176     := 0;  -- pixel offset due to previous pixels in the line
  begin
    if(RESET_N = '0') then
      wraddress <= (others => '0');
      wrdata    <= (others => '0');
      wren      <= '0';
      line_nr   := 0;
		line_offset := 0;
      pixel_nr  := 0;
      fb_index  <= '0';  -- starting by writing on fb1, reading on fb2
    else
      if (OSC_50' event and OSC_50 = '1') then
        if(fv = '0') then
          wraddress <= (others => '0');
          wrdata    <= (others => '0');
          wren      <= '0';
          line_nr   := 0;
			 line_offset := 0;
          pixel_nr  := 0;
        elsif(dv = '0') then            -- the frame is valid
          pixel_nr := 0;
        else
          if(line_nr = 144) then        -- end of frame, incrementing line number and flipping frame buffer
            wraddress <= (others => '0');
            wrdata    <= (others => '0');
            wren      <= '0';
            fb_index  <= not fb_index;  -- flipping the frame buffer
            line_nr   := line_nr + 1;
				line_offset := line_offset + 176;
          elsif(line_nr = 145) then        -- end of frame
            wraddress <= (others => '0');
            wrdata    <= (others => '0');
            wren      <= '0';
          elsif(pixel_nr = 176) then    -- end of line, incrementing line number
            wraddress <= (others => '0');
            wrdata    <= (others => '0');
            wren      <= '0';
            line_nr   := line_nr + 1;
				line_offset := line_offset + 176;
            pixel_nr  := pixel_nr + 1;
          elsif(pixel_nr = 177) then    -- end of line
            wraddress <= (others => '0');
            wrdata    <= (others => '0');
            wren      <= '0';
          else
            pixel_offset := pixel_nr;
            wraddress    <= std_logic_vector(to_unsigned(pixel_offset, 15) + to_unsigned(line_offset, 15));
            wrdata       <= data;
				--wrdata       <= (7 => data(7), 6 => data(6), 5 => data(5), 4 => data(4), 3 => data(3), 2 => data(2), 1 => data(1), 0 => data(0), others => '0');
            wren         <= '1';
            pixel_nr     := pixel_nr + 1;
          end if;
        end if;
      end if;
    end if;
  end process;

  CLOCK108 <= OSC_108_own;
  
end hdl;

