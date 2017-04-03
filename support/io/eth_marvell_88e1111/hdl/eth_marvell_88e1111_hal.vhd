library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
library std;

use work.axi.all;
use work.ipv4_types.all;
use work.arp_types.all;

use work.ComFlow_pkg.all;

entity eth_marvell_88e1111_hal is
    port (
        clk_proc            : in  std_logic;
        clk_hal             : out std_logic;
        reset_n             : in  std_logic;

        ---------------------- external ports --------------------
        gtx_clk             : out std_logic;
        tx_en               : out std_logic;
        tx_data             : out std_logic_vector(3 downto 0);
        rx_clk              : in  std_logic;
        rx_dv               : in  std_logic;
        rx_data             : in  std_logic_vector(3 downto 0);
        phy_reset_l         : out std_logic;
        phy_mdc             : out std_logic;
        phy_mdio            : inout std_logic;

        ---------------------- hal com --------------------------
        out_data_o          : out std_logic_vector(7 downto 0);
        out_data_wr_o       : out std_logic;
        out_data_full_i     : in  std_logic;
        out_data_end_o      : out std_logic;

        in_data_i           : in  std_logic_vector(7 downto 0);
        in_data_rd_o        : out std_logic;
        in_data_empty_i     : in  std_logic;
        in_data_rdy_i       : in  std_logic;
        in_data_size_packet : in  std_logic_vector(15 downto 0)
    );
end eth_marvell_88e1111_hal;

architecture rtl of eth_marvell_88e1111_hal is

component UDP_MAC_GE
    port (
        ---------------------------------------------------------------------------
        -- RGMII Interface
        ---------------------------------------------------------------------------
        gtx_clk     : out std_logic;
        tx_en       : out std_logic;
        tx_data     : out std_logic_vector(3 downto 0);
        rx_clk      : in  std_logic;
        rx_dv       : in  std_logic;
        rx_data     : in  std_logic_vector(3 downto 0);

        --phy
        phy_reset_l : out std_logic;
        phy_mdc     : out std_logic;
        phy_mdio    : inout std_logic;

        ---------------------------------------------------------------------------
        -- user Interface
        ---------------------------------------------------------------------------
        udp_tx_start            : in std_logic;                         -- indicates req to tx UDP
        udp_txi                 : in udp_tx_type;                       -- UDP tx cxns
        udp_tx_result           : out std_logic_vector (1 downto 0);    -- tx status (changes during transmission)
        udp_tx_data_out_ready   : out std_logic;                        -- indicates udp_tx is ready to take data

        -- UDP RX signals
        udp_rx_start            : out std_logic;                        -- indicates receipt of udp header
        udp_rxo                 : out udp_rx_type;

        CLK_OUT				    : OUT STD_LOGIC
    );
end component;

-- udp
    signal udp_tx_start          : std_logic;
    signal udp_txi               : udp_tx_type;
    signal udp_tx_result         : std_logic_vector (1 downto 0);
    signal udp_tx_data_out_ready : std_logic;
    signal udp_rx_start          : std_logic;
    signal udp_rxo               : udp_rx_type;
    
    signal udp_header_valid_prev : std_logic;
    signal out_data_end_s : std_logic;
    
    signal CLK_OUT               : std_logic;

    constant IP_DEST : std_logic_vector(31 downto 0) := x"AC1B014A";
    constant PORT_DEST : std_logic_vector(15 downto 0) := x"079B";
	constant PORT_SRC : std_logic_vector(15 downto 0) := x"079B";
    
    -- fsm read
	type fsm_read_state_t is (Read_Idle, Read_Receive);
	signal fsm_read_state : fsm_read_state_t := Read_Idle;
	
	-- fsm write
	type fsm_write_state_t is (Write_Idle, Wait_udp_tx_ready, wait_one_cycle_to_read_ff, transmit_tx);
	signal fsm_write_state : fsm_write_state_t := Write_Idle;

