library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity dynroiBinMask_process is
	generic (
		CLK_PROC_FREQ : integer;
		IN_SIZE       : integer;
		OUT_SIZE      : integer
	);
	port (
		clk_proc              : in std_logic;
		reset_n               : in std_logic;

		---------------- dynamic parameters ports ---------------
		status_reg_enable_bit : in std_logic;
		status_reg_bypass_bit : in std_logic;
		in_size_reg_in_w_reg  : in std_logic_vector(11 downto 0);
		in_size_reg_in_h_reg  : in std_logic_vector(11 downto 0);

		------------------------- in flow -----------------------
		in_data               : in std_logic_vector(IN_SIZE-1 downto 0);
		in_fv                 : in std_logic;
		in_dv                 : in std_logic;

		------------------------ out flow -----------------------
		out_data              : out std_logic_vector(OUT_SIZE-1 downto 0);
		out_fv                : out std_logic;
		out_dv                : out std_logic
	);
end dynroiBinMask_process;

architecture rtl of dynroiBinMask_process is

constant X_COUNTER_SIZE : integer := 12;
constant Y_COUNTER_SIZE : integer := 12;

signal x_pos    : unsigned(X_COUNTER_SIZE-1 downto 0);
signal y_pos    : unsigned(Y_COUNTER_SIZE-1 downto 0);

signal x_min	: unsigned(X_COUNTER_SIZE-1 downto 0);
signal x_max	: unsigned(X_COUNTER_SIZE-1 downto 0);
signal x		: unsigned(X_COUNTER_SIZE-1 downto 0);

signal y_min	: unsigned(Y_COUNTER_SIZE-1 downto 0);
signal y_max	: unsigned(Y_COUNTER_SIZE-1 downto 0);
signal y 		: unsigned(Y_COUNTER_SIZE-1 downto 0);

signal w		: unsigned(X_COUNTER_SIZE-1 downto 0);
signal h		: unsigned(Y_COUNTER_SIZE-1 downto 0);

signal bypass_s : std_logic;
signal enabled  : std_logic;

begin
	data_process : process (clk_proc, reset_n)
	begin
		if(reset_n='0') then
			x_pos 	 	   <= to_unsigned(0, X_COUNTER_SIZE);
            y_pos    	   <= to_unsigned(0, Y_COUNTER_SIZE);
			--Cleaning frame coordinates
			x_max 		<= (others=>'0');
			y_max 		<= (others=>'0');
			x_min 		<= unsigned(in_size_reg_in_w_reg);
			y_min 		<= unsigned(in_size_reg_in_h_reg);
			--
			x 			<= (others=>'0');
			y 			<= (others=>'0');
			w 			<= unsigned(in_size_reg_in_w_reg);
			h 			<= unsigned(in_size_reg_in_h_reg);
			
			out_data <= (others => '0');
			out_dv <= '0';
			out_fv <= '0';
			
		elsif(rising_edge(clk_proc)) then
			if in_fv = '1' and status_reg_enable_bit = '1' and enabled = '1' then
				out_fv <= '1';
			else
				out_fv <= '0';
			end if;
            out_dv <= '0';
			out_data <= (others => '0');
			
            if(in_fv = '0') then
                x_pos <= to_unsigned(0, X_COUNTER_SIZE);
                y_pos <= to_unsigned(0, Y_COUNTER_SIZE);
				
				--Updating last frame coordinates
				if x_max > 0 then
					x <= x_min;
					y <= y_min;
					w <= x_max-x_min;
					h <= y_max-y_min;
				end if;
				
				--Cleaning frame coordinates
				x_max 		<= (others=>'0');
				y_max 		<= (others=>'0');
				x_min 		<= unsigned(in_size_reg_in_w_reg);
				y_min 		<= unsigned(in_size_reg_in_h_reg);
				
                bypass_s <= status_reg_bypass_bit;
                enabled  <= status_reg_enable_bit;
                
            else
				if status_reg_enable_bit = '1' and enabled = '1' then
					
					if(in_dv = '1' ) then
					
						x_pos <= x_pos + 1;
						if(x_pos=unsigned(in_size_reg_in_w_reg)-1) then
							y_pos <= y_pos + 1;
							x_pos <= to_unsigned(0, X_COUNTER_SIZE);
						end if;
						
						--ROI							
						out_dv <= '1';
						if(bypass_s = '0') then
							if(y_pos >= y
							and y_pos < y + h
							and x_pos >= x
							and x_pos < x + w )then
								--out_dv <= '1';
								out_data <= (others=>'1');
							else
								--out_dv <= '1';
								out_data <= (others=>'0');
							end if;
						else
							out_data <= in_data;
						end if;
							
						-- This will give the smallest area including all non-black points
						if in_data /= (in_data'range => '0') then
							if x_pos < x_min then
								x_min <= x_pos;
							end if;
							if x_pos > x_max then
								x_max <= x_pos;
							end if;
							--
							if y_pos < y_min then
								y_min <= y_pos;
							end if;
							if y_pos > y_max then
								y_max <= y_pos;
							end if;				
						end if;
						
					end if;
				else 
					enabled <= '0';
				end if;
            end if;
		end if;
	end process;
end rtl;
