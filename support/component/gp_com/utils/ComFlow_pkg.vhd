
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
	constant InitFlagCodes : my_array_t := (
        X"AA", -- Start of Frame Flag
        X"BA", --End of Frame Flag
        X"BC", -- Start+end Flow
        X"DA", -- Start of Line
        X"DB" -- End of Line
    );

	-- Component Declaration
    component com_to_flow
        generic (
            FIFO_DEPTH    : POSITIVE   := 1024;
            FLOW_ID       : INTEGER    := 1;
            FLAGS_CODES   : my_array_t := InitFlagCodes;
            FLOW_SIZE     : INTEGER    := 16;
            DATA_HAL_SIZE : INTEGER    := 16
        );
        port(
            clk_hal       : in std_logic;
            clk_proc      : in std_logic;
            rst_n         : in std_logic;

            data_wr_i     : in std_logic;
            data_i        : in std_logic_vector(DATA_HAL_SIZE-1 downto 0);
            pktend_i      : in std_logic;
            enable_i      : in std_logic;

            data_o        : out std_logic_vector(FLOW_SIZE-1 downto 0);
            fv_o          : out std_logic;
            dv_o          : out std_logic;
            flow_full_o   : out std_logic
        );
    end component;

    component flow_to_com is
        generic (
            FLOW_SIZE       : POSITIVE   := 8;
            DATA_HAL_SIZE   : POSITIVE := 16;
            FIFO_DEPTH      : POSITIVE   := 1024;
            FLOW_ID         : INTEGER    := 1;
            PACKET_SIZE     : INTEGER    := 256;
            FLAGS_CODES     : my_array_t := InitFlagCodes
        );
        port(
            clk_proc        : in std_logic;
            clk_hal         : in std_logic;
            rst_n           : in std_logic;
            
            in_data         : in std_logic_vector(FLOW_SIZE-1 downto 0);
            in_fv           : in std_logic;
            in_dv           : in std_logic;

            rdreq_i         : in std_logic;
            enable_flow_i   : in std_logic;
            enable_global_i : in std_logic;

            data_o          : out std_logic_vector(DATA_HAL_SIZE-1 downto 0);
            flow_rdy_o      : out std_logic;
            f_empty_o       : out std_logic;
            size_packet_o   : out std_logic_vector(15 downto 0)
        );
    end component;

	constant BURSTMODE :std_logic_vector(7 downto 0) := X"BC";

    component com_to_master_pi
        generic (
            FIFO_DEPTH          : POSITIVE := 64;
            FLOW_ID_SET         : INTEGER  := 12;
            --FLOW_ID_GET       : INTEGER  := 13
            MASTER_ADDR_WIDTH   : INTEGER;
            DATA_HAL_SIZE       : POSITIVE := 16
        );
        port (
            clk_hal             : in std_logic; -- clk_usb
            clk_proc            : in std_logic; -- clk_design
            rst_n               : in std_logic;
            
            -- USB driver connexion
            data_wr_i           : in std_logic;
            data_i              : in std_logic_vector(DATA_HAL_SIZE-1 downto 0);
            -- rdreq_i          : in std_logic;
            pktend_i            : in std_logic;
            fifo_full_o         : out std_logic;

            -- signaux pour wishbone
            param_addr_o        : out std_logic_vector(MASTER_ADDR_WIDTH-1 downto 0);
            param_data_o        : out std_logic_vector(31 downto 0);
            param_wr_o          : out std_logic;

            -- may add RAM arbiter connexion
            -- tmp signal to trigger caph update reg
            tmp_update_port_o   : out std_logic
        );
    end component;

    component flow_to_com_arb4
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

            -- fv 1signals
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
    end component;
    
    component gp_com
        generic (
            IN0_SIZE          : INTEGER := 8;
            IN1_SIZE          : INTEGER := 8;
            IN2_SIZE          : INTEGER := 8;
            IN3_SIZE          : INTEGER := 8;
            OUT0_SIZE         : INTEGER := 8;
            OUT1_SIZE         : INTEGER := 8;
            IN0_NBWORDS       : INTEGER := 1280;
            IN1_NBWORDS       : INTEGER := 1280;
            IN2_NBWORDS       : INTEGER := 1280;
            IN3_NBWORDS       : INTEGER := 1280;
            OUT0_NBWORDS      : INTEGER := 1024;
            OUT1_NBWORDS      : INTEGER := 1024;
            CLK_PROC_FREQ     : INTEGER;
            CLK_HAL_FREQ      : INTEGER;
            DATA_HAL_SIZE     : INTEGER;
		    PACKET_HAL_SIZE   : INTEGER;
            MASTER_ADDR_WIDTH : INTEGER
        );
        port (
            clk_proc           : in std_logic;
            reset_n            : in std_logic;

            ------ hal connections ------
            clk_hal            : in std_logic;

            from_hal_data      : in   std_logic_vector(DATA_HAL_SIZE-1 downto 0);
            from_hal_wr	       : in   std_logic;
            from_hal_full      : out  std_logic;
            from_hal_pktend    : in   std_logic;

            to_hal_data        : out  std_logic_vector(DATA_HAL_SIZE-1 downto 0);
            to_hal_rd	       : in   std_logic;
            to_hal_empty       : out  std_logic;
            to_hal_rdy         : out  std_logic;
            to_hal_size_packet : out  std_logic_vector(15 downto 0);

            -------- slave -------
            status_enable      : in std_logic;
            flow_in0_enable    : in std_logic;
            flow_in1_enable    : in std_logic;
            flow_in2_enable    : in std_logic;
            flow_in3_enable    : in std_logic;
                               
            ------ in0 flow ------
            in0_data           : in std_logic_vector(IN0_SIZE-1 downto 0);
            in0_fv             : in std_logic;
            in0_dv             : in std_logic;
            ------ in1 flow ------
            in1_data           : in std_logic_vector(IN1_SIZE-1 downto 0);
            in1_fv             : in std_logic;
            in1_dv             : in std_logic;
            ------ in2 flow ------
            in2_data           : in std_logic_vector(IN2_SIZE-1 downto 0);
            in2_fv             : in std_logic;
            in2_dv             : in std_logic;
            ------ in3 flow ------
            in3_data           : in std_logic_vector(IN3_SIZE-1 downto 0);
            in3_fv             : in std_logic;
            in3_dv             : in std_logic;


            ------ out0 flow ------
            out0_data          : out std_logic_vector(OUT0_SIZE-1 downto 0);
            out0_fv            : out std_logic;
            out0_dv            : out std_logic;
            ------ out1 flow ------
            out1_data          : out std_logic_vector(OUT1_SIZE-1 downto 0);
            out1_fv            : out std_logic;
            out1_dv            : out std_logic;

            ---- ===== Masters =====

            ------ bus_master ------
            master_addr_o      : out std_logic_vector(MASTER_ADDR_WIDTH-1 downto 0);
            master_wr_o        : out std_logic;
            master_rd_o        : out std_logic;
            master_datawr_o    : out std_logic_vector(31 downto 0);
            master_datard_i    : in std_logic_vector(31 downto 0)
        );
    end component;

end package ComFlow_pkg;

package body ComFlow_pkg is

	function clog2(x : integer) return integer is
	begin
        return integer(ceil(log2(real(x))));
	end;

end ComFlow_pkg;
