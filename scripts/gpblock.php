<?php

define("LIB_PATH", realpath(dirname(__FILE__)."/..").DIRECTORY_SEPARATOR);
define("SUPPORT_PATH", LIB_PATH . "support" . DIRECTORY_SEPARATOR);
set_include_path(get_include_path().PATH_SEPARATOR.LIB_PATH);
set_include_path(get_include_path().PATH_SEPARATOR.LIB_PATH.DIRECTORY_SEPARATOR.'scripts');
set_include_path(get_include_path().PATH_SEPARATOR.LIB_PATH.DIRECTORY_SEPARATOR.'scripts'.DIRECTORY_SEPARATOR.'model');
set_include_path(get_include_path().PATH_SEPARATOR.LIB_PATH.DIRECTORY_SEPARATOR.'scripts'.DIRECTORY_SEPARATOR.'system_interconnect');
set_include_path(get_include_path().PATH_SEPARATOR.LIB_PATH.DIRECTORY_SEPARATOR.'support');

require_once("process.php");
require_once("io.php");
require_once("gpstudio.php");
require_once('toolchain'.DIRECTORY_SEPARATOR.'hdl'.DIRECTORY_SEPARATOR."block_generator.php");

$options = getopt("a:");
if(array_key_exists('a',$options)) $action = $options['a']; else error("You should specify an action with -a"."\n",1);

// new block creation
if($action=="newprocess")
{
	$options = getopt("a:n:");
	if(array_key_exists('n',$options)) $blockName=$options['n']; else error("You should specify a process name with -n"."\n",1);
	
	$block = new Process();
	$block->name=$blockName;
	$blockName.=".proc";
}
elseif($action=="newio")
{
	$options = getopt("a:n:");
	if(array_key_exists('n',$options)) $blockName=$options['n']; else error("You should specify an io name with -n"."\n",1);
	
	$block = new IO();
	$block->name=$blockName;
	$blockName.=".io";
}
else
{
	// find io or process and open it
	$blockName=findprocess();
	if(!file_exists($blockName))
	{
		$blockName=findio();
		if(!file_exists($blockName))
		{
			if(strpos($action, "list")===false) error("Cannot find a valid block (process or io) in the current directory.",1);
		}
		else $block = new IO($blockName);
	}
	else $block = new Process($blockName);
}

$save = true;

