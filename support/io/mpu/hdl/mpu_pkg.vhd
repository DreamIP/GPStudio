--------------------------------------------------------------------
-- Package containing several constant data
--------------------------------------------------------------------

library ieee;
USE ieee.std_logic_1164.all;


package mpu_pkg is

	constant ADDR_I2C_MPU 			: STD_LOGIC_VECTOR(6 DOWNTO 0) := "1101000";
	constant ADDR_I2C_COMPASS 		: STD_LOGIC_VECTOR(6 DOWNTO 0) := "0011110";
	constant PWR_MGMT_1				: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"6B";
	constant USER_CTRL				: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"6A";
	constant ACCEL_CONFIG_REG		: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"1C";
	constant SMPLRT_DIV				: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"19";
	constant FIFO_EN					: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"23";
	constant GYRO_CONFIG_REG		: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"1B";
	constant BYPASS_MPU				: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"37";
	constant COMPASS_CONF_A			: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"00";
	constant COMPASS_CONF_B			: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"01";
	constant COMPASS_MODE			: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"02";
	constant I2C_MST_CTRL			: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"24";
	constant I2C_SLV0_ADDR			: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"25";
	constant I2C_SLV0_REG			: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"26";
	constant I2C_SLV0_CTRL			: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"27";
	constant FIFO_READ				: STD_LOGIC_VECTOR(7 DOWNTO 0) := x"74";
--	constant COUNT_INIT				: INTEGER RANGE 0 TO 16_000:=15_000;
--	constant COUNT_FIFO_RST			: INTEGER RANGE 0 TO 64_000:=45_000;
--	--constant COUNT_ONE_ACQUI		: INTEGER RANGE 0 TO 1_700_000:=416_667;--1_600_000;
--	constant COUNT_START_ACQUI		: INTEGER RANGE 0 TO 64_000:=50_000;
--	constant COUNT_START_FIFO_RST	: INTEGER RANGE 0 TO 64_000:=50_000;--20_000
--	constant COUNT_END_FIFO_RST	: INTEGER RANGE 0 TO 64_000:=60_000;
	


end mpu_pkg;
