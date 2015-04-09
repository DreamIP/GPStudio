<?php

require_once("toolchain.php");
require_once("toolchain/hdl/hdl.php");

function pgcd($a,$b)
{
    for($c = $a % $b ; $c != 0; $c= $a%$b ) :
       $a = $b; $b = $c;
    endfor;
         
    return $b;
}

class Altera_quartus_toolchain extends HDL_toolchain
{
	private $nopartitions;
	private $noqsf;
	
	public function configure_project($node)
	{
		global $argv;
		parent::configure_project($node);
		
		if(in_array("nopartitions", $argv)) $this->nopartitions = 1; else $this->nopartitions = 0;
		if(in_array("noqsf", $argv)) $this->noqsf = 1; else $this->noqsf = 0;
	}
	
	public function generate_project($node, $path)
	{
		parent::generate_project($node, $path);
		$this->generate_tcl($node, $path);
		$this->generate_project_file($node, $path);
	}
	
	protected function generate_project_file($node, $path)
	{
		$content='';
		
		$content.="QUARTUS_VERSION = \"13.1\""."\n";
		$content.="PROJECT_REVISION = \"start\""."\n";
		
		// copy all files for block declared in library
		foreach($node->blocks as $block)
		{
			if($block->in_lib)
			{
				foreach($block->files as $file)
				{
					if($file->generated!==true)
					{
						if(!file_exists($block->path.$file->path))
						{
							warning("$block->path$file->path doesn't exists",5,$block->name);
						}
						else
						{
							$subpath = 'IP'.DIRECTORY_SEPARATOR.$block->driver.DIRECTORY_SEPARATOR.dirname($file->path);
							
							// create directory
							if(!is_dir($path.DIRECTORY_SEPARATOR.$subpath)) mkdir($path.DIRECTORY_SEPARATOR.$subpath, 0777, true);
							
							// check if copy is needed
							$needToCopy = false;
							if(file_exists($path.DIRECTORY_SEPARATOR.$subpath.DIRECTORY_SEPARATOR.$file->name))
							{
								if(filemtime($block->path.$file->path) > filemtime($path.DIRECTORY_SEPARATOR.$subpath.DIRECTORY_SEPARATOR.$file->name))
								{
									$needToCopy = true;
								}
							}
							else
							{
								$needToCopy = true;
							}
							
							// copy if need
							if($needToCopy)
							{
								if (!copy($block->path.$file->path, $path.DIRECTORY_SEPARATOR.$subpath.DIRECTORY_SEPARATOR.$file->name))
								{
									warning("failed to copy $file->name",5,$block->name);
								}
							}
							
							// update the path file to the new copy path relative to project
							$file->path=$subpath.DIRECTORY_SEPARATOR.$file->name;
						}
					}
				}
			}
		}
		
		// save file if it's different
		$filename = $path.DIRECTORY_SEPARATOR."$node->name.qpf";
		$needToReplace = false;
		
		if(!file_exists($filename)) $needToReplace = true;
		
		if($needToReplace)
		{
			if (!$handle = fopen($filename, 'w')) error("$filename cannot be openned",5,"Altera toolchain");
			if (fwrite($handle, $content) === FALSE) error("$filename cannot be written",5,"Altera toolchain");
			fclose($handle);
		}
	}
	
