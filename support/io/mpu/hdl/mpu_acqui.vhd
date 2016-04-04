--------------------------------------------------------------------------------------
-- The configuration settings choose by the user are read here. If a modification is
-- detected, a configuration is started with the new values.
-- This bloc generates the triggers to acquire data at the sample rate define by user. 
--------------------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;
use work.mpu_pkg.all;

entity mpu_acqui is
port (
		clk_proc		: in std_logic;
		reset			: in std_logic;
		sda			: inout std_logic;
		scl			: inout std_logic;
		AD0			: out std_logic;
		parameters  : in std_logic_vector(31 downto 0);
		data_out		: out std_logic_vector(9 downto 0)
);
end mpu_acqui;


architecture RTL of mpu_acqui is 

signal en					: std_logic;
signal spl_rate 			: std_logic_vector(7 downto 0);
signal gyro_config 		: std_logic_vector(1 downto 0);
signal accel_config		: std_logic_vector(1 downto 0);
signal gain_compass 		: std_logic_vector(2 downto 0);
signal freq_compass 		: std_logic_vector(2 downto 0);
signal trigger				: std_logic;
signal config_button		: std_logic;
signal reset_imu			: std_logic;
signal wr_en				: std_logic;
signal rd_en				: std_logic;
signal rd_en_dl			: std_logic;
signal data_fifo_in		: std_logic_vector(7 downto 0);
signal count_fifo			: std_logic_vector(5 downto 0);
signal wr_en_dl			: std_logic;
signal wr_en_flag			: std_logic;
signal reset_fifo_buffer: std_logic;
signal stop_read			: std_logic;
signal reset_i2c			: std_logic;
signal rd_en_dl2			: std_logic;
signal mode_auto			: std_logic;
signal trigger_reg		: std_logic;
signal trigger_auto 		: std_logic;
signal config_change		: std_logic;

signal spl_rate_dl		: std_logic_vector(7 downto 0);
signal gyro_config_dl	: std_logic_vector(1 downto 0);
signal accel_config_dl	: std_logic_vector(1 downto 0);
signal gain_compass_dl	: std_logic_vector(2 downto 0);
signal freq_compass_dl	: std_logic_vector(2 downto 0);
signal mode_auto_dl		: std_logic;

signal config_init,config_init_dl		: std_logic;

type data_type is
	record
		data	: std_logic_vector(7 downto 0);
		dv		: std_logic;
		fv		: std_logic;
	end record;

signal outdata : data_type;


begin

AD0 			<= '0';
reset_i2c 	<= reset and reset_imu;

process(clk_proc,reset,mode_auto)
variable count_param		: integer range 0 TO 1_700_000;
variable count_rst_fifo : integer range 0 TO 20;
begin
	
	if reset='0' then
		trigger_auto 		<= '0';
		reset_fifo_buffer <= '0';
		count_param			:= 0;
		count_rst_fifo		:= 0;
		config_init 		<= '0';
		
	elsif clk_proc'event and clk_proc='1' then
		config_init_dl <= config_init;
		
		if mode_auto='1' then
			count_param 	:= count_param + 1;
			
			if count_param < COUNT_INIT then
					config_init <= '1';
					
			elsif count_param < COUNT_FIFO_RST then
				config_init <= '0';
				reset_fifo_buffer <= '1';		
				
			elsif count_param < COUNT_END_FIFO_RST then
			
				if count_rst_fifo /= 0 then
					trigger_auto 	<= '1';
				else
					trigger_auto 	<= '0';
				end if;
				   reset_fifo_buffer <= '0';
				
			elsif count_param <= COUNT_ONE_ACQUI+COUNT_START_ACQUI then  
				trigger_auto 		<= '0';
				
				if count_param = COUNT_ONE_ACQUI+COUNT_START_ACQUI then
					count_rst_fifo := count_rst_fifo+1;
					
					if count_rst_fifo = 2 then				
						count_param		:= COUNT_START_FIFO_RST;
						count_rst_fifo	:= 0;
						
					else
						count_param		:= COUNT_START_ACQUI; 
					end if;
				end if;
			end if;
		else
			count_rst_fifo		:= 0;
			count_param			:= COUNT_START_ACQUI; 
			trigger_auto 	<= '0';
			reset_fifo_buffer <= '0';

		end if;
	end if;