switch($action)
{
	// =========================== global commands =====================
	case "-h":
	case "--help":
		echo "# gpblock command line tool to manage a gpstudio block (v0.95)"."\n";
		$save = false;
		break;
	case "-v":
	case "--version":
		echo "# gpblock command line tool to manage a gpstudio block (v0.95)"."\n";
		$save = false;
		break;
	
	// =========================== project commands ====================
	case "newprocess":
		// nothing to do
		break;
		
	case "newio":
		// nothing to do
		break;
	
	case "generate":
		$options = getopt("a:o:");
		if(array_key_exists('o',$options)) $outDir = $options['o']; else $outDir=getcwd();
		mkdir_rec($outDir);
		
		$block_generator = new Block_generator($block);
		
		$block_generator->generateTopBlock($outDir);
		message($block_generator->block_generator->name.'.vhd'.' generated');
		
		$block_generator->generateSlave($outDir);
		message($block_generator->slave_generator->name.'.vhd'.' generated');
		
		$block_generator->generateProcess($outDir);
		message($block_generator->process_generator->name.'.vhd'.' generated');
		
		$save=false;
		break;
	
	case "generatetop":
		$options = getopt("a:o:");
		if(array_key_exists('o',$options)) $outDir = $options['o']; else $outDir=getcwd();
		mkdir_rec($outDir);
		
		$block_generator = new Block_generator($block);
		$block_generator->generateTopBlock($outDir);
		message($block_generator->block_generator->name.'.vhd'.' generated');
		
		$save=false;
		break;
	
	case "generateslave":
		$options = getopt("a:o:");
		if(array_key_exists('o',$options)) $outDir = $options['o']; else $outDir=getcwd();
		mkdir_rec($outDir);
		
		$block_generator = new Block_generator($block);
		$block_generator->generateSlave($outDir);
		message($block_generator->slave_generator->name.'.vhd'.' generated');
		
		$save=false;
		break;
	
	case "generateprocess":
		$options = getopt("a:o:");
		if(array_key_exists('o',$options)) $outDir = $options['o']; else $outDir=getcwd();
		mkdir_rec($outDir);
		
		$block_generator = new Block_generator($block);
		$block_generator->generateProcess($outDir);
		message($block_generator->process_generator->name.'.vhd'.' generated');
		
		$save=false;
		break;
		
	case "showblock":
		$block->print_flow();
		break;
	
	// =========================== files commands ======================
	case "addfile":
		$options = getopt("a:p:t:g:");
		if(array_key_exists('p',$options)) $path = $options['p']; else error("You should specify a path for the file with -p"."\n",1);
		if(array_key_exists('t',$options)) $type = $options['t']; else error("You should specify a type for the file with -t"."\n",1);
		if(array_key_exists('g',$options)) $group = $options['g']; else error("You should specify a group for the file with -g"."\n",1);
		
		if($block->getFileByPath($path)!=NULL) error("This file already exists added with the same path."."\n",1);
		if(!file_exists($path)) warning("This file does not exist, you should create it."."\n",4);
		
		$file = new File();
		$file->name = basename($path);
		$file->path = $path;
		$file->type = $type;
		$file->group = $group;
		
		$block->addFile($file);
		break;
		
	case "delfile":
		$options = getopt("a:p:");
		if(array_key_exists('p',$options)) $path = $options['p']; else error("You should specify a path for the file with -p"."\n",1);
		
		if($block->getFileByPath($path)==NULL) error("A file does not exist with the path '$path'."."\n",1);
		
		$block->delFileByPath($path);
		break;
		
	case "showfile":
		echo "files :" . "\n";
		foreach($block->files as $file)
		{
			echo "  + ".$file. "\n";
		}
		$save = false;
		break;
	
	// ======================= flows interface commands ================
	case "addflow":
		$options = getopt("a:n:d:s:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the flow with -n"."\n",1);
		if(array_key_exists('d',$options)) $direction = $options['d']; else error("You should specify a direction for the flow with -d [in-out]"."\n",1);
		if(array_key_exists('s',$options)) $size = (int)$options['s']; else error("You should specify a size for the flow with -s"."\n",1);
		
		if($block->getFlow($name)!=NULL) error("This flow name already exists."."\n",1);
		if($direction!="in" and $direction!="out") error("You should specify a direction for the flow with -d [in-out]"."\n",1);
		
		$flow = new Flow();
		$flow->name = $name;
		$flow->type = $direction;
		$flow->size = $size;
		
		$block->addFlow($flow);
		break;
		
	case "delflow":
		$options = getopt("a:n:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the flow with -n"."\n",1);
		
		if($block->getFlow($name)==NULL) error("A flow does not exist with the name '$name'."."\n",1);
		
		$block->delFlow($name);
		break;
		
	case "showflow":
		echo "flows :" . "\n";
		foreach($block->flows as $flow)
		{
			echo "  + ".$flow. "\n";
		}
		$save = false;
		break;
		
	case "renameflow":
		$options = getopt("a:n:v:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the flow with -n"."\n",1);
		if(array_key_exists('v',$options)) $newname = $options['v']; else error("You should specify a new name for the flow with -v"."\n",1);
		
		if($block->getFlow($newname)!=NULL) error("This flow name '$newname' already exists."."\n",1);
		if(($flow=$block->getFlow($name))==NULL) error("A flow does not exist with the name '$name'."."\n",1);
		
		$flow->name = $newname;
		
		break;
		
	case "setflow":
		$options = getopt("a:n:d:s:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the flow with -n"."\n",1);
		
		if(($flow=$block->getFlow($name))==NULL) error("A flow does not exist with the name '$name'."."\n",1);
		
		if(array_key_exists('d',$options))
		{
			if($options['d']!="in" and $options['d']!="out") error("You should specify a direction for the flow with -d [in-out]"."\n",1);
			else $direction=$options['d'];
		}
		else $direction=$flow->type;
		if(array_key_exists('s',$options)) $size = (int)$options['s']; else $size=$flow->size;
		
		$flow->type = $direction;
		$flow->size = $size;
		
		break;
	
	// =========================== params commands ====================
	case "addparam":
		$options = getopt("a:n:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the param with -n"."\n",1);
		
		if($block->getParam($name)!=NULL) error("This param name already exists."."\n",1);
		
		$param = new Param();
		$param->name = $name;
		$param->hard = true;
		$param->default = 0;
		$param->value = 0;
		
		$block->addParam($param);
		break;
		
	case "delparam":
		$options = getopt("a:n:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the param with -n"."\n",1);
		
		if($block->getParam($name)==NULL) error("A param does not exist with the name '$name'."."\n",1);
		
		$block->delParam($name);
		break;
		
	case "showparam":
		echo "param :" . "\n";
		foreach($block->params as $param)
		{
			echo "  + ".$param. "\n";
		}
		$save = false;
		break;
		
	case "renameparam":
		$options = getopt("a:n:v:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the param with -n"."\n",1);
		if(array_key_exists('v',$options)) $newname = $options['v']; else error("You should specify a new name for the param with -v"."\n",1);
		
		if($block->getParam($newname)!=NULL) error("This param name '$newname' already exists."."\n",1);
		if(($param=$block->getParam($name))==NULL) error("A param does not exist with the name '$name'."."\n",1);
		
		$param->name = $newname;
		
		break;
		
	case "setparam":
		$options = getopt("a:n:t:v:r:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the param with -n"."\n",1);
		
		if(($param=$block->getParam($name))==NULL) error("A param does not exist with the name '$name'."."\n",1);
		
		if(array_key_exists('t',$options)) $type = $options['t']; else $type=$param->type;
		if(array_key_exists('v',$options)) $value = $options['v']; else $value=$param->value;
		if(array_key_exists('r',$options)) $regaddr = $options['r']; else $regaddr=$param->regaddr;
		
		$param->type = $type;
		$param->default = $value;
		$param->value = $value;
		$param->regaddr = $regaddr;
		
		break;
		
	case "fixparam":
		$options = getopt("a:n:v:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the param with -n"."\n",1);
		if(array_key_exists('v',$options)) $hard = $options['v']; else $hard="true";
		
		if($hard=="true" or $hard=="TRUE" or $hard=="1") $hard=true;
		elseif($hard=="false" or $hard=="FALSE" or $hard=="0") $hard=false;
		else error("The value of -v option need to be true/false or 0/1."."\n",1);
		
		if(($param=$block->getParam($name))==NULL) error("A param does not exist with the name '$name'."."\n",1);
		
		$param->hard = $hard;
		
		break;
		
	case "setpisizeaddr":
		$options = getopt("a:v:");
		if(array_key_exists('v',$options)) $size = $options['v']; else error("You should specify a PI address relative size in bit with -v"."\n",1);
		if(!is_numeric($size)) error("You should specify a PI address relative size in bit with -v"."\n",1);
		
		$block->pi_size_addr_rel = $size;
		
		break;
	
	// ========================= bitfields commands ====================
	case "addbitfield":
		$options = getopt("a:n:b:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the bitfield with -n"."\n",1);
		if(array_key_exists('b',$options)) $bitfield = (string)$options['b']; else error("You should specify a bitfield bits selection for the bitfield with -b\nExemple: 3,0 => [3 0] or 3-0 => [3 2 1 0] or 6-4,0 => [6 5 4 0]"."\n",1);
		
		$subPath=explode('.',$name);
		if(count($subPath)!=2) error("Invalide name for a bitfield (param.parambitfield)."."\n",1);
		$param=$block->getParam($subPath[0]);
		if($param==NULL) error("Unknow param name '".$subPath[0]."'."."\n",1);
		$paramBitField=$param->getParambitfield($subPath[1]);
		if($paramBitField!=NULL) error("This bitfield name already exists."."\n",1);
		
		$paramBitField = new ParamBitfield();
		$paramBitField->name = $subPath[1];
		$paramBitField->bitfield = $bitfield;
		
		$param->addParamBitfield($paramBitField);
		break;
		
	case "delbitfield":
		$options = getopt("a:n:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the bitfield with -n"."\n",1);
		
		if($block->getParamBitField($name)==NULL) error("A bitfield does not exist with the name '$name'."."\n",1);
		
		$block->delParamBitField($name);
		break;
		
	case "renamebitfield":
		$options = getopt("a:n:v:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the bitfield with -n"."\n",1);
		if(array_key_exists('v',$options)) $newname = $options['v']; else error("You should specify a new name for the bitfield with -v"."\n",1);
		
		if($block->getParambitfield($newname)!=NULL) error("This bitfield name '$newname' already exists."."\n",1);
		if(($bitfield=$block->getParambitfield($name))==NULL) error("A bitfield does not exist with the name '$name'."."\n",1);
		
		$bitfield->name = $newname;
		
		break;
		
	case "setbitfield":
		$options = getopt("a:n:b:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the bitfield with -n"."\n",1);
		if(array_key_exists('b',$options)) $bitfield = (string)$options['b']; else error("You should specify a bitfield bits selection for the bitfield with -b\nExemple: 3,0 => [3 0] or 3-0 => [3 2 1 0] or 6-4,0 => [6 5 4 0]"."\n",1);
		
		if(($paramBitField=$block->getParamBitField($name))==NULL) error("A bitfield does not exist with the name '$name'."."\n",1);
		
		$paramBitField->bitfield = $bitfield;
		
		break;
	
	// =========================== resets commands ====================
	case "addreset":
		$options = getopt("a:n:d:g:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the reset with -n"."\n",1);
		if(array_key_exists('d',$options)) $direction = $options['d']; else error("You should specify a direction for the reset with -d [in-out]"."\n",1);
		if(array_key_exists('g',$options)) $group = $options['g']; else error("You should specify a group for the reset with -g"."\n",1);
		
		if($block->getReset($name)!=NULL) error("This reset name already exists."."\n",1);
		if($direction!="in" and $direction!="out") error("You should specify a direction for the reset with -d [in-out]"."\n",1);
		
		$reset = new Reset();
		$reset->name = $name;
		$reset->direction = $direction;
		$reset->group = $group;
		
		$block->addReset($reset);
		break;
		
	case "delreset":
		$options = getopt("a:n:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the reset with -n"."\n",1);
		
		if($block->getReset($name)==NULL) error("A reset does not exist with the name '$name'."."\n",1);
		
		$block->delReset($name);
		break;
		
	case "showreset":
		echo "resets :" . "\n";
		foreach($block->resets as $reset)
		{
			echo "  + ".$reset. "\n";
		}
		$save = false;
		break;
		
	case "renamereset":
		$options = getopt("a:n:v:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the reset with -n"."\n",1);
		if(array_key_exists('v',$options)) $newname = $options['v']; else error("You should specify a new name for the reset with -v"."\n",1);
		
		if($block->getReset($newname)!=NULL) error("This reset name '$newname' already exists."."\n",1);
		if(($reset=$block->getReset($name))==NULL) error("A reset does not exist with the name '$name'."."\n",1);
		
		$reset->name = $newname;
		
		break;
		
	case "setreset":
		$options = getopt("a:n:d:g:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the reset with -n"."\n",1);
		
		if(($reset=$block->getReset($name))==NULL) error("A reset does not exist with the name '$name'."."\n",1);
		
		if(array_key_exists('d',$options))
		{
			if($options['d']!="in" and $options['d']!="out") error("You should specify a direction for the reset with -d [in-out]"."\n",1);
			else $direction=$options['d'];
		}
		else $direction=$reset->direction;
		if(array_key_exists('g',$options)) $group = $options['g']; else $group=$reset->group;
		
		$reset->direction = $direction;
		$reset->group = $group;
		
		break;
	
	// =========================== clocks commands ====================
	case "addclock":
		$options = getopt("a:n:d:g:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the clock with -n"."\n",1);
		if(array_key_exists('d',$options)) $direction = $options['d']; else error("You should specify a direction for the clock with -d [in-out]"."\n",1);
		if(array_key_exists('g',$options)) $domain = $options['g']; else $domain="";
		
		if($block->getClock($name)!=NULL) error("This clock name already exists."."\n",1);
		if($direction!="in" and $direction!="out") error("You should specify a direction for the clock with -d [in-out]"."\n",1);
		
		$clock = new Clock();
		$clock->name = $name;
		$clock->direction = $direction;
		$clock->domain = $domain;
		
		$block->addClock($clock);
		break;
		
	case "delclock":
		$options = getopt("a:n:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the clock with -n"."\n",1);
		
		if($block->getClock($name)==NULL) error("A clock does not exist with the name '$name'."."\n",1);
		
		$block->delClock($name);
		break;
		
	case "showclock":
		echo "clocks :" . "\n";
		foreach($block->clocks as $clock)
		{
			echo "  + ".$clock. "\n";
		}
		$save = false;
		break;
		
	case "renameclock":
		$options = getopt("a:n:v:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the clock with -n"."\n",1);
		if(array_key_exists('v',$options)) $newname = $options['v']; else error("You should specify a new name for the clock with -v"."\n",1);
		
		if($block->getClock($newname)!=NULL) error("This clock name '$newname' already exists."."\n",1);
		if(($clock=$block->getClock($name))==NULL) error("A clock does not exist with the name '$name'."."\n",1);
		
		$clock->name = $newname;
		
		break;
		
	case "setclock":
		$options = getopt("a:n:d:g:m:f:r:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the clock with -n"."\n",1);
		
		if(($clock=$block->getClock($name))==NULL) error("A clock does not exist with the name '$name'."."\n",1);
		
		if(array_key_exists('d',$options))
		{
			if($options['d']!="in" and $options['d']!="out") error("You should specify a direction for the clock with -d [in-out]"."\n",1);
			else $direction=$options['d'];
		}
		else $direction=$clock->direction;
		if(array_key_exists('g',$options)) $domain = $options['g']; else $domain=$clock->domain;
		if(array_key_exists('m',$options)) $ratio = $options['m']; else $ratio=$clock->ratio;
		if(array_key_exists('f',$options)) $typical = Clock::convert($options['f']); else $ratio=$clock->typical;
		if(array_key_exists('r',$options))
		{
			$range=explode(':',$options['r']);
			if(count($range)!=2) error("You should specify a range whith min:max syntax."."\n",1);
			$min=Clock::convert($range[0]);
			$max=Clock::convert($range[1]);
		}
		else
		{
			$min=$clock->min;
			$max=$clock->max;
		}
		
		$clock->direction = $direction;
		$clock->domain = $domain;
		$clock->ratio = $ratio;
		$clock->typical = $typical;
		$clock->min = $min;
		$clock->max = $max;
		
		break;
	
	// =========================== extport commands ======================
	case "addextport":
		$options = getopt("a:n:t:s:");
		if($block->type()!="io" and $block->type()!="iocom") error("This command can only be used on an io block."."\n",1);
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the external port with -n"."\n",1);
		if(array_key_exists('t',$options)) $type = $options['t']; else error("You should specify a type for the external port with -t [in-out-inout]"."\n",1);
		if(array_key_exists('s',$options)) $size = $options['s']; else error("You should specify a size for the external port with -s"."\n",1);
		
		if($block->getExtPort($name)!=NULL) error("This port already exists added with the same name."."\n",1);
		if($type!="in" and $type!="out" and $type!="out") error("You should specify a type for the external port with -t [in-out-inout]"."\n",1);
		
		$port = new Port();
		$port->name = $name;
		$port->type = $type;
		$port->size = $size;
		
		$block->addExtPort($port);
		break;
		
	case "delextport":
		$options = getopt("a:n:");
		if($block->type()!="io" and $block->type()!="iocom") error("This command can only be used on an io block."."\n",1);
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the file with -n"."\n",1);
		
		if($block->getExtPort($name)==NULL) error("A file does not exist with the path '$path'."."\n",1);
		
		$block->delExtPort($name);
		break;
		
	case "showextport":
		echo "external ports :" . "\n";
		if($block->type()!="io" and $block->type()!="iocom") error("This command can only be used on an io block."."\n",1);
		foreach($block->ext_ports as $ext_port)
		{
			echo "  + ".$ext_port. "\n";
		}
		$save = false;
		break;
	
	// ======================= properties commands ====================
	case "addproperty":
		$options = getopt("a:n:l:t:v:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the property with -n"."\n",1);
		if(array_key_exists('l',$options)) $caption = $options['l']; else error("You should specify a label for the property with -l"."\n",1);
		if(array_key_exists('t',$options)) $type = $options['t']; else error("You should specify a type for the property with -t"."\n",1);
		if(array_key_exists('v',$options)) $value = $options['v']; else $value="";
		
		$parent=$block;
		$subprops = explode('.', $name);
		if(count($subprops)==0) error("Invalid property name '$name'."."\n",1);
		
		for($i=0; $i<count($subprops); $i++)
		{
			$property = $parent->getProperty($subprops[$i]);
			echo $subprops[$i]."\n";
			if($property==null)
			{
				if($i<count($subprops)-1) error("Invalid property name '$name' $parent->name."."\n",1);
			}
			else
			{
				if($i==count($subprops)-1) error("This property name already exists."."\n",1);
				$parent = $property;
			}
		}
		
		$property = new Property();
		$property->name = $subprops[count($subprops)-1];
		$property->caption = $caption;
		$property->type = $type;
		$property->value = $value;
		
		$parent->addProperty($property);
		break;
		
	case "delproperty":
		$options = getopt("a:n:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the property with -n"."\n",1);
		
		if($block->getPropertyPath($name)==NULL) error("A property does not exist with the name '$name'."."\n",1);
		
		$block->delProperty($name);
		break;
		
	case "showproperty":
		echo "properties :" . "\n";
		foreach($block->properties as $property)
		{
			echo "  + ".$property. "\n";
		}
		$save = false;
		break;
		
	case "renameproperty":
		$options = getopt("a:n:v:");

		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the property with -n"."\n",1);
		if(array_key_exists('v',$options)) $newname = $options['v']; else error("You should specify a new name for the property with -v"."\n",1);
		
		if(strpos($newname, '.')!==false) error("Property name cannot contains . (dot)"."\n",1);
		
		if(($property=$block->getPropertyPath($name))==NULL) error("A property does not exist with the name '$name'."."\n",1);
		if($property->parentProperty==NULL)
		{
			if($block->getProperty($newname)!=NULL) error("This property name '$newname' already exists."."\n",1);
		}
		else
		{
			$newpath=$property->parentProperty->path().".".$newname;
			if($block->getPropertyPath($newpath)!=NULL) error("This property name '$newpath' already exists."."\n",1);
		}
		
		$property->name = $newname;
		
		break;
		
	case "setproperty":
		$options = getopt("a:n:l:t:v:r:s:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name for the property with -n"."\n",1);
		
		if(($property=$block->getPropertyPath($name))==NULL) error("A property does not exist with the name '$name'."."\n",1);
		
		if(array_key_exists('l',$options)) $caption = $options['l']; else $caption=$property->caption;
		if(array_key_exists('t',$options)) $type = $options['t']; else $type=$property->type;
		if(array_key_exists('v',$options)) $value = $options['v']; else $value=$property->value;
		if(array_key_exists('r',$options))
		{
			$range=explode(':',$options['r']);
			if(count($range)!=2) error("You should specify a range whith min:max syntax."."\n",1);
			$min=$range[0];
			$max=$range[1];
		}
		else
		{
			$min=$property->min;
			$max=$property->max;
		}
		if(array_key_exists('s',$options)) $step = $options['s']; else $step=$property->step;
		
		$property->caption = $caption;
		$property->type = $type;
		$property->value = $value;
		$property->min = $min;
		$property->max = $max;
		$property->step = $step;
		
		break;
	
	// ========================= global commands ======================
		
	case "sethelp":
		$options = getopt("a:n:v:");
		if(array_key_exists('n',$options)) $name = $options['n']; else error("You should specify a name of the instance to set help with -n"."\n",1);
		if(array_key_exists('v',$options)) $desc = $options['v']; else error("You should specify the help text with -v"."\n",1);
		
		if(($instance=$block->getFileByPath($name))!=NULL) $instance->desc=$desc;
		elseif(($instance=$block->getFlow($name))!=NULL) $instance->desc=$desc;
		elseif(($instance=$block->getParam($name))!=NULL) $instance->desc=$desc;
		elseif(($instance=$block->getReset($name))!=NULL) $instance->desc=$desc;
		elseif(($instance=$block->getClock($name))!=NULL) $instance->desc=$desc;
		elseif(($instance=$block->getPropertyPath($name))!=NULL) $instance->desc=$desc;
		else error("An instance does not exist with the name '$name'."."\n",1);
		
		break;
	
	default:
		error("Action $action is unknow.",1);
}

if($save) $block->saveBlockDef($blockName);

?>
