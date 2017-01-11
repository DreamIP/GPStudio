-- Package used in com block.
-- Can be modified to change the size max of the fifo.

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package com_package is

    type fifo_id is array (0 to 15) of std_logic_vector(5 downto 0);
    type fifo_size is array (0 to 15) of integer range 0 to 32768;
    type fifo_out_clk is array (0 to 15) of std_logic;
    type param_t is array(0 to 49) of std_logic_vector(31 downto 0);

    type array_std_logic    is array (0 to 15) of std_logic;
    type array_flow_t       is array (0 to 15) of flow_t;
    type array_bus16        is array (0 to 15) of std_logic_vector(15 downto 0);
    type array_bus8         is array (0 to 15) of std_logic_vector(7 downto 0);
end com_package;
