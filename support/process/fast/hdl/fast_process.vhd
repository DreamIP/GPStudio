library ieee;
	use	ieee.std_logic_1164.all;
	use	ieee.numeric_std.all;

library work;
	use work.fast_types.all;

entity fast_process is
    generic(
        PIXEL_SIZE      :   integer;
        IMAGE_WIDTH     :   integer
        );
    port(
        clk	        :	in 	std_logic;
        reset_n	    :	in	std_logic;
        enable	    :	in	std_logic;

        in_data     :	in 	std_logic_vector ((PIXEL_SIZE-1) downto 0);
        in_dv	    :	in	std_logic;
        in_fv	    :	in	std_logic;

        out1_data   :	out	std_logic_vector ((PIXEL_SIZE-1) downto 0);
        out1_dv	    :	out	std_logic;
        out1_fv	    :	out	std_logic

    );
end entity;

architecture structural of fast_process is
    --------------------------------------------------------------------------------
    -- COMPONENTS
    --------------------------------------------------------------------------------
    component neighExtractor
    generic(
		PIXEL_SIZE      :   integer;
		IMAGE_WIDTH     :   integer;
		KERNEL_SIZE     :   integer
	);

    port(
		clk	            :	in 	std_logic;
        reset_n	        :	in	std_logic;
        enable	        :	in	std_logic;
        in_data         :	in 	std_logic_vector((PIXEL_SIZE-1) downto 0);
        in_dv	        :	in	std_logic;
        in_fv	        :	in	std_logic;
        out_data        :	out	pixel_array (0 to (KERNEL_SIZE * KERNEL_SIZE)- 1);
        out_dv			:	out std_logic;
        out_fv			:	out std_logic
    );
    end component;

    --------------------------------------------------------------------------------
    component fastElement
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
    end component;


    --------------------------------------------------------------------------------
    -- CONSTANTS
    --------------------------------------------------------------------------------
    constant CONST_C1_KERNEL_SIZE : integer := 7; 
    constant CONST_C1_NORM  : std_logic_vector := std_logic_vector(to_unsigned(7,PIXEL_SIZE)); 
    constant CONST_C1_KERNEL : pixel_array (0 to CONST_C1_KERNEL_SIZE * CONST_C1_KERNEL_SIZE - 1) :=
        (others => (std_logic_vector(to_unsigned(1,PIXEL_SIZE))));

    --------------------------------------------------------------------------------
    -- SIGNALS
    --------------------------------------------------------------------------------
    signal ne1_data : pixel_array (0 to CONST_C1_KERNEL_SIZE * CONST_C1_KERNEL_SIZE - 1);
    signal ne1_dv   : std_logic;
    signal ne1_fv   : std_logic;

    --------------------------------------------------------------------------------
    -- STRUCTURAL DESCRIPTION
    --------------------------------------------------------------------------------

    begin

        NE1_INST : neighExtractor
        generic map(
            PIXEL_SIZE     =>  PIXEL_SIZE,
		    IMAGE_WIDTH    =>  IMAGE_WIDTH,
	        KERNEL_SIZE    =>  CONST_C1_KERNEL_SIZE
        )
        port map(
            clk	           =>  clk,
            reset_n	       =>  reset_n,
            enable	       =>  enable,
            in_data        =>  in_data,
            in_dv	       =>  in_dv,
            in_fv	       =>  in_fv,
            out_data       =>  ne1_data,
            out_dv		   =>  ne1_dv,
            out_fv		   =>  ne1_fv
        );

        --------------------------------------------------------------------------------

        CE1_INST : fastElement
        generic map(
            PIXEL_SIZE     => PIXEL_SIZE,
            KERNEL_SIZE    => CONST_C1_KERNEL_SIZE
        )
        port map(
            clk            => clk,
            reset_n        => reset_n,
            enable         => enable,
            in_data        => ne1_data,
            in_dv    	   => ne1_dv,
            in_fv    	   => ne1_fv,
            in_kernel      => CONST_C1_KERNEL,
            --in_norm        => CONST_C1_NORM,
            out_data       => out1_data,
            out_dv    	   => out1_dv,
            out_fv    	   => out1_fv
        );

end structural;
