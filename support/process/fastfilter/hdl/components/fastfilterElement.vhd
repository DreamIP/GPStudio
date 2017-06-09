library ieee;
	use	ieee.std_logic_1164.all;
	use	ieee.numeric_std.all;

library work;
	use work.fastfilter_types.all;

entity fastfilterElement is

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
		in_norm     :   in  std_logic_vector(PIXEL_SIZE-1 downto 0);

        out_data    :   out std_logic_vector(PIXEL_SIZE-1 downto 0);
        out_dv    	:   out std_logic;
        out_fv    	:   out std_logic
    );
end fastfilterElement;

architecture bhv of fastfilterElement is

    -- Signals
    type	pixel_array_s1 is array (0 to KERNEL_SIZE * KERNEL_SIZE - 1) of signed (PIXEL_SIZE downto 0);
	signal	px	    	:	pixel_array_s1 ;
	signal	kernel_s	:	pixel_array_s1 ;
	signal	res			:	signed (PIXEL_SIZE downto 0);
    signal  all_valid   :   std_logic;

	signal tmp0			:	signed(pixel_size downto 0);
	signal tmp1			:	signed(pixel_size downto 0);
	signal tmp2			:	signed(pixel_size downto 0);
	signal tmp3			:	signed(pixel_size downto 0);
	signal tmp4			:	signed(pixel_size downto 0);
	signal tmp5			:	signed(pixel_size downto 0);
	signal tmp6			:	signed(pixel_size downto 0);
	signal tmp7			:	signed(pixel_size downto 0);
	signal tmp8			:	signed(pixel_size downto 0);
	signal tmp9			:	signed(pixel_size downto 0);
	signal tmp10		:	signed(pixel_size downto 0);
	signal tmp11		:	signed(pixel_size downto 0);
	signal tmp12		:	signed(pixel_size downto 0);
	
	signal px1		:	signed(pixel_size downto 0);
	signal px2		:	signed(pixel_size downto 0);
	signal px3		:	signed(pixel_size downto 0);
	
	
    begin
		
		-- All valid : Logic and
		all_valid    <=    in_dv and in_fv and enable;

        UNSIGNED_CAST		:   for i in 0 to ( KERNEL_SIZE * KERNEL_SIZE - 1 ) generate
            px(i)      <=  signed('0' & in_data(i));
        end generate;

    process(clk)
 
        begin
			
				if (all_valid='1') then	
	
					tmp0 <= px(0);
					tmp1 <= px(1);
					tmp2 <= px(2);
					tmp3 <= px(3);
					tmp4 <= px(4);
					tmp5 <= px(5);
					tmp6 <= px(6);
					tmp7 <= px(7);
					tmp8 <= px(8);
					tmp9 <= px(9);
					tmp10 <= px(10);
					tmp11 <= px(11);
					tmp12 <= px(12);
					
					px1 <= px(13);
					px2 <= px(18);
					px3 <= px(17);
					
					 if ( 	(tmp12(0) = '1') 
							 and (tmp0(0) = '0') 
							 and (tmp1(0) = '0') 
							 and (tmp2(0) = '0')
							 and (tmp3(0) = '0')
							 and (tmp4(0) = '0') 
							 and (tmp5(0) = '0') 
							 and (tmp6(0) = '0')
							 and (tmp7(0) = '0')
							 and (tmp8(0) = '0') 
							 and (tmp9(0) = '0') 
							 and (tmp10(0) = '0')
							 and (tmp11(0) = '0')) then 
														
							
							if ( 	px1(0) = '1' and px2(0) = '0' and px3(0) = '0' and px1 < tmp11  ) then 
								res <= (others => '1');
								
							elsif ( px1(0) = '0' and px2(0) = '1' and px3(0) = '0' and px2 < tmp11 ) then
								res <= (others => '1');
								
							elsif ( px1(0) = '0' and px2(0) = '0' and px3(0) = '1' and px3 < tmp11 ) then
								res <= (others => '1');
								
							elsif ( px1(0) = '1' and px2(0) = '1' and px3(0) = '0' and px1 < tmp11 and px2 < tmp11) then
								res <= (others => '1');

							elsif ( px1(0) = '1' and px2(0) = '0' and px3(0) = '1' and px1 < tmp11 and px3 < tmp11 ) then
								res <= (others => '1');	
								
							elsif ( px1(0) = '0' and px2(0) = '1' and px3(0) = '1' and px2 < tmp11 and px3 < tmp11 ) then
								res <= (others => '1');

							elsif ( px1(0) = '1' and px2(0) = '1' and px3(0) = '0' and px1 < tmp11 and px2 < tmp11 ) then
								res <= (others => '1');		

							elsif ( px1(0) = '1' and px2(0) = '1' and px3(0) = '1' and px1 < tmp11 and px3 < tmp11 and px2 < tmp11) then
								res <= (others => '1');

							elsif ( px1(0) = '0' and px2(0) = '0' and px3(0) = '0' ) then
								res <= (others => '1');
								
							end if;
								
										
					else
						res <= px(11);
					end if;
					
				
								
				end if;
			out_data	<= std_logic_vector (res(PIXEL_SIZE-1 downto 0));
    end process;

    
	out_fv <= in_fv;
	out_dv <= in_dv;
	

end bhv;
