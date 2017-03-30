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
    
    signal CLK_OUT               : std_logic;

    constant IP_DEST : std_logic_vector(31 downto 0) := x"AC1B014A";
    
    -- fsm read
	type fsm_read_state_t is (Read_Idle, Read_Receive, Read_End);
	signal fsm_read_state : fsm_read_state_t := Read_Idle;

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
        udp_tx_start            => '0',--udp_tx_start,
        udp_txi                 => udp_txi,
        udp_tx_result           => open,
        udp_tx_data_out_ready   => udp_tx_data_out_ready,

        -- UDP RX signals
        udp_rx_start            => udp_rx_start,
        udp_rxo                 => udp_rxo,
        CLK_OUT                 => CLK_OUT
    );

    clk_hal <= CLK_OUT;

    read_proc : process (CLK_OUT, reset_n)
    begin
	    if(reset_n='0') then
			fsm_read_state <= Read_Idle;
		elsif(rising_edge(CLK_OUT)) then
            case fsm_read_state is
                when Read_Idle =>
                    if(udp_rxo.hdr.is_valid = '1' and udp_rxo.hdr.src_ip_addr = IP_DEST) then
                        fsm_read_state <= Read_Receive;
                    end if;
                    out_data_wr_o <= '0';
                    out_data_end_o <= '0';


                when Read_Receive =>
                    if(udp_rxo.data.data_in_valid = '1') then
                        out_data_o <= udp_rxo.data.data_in;
                        out_data_wr_o <= '1';
                        if(udp_rxo.data.data_in_last = '1') then
                            out_data_end_o <= '1';
                            fsm_read_state <= Read_End;
                        else
                            out_data_end_o <= '0';
                            fsm_read_state <= Read_Receive;
                        end if;
                    end if;
                    


                when Read_End =>
                            fsm_read_state <= Read_Idle;

                when others =>
            end case;
		end if;
	end process;

end rtl;
