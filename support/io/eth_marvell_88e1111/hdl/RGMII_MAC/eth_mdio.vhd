-- This code is used to configure the Marvell 88e1111 and handle the MDIO pins (PHY_RESET, PHY_MDC and PHY_MDIO).
-- It can write and read the internals registers of the marvell.
-- Right now, the configuration used is the configuration by default and this entity only outputs an hardware reset when power up.
-- You can add a configuration only by uncommenting the request signal (just set your address register and the data you want to write).

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity eth_mdio is 
Port(
	CLK         : in STD_LOGIC;
	RESET       : in STD_LOGIC;
	E_RST_L     : out STD_LOGIC;
	E_MDC       : out STD_LOGIC;
	E_MDIO      : inout STD_LOGIC);
end eth_mdio;

architecture Behavioral of eth_mdio is

	signal count        : unsigned(27 downto 0) ;
	signal request      : unsigned(0 to 31);--write
	signal request_r    : unsigned(0 to 13);--read
	signal stop_count_s : std_logic:='0';
	type state_mdio is (idle, set_conf, reset_st,read_st,wait_st,config);
	signal state        : state_mdio;
	signal count_conf   : unsigned(3 downto 0):=x"0";
	signal reset_done   : std_logic:='0';
	signal tempo        : std_logic:='1';
	
begin
process(CLK,RESET)
begin
    if RESET = '0' then
        E_RST_L         <= '0';--hold reset condition for 1 second
        E_MDC           <= '1';
        E_MDIO          <= 'Z';	
        stop_count_s    <= '0';
        count           <= x"0000000";
        
    elsif CLK'event and CLK = '1' then
            if (count(11) = '1' or count(4) = '1') then
                E_MDC <= '1';
            else--maximum frequency of E_MDC is 8.3 MHz
                E_MDC <= '0';--3.9 MHz
            end if;
        
        case state is		
            when idle =>
                if count = x"F000000" and stop_count_s='0' then
                    state <= wait_st;
                    count <= x"0000000";
                elsif count > x"0140000" and count < x"0280000" and stop_count_s='0' then
                    E_RST_L <= '0';
                    count <= count +1;
                elsif count = x"F000000" and stop_count_s='1' then
                    state <= read_st;
                    count <= x"8000000";
                else
                    E_RST_L <= '1';
                    count <= count +1;
                end if;
            
            when wait_st =>
                if count = x"FFFFFFF" then
                    count <= x"8000000";
                    if reset_done='0' then
                        state <= set_conf;
                    elsif reset_done='1' and tempo='1' then 
                        state <= wait_st;
                        tempo <= '0';
                    else	
                        state <= config;
                    end if;
                else
                    count <= count +1;
                end if;
                    
            
            when set_conf =>
                    --request <= "0101" & "10010" & "10100" & "100000110011100000"; -- set delay for RX and TX in rgmii mode
                if count=x"8000FFF" then
                    state <= reset_st;
                    count <= x"8000000";
                else
                    count <= count +1;
                end if;
                
            when reset_st =>					
                --request <= "0101" & "10010" & "00000" & "101001000101000000"; -- reset PHY
                if count=x"8000FFF" then
                    --stop_count_s <= '1';
                    count <= x"0000000";
                    state <= wait_st;
                    reset_done<='1' ;
                else
                    count <= count +1;
                end if;	
                
            when config => 
                if count_conf=x"0" then
                    --request <= "0101" & "10010" & "10100" & "100000110011100001"; -- set reg 20 for rgmii
                    --request <= "0101" & "10010" & "00000" & "100101000101000000"; -- stop loopback = 100101000101000000 --reg 0
                else
                    --request <= "0101" & "10010" & "11110" & "101000100100100000"; -- packet generator activated
                end if;
                
                if count=x"8000FFF" then
                    
                    if count_conf=x"0" then
                        state <= config;
                        count <= x"8000000";
                        count_conf <= count_conf +1;
                    else	
                        stop_count_s <= '1';
                        state <= idle;
                    end if;
                else
                    count <= count +1;
                end if;
            
            when read_st =>
                
                request_r <=  "0110" & "10010" & "10100";
                if count=x"8000FFF" then
                    state <= idle;
                    count <= x"8000000";
                else
                    count <= count +1;
                end if;
            
            when others =>
                state <= idle;
            end case;	
    end if;
end process;

    E_MDIO <= '1' when count(11 downto 5) < "0100000" and (state=set_conf or state=reset_st or state=read_st or state=config) --32 1's preamble
                         else request(to_integer(count(9 downto 5))) when (state=set_conf or state=reset_st or state=config) and count(11 downto 5) <= "0111111" --write data
                         else request_r(to_integer(count(9 downto 5))) when state=read_st and count(11 downto 5) <= "0101101" -- read data
                         else 'Z';
end Behavioral;