end process;


mpu_i2c_inst : entity work.mpu_i2c(behavioral) port map (
	clk   				=> clk_proc,
	en						=> en,
	reset_n				=> reset_i2c,
	config_button  	=> config_button,
	trigger 				=> trigger,
	data_read 			=> data_fifo_in,
	fifo_wr_en			=> wr_en,
	spl_rate				=> spl_rate,
	gyro_config			=> gyro_config,
	accel_config		=> accel_config,
	gain_compass		=> gain_compass,
	freq_compass		=> freq_compass,
	reset_fifo_buffer => reset_fifo_buffer,
	sda 					=> sda,
	scl 					=> scl
	);
	
mpu_fifo_inst_1 : entity work.mpu_fifo(syn) port map (
		data			=> data_fifo_in,
		rdclk			=> clk_proc,
		rdreq			=> rd_en,
		wrclk			=> clk_proc,
		wrreq			=> wr_en_flag,
		q				=> outdata.data,
		rdempty		=> open,
		rdusedw		=> count_fifo,
		wrfull		=> open
	);


process(clk_proc,reset)
begin
		if reset='0' then
			rd_en_dl 	<= '0';
			rd_en 	 	<= '0';
		elsif clk_proc'event and clk_proc='1' then
		
			rd_en_dl 	<= rd_en;
			rd_en_dl2 	<= rd_en_dl;
			spl_rate_dl <= spl_rate;
			accel_config_dl <= accel_config;
			gyro_config_dl <= gyro_config;
			gain_compass_dl <= gain_compass;
			freq_compass_dl <= freq_compass;
			mode_auto_dl 	 <= mode_auto;
			
			
		-----Assignations des données paramètres
			en 				<= parameters(31);
			spl_rate 		<= parameters(30 downto 23);
			gyro_config 	<= parameters(22 downto 21);
			accel_config 	<= parameters(20 downto 19);
			trigger_reg		<= parameters(18);
			config_button	<= config_change;--parameters(17);
			reset_imu		<= parameters(16);
			mode_auto		<= parameters(15);
			gain_compass	<= parameters(14 downto 12);
			freq_compass	<= parameters(11 downto 9);

			if count_fifo = "010100" then		----Nombre de données à lire dans la Fifo
				rd_en		<='1';
			elsif count_fifo = "000010" then ----Fifo vide, fin du read 
				rd_en		<='0';
			end if;
			
		end if;

end process;

config_change <= '1' when ((spl_rate/=spl_rate_dl or gyro_config/=gyro_config_dl or accel_config/=accel_config_dl 
									or gain_compass/=gain_compass_dl or freq_compass/=freq_compass_dl) and mode_auto='0') or (config_init='1' and config_init_dl='0')
									or mode_auto/=mode_auto_dl
					else '0';


process(clk_proc,reset)
variable count : integer range 0 to 50_000;
begin
	if reset = '0' then
		wr_en_dl 	<= '0';
		wr_en_flag  <= '0';	
	elsif clk_proc'event and clk_proc = '1' then
		wr_en_dl 	<= wr_en;						----Flag d'ecriture dans la fifo
		wr_en_flag  <= wr_en and not wr_en_dl;
	end if;
end process;

outdata.dv <= rd_en_dl;
outdata.fv <= rd_en or rd_en_dl2;
data_out   <= outdata.fv & outdata.dv & outdata.data;

trigger <= trigger_auto when mode_auto='1' else
			  trigger_reg;

end RTL;
