library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
use IEEE.math_real.all;
library std;
library altera_mf;
use altera_mf.altera_mf_components.all;

entity matrix_extractor_3_3 is
	generic (
		LINE_WIDTH_MAX : integer;
		PIX_WIDTH : integer;
		OUTVALUE_WIDTH : integer
	);
	port (
		clk_proc : in std_logic;
		reset_n : in std_logic;

		------------------------- in flow -----------------------
		in_data : in std_logic_vector((PIX_WIDTH-1) downto 0);
		in_fv : in std_logic;
		in_dv : in std_logic;

		------------------------ out flow -----------------------
		out_data : out std_logic_vector((PIX_WIDTH-1) downto 0);
		out_fv : out std_logic;
		out_dv : out std_logic;
		
		------------------------ matrix out ---------------------
		p00 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		p01 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		p02 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		p10 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		p11 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		p12 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		p20 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		p21 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		p22 : out std_logic_vector((PIX_WIDTH-1) downto 0);
		matrix_dv : out std_logic;
		
		---------------------- computed value -------------------
		value_data : in std_logic_vector((OUTVALUE_WIDTH-1) downto 0);
		value_dv : in std_logic;

		------------------------- params ------------------------
		enable_i : in std_logic;
		widthimg_i : in std_logic_vector(15 downto 0)
	);
end matrix_extractor_3_3;

architecture rtl of matrix_extractor_3_3 is

constant FIFO_LENGHT : integer := LINE_WIDTH_MAX;
constant FIFO_LENGHT_WIDTH : integer := integer(ceil(log2(real(FIFO_LENGHT))));

component gp_fifo
   generic (
        DATA_WIDTH      : positive;
        FIFO_DEPTH      : positive
    );
    port (
        clk             : in std_logic;
        reset_n         : in std_logic;
        data_wr         : in std_logic;
        data_in         : in std_logic_vector(DATA_WIDTH-1 downto 0);
        full            : out std_logic;
        data_rd         : in std_logic;
        data_out        : out std_logic_vector(DATA_WIDTH-1 downto 0);
        empty           : out std_logic
    );
end component;

signal enable_reg :		std_logic;

signal x_pos : unsigned(15 downto 0);
signal y_pos : unsigned(15 downto 0);

signal p00_s, p01_s, p02_s : std_logic_vector((PIX_WIDTH-1) downto 0);
signal p10_s, p11_s, p12_s : std_logic_vector((PIX_WIDTH-1) downto 0);
signal p20_s, p21_s, p22_s : std_logic_vector((PIX_WIDTH-1) downto 0);

signal line_reset : std_logic;

signal line0_read : std_logic;
signal line0_out : std_logic_vector((PIX_WIDTH-1) downto 0);
signal line0_write : std_logic;
signal line0_empty : std_logic;
signal line0_pix_out : std_logic_vector((PIX_WIDTH-1) downto 0);

signal line1_read : std_logic;
signal line1_out : std_logic_vector((PIX_WIDTH-1) downto 0);
signal line1_write : std_logic;
signal line1_empty : std_logic;
signal line1_pix_out : std_logic_vector((PIX_WIDTH-1) downto 0);

signal dummy_dv : std_logic;
signal out_dv_s : std_logic;
signal cell : std_logic_vector((LINE_WIDTH_MAX-1) downto 0);

begin

	line0_fifo : gp_fifo
    generic map (
    	DATA_WIDTH => PIX_WIDTH,
    	FIFO_DEPTH => FIFO_LENGHT
	)
    port map (
		data_in => p10_s,
		clk => clk_proc,
		data_wr => line0_write,
		data_out => p02_s,
		data_rd => line0_read,
		reset_n => line_reset,
		empty => line0_empty
    );

	line1_fifo : gp_fifo
    generic map (
    	DATA_WIDTH => PIX_WIDTH,
    	FIFO_DEPTH => FIFO_LENGHT
	)
    port map (
		data_in => p20_s,
		clk => clk_proc,
		data_wr => line1_write,
		data_out => p12_s,
		data_rd => line1_read,
		reset_n => line_reset,
		empty => line1_empty
    );

    process (clk_proc, reset_n)
	begin
		if(reset_n='0') then
			x_pos <= to_unsigned(0, 16);
			y_pos <= to_unsigned(0, 16);
			line0_read <= '0';
			line0_write <= '0';
			line1_read <= '0';
			line1_write <= '0';
			line_reset <= '0';
			
		elsif(rising_edge(clk_proc)) then
			matrix_dv <= '0';
			if(in_fv='0') then
				x_pos <= to_unsigned(0, 16);
				y_pos <= to_unsigned(0, 16);
				line0_read <= '0';
				line0_write <= '0';
				line1_read <= '0';
				line1_write <= '0';
                line_reset <= '0';
                matrix_dv <= '0';

			else
                line_reset <= '1';
				if (line0_read='1') then
					p01_s <= p02_s;
					p00_s <= p01_s;
				end if;
				if (line1_read='1') then
					p11_s <= p12_s;
					p10_s <= p11_s;
				end if;
				
				if (in_dv='1') then
					
                    -- counter y_pos and x_pos
					x_pos <= x_pos+1;
					if(x_pos = unsigned(widthimg_i)-1) then
						y_pos <= y_pos + 1;
						x_pos <= to_unsigned(0, 16);
					end if;
					
					p22_s <= in_data;
					p21_s <= p22_s;
					p20_s <= p21_s;
                    
                    -- line1 write command
                    if((y_pos = to_unsigned(0, 16)) and (x_pos >= to_unsigned(2, 16))) then
                        line1_write <= '1';
                    elsif(y_pos > to_unsigned(0, 16)) then
                        line1_write <= '1';
                    else
                        line1_write <= '0';
                    end if;
                    
                    -- line0 write command
                    if(y_pos = to_unsigned(0, 16)) then
                        line0_write <= '0';
                    elsif((y_pos = to_unsigned(1, 16)) and (x_pos >= to_unsigned(2, 16))) then
                        line0_write <= '1';
                    elsif((y_pos > to_unsigned(1, 16))) then
                        line0_write <= '1';
                    else
                        line0_write <= '0';
                    end if;
                    
                    -- matrix_dv_next command
                    if((x_pos >= to_unsigned(2, 16)) and (y_pos >= to_unsigned(2, 16))) then
						matrix_dv <= '1';
					end if;
					
                    -- line1 read command
					if(y_pos = to_unsigned(0, 16)) then
                        if(x_pos = unsigned(widthimg_i)-2) then
                            line1_read <= '1';
                        else
                            line1_read <= '0';
                        end if;
					else
						line1_read <= '1';
					end if;
					
                    -- line0 read command
					if(y_pos = to_unsigned(0, 16)) then
						line0_read <= '0';
					elsif(y_pos = to_unsigned(1, 16)) then
                        if(x_pos = unsigned(widthimg_i)-2) then
                            line0_read <= '1';
                        else
                            line0_read <= '0';
                        end if;
					else
						line0_read <= '1';
					end if;
				else
					line0_read <= '0';
					line0_write <= '0';
					line1_read <= '0';
					line1_write <= '0';
				end if;
			end if;
		end if;
	end process;

	p00 <= p00_s; p01 <= p01_s; p02 <= p02_s;
	p10 <= p10_s; p11 <= p11_s; p12 <= p12_s;
	p20 <= p20_s; p21 <= p21_s; p22 <= p22_s;

	out_data <= value_data;
	out_dv   <= value_dv;
	out_fv   <= in_fv;
end rtl;
