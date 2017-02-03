library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

entity usb3_ft600x is
	generic (
		CLK_PROC_FREQ : integer;
		OUT0_SIZE     : integer;
		OUT1_SIZE     : integer;
		IN0_SIZE      : integer;
		IN1_SIZE      : integer
	);
	port (
		clk_proc  : in std_logic;
		reset_n   : in std_logic;

		--------------------- external ports --------------------
		ftreset_n : out std_logic;
		ftclk     : in std_logic;
		be        : inout std_logic_vector(1 downto 0);
		data      : inout std_logic_vector(15 downto 0);
		txe_n     : in std_logic;
		rxf_n     : in std_logic;
		siwu_n    : out std_logic;
		wr_n      : out std_logic;
		rd_n      : out std_logic;
		oe_n      : out std_logic;

		------------------------ out0 flow ----------------------
		out0_data : out std_logic_vector(OUT0_SIZE-1 downto 0);
		out0_fv   : out std_logic;
		out0_dv   : out std_logic;

		------------------------ out1 flow ----------------------
		out1_data : out std_logic_vector(OUT1_SIZE-1 downto 0);
		out1_fv   : out std_logic;
		out1_dv   : out std_logic;

		------------------------ in0 flow -----------------------
		in0_data  : in std_logic_vector(IN0_SIZE-1 downto 0);
		in0_fv    : in std_logic;
		in0_dv    : in std_logic;

		------------------------ in1 flow -----------------------
		in1_data  : in std_logic_vector(IN1_SIZE-1 downto 0);
		in1_fv    : in std_logic;
		in1_dv    : in std_logic
	);
end usb3_ft600x;

architecture rtl of usb3_ft600x is

    type read_sm is (w_wait, w_start_read, w_start_read2, w_start_read3, w_read);
    signal read_sm_state : read_sm := w_wait;

begin
    ftreset_n <= reset_n;

	process (ftclk, reset_n)
    begin
        if(reset_n = '0') then
            wr_n <= '1';
            rd_n <= '1';
            oe_n <= '1';
            data <= "ZZZZZZZZZZZZZZZZ";
            be <= "ZZ";
            read_sm_state <= w_wait;

        elsif(rising_edge(ftclk)) then
            case read_sm_state is
			when w_wait =>
                wr_n <= '1';
                rd_n <= '1';
                oe_n <= '1';
                if(rxf_n = '0') then
                    read_sm_state <= w_start_read;
                end if;
                
			when w_start_read =>
                read_sm_state <= w_start_read2;
                data <= "ZZZZZZZZZZZZZZZZ";
                be <= "ZZ";
                
			when w_start_read2 =>
                read_sm_state <= w_start_read3;
                oe_n <= '0';
                
			when w_start_read3 =>
                read_sm_state <= w_read;
                rd_n <= '0';
                
			when w_read =>
                out0_data <= data(7 downto 0);
                
                if(rxf_n = '1') then
                    read_sm_state <= w_wait;
                end if;
			
			when others =>
			end case;
        end if;
    end process;

end rtl;