begin

    udp_mac_ge_inst : UDP_MAC_GE
    port map (
        ---------------------------------------------------------------------------
        -- RGMII Interface
        ---------------------------------------------------------------------------
        gtx_clk     => gtx_clk,
        tx_en       => tx_en,
        tx_data     => tx_data,
        rx_clk      => rx_clk,
        rx_dv       => rx_dv,
        rx_data     => rx_data,

        --phy
        phy_reset_l => phy_reset_l,
        phy_mdc     => phy_mdc,
        phy_mdio    => phy_mdio,

        ---------------------------------------------------------------------------
        -- user Interface
        ---------------------------------------------------------------------------
        udp_tx_start            => udp_tx_start,
        udp_txi                 => udp_txi,
        udp_tx_result           => open,
        udp_tx_data_out_ready   => udp_tx_data_out_ready,

        -- UDP RX signals
        udp_rx_start            => udp_rx_start,
        udp_rxo                 => udp_rxo,
        CLK_OUT                 => CLK_OUT
    );

    clk_hal <= CLK_OUT;

	--------------------------------------------
	-----------------READ SIDE------------------
    read_proc : process (CLK_OUT, reset_n)
    begin
	    if(reset_n='0') then
			fsm_read_state <= Read_Idle;
            udp_header_valid_prev <= '0';

		elsif(rising_edge(CLK_OUT)) then
            udp_header_valid_prev <= udp_rxo.hdr.is_valid;
            out_data_end_o <= out_data_end_s;
            
            case fsm_read_state is
                when Read_Idle =>
                    if(udp_header_valid_prev = '0'
                    and udp_rxo.hdr.is_valid = '1'
                    and udp_rxo.hdr.src_ip_addr = IP_DEST
                    and udp_rxo.hdr.src_port = PORT_DEST) then
                        fsm_read_state <= Read_Receive;
                    end if;
                    out_data_wr_o <= '0';
                    out_data_end_s <= '0';


                when Read_Receive =>
                    if(udp_rxo.data.data_in_valid = '1') then
                        out_data_o <= udp_rxo.data.data_in;
                        out_data_wr_o <= '1';
                        if(udp_rxo.data.data_in_last = '1') then
                            out_data_end_s <= '1';
                            fsm_read_state <= Read_Idle;
                        else
                            out_data_end_s <= '0';
                            fsm_read_state <= Read_Receive;
                        end if;
                    end if;

                when others =>
            end case;
		end if;
	end process;
	
	--------------------------------------------
	-----------------WRITE SIDE------------------
	udp_txi.hdr.dst_ip_addr		<= IP_DEST;
	udp_txi.hdr.dst_port		<= PORT_DEST;
	udp_txi.hdr.src_port		<= PORT_SRC;
	--udp_txi.hdr.data_length		<= in_data_size_packet;
	udp_txi.hdr.checksum		<= (others => '0');
	
	
	
	write_proc : process(CLK_OUT, reset_n)
	begin		
	    if(reset_n='0') then
			in_data_rd_o				<= '0';
			udp_tx_start				<= '0';
			udp_txi.hdr.data_length		<= (others => '0');
			udp_txi.data.data_out_valid	<= '0';
			udp_txi.data.data_out_last	<= '0';
			udp_txi.data.data_out		<= (others => '0');			
			fsm_write_state 			<= Write_Idle;
			
		elsif rising_edge(CLK_OUT) then
			case fsm_write_state is
			
			------
			when Write_Idle	=>
				if in_data_rdy_i = '1' and in_data_empty_i = '0' then
					in_data_rd_o				<= '0';
					udp_tx_start				<= '1';--
					udp_txi.hdr.data_length		<= in_data_size_packet;
					fsm_write_state				<= Wait_udp_tx_ready;
				else
					in_data_rd_o				<= '0';
					udp_tx_start				<= '0';					
					udp_txi.hdr.data_length		<= (others => '0');
					fsm_write_state				<= Write_Idle;								
				end if;
				udp_txi.data.data_out_valid	<= '0';
				udp_txi.data.data_out_last	<= '0';
				udp_txi.data.data_out		<= (others => '0');	
				
			------	
			when Wait_udp_tx_ready =>
				if udp_tx_data_out_ready = '1' then
					in_data_rd_o	<= '1';--					
					udp_tx_start	<= '0';--
					fsm_write_state	<= wait_one_cycle_to_read_ff;
				else
					in_data_rd_o	<= '0';
					udp_tx_start	<= '1';--
					fsm_write_state	<= Wait_udp_tx_ready;				
				end if;
				udp_txi.hdr.data_length		<= udp_txi.hdr.data_length;
				udp_txi.data.data_out_valid	<= '0';
				udp_txi.data.data_out_last	<= '0';
				udp_txi.data.data_out		<= (others => '0');	
				
			------	
			when wait_one_cycle_to_read_ff =>
				in_data_rd_o				<= '1';
				udp_tx_start				<= '0';
				udp_txi.hdr.data_length		<= udp_txi.hdr.data_length;
				udp_txi.data.data_out_valid	<= '0';
				udp_txi.data.data_out_last	<= '0';
				udp_txi.data.data_out		<= (others => '0');			
				fsm_write_state 			<= transmit_tx;
				
			------	
			when transmit_tx =>				
				if in_data_rdy_i = '0' then --stop reading ff
					in_data_rd_o				<= '0';--stop
					udp_txi.data.data_out_last	<= '1';
					fsm_write_state				<= Write_Idle;					
				else
					in_data_rd_o				<= '1';--hold on
					udp_txi.data.data_out_last	<= '0';
					fsm_write_state				<= transmit_tx;				
				end if;
				udp_tx_start				<= '0';
				udp_txi.hdr.data_length		<= udp_txi.hdr.data_length;
				udp_txi.data.data_out_valid	<= '1';
				udp_txi.data.data_out		<= in_data_i;
				
				
			when others =>
				in_data_rd_o				<= '0';
				udp_tx_start				<= '0';
				udp_txi.hdr.data_length		<= (others => '0');
				udp_txi.data.data_out_valid	<= '0';
				udp_txi.data.data_out_last	<= '0';
				udp_txi.data.data_out		<= (others => '0');			
				fsm_write_state 			<= Write_Idle;
			
			end case;			
		
		end if;
	end process;
end rtl;
