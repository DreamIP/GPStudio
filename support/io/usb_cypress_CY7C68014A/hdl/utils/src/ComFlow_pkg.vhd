
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.math_real.all;

-- package declaration -- TODO: deplacer dans un fichier externe
package ComFlow_pkg is

	function clog2 ( x : integer) return integer;

	-- Number of flow in design
	constant NBFLOW : integer := 2;
	-- struct pour stocker les ID flow : used for Return Status in USB Driver
	type IDFLOW_t is array (0 to NBFLOW-1) of integer range 0 to 255;


	constant TX_PACKET_SIZE : integer := 256;

	--- Define pour simplifier lecture des codes read/write flow
	constant SoF:integer := 0;
	constant EoF:integer := 1;
	constant Data:integer:= 2;
	constant SoL:integer := 3;
	constant EoL:integer := 4;
	-- Struct pour les flags s
	type my_array_t is array (0 to 4) of std_logic_vector(7 downto 0);
	constant InitFlagCodes : my_array_t := (X"AA", -- Start of Frame Flag
											X"BA", --End of Frame Flag
											X"BC", -- Start+end Flow
											X"DA", -- Start of Line
											X"DB" -- End of Line

								);

	-- Component Declaration
    component com_to_flow
        generic (
            FIFO_DEPTH  : POSITIVE := 1024;
            FLOW_ID     : INTEGER := 1;
            FLAGS_CODES : my_array_t := InitFlagCodes;
            OUTPUT_SIZE : INTEGER:=16
        );
        port(
            clk_in      : in std_logic;
            clk_out     : in std_logic;
            rst_n       : in std_logic;

            data_wr_i   : in std_logic;
            data_i      : in std_logic_vector(15 downto 0);
            pktend_i    : in std_logic;
            enable_i    : in std_logic;

            data_o      : out std_logic_vector(OUTPUT_SIZE-1 downto 0);
            fv_o        : out std_logic;
            dv_o        : out std_logic;
            flow_full_o : out std_logic
        );
    end component;

    component flow_to_com is
        generic (
            FIFO_DEPTH      : POSITIVE := 1024;
            FLOW_ID         : INTEGER := 1;
            PACKET_SIZE     : INTEGER := 256;
            FLAGS_CODES     : my_array_t := InitFlagCodes
        );
        port(
            clk_in          : in std_logic;
            clk_out         : in std_logic;
            rst_n           : in std_logic;
            
            in_data         : in std_logic_vector(15 downto 0);
            in_fv           : in std_logic;
            in_dv           : in std_logic;
            rdreq_i         : in std_logic;
            enable_i        : in std_logic;

            data_o          : out std_logic_vector(15 downto 0);
            flow_rdy_o      : out std_logic;
            f_empty_o       : out std_logic
        );
    end component;

	constant BURSTMODE :std_logic_vector(7 downto 0) := X"BC";
    component com_to_master_pi
        generic (
            FIFO_DEPTH          : POSITIVE := 64;
            FLOW_ID_SET         : integer := 12;
            --FLOW_ID_GET       : integer := 13
            MASTER_ADDR_WIDTH   : integer
        );
        port (
            clk_in              : in std_logic; -- clk_usb
            clk_out             : in std_logic; -- clk_design
            rst_n               : in std_logic;
            
            -- USB driver connexion
            data_wr_i           : in std_logic;
            data_i              : in std_logic_vector(15 downto 0);
            -- rdreq_i          : in std_logic;
            pktend_i            : in std_logic;
            fifo_full_o         : out std_logic;

            -- signaux pour wishbone
            param_addr_o        : out std_logic_vector(MASTER_ADDR_WIDTH-1 DOWNTO 0);
            param_data_o        : out std_logic_vector(31 downto 0);
            param_wr_o          : out std_logic;

            -- may add RAM arbiter connexion
            -- tmp signal to trigger caph update reg
            tmp_update_port_o   : out std_logic
        );
    end component;

	component usb_sm
        port(
            usb_ifclk    : in    std_logic;
            usb_flaga    : in    std_logic;
            usb_flagb    : in    std_logic;
            usb_flagc    : in    std_logic;
            usb_flagd    : in    std_logic;
            usb_fd_io    : inout std_logic_vector(15 downto 0);
            usb_sloe     : out   std_logic;
            usb_slrd     : out   std_logic;
            usb_slwr     : out   std_logic;
            usb_pktend   : out   std_logic;
            usb_addr     : out   std_logic_vector(1 downto 0);

            usb_rst		  : in 	std_logic;

            flow_in_data_o  : out   std_logic_vector(15 downto 0);
            flow_in_wr_o	: out   std_logic;
            flow_in_full_i: in   std_logic;
            flow_in_end_o : out std_logic;

            flow_out_data_i  : in   std_logic_vector(15 downto 0);
            flow_out_rd_o	: out   std_logic;
            flow_out_empty_i: in   std_logic;
            flow_out_rdy_i : in std_logic
        );
	end component;

    component fv_signal_synchroniser
        port (
            clk      : in  std_logic;
            rst_n    : in  std_logic;
            fv_i     : in  std_logic;
            signal_i : in  std_logic;
            signal_o : out std_logic
        );
    end component;

    component usb_cypress_CY7C68014A_slave is
        generic (
            CLK_PROC_FREQ   : integer
        );
        port(
            clk_proc        : in std_logic;
            reset_n         : in std_logic;

            addr_rel_i      : in std_logic_vector(3 downto 0);
            wr_i            : in std_logic;
            rd_i            : in std_logic;
            datawr_i        : in std_logic_vector(31 downto 0);
            datard_o        : out std_logic_vector(31 downto 0);

            status_enable   : out std_logic;
            flow_in0_enable : out std_logic;
            flow_in1_enable : out std_logic;
            flow_in2_enable : out std_logic;
            flow_in3_enable : out std_logic
        );
    end component;

    component flow_to_com_arb4
        port (
            clk             : in std_logic;
            rst_n           : in std_logic;
            
            -- fv 0 signals
            rdreq_0_o       : out std_logic;
            data_0_i        : in std_logic_vector(15 downto 0);
            flow_rdy_0_i    : in std_logic;
            f_empty_0_i     : in std_logic;

            -- fv 1signals
            rdreq_1_o       : out std_logic;
            data_1_i        : in std_logic_vector(15 downto 0);
            flow_rdy_1_i    : in std_logic;
            f_empty_1_i     : in std_logic;

            -- fv 2 signals
            rdreq_2_o       : out std_logic;
            data_2_i        : in std_logic_vector(15 downto 0);
            flow_rdy_2_i    : in std_logic;
            f_empty_2_i     : in std_logic;

            -- fv 3 signals
            rdreq_3_o       : out std_logic;
            data_3_i        : in std_logic_vector(15 downto 0);
            flow_rdy_3_i    : in std_logic;
            f_empty_3_i     : in std_logic;

            -- fv usb signals
            rdreq_usb_i     : in std_logic;
            data_usb_o      : out std_logic_vector(15 downto 0);
            flow_rdy_usb_o  : out std_logic;
            f_empty_usb_o   : out std_logic
        );
    end component;

    component flowto16
        generic (
            INPUT_SIZE: integer;
            FIFO_DEPTH : integer := 32
        );
        port (
            rst_n       : in  std_logic;
            clk         : in  std_logic;
            in_data     : in  std_logic_vector(INPUT_SIZE-1 downto 0);
            in_fv       : in  std_logic;
            in_dv       : in  std_logic;
            out_data    : out std_logic_vector(15 downto 0);
            out_fv      : out std_logic;
            out_dv      : out std_logic
        );
    end component;

end package ComFlow_pkg;

package body ComFlow_pkg is

	function clog2(x : integer) return integer is
	begin
        return integer(ceil(log2(real(x))));
	end;

end ComFlow_pkg;
