<?php

require_once("vhdl_generator.php");

function generate_top_level($node, $path)
{
	$generator = new VHDL_generator('top');
	
	// clocks
	foreach($node->board->clocks as $clock)
	{
		$generator->addPort($clock->name, 1, 'in');
	}
	// resets
	foreach($node->board->resets as $reset)
	{
		$generator->addPort($reset->name, 1, $reset->direction);
	}

	// external ios
	foreach($node->blocks as $block)
	{
		if(!empty($block->ext_ports))
		{
			$generator->addPortComment(str_pad(' '.$block->name.' flow ',55,'-',STR_PAD_BOTH));
			foreach($block->ext_ports as $port)
			{
				$generator->addPort($port->name, $port->size, $port->type);
			}
		}
	}
	
	// signals for clocks
	$clocks = array();
	foreach($node->blocks as $block)
	{
		foreach($block->clocks as $clock)
		{
			if(!in_array($clock->group, $clocks)) array_push($clocks, $clock->group);
		}
	}
	foreach($clocks as $clock)
	{
		$generator->addSignal($clock, 1, 'std_logic');
	}
	
	// signals for resets
	$resets = array();
	foreach($node->blocks as $block)
	{
		foreach($block->resets as $reset)
		{
			if(!in_array($reset->group, $resets)) array_push($resets, $reset->group);
		}
	}
	foreach($resets as $reset)
	{
		$generator->addSignal($reset, 1, 'std_logic');
	}
	
	// signals for flows interconnect
	$generator->addSignalComment(str_pad(' flows part ',55,'=',STR_PAD_BOTH));
	foreach($node->blocks as $block)
	{
		if(!empty($block->flows) and $block->name!='fi')
		{
			$generator->addSignalComment(str_pad(' '.$block->name.' ',55,'-',STR_PAD_BOTH));
			foreach($block->flows as $flow)
			{
				$generator->addSignal($block->name . '_' . $flow->name . '_data_s', $flow->size, 'std_logic_vector');
				$generator->addSignal($block->name . '_' . $flow->name . '_fv_s', 1, 'std_logic');
				$generator->addSignal($block->name . '_' . $flow->name . '_dv_s', 1, 'std_logic');
			}
		}
	}
	
	// signals for bus interconnect
	$generator->addSignalComment(str_pad(' bus part ',55,'=',STR_PAD_BOTH));
	foreach($node->blocks as $block)
	{
		if(!empty($block->interfaces))
		{
			foreach($block->interfaces as $interface)
			{
				if($interface->type=='bi_slave')
				{
					$generator->addSignalComment(str_pad(' '.$block->name.' '.$interface->name.' ',55,'-',STR_PAD_BOTH));
					$generator->addSignal($block->name.'_addr_rel_s', $interface->size_addr, 'std_logic_vector');
					$generator->addSignal($block->name.'_wr_s', 1, 'std_logic');
					$generator->addSignal($block->name.'_rd_s', 1, 'std_logic');
					$generator->addSignal($block->name.'_datawr_s', 32, 'std_logic_vector');
					$generator->addSignal($block->name.'_datard_s', 32, 'std_logic_vector');
				}
				if($interface->type=='bi_master')
				{
					$generator->addSignalComment(str_pad(' '.$block->name.' '.$interface->name.' ',55,'-',STR_PAD_BOTH));
					$generator->addSignal($block->name.'_master_addr_s', $interface->size_addr, 'std_logic_vector');
					$generator->addSignal($block->name.'_master_wr_s', 1, 'std_logic');
					$generator->addSignal($block->name.'_master_rd_s', 1, 'std_logic');
					$generator->addSignal($block->name.'_master_datawr_s', 32, 'std_logic_vector');
					$generator->addSignal($block->name.'_master_datard_s', 32, 'std_logic_vector');
				}
			}
		}
	}
	
	$generator->blocks = $node->blocks;
	
	$code = "";
	foreach($node->board->clocks as $clock)
	{
		$code.='	'.$clock->group.'	<=	'.$clock->name.";\n";
	}
	
	$generator->code=$code;
	$generator->save_as_ifdiff($path.DIRECTORY_SEPARATOR.'top.vhd');
}

?>