	protected function generate_tcl($node, $path)
	{
		$content='';
		
		// global board attributes
		$content.="# =============== global assignement ===============\n";
		foreach($node->board->toolchain->attributes as $attribute)
		{
			$value = $attribute->value;
			if(strpos($value,' ')!==false and strpos($value,'-section_id')===false) $value = '"'.$value.'"';
			$content.='set_'.$attribute->type.'_assignment -name '.$attribute->name.' '.$value."\n";
		}
		$content.="\n# --------- pins ---------\n";
		foreach($node->board->pins as $pin)
		{
			if(!empty($pin->mapto)) $content.='set_location_assignment '.$pin->name.' -to '.$pin->mapto."\n";
			foreach($pin->attributes as $attribute)
			{
				$value = $attribute->value;
				if(strpos($value,' ')!==false and strpos($value,'-section_id')===false) $value = '"'.$value.'"';
				$content.='set_'.$attribute->type.'_assignment -name '.$attribute->name.' '.$value.' -to '.$pin->name."\n";
			}
		}
		$content.="\n# --------- files ---------\n";
		$content.='set_global_assignment -name VHDL_FILE top.vhd'."\n";
		
		if($this->nopartitions==0)
		{
			$content.="\n# -------- partitions --------\n";
			$content.='set_instance_assignment -name PARTITION_HIERARCHY root_partition -to | -section_id Top'."\n";
		}
		
		// blocks assignement
		$content.="\n\n# ================================== blocks assignement ==================================\n";
		foreach($node->blocks as $block)
		{
			$content.="\n# **************************************** $block->name ****************************************";
			
			// pins
			if(!empty($block->pins))
			{
				$content.="\n# --------- pins ---------\n";
				foreach($block->pins as $pin)
				{
					if(!empty($pin->mapto)) $content.='set_location_assignment '.$pin->name.' -to '.$block->name.'_'.$pin->mapto."\n";
					foreach($pin->attributes as $attribute)
					{
						$value = $attribute->value;
						if(strpos($value,' ')!==false and strpos($value,'-section_id')===false) $value = '"'.$value.'"';
						$content.='set_'.$attribute->type.'_assignment -name '.$attribute->name.' '.$value.' -to '.$pin->name."\n";
					}
				}
			}
			
			// files
			$content.="\n# --------- files ---------\n";
			foreach($block->files as $file)
			{
				if($file->group=="hdl")
				{
					$type='';
					if($file->type=="verilog") {$type='VERILOG_FILE';}
					elseif($file->type=="vhdl") {$type='VHDL_FILE';}
					elseif($file->type=="qip") {$type='QIP_FILE';}
					elseif($file->type=="sdc") {$type='SDC_FILE';}
					elseif($file->type=="hex") {$type='HEX_FILE';}
					
					if($block->in_lib and $file->generated===false)
					{
						$subpath = 'IP'.DIRECTORY_SEPARATOR.$block->driver.DIRECTORY_SEPARATOR.dirname($file->path);
					}
					else
					{
						$subpath = dirname($file->path);
					}
					if(!empty($subpath)) $subpath .= DIRECTORY_SEPARATOR;
					$content.="set_global_assignment -name $type ".$subpath.$file->name."\n";
				}
			}
			
			// partitions
			if($this->nopartitions==0)
			{
				$content.="\n# --------- partitions ---------\n";
				if($block->name==$block->driver)
				{
					$instance=$block->driver.':'.$block->name.'_inst';
				}
				else
				{
					$instance=$block->driver.':'.$block->name;
				}
				$partition_name = substr(str_replace('_','',$block->driver), 0, 4).'_'.substr(md5('top/'.$instance), 0, 4);
				$content.="set_instance_assignment -name PARTITION_HIERARCHY $partition_name -to \"$instance\" -section_id \"$instance\""."\n";
				$content.="set_global_assignment -name PARTITION_NETLIST_TYPE POST_SYNTH -section_id \"$instance\""."\n";
				$content.="set_global_assignment -name PARTITION_FITTER_PRESERVATION_LEVEL PLACEMENT_AND_ROUTING -section_id \"$instance\""."\n";
			}
		}
		
		if($this->noqsf==0)
		{
			// save file if it's different
			$filename = $path.DIRECTORY_SEPARATOR."$node->name.qsf";
			$needToReplace = false;
			
			if(file_exists($filename))
			{
				$handle = fopen($filename, 'r');
				$actualContent = fread($handle, filesize($filename));
				fclose($handle);
				if($actualContent != $content) $needToReplace = true;
			}
			else $needToReplace = true;
			
			if($needToReplace)
			{
				if (!$handle = fopen($filename, 'w')) error("$filename cannot be openned",5,"Altera toolchain");
				if (fwrite($handle, $content) === FALSE) error("$filename cannot be written",5,"Altera toolchain");
				fclose($handle);
			}
		}
	}
	
	function getRessourceAttributes($type)
	{
		$attr = array();
		switch ($type)
		{
			case 'pll':
				$attr['maxPLL']=4;
				$attr['clkByPLL']=5;
				break;
			default:
				warning("ressource $type does'nt exist");
		}
		return $attr;
	}
	
