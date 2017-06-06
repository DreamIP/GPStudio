library ieee;
	use	ieee.std_logic_1164.all;
	use	ieee.numeric_std.all;

library work;
	use work.fast_types.all;

entity fastElement is

    generic(
        KERNEL_SIZE :    integer;
        PIXEL_SIZE  :    integer 
    );

    port(
        clk         :   in  std_logic;
        reset_n     :   in  std_logic;
        enable      :   in  std_logic;

        in_data     :   in  pixel_array (0 to KERNEL_SIZE * KERNEL_SIZE - 1);
        in_dv    	:   in  std_logic;
        in_fv    	:   in  std_logic;

        in_kernel   :   in  pixel_array (0 to KERNEL_SIZE * KERNEL_SIZE - 1);
        --in_norm     :   in  std_logic_vector(PIXEL_SIZE-1 downto 0);

        out_data    :   out std_logic_vector(PIXEL_SIZE-1 downto 0);
        out_dv    	:   out std_logic;
        out_fv    	:   out std_logic
    );
end fastElement;

architecture bhv of fastElement is

        -- Signals
    type	pixel_array_s1 is array (0 to KERNEL_SIZE * KERNEL_SIZE - 1) of signed ((PIXEL_SIZE) downto 0);
	signal	px	    	:	pixel_array_s1 ;
	signal	kernel_s	:	pixel_array_s1 ;
	signal	res			:	signed (PIXEL_SIZE downto 0);
    signal  all_valid   :   std_logic;
	
	signal ev_c1b	: std_logic;
	signal ev_c2b	: std_logic;
	signal ev_c3b	: std_logic;
	signal ev_c4b	: std_logic;
	signal ev_c5b	: std_logic;
	signal ev_c6b	: std_logic;
	signal ev_c7b	: std_logic;
	signal ev_c8b	: std_logic;
	signal ev_c9b	: std_logic;
	signal ev_c10b	: std_logic;
	signal ev_c11b	: std_logic;
	signal ev_c12b	: std_logic;
	signal ev_c13b	: std_logic;
	signal ev_c14b	: std_logic;
	signal ev_c15b	: std_logic;
	signal ev_c16b	: std_logic;
	
	signal ev_c1h	: std_logic;
	signal ev_c2h	: std_logic;
	signal ev_c3h	: std_logic;
	signal ev_c4h	: std_logic;
	signal ev_c5h	: std_logic;
	signal ev_c6h	: std_logic;
	signal ev_c7h	: std_logic;
	signal ev_c8h	: std_logic;
	signal ev_c9h	: std_logic;
	signal ev_c10h	: std_logic;
	signal ev_c11h	: std_logic;
	signal ev_c12h	: std_logic;
	signal ev_c13h	: std_logic;
	signal ev_c14h	: std_logic;
	signal ev_c15h	: std_logic;
	signal ev_c16h	: std_logic;
	
	signal tmp		: signed(pixel_size downto 0);
		
    begin
		
		-- All valid : Logic and
		all_valid    <=    in_dv and in_fv and enable;

        SIGNED_CAST		:   for i in 0 to ( KERNEL_SIZE * KERNEL_SIZE - 1 ) generate
            px(i)      <=  signed('0' & in_data(i));
        end generate;

    process(clk)
		
		variable threshold	: signed(6 downto 0) := "0001111";	-- threshold value
	------------- Variables Bresenham circle points -------------------------
		variable p1b		: 	std_logic; -- pixel-1-bas if the pixel 1 of the Bresenham circle is smaller 
										   -- than the center pixel minus the threshold value
		variable p2b		: 	std_logic; 
		variable p3b		: 	std_logic;
		variable p4b		: 	std_logic;
		variable p5b		: 	std_logic;
		variable p6b		: 	std_logic;
		variable p7b		: 	std_logic;
		variable p8b		: 	std_logic;
		variable p9b		: 	std_logic;
		variable p10b		: 	std_logic;
		variable p11b		: 	std_logic;
		variable p12b		: 	std_logic;
		variable p13b		: 	std_logic;
		variable p14b		: 	std_logic;
		variable p15b		: 	std_logic;
		variable p16b		: 	std_logic;
		variable p1h		: 	std_logic; -- pixel-1-haut if the pixel 1 of the Bresenham circle is bigger 
										   -- than the center pixel plus the threshold value
		variable p2h		: 	std_logic;
		variable p3h		: 	std_logic;
		variable p4h		: 	std_logic;
		variable p5h		: 	std_logic;
		variable p6h		: 	std_logic;
		variable p7h		: 	std_logic;
		variable p8h		: 	std_logic;
		variable p9h		: 	std_logic;
		variable p10h		: 	std_logic;
		variable p11h		: 	std_logic;
		variable p12h		: 	std_logic;
		variable p13h		: 	std_logic;
		variable p14h		: 	std_logic;
		variable p15h		: 	std_logic;
		variable p16h		: 	std_logic;
	 
        begin
		----------------- Bresenham Circle pixels -------------------------
			if (px(3) <= px(24)-threshold) then 
				p1b := '1';
				p1h := '0';
			elsif (px(3) >= px(24)+threshold) then
				p1b := '0';
				p1h := '1';
			else
				p1b := '0';
				p1h := '0';
			end if;				
			if (px(4) <= px(24)-threshold) then 
				p2b := '1';
				p2h := '0';
			elsif (px(4) >= px(24)+threshold) then
				p2b := '0';
				p2h := '1';
			else
				p2b := '0';
				p2h := '0';
			end if;						
			if (px(12) <= px(24)-threshold) then 
				p3b := '1';
				p3h := '0';
			elsif (px(12) >= px(24)+threshold) then
				p3b := '0';
				p3h := '1';
			else
				p3b := '0';
				p3h := '0';
			end if;						
			if (px(20) <= px(24)-threshold) then 
				p4b := '1';
				p4h := '0';
			elsif (px(20) >= px(24)+threshold) then
				p4b := '0';
				p4h := '1';
			else
				p4b := '0';
				p4h := '0';
			end if;						
			if (px(27) <= px(24)-threshold) then 
				p5b := '1';
				p5h := '0';
			elsif (px(27) >= px(24)+threshold) then
				p5b := '0';
				p5h := '1';
			else
				p5b := '0';
				p5h := '0';
			end if;		
			if (px(34) <= px(24)-threshold) then 
				p6b := '1';
				p6h := '0';
			elsif (px(34) >= px(24)+threshold) then
				p6b := '0';
				p6h := '1';
			else
				p6b := '0';
				p6h := '0';
			end if;				
			if (px(40) <= px(24)-threshold) then 
				p7b := '1';
				p7h := '0';
			elsif (px(40) >= px(24)+threshold) then
				p7b := '0';
				p7h := '1';
			else
				p7b := '0';
				p7h := '0';
			end if;						
			if (px(46) <= px(24)-threshold) then 
				p8b := '1';
				p8h := '0';
			elsif (px(46) >= px(24)+threshold) then
				p8b := '0';
				p8h := '1';
			else
				p8b := '0';
				p8h := '0';
			end if;				
			if (px(45) <= px(24)-threshold) then 
				p9b := '1';
				p9h := '0';
			elsif (px(45) >= px(24)+threshold) then
				p9b := '0';
				p9h := '1';
			else
				p9b := '0';
				p9h := '0';
			end if;				
			if (px(44) <= px(24)-threshold) then 
				p10b := '1';
				p10h := '0';
			elsif (px(44) >= px(24)+threshold) then
				p10b := '0';
				p10h := '1';
			else
				p10b := '0';
				p10h := '0';
			end if;						
			if (px(36) <= px(24)-threshold) then 
				p11b := '1';
				p11h := '0';
			elsif (px(36) >= px(24)+threshold) then
				p11b := '0';
				p11h := '1';
			else
				p11b := '0';
				p11h := '0';
			end if;						
			if (px(28) <= px(24)-threshold) then 
				p12b := '1';
				p12h := '0';
			elsif (px(28) >= px(24)+threshold) then
				p12b := '0';
				p12h := '1';
			else
				p12b := '0';
				p12h := '0';
			end if;						
			if (px(21) <= px(24)-threshold) then 
				p13b := '1';
				p13h := '0';
			elsif (px(21) >= px(24)+threshold) then
				p13b := '0';
				p13h := '1';
			else
				p13b := '0';
				p13h := '0';
			end if;						
			if (px(14) <= px(24)-threshold) then 
				p14b := '1';
				p14h := '0';
			elsif (px(14) >= px(24)+threshold) then
				p14b := '0';
				p14h := '1';
			else
				p14b := '0';
				p14h := '0';
			end if;						
			if (px(8) <= px(24)-threshold) then 
				p15b := '1';
				p15h := '0';
			elsif (px(8) >= px(24)+threshold) then
				p15b := '0';
				p15h := '1';
			else
				p15b := '0';
				p15h := '0';
			end if;				
			if (px(2) <= px(24)-threshold) then 
				p16b := '1';
				p16h := '0';
			elsif (px(2) >= px(24)+threshold) then
				p16b := '0';
				p16h := '1';
			else
				p16b := '0';
				p16h := '0';
			end if;
			----------------- Bresenham Circle pixels -------------------------
			
			
			if (all_valid='1') then	
	
								-- 1 ---------------------------- High-speed test FAST -------------------------------------------------
								
								-- First pixel 1 and 9 are tested, if they are too brighter or darker. If so, then checks 5 and 13. If the central pixel is a corner, then at least of these
								-- must all be brighter then the central pixel brightness plus the threshold value or darker than the central pixel brightness
								-- minus the threshold value
								
								if ((px(3) <= px(24)-threshold  or px(3) >= px(24)+threshold) and (px(45) <= px(24)-threshold or px(45) >= px(24)+threshold)) then 
									-- 2 ---------------------------------------------------------------------------------------------------
									if ((px(27) <= px(24)-threshold or px(27) >= px(24)+threshold) and (px(21) <= px(24)-threshold or px(21) >= px(24)+threshold)) then						
										-- 3 ---------------------------------------------------------------------------------------------------
										 if ((p1b ='1'  and p5b='1'  and p9b='1')  or
											 (p5b ='1'  and p9b='1'  and p13b='1') or
											 (p9b ='1'  and p13b='1' and p1b='1')  or
											 (p13b ='1' and p1b='1'  and p5b='1')  or
											 (p1h ='1'  and p5h='1'  and p9h='1')  or
											 (p5h ='1'  and p9h='1'  and p13h='1') or
											 (p9h ='1'  and p13h='1' and p1h='1')  or
											 (p13h ='1' and p1h='1'  and p5h='1')  or
											 (p13h ='1' and p1h='1'  and p5h='1' and p9h='1')  or
											 (p13b ='1' and p1b='1'  and p5b='1' and p9b='1')) then 
										
										
											-- 4 ---------------------------------------------------------------------------------------------------
											-- Set of 12 contiguous pixels in the Bresenham circle which are ALL brighter than the central pixel brightness
											-- plus the threshold value or ALL darker than the central pixel brightness minus the threshold value.
											
											
											if (p1b ='1' and p2b='1' and p3b='1' and p4b='1' and p5b='1' and p6b='1' and p7b='1' and p8b='1' and p9b='1' and p10b='1' and p11b='1' and p12b='1') then
												ev_c1b <= '1';
												ev_c1h <= '0';
											elsif (p1h ='1' and p2h='1' and p3h='1' and p4h='1' and p5h='1' and p6h='1' and p7h='1' and p8h='1' and p9h='1' and p10h='1' and p11h='1' and p12h='1') then
												ev_c1b <= '0';
												ev_c1h <= '1';
											else
												ev_c1b <= '0';
												ev_c1h <= '0';
											end if;											
											if (p13b ='1' and p2b='1' and p3b='1' and p4b='1' and p5b='1' and p6b='1' and p7b='1' and p8b='1' and p9b='1' and p10b='1' and p11b='1' and p12b='1') then
												ev_c2b <= '1';
												ev_c2h <= '0';
											elsif (p13h ='1' and p2h='1' and p3h='1' and p4h='1' and p5h='1' and p6h='1' and p7h='1' and p8h='1' and p9h='1' and p10h='1' and p11h='1' and p12h='1') then
												ev_c2b <= '0';
												ev_c2h <= '1';
											else
												ev_c2b <= '0';
												ev_c2h <= '0';
											end if;											
											if (p13b ='1' and p14b='1' and p3b='1' and p4b='1' and p5b='1' and p6b='1' and p7b='1' and p8b='1' and p9b='1' and p10b='1' and p11b='1' and p12b='1') then
												ev_c3b <= '1';
												ev_c3h <= '0';
											elsif (p13h ='1' and p14h='1' and p3h='1' and p4h='1' and p5h='1' and p6h='1' and p7h='1' and p8h='1' and p9h='1' and p10h='1' and p11h='1' and p12h='1') then
												ev_c3b <= '0';
												ev_c3h <= '1';
											else
												ev_c3b <= '0';
												ev_c3h <= '0';
											end if;											
											if (p13b ='1' and p14b='1' and p15b='1' and p4b='1' and p5b='1' and p6b='1' and p7b='1' and p8b='1' and p9b='1' and p10b='1' and p11b='1' and p12b='1') then
												ev_c4b <= '1';
												ev_c4h <= '0';
											elsif (p13h ='1' and p14h='1' and p15h='1' and p4h='1' and p5h='1' and p6h='1' and p7h='1' and p8h='1' and p9h='1' and p10h='1' and p11h='1' and p12h='1') then
												ev_c4b <= '0';
												ev_c4h <= '1';
											else
												ev_c4b <= '0';
												ev_c4h <= '0';
											end if;											
											if (p13b ='1' and p14b='1' and p15b='1' and p16b='1' and p5b='1' and p6b='1' and p7b='1' and p8b='1' and p9b='1' and p10b='1' and p11b='1' and p12b='1') then
												ev_c5b <= '1';
												ev_c5h <= '0';
											elsif (p13h ='1' and p14h='1' and p15h='1' and p16h='1' and p5h='1' and p6h='1' and p7h='1' and p8h='1' and p9h='1' and p10h='1' and p11h='1' and p12h='1') then
												ev_c5b <= '0';
												ev_c5h <= '1';
											else
												ev_c5b <= '0';
												ev_c5h <= '0';
											end if;											
											if (p13b ='1' and p14b='1' and p15b='1' and p16b='1' and p1b='1' and p6b='1' and p7b='1' and p8b='1' and p9b='1' and p10b='1' and p11b='1' and p12b='1') then
												ev_c6b <= '1';
												ev_c6h <= '0';
											elsif (p13h ='1' and p14h='1' and p15h='1' and p16h='1' and p1h='1' and p6h='1' and p7h='1' and p8h='1' and p9h='1' and p10h='1' and p11h='1' and p12h='1') then
												ev_c6b <= '0';
												ev_c6h <= '1';
											else
												ev_c6b <= '0';
												ev_c6h <= '0';
											end if;											
											if (p13b ='1' and p14b='1' and p15b='1' and p16b='1' and p1b='1' and p2b='1' and p7b='1' and p8b='1' and p9b='1' and p10b='1' and p11b='1' and p12b='1') then
												ev_c7b <= '1';
												ev_c7h <= '0';
											elsif (p13h ='1' and p14h='1' and p15h='1' and p16h='1' and p1h='1' and p2h='1' and p7h='1' and p8h='1' and p9h='1' and p10h='1' and p11h='1' and p12h='1') then
												ev_c7b <= '0';
												ev_c7h <= '1';
											else
												ev_c7b <= '0';
												ev_c7h <= '0';
											end if;											
											if (p13b ='1' and p14b='1' and p15b='1' and p16b='1' and p1b='1' and p2b='1' and p3b='1' and p8b='1' and p9b='1' and p10b='1' and p11b='1' and p12b='1') then
												ev_c8b <= '1';
												ev_c8h <= '0';
											elsif (p13h ='1' and p14h='1' and p15h='1' and p16h='1' and p1h='1' and p2h='1' and p3h='1' and p8h='1' and p9h='1' and p10h='1' and p11h='1' and p12h='1') then
												ev_c8b <= '0';
												ev_c8h <= '1';
											else
												ev_c8b <= '0';
												ev_c8h <= '0';
											end if;											
											if (p13b ='1' and p14b='1' and p15b='1' and p16b='1' and p1b='1' and p2b='1' and p3b='1' and p4b='1' and p9b='1' and p10b='1' and p11b='1' and p12b='1') then
												ev_c9b <= '1';
												ev_c9h <= '0';
											elsif (p13h ='1' and p14h='1' and p15h='1' and p16h='1' and p1h='1' and p2h='1' and p3h='1' and p4h='1' and p9h='1' and p10h='1' and p11h='1' and p12h='1') then
												ev_c9b <= '0';
												ev_c9h <= '1';
											else
												ev_c9b <= '0';
												ev_c9h <= '0';
											end if;											
											if (p13b ='1' and p14b='1' and p15b='1' and p16b='1' and p1b='1' and p2b='1' and p3b='1' and p4b='1' and p5b='1' and p10b='1' and p11b='1' and p12b='1') then
												ev_c10b <= '1';
												ev_c10h <= '0';
											elsif (p13h ='1' and p14h='1' and p15h='1' and p16h='1' and p1h='1' and p2h='1' and p3h='1' and p4h='1' and p5h='1' and p10h='1' and p11h='1' and p12h='1') then
												ev_c10b <= '0';
												ev_c10h <= '1';
											else
												ev_c10b <= '0';
												ev_c10h <= '0';
											end if;											
											if (p13b ='1' and p14b='1' and p15b='1' and p16b='1' and p1b='1' and p2b='1' and p3b='1' and p4b='1' and p5b='1' and p6b='1' and p11b='1' and p12b='1') then
												ev_c11b <= '1';
												ev_c11h <= '0';
											elsif (p13h ='1' and p14h='1' and p15h='1' and p16h='1' and p1h='1' and p2h='1' and p3h='1' and p4h='1' and p5h='1' and p6h='1' and p11h='1' and p12h='1') then
												ev_c11b <= '0';
												ev_c11h <= '1';
											else
												ev_c11b <= '0';
												ev_c11h <= '0';
											end if;											
											if (p13b ='1' and p14b='1' and p15b='1' and p16b='1' and p1b='1' and p2b='1' and p3b='1' and p4b='1' and p5b='1' and p6b='1' and p7b='1' and p12b='1') then
												ev_c12b <= '1';
												ev_c12h <= '0';
											elsif (p13h ='1' and p14h='1' and p15h='1' and p16h='1' and p1h='1' and p2h='1' and p3h='1' and p4h='1' and p5h='1' and p6h='1' and p7h='1' and p12h='1') then
												ev_c12b <= '0';
												ev_c12h <= '1';
											else
												ev_c12b <= '0';
												ev_c12h <= '0';
											end if;											
											if (p13b ='1' and p14b='1' and p15b='1' and p16b='1' and p1b='1' and p2b='1' and p3b='1' and p4b='1' and p5b='1' and p6b='1' and p7b='1' and p8b='1') then
												ev_c13b <= '1';
												ev_c13h <= '0';
											elsif (p13h ='1' and p14h='1' and p15h='1' and p16h='1' and p1h='1' and p2h='1' and p3h='1' and p4h='1' and p5h='1' and p6h='1' and p7h='1' and p8h='1') then
												ev_c13b <= '0';
												ev_c13h <= '1';
											else
												ev_c13b <= '0';
												ev_c13h <= '0';
											end if;											
											if (p9b ='1' and p14b='1' and p15b='1' and p16b='1' and p1b='1' and p2b='1' and p3b='1' and p4b='1' and p5b='1' and p6b='1' and p7b='1' and p8b='1') then
												ev_c14b <= '1';
												ev_c14h <= '0';
											elsif (p9h ='1' and p14h='1' and p15h='1' and p16h='1' and p1h='1' and p2h='1' and p3h='1' and p4h='1' and p5h='1' and p6h='1' and p7h='1' and p8h='1') then
												ev_c14b <= '0';
												ev_c14h <= '1';
											else
												ev_c14b <= '0';
												ev_c14h <= '0';
											end if;											
											if (p9b ='1' and p10b='1' and p15b='1' and p16b='1' and p1b='1' and p2b='1' and p3b='1' and p4b='1' and p5b='1' and p6b='1' and p7b='1' and p8b='1') then
												ev_c15b <= '1';
												ev_c15h <= '0';
											elsif (p9h ='1' and p10h='1' and p15h='1' and p16h='1' and p1h='1' and p2h='1' and p3h='1' and p4h='1' and p5h='1' and p6h='1' and p7h='1' and p8h='1') then
												ev_c15b <= '0';
												ev_c15h <= '1';
											else
												ev_c15b <= '0';
												ev_c15h <= '0';
											end if;											
											if (p9b ='1' and p10b='1' and p11b='1' and p16b='1' and p1b='1' and p2b='1' and p3b='1' and p4b='1' and p5b='1' and p6b='1' and p7b='1' and p8b='1') then
												ev_c16b <= '1';
												ev_c16h <= '0';
											elsif (p9h ='1' and p10h='1' and p11h='1' and p16h='1' and p1h='1' and p2h='1' and p3h='1' and p4h='1' and p5h='1' and p6h='1' and p7h='1' and p8h='1') then
												ev_c16b <= '0';
												ev_c16h <= '1';
											else
												ev_c16b <= '0';
												ev_c16h <= '0';
											end if;
											-- 4 ---------------------------------------------------------------------------------------------------
												
												-- 5 ---------------------------------------------------------------------------------------------------
												if (ev_c1b ='1' or ev_c2b='1' or ev_c3b='1' or ev_c4b='1' or ev_c5b='1' or ev_c6b='1' or ev_c7b='1' or ev_c8b='1' or ev_c9b='1' or ev_c10b='1' or ev_c11b='1' or ev_c12b='1' or ev_c13b='1' or ev_c14b='1' or ev_c15b='1' or ev_c16b='1'
												 or ev_c1h ='1' or ev_c2h='1' or ev_c3h='1' or ev_c4h='1' or ev_c5h='1' or ev_c6h='1' or ev_c7h='1' or ev_c8h='1' or ev_c9h='1' or ev_c10h='1' or ev_c11h='1' or ev_c12h='1' or ev_c13h='1' or ev_c14h='1' or ev_c15h='1' or ev_c16h='1'
												 ) then	
													-- Corner candidate
													
                                                      tmp <= px(23);
													  tmp(0) <= '1';
													  out_data	<= std_logic_vector (tmp(PIXEL_SIZE-1 downto 0));
													 

												else
													tmp <= px(23);
													tmp(0) <= '0';
													out_data	<= std_logic_vector (tmp(PIXEL_SIZE-1 downto 0));
												end if;
												-- 5 ---------------------------------------------------------------------------------------------------
										 else 
											tmp <= px(23);
											tmp(0) <= '0';
											out_data	<= std_logic_vector (tmp(PIXEL_SIZE-1 downto 0));
										 end if;
										-- -- 3 ---------------------------------------------------------------------------------------------------
						
									else 
										tmp <= px(23);
										tmp(0) <= '0';
										out_data	<= std_logic_vector (tmp(PIXEL_SIZE-1 downto 0));
									end if;
									-- 2 ---------------------------------------------------------------------------------------------------
									
								else 
									tmp <= px(23);
									tmp(0) <= '0';
									out_data	<= std_logic_vector (tmp(PIXEL_SIZE-1 downto 0));
								end if;
								-- 1 ---------------------------------------------------------------------------------------------------
				end if;
				
    end process;
	
	out_dv <= in_dv;
	out_fv <= in_fv;

end bhv;
