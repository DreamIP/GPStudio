-- **************************************************************************
--	READ FLOW
-- **************************************************************************
-- Ce composant est connecte a un com_flow_fifo en entree et a un processing (FV/LV/Data) en sortie
--
-- 16/10/2014 - creation
--------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.ComFlow_pkg.all;

-- Flow out arbiter 4 ways
entity flow_to_com_arb4 is
    generic (
        DATA_HAL_SIZE       : POSITIVE := 16
    );
    port (
        clk             : in std_logic;
        rst_n           : in std_logic;

        -- fv 0 signals
        rdreq_0_o       : out std_logic;
        data_0_i        : in std_logic_vector(DATA_HAL_SIZE-1 downto 0);
        flow_rdy_0_i    : in std_logic;
        f_empty_0_i     : in std_logic;
        size_packet_0_i : in std_logic_vector(15 downto 0);

        -- fv 1 signals
        rdreq_1_o       : out std_logic;
        data_1_i        : in std_logic_vector(DATA_HAL_SIZE-1 downto 0);
        flow_rdy_1_i    : in std_logic;
        f_empty_1_i     : in std_logic;
        size_packet_1_i : in std_logic_vector(15 downto 0);

        -- fv 2 signals
        rdreq_2_o       : out std_logic;
        data_2_i        : in std_logic_vector(DATA_HAL_SIZE-1 downto 0);
        flow_rdy_2_i    : in std_logic;
        f_empty_2_i     : in std_logic;
        size_packet_2_i : in std_logic_vector(15 downto 0);

        -- fv 3 signals
        rdreq_3_o       : out std_logic;
        data_3_i        : in std_logic_vector(DATA_HAL_SIZE-1 downto 0);
        flow_rdy_3_i    : in std_logic;
        f_empty_3_i     : in std_logic;
        size_packet_3_i : in std_logic_vector(15 downto 0);

        -- fv usb signals
        rdreq_usb_i     : in std_logic;
        data_usb_o      : out std_logic_vector(DATA_HAL_SIZE-1 downto 0);
        flow_rdy_usb_o  : out std_logic;
        f_empty_usb_o   : out std_logic;
        size_packet_o   : out std_logic_vector(15 downto 0)
    );
end flow_to_com_arb4;

architecture rtl of flow_to_com_arb4 is
---------------------------------------------------------
--	SIGNALS
---------------------------------------------------------
    signal sel : std_logic_vector(1 downto 0) := (others=>'0');

    type fsm_state_t is (Idle, Hold);
    signal fsm_state    : fsm_state_t := Idle;
    signal rdreq_usb_r  : std_logic := '0';
    signal priority     : unsigned(1 downto 0) := (others=>'0');

begin

FSM:process (clk, rst_n)
begin
	if (rst_n = '0') then
		sel <= "00";
		rdreq_usb_r <= '0';
		priority <= "00";
	elsif rising_edge(clk) then
		rdreq_usb_r <= rdreq_usb_i;
		case (fsm_state) is
			when Idle =>

				if (flow_rdy_0_i = '1' or flow_rdy_1_i ='1' or flow_rdy_2_i ='1' or flow_rdy_3_i ='1') then
					fsm_state <= Hold;
				end if;

				case priority is
					when "00" =>
						if (flow_rdy_0_i = '1') then
							sel <= "00";
						elsif (flow_rdy_1_i = '1') then
							sel <= "01";
						elsif (flow_rdy_2_i = '1') then
							sel <= "10";
						elsif (flow_rdy_3_i = '1') then
							sel <= "11";
						end if;

					when "01" =>
						if (flow_rdy_1_i = '1') then
							sel <= "01";
						elsif (flow_rdy_0_i = '1') then
							sel <= "00";
						elsif (flow_rdy_2_i = '1') then
							sel <= "10";
						elsif (flow_rdy_3_i = '1') then
							sel <= "11";
						end if;

					when "10" =>
						if (flow_rdy_2_i = '1') then
							sel <= "10";
						elsif (flow_rdy_0_i = '1') then
							sel <= "00";
						elsif (flow_rdy_1_i = '1') then
							sel <= "01";
						elsif (flow_rdy_3_i = '1') then
							sel <= "11";
						end if;

					when "11" =>
						if (flow_rdy_3_i = '1') then
							sel <= "11";
						elsif (flow_rdy_0_i = '1') then
							sel <= "00";
						elsif (flow_rdy_1_i = '1') then
							sel <= "01";
						elsif (flow_rdy_2_i = '1') then
							sel <= "10";
						end if;
				end case;

			when Hold =>
				if (rdreq_usb_r='1' and rdreq_usb_i='0') then
					priority <= priority + "1";
					fsm_state <= Idle;
				end if;
		end case;
	end if;

end process;


SEL_inst: process (sel, rdreq_usb_i,
                   data_0_i, flow_rdy_0_i, f_empty_0_i,
                   data_1_i, flow_rdy_1_i, f_empty_1_i,
                   data_2_i, flow_rdy_2_i, f_empty_2_i,
                   data_3_i, flow_rdy_3_i, f_empty_3_i)
begin
	case (sel) is
		when "00" =>
			rdreq_0_o <= rdreq_usb_i;
			rdreq_1_o <= '0';
			rdreq_2_o <= '0';
			rdreq_3_o <= '0';
			data_usb_o <= data_0_i;
			flow_rdy_usb_o <= flow_rdy_0_i;
			f_empty_usb_o <= f_empty_0_i;
			size_packet_o <= size_packet_0_i;

		when "01" =>
			rdreq_0_o <= '0';
			rdreq_1_o <= rdreq_usb_i;
			rdreq_2_o <= '0';
			rdreq_3_o <= '0';
			data_usb_o <= data_1_i;
			flow_rdy_usb_o <= flow_rdy_1_i;
			f_empty_usb_o <= f_empty_1_i;
			size_packet_o <= size_packet_1_i;

		when "10" =>
			rdreq_0_o <= '0';
			rdreq_1_o <= '0';
			rdreq_2_o <= rdreq_usb_i;
			rdreq_3_o <= '0';
			data_usb_o <= data_2_i;
			flow_rdy_usb_o <= flow_rdy_2_i;
			f_empty_usb_o <= f_empty_2_i;
			size_packet_o <= size_packet_2_i;

		when "11" =>
			rdreq_0_o <= '0';
			rdreq_1_o <= '0';
			rdreq_2_o <=  '0';
			rdreq_3_o <= rdreq_usb_i;
			data_usb_o <= data_3_i;
			flow_rdy_usb_o <= flow_rdy_3_i;
			f_empty_usb_o <= f_empty_3_i;
			size_packet_o <= size_packet_3_i;

		when others =>
			rdreq_0_o <= '0';
			rdreq_1_o <= '0';
			rdreq_2_o <= '0';
			rdreq_3_o <= '0';
			data_usb_o <= (others=>'0');
			flow_rdy_usb_o <= '0';
			f_empty_usb_o <= '0';
			size_packet_o <= (others=>'0');
		end case;
end process;

end rtl;