	function getRessourceDeclare($type, $params)
	{
		$declare='';
		switch ($type)
		{
			case 'pll':
				$declare.='	COMPONENT altpll'."\n";
				$declare.='	GENERIC ('."\n";
				$declare.='		bandwidth_type		: STRING;'."\n";
				$declare.='		clk0_divide_by		: NATURAL;'."\n";
				$declare.='		clk0_duty_cycle		: NATURAL;'."\n";
				$declare.='		clk0_multiply_by		: NATURAL;'."\n";
				$declare.='		clk0_phase_shift		: STRING;'."\n";
				$declare.='		clk1_divide_by		: NATURAL;'."\n";
				$declare.='		clk1_duty_cycle		: NATURAL;'."\n";
				$declare.='		clk1_multiply_by		: NATURAL;'."\n";
				$declare.='		clk1_phase_shift		: STRING;'."\n";
				$declare.='		clk2_divide_by		: NATURAL;'."\n";
				$declare.='		clk2_duty_cycle		: NATURAL;'."\n";
				$declare.='		clk2_multiply_by		: NATURAL;'."\n";
				$declare.='		clk2_phase_shift		: STRING;'."\n";
				$declare.='		clk3_divide_by		: NATURAL;'."\n";
				$declare.='		clk3_duty_cycle		: NATURAL;'."\n";
				$declare.='		clk3_multiply_by		: NATURAL;'."\n";
				$declare.='		clk3_phase_shift		: STRING;'."\n";
				$declare.='		clk4_divide_by		: NATURAL;'."\n";
				$declare.='		clk4_duty_cycle		: NATURAL;'."\n";
				$declare.='		clk4_multiply_by		: NATURAL;'."\n";
				$declare.='		clk4_phase_shift		: STRING;'."\n";
				$declare.='		compensate_clock		: STRING;'."\n";
				$declare.='		inclk0_input_frequency		: NATURAL;'."\n";
				$declare.='		intended_device_family		: STRING;'."\n";
				$declare.='		lpm_hint		: STRING;'."\n";
				$declare.='		lpm_type		: STRING;'."\n";
				$declare.='		operation_mode		: STRING;'."\n";
				$declare.='		pll_type		: STRING;'."\n";
				$declare.='		port_activeclock		: STRING;'."\n";
				$declare.='		port_areset		: STRING;'."\n";
				$declare.='		port_clkbad0		: STRING;'."\n";
				$declare.='		port_clkbad1		: STRING;'."\n";
				$declare.='		port_clkloss		: STRING;'."\n";
				$declare.='		port_clkswitch		: STRING;'."\n";
				$declare.='		port_configupdate		: STRING;'."\n";
				$declare.='		port_fbin		: STRING;'."\n";
				$declare.='		port_inclk0		: STRING;'."\n";
				$declare.='		port_inclk1		: STRING;'."\n";
				$declare.='		port_locked		: STRING;'."\n";
				$declare.='		port_pfdena		: STRING;'."\n";
				$declare.='		port_phasecounterselect		: STRING;'."\n";
				$declare.='		port_phasedone		: STRING;'."\n";
				$declare.='		port_phasestep		: STRING;'."\n";
				$declare.='		port_phaseupdown		: STRING;'."\n";
				$declare.='		port_pllena		: STRING;'."\n";
				$declare.='		port_scanaclr		: STRING;'."\n";
				$declare.='		port_scanclk		: STRING;'."\n";
				$declare.='		port_scanclkena		: STRING;'."\n";
				$declare.='		port_scandata		: STRING;'."\n";
				$declare.='		port_scandataout		: STRING;'."\n";
				$declare.='		port_scandone		: STRING;'."\n";
				$declare.='		port_scanread		: STRING;'."\n";
				$declare.='		port_scanwrite		: STRING;'."\n";
				$declare.='		port_clk0		: STRING;'."\n";
				$declare.='		port_clk1		: STRING;'."\n";
				$declare.='		port_clk2		: STRING;'."\n";
				$declare.='		port_clk3		: STRING;'."\n";
				$declare.='		port_clk4		: STRING;'."\n";
				$declare.='		port_clk5		: STRING;'."\n";
				$declare.='		port_clkena0		: STRING;'."\n";
				$declare.='		port_clkena1		: STRING;'."\n";
				$declare.='		port_clkena2		: STRING;'."\n";
				$declare.='		port_clkena3		: STRING;'."\n";
				$declare.='		port_clkena4		: STRING;'."\n";
				$declare.='		port_clkena5		: STRING;'."\n";
				$declare.='		port_extclk0		: STRING;'."\n";
				$declare.='		port_extclk1		: STRING;'."\n";
				$declare.='		port_extclk2		: STRING;'."\n";
				$declare.='		port_extclk3		: STRING;'."\n";
				$declare.='		width_clock		: NATURAL'."\n";
				$declare.='	);'."\n";
				$declare.='	PORT ('."\n";
				$declare.='			clk	: OUT STD_LOGIC_VECTOR (4 DOWNTO 0);'."\n";
				$declare.='			inclk	: IN STD_LOGIC_VECTOR (1 DOWNTO 0)'."\n";
				$declare.='	);'."\n";
				$declare.='	END COMPONENT;'."\n";
				break;
			default:
				warning("ressource $type does'nt exist");
		}
		return $declare;
	}
	
	function getRessourceInstance($type, $params)
	{
		$instance='';
		switch ($type)
		{
			case 'pll':
				$clkin = $params['clkin'];
				$pllname = $params['pllname'];
				
				$instance.='	'.$pllname.' : altpll'."\n";
				$instance.='	GENERIC MAP ('."\n";
				$instance.='		bandwidth_type => "AUTO",'."\n";
				$instance.='		compensate_clock => "CLK0",'."\n";
				$instance.='		intended_device_family => "Cyclone III",'."\n";
				$instance.='		lpm_hint => "CBX_MODULE_PREFIX=pll",'."\n";
				$instance.='		lpm_type => "altpll",'."\n";
				$instance.='		operation_mode => "NORMAL",'."\n";
				$instance.='		pll_type => "AUTO",'."\n";
				$instance.='		port_inclk0 => "PORT_USED",'."\n";
				
				$instance.='		port_activeclock => "PORT_UNUSED",'."\n";
				$instance.='		port_areset => "PORT_UNUSED",'."\n";
				$instance.='		port_clkbad0 => "PORT_UNUSED",'."\n";
				$instance.='		port_clkbad1 => "PORT_UNUSED",'."\n";
				$instance.='		port_clkloss => "PORT_UNUSED",'."\n";
				$instance.='		port_clkswitch => "PORT_UNUSED",'."\n";
				$instance.='		port_configupdate => "PORT_UNUSED",'."\n";
				$instance.='		port_fbin => "PORT_UNUSED",'."\n";
				$instance.='		port_inclk1 => "PORT_UNUSED",'."\n";
				$instance.='		port_locked => "PORT_UNUSED",'."\n";
				$instance.='		port_pfdena => "PORT_UNUSED",'."\n";
				$instance.='		port_phasecounterselect => "PORT_UNUSED",'."\n";
				$instance.='		port_phasedone => "PORT_UNUSED",'."\n";
				$instance.='		port_phasestep => "PORT_UNUSED",'."\n";
				$instance.='		port_phaseupdown => "PORT_UNUSED",'."\n";
				$instance.='		port_pllena => "PORT_UNUSED",'."\n";
				$instance.='		port_scanaclr => "PORT_UNUSED",'."\n";
				$instance.='		port_scanclk => "PORT_UNUSED",'."\n";
				$instance.='		port_scanclkena => "PORT_UNUSED",'."\n";
				$instance.='		port_scandata => "PORT_UNUSED",'."\n";
				$instance.='		port_scandataout => "PORT_UNUSED",'."\n";
				$instance.='		port_scandone => "PORT_UNUSED",'."\n";
				$instance.='		port_scanread => "PORT_UNUSED",'."\n";
				$instance.='		port_scanwrite => "PORT_UNUSED",'."\n";
				$instance.='		port_clkena0 => "PORT_UNUSED",'."\n";
				$instance.='		port_clkena1 => "PORT_UNUSED",'."\n";
				$instance.='		port_clkena2 => "PORT_UNUSED",'."\n";
				$instance.='		port_clkena3 => "PORT_UNUSED",'."\n";
				$instance.='		port_clkena4 => "PORT_UNUSED",'."\n";
				$instance.='		port_clkena5 => "PORT_UNUSED",'."\n";
				$instance.='		port_extclk0 => "PORT_UNUSED",'."\n";
				$instance.='		port_extclk1 => "PORT_UNUSED",'."\n";
				$instance.='		port_extclk2 => "PORT_UNUSED",'."\n";
				$instance.='		port_extclk3 => "PORT_UNUSED",'."\n";
		
				$instance.='		inclk0_input_frequency => '.(1000000000000/$clkin).','."\n";
			
				$clkId=0;
				foreach($params['clks'] as $clk)
				{
					$clockFreq = $clk[0];
					$clockShift = $clk[1];
			
					$div = $clkin / pgcd($clockFreq, $clkin);
					$mul = floor($clockFreq / ($clkin / $div));
					if($clockShift==0) $shift=0; else $shift=($clockShift/360)*10000000;
					
					$instance.='		-- clk'.$clkId.' at '.formatFreq($clockFreq).' '.$clockShift.'° shifted'."\n";
					$instance.='		port_clk'.$clkId.' => "PORT_USED",'."\n";
					$instance.='		clk'.$clkId.'_duty_cycle => 50,'."\n";
					$instance.='		clk'.$clkId.'_divide_by => '.$div.','."\n";
					$instance.='		clk'.$clkId.'_multiply_by => '.$mul.','."\n";
					$instance.='		clk'.$clkId.'_phase_shift => "'.$shift.'",'."\n";
					
					$clkId++;
				}
				for($i=$clkId; $i<=5; $i++)
				{
					$instance.='		port_clk'.$i.' => "PORT_UNUSED",'."\n";
				}
				$instance.='		width_clock => 5'."\n";
				$instance.='	)'."\n";
				$instance.='	PORT MAP ('."\n";
				$instance.='		inclk => '.$pllname.'_in,'."\n";
				$instance.='		clk => '.$pllname.'_out'."\n";
				$instance.='	);'."\n";
			
				break;
			default:
				warning("ressource $type does'nt exist");
		}
		return $instance;
	}
}

?>
