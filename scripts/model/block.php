<?php

/** 
 * Block is the base of IO and Process block. 
 * @brief Block is the base of IO and Process block.
 * @see IO Process
 * @ingroup base
 */

require_once("file.php");
require_once("param.php");
require_once("property.php");
require_once("flow.php");
require_once("clock.php");
require_once("reset.php");
require_once("pin.php");
require_once("port.php");
require_once("interfacebus.php");
require_once("attribute.php");

class Block
{
	/**
	* Name of the block
	* @var string $name
	*/
	public $name;

	/**
	* Path where the root of files and define of the block is putted
	* @var string $path
	*/
	public $path;

	/**
	* Specify if the block is defined in the library or not
	* @var bool $in_lib
	*/
	public $in_lib;

	/**
	* Specify the name of the driver for the block
	* @var string $driver
	*/
	public $driver;

	/**
	* Specify the categorie of the block eg : communication, imagesensor, descriptor...
	* @var string $categ
	*/
	public $categ;
	
	/**
	* The absolute adress of the block on BI
	* @var int $addr_abs
	*/
	public $addr_abs;

	/**
	* Size of relative adress bus
	* @var int $pi_size_addr_rel
	*/
	public $pi_size_addr_rel;

	/**
	* Number of master in the block on BI
	* @var int $master_count
	*/
	public $master_count;
	
	/**
	* X position on schematic (optional)
	* @var int $x_pos
	*/
	public $x_pos;
	
	/**
	* Y position on schematic (optional)
	* @var int $y_pos
	*/
	public $y_pos;

	/**
	* Specify the external file script to configure the block (optional)
	* @var string $configscriptfile
	*/
	public $configscriptfile;

	/**
	* Specify the external file script to generate the block (optional)
	* @var string $generatescriptfile
	*/
	public $generatescriptfile;

	/**
	* Description of the flow (optional)
	* @var string $desc
	*/
	public $desc;


	/**
	* Array of parameters class (can be Generic or dynamics parameter on BI)
	* @var array|Param $params
	*/
	public $params;
	
	/**
	* Array of property class specify the high level properties
	* @var array|Property $properties
	*/
	public $properties;

	/**
	* Array of files whith define the implementation of the block
	* @var array|File $files
	*/
	public $files;
	
	/**
    * Array of flows in the block can be input flow or output
    * @var array|Flow $flows
    */
	public $flows;

	/**
	* Array of clocks to drive the block
	* @var array|Clock $clocks
	*/
	public $clocks;

	/**
	* Array of resets, can be different type of resets
	* @var array|Reset $resets
	*/
	public $resets;

	/**
	* Array of interfaces of the block
	* @var array|InterfaceBus $interfaces
	*/
	public $interfaces;
	
	/**
	* Array of attributes of the block (optional)
	* @var array|Attribute $attributes
	*/
	public $attributes;

	/**
	* Reference to the associated parent node
	* @var Node $parentNode
	*/
	public $parentNode;

	
	protected $xml;
	
	function __construct()
	{
		$this->params = array();
		$this->properties = array();
		$this->files = array();
		$this->flows = array();
		$this->clocks = array();
		$this->resets = array();
		$this->interfaces = array();
		$this->attributes = array();
		
		$this->addr_abs = -1;
		$this->master_count = 0;
		$this->x_pos=-1;
		$this->y_pos=-1;
		$this->configscriptfile='';
		$this->generatescriptfile='';
		
		$this->in_lib=false;
		
		$clock = new Clock();
		$clock->name='clk_proc';
		$clock->domain='clk_proc';
		$this->addClock($clock);
		
		/*$reset = new Reset();
		$reset->name='reset';
		$reset->group='reset';
		$this->addReset($reset);*/
	}
	
	function print_flow()
	{
		// count flow in and out
		$flowIn=array();
		$maxInLenght=0;
		$flowOut=array();
		$maxOutLenght=0;
		foreach($this->flows as $flow)
		{
			if($flow->type=="in")  {$flowIn[]=$flow; $maxInLenght=max($maxInLenght, strlen($flow->name.$flow->size)+3);}
			if($flow->type=="out") {$flowOut[]=$flow; $maxOutLenght=max($maxOutLenght, strlen($flow->name.$flow->size)+3);}
		}
		$lenghtBlock=strlen($this->name)+8;
		$maxInLenght+=4;
		$maxOutLenght+=4;
		
		echo str_repeat(' ',$maxInLenght).str_repeat('-',$lenghtBlock+2).str_repeat(' ',$maxOutLenght)."\n";
		$maxFlow = max(count($flowIn), count($flowOut));
		for($i=0;$i<$maxFlow;$i++)
		{
			// first in line
			if($i<count($flowIn)) echo str_pad($flowIn[$i]->name.' ('.$flowIn[$i]->size.')',$maxInLenght,' ',STR_PAD_BOTH);
			else echo str_repeat(' ',$maxInLenght);
			
			// block 1
			echo '|';
			if($i==floor($maxFlow/2)) echo str_pad($this->name,$lenghtBlock,' ',STR_PAD_BOTH); else echo str_repeat(' ',$lenghtBlock);
			echo '|';
			
			// first out line
			if($i<count($flowOut)) echo str_pad($flowOut[$i]->name.' ('.$flowOut[$i]->size.')',$maxOutLenght,' ',STR_PAD_BOTH);
			else echo str_repeat(' ',$maxOutLenght);
			echo "\n";
			
			// second in line
			if($i<count($flowIn)) echo str_repeat('-',$maxInLenght-1).'>';
			else echo str_repeat(' ',$maxInLenght);
			
			// block 2
			echo '|';
			echo str_repeat(' ',$lenghtBlock);
			echo '|';
			
			// second out line
			if($i<count($flowOut)) echo str_repeat('-',$maxOutLenght-1).'>';
			else echo str_repeat(' ',$maxOutLenght);
			echo "\n";
		}
		echo str_repeat(' ',$maxInLenght).str_repeat('-',$lenghtBlock+2).str_repeat(' ',$maxOutLenght)."\n";
	}
	
	function configure($node, $block)
	{
		if(!empty($this->configscriptfile))
		{
			if(file_exists($this->path.$this->configscriptfile))
			{
				$script = str_replace(SUPPORT_PATH,'',$this->path.$this->configscriptfile);
				$configureBlock = (include $script);
				if($configureBlock!==FALSE)
				{
					$configureBlock($node, $block);
				}
			}
		}
	}
	
	function generate($node, $block, $path, $language)
	{
		if(!empty($this->generatescriptfile))
		{
			if(file_exists($this->path.$this->generatescriptfile))
			{
				$script = str_replace(SUPPORT_PATH,'',$this->path.$this->generatescriptfile);
				$generateBlock = (include $script);
				if($generateBlock!==FALSE)
				{
					$generateBlock($node, $block, $path, $language);
				}
			}
		}
	}
	
	/** Add a parameter to the block 
	 *  @param Param $param parameter to add to the block **/
	function addParam($param)
	{
		$param->parentBlock = $this;
		array_push($this->params, $param);
	}
	
	/** return a reference to the parameter with the name $name, if not found, return null
	 *  @param string $name name of the parameter to search
	 *  @return Param found parameter **/
	function getParam($name)
	{
		foreach($this->params as $param)
		{
			if($param->name==$name) return $param;
		}
		return null;
	}
	
	/** delete a param from his name
	 *  @param string $name name of the param to delete  **/
	function delParam($name)
	{
		$i=0;
		foreach($this->params as $param)
		{
			if($param->name==$name) {unset($this->params[$i]); return;}
			$i++;
		}
		return null;
	}
	
	/** return a reference to the bitfield with the path $path, if not found, return null
	 *  @param string $path path of the parambitfield to search (param.parambitfield)
	 *  @return ParamBitfield found bitfield **/
	function getParamBitField($path)
	{
		$subPath=explode('.',$path);
		if(count($subPath)!=2) return NULL;
		$param=$this->getParam($subPath[0]);
		if($param==NULL) return NULL;
		$paramBitField=$param->getParambitfield($subPath[1]);
		return $paramBitField;
	}
	
	/** return a reference to the bitfield with the path $path, if not found, return null
	 *  @param string $path path of the parambitfield to search (param.parambitfield)
	 *  @return ParamBitfield found bitfield **/
	function delParamBitField($path)
	{
		$subPath=explode('.',$path);
		if(count($subPath)!=2) return NULL;
		$param=$this->getParam($subPath[0]);
		if($param==NULL) return NULL;
		return $param->delParambitfield($subPath[1]);
	}
	
	/** Add a property to the block 
	 *  @param Property $property property to add to the block **/
	function addProperty($property)
	{
		$property->parentBlock = $this;
		array_push($this->properties, $property);
	}
	
	/** return a reference to the property with the name $name, if not found, return null
	 *  @param string $name name of the property to search
	 *  @return Property found property **/
	function getProperty($name)
	{
		foreach($this->properties as $property)
		{
			if($property->name==$name) return $property;
		}
		return null;
	}
	
	/** return a reference to the property with the access path $path, if not found, return null
	 *  @param string $path path of the property to search, separed by . (dot)
	 *  @return Property found property **/
	function getPropertyPath($path)
	{
		$subprops = explode('.', $path);
		if(count($subprops)==0) return null;
		
		$property = $this->getProperty($subprops[0]);
		if($property==null) return null;
		
		for($i=1; $i<count($subprops); $i++)
		{
			$property = $property->getSubProperty($subprops[$i]);
			if($property==null) return null;
		}
		return $property;
	}
	
	/** delete a property from his name
	 *  @param string $name name of the property to delete  **/
	function delProperty($name)
	{
		$i=0;
		foreach($this->properties as $property)
		{
			if($property->name==$name) {unset($this->properties[$i]); return;}
			$i++;
		}
		return null;
	}
	
	/** Add a file to the block 
	 *  @param File $file file to add to the block **/
	function addFile($file)
	{
		$file->parentBlock = $this;
		array_push($this->files, $file);
	}
	
	/** return a reference to the file with the name $name, if not found, return null
	 *  @param string $name name of the file to search
	 *  @return File found file **/
	function getFile($name)
	{
		foreach($this->files as $file)
		{
			if($file->name==$name) return $file;
		}
		return null;
	}
	
	/** return a reference to the file with the path $path, if not found, return null
	 *  @param string $path path of the file to search
	 *  @return File found file **/
	function getFileByPath($path)
	{
		foreach($this->files as $file)
		{
			if($file->path==$path) return $file;
		}
		return null;
	}
	
	/** delete a file from his path
	 *  @param string $path path of the file to delete  **/
	function delFileByPath($path)
	{
		$i=0;
		foreach($this->files as $file)
		{
			if($file->path==$path) {unset($this->files[$i]); return;}
			$i++;
		}
		return null;
	}
	
	/** Add a flow to the block 
	 *  @param Flow $flow flow to add to the block **/
	function addFlow($flow)
	{
		$flow->parentBlock = $this;
		array_push($this->flows, $flow);
	}
	
	/** return a reference to the flow with the name $name, if not found, return null
	 *  @param string $name name of the flow to search
	 *  @return Flow found flow **/
	function getFlow($name)
	{
		foreach($this->flows as $flow)
		{
			if($flow->name==$name) return $flow;
		}
		return null;
	}
	
	/** delete a flow from his name
	 *  @param string $name name of the flow to delete  **/
	function delFlow($name)
	{
		$i=0;
		foreach($this->flows as $flow)
		{
			if($flow->name==$name) {unset($this->flows[$i]); return;}
			$i++;
		}
		return null;
	}
	
	/** Add a clock to the block 
	 *  @param Clock $clock clock to add to the block **/
	function addClock($clock)
	{
		$clock->parentBlock = $this;
		array_push($this->clocks, $clock);
	}
	
	/** return a reference to the clock with the name $name, if not found, return null
	 *  @param string $name name of the clock to search
	 *  @return Clock found clock **/
	function getClock($name)
	{
		foreach($this->clocks as $clock)
		{
			if($clock->name==$name) return $clock;
		}
		return null;
	}
	
	/** delete a clock from his name
	 *  @param string $name name of the clock to delete  **/
	function delClock($name)
	{
		$i=0;
		foreach($this->clocks as $clock)
		{
			if($clock->name==$name) {unset($this->clocks[$i]); return;}
			$i++;
		}
		return null;
	}
	
	/** Add a reset to the block 
	 *  @param Reset $reset reset to add to the block **/
	function addReset($reset)
	{
		$reset->parentBlock = $this;
		array_push($this->resets, $reset);
	}
	
	/** return a reference to the reset with the name $name, if not found, return null
	 *  @param string $name name of the reset to search
	 *  @return Reset found reset **/
	function getReset($name)
	{
		foreach($this->resets as $reset)
		{
			if($reset->name==$name) return $reset;
		}
		return null;
	}
	
	/** delete a reset from his name
	 *  @param string $name name of the reset to delete  **/
	function delReset($name)
	{
		$i=0;
		foreach($this->resets as $reset)
		{
			if($reset->name==$name) {unset($this->resets[$i]); return;}
			$i++;
		}
		return null;
	}
	
	/** Add a pin to the block 
	 *  @param Pin $pin pin to add to the block **/
	function addPin($pin)
	{
		$pin->parentBlock = $this;
		array_push($this->pins, $pin);
	}
	
	/** return a reference to the pin with the name $name, if not found, return null
	 *  @param string $name name of the pin to search
	 *  @return Pin found pin **/
	function getPin($name)
	{
		foreach($this->pins as $pin)
		{
			if($pin->name==$name) return $pin;
		}
		return null;
	}
	
	/** Add an external port to the block 
	 *  @param Port $extPort port to add to the block **/
	function addExtPort($extPort)
	{
		$extPort->parentBlock = $this;
		array_push($this->ext_ports, $extPort);
	}
	
	/** return a reference to the external port with the name $name, if not found, return null
	 *  @param string $name name of the external port to search
	 *  @return Port found external port **/
	function getExtPort($name)
	{
		foreach($this->ext_ports as $extPort)
		{
			if($extPort->name==$name) return $extPort;
		}
		return null;
	}
	
	/** Add an interface to the block 
	 *  @param Interface $interface interface to add to the block **/
	function addInterface($interface)
	{
		$interface->parentBlock = $this;
		array_push($this->interfaces, $interface);
	}
	
	/** return a reference to the interface with the name $name, if not found, return null
	 *  @param string $name name of the interface to search
	 *  @return Interface found interface **/
	function getInterface($name)
	{
		foreach($this->interfaces as $interface)
		{
			if($interface->name==$name) return $interface;
		}
		return null;
	}
	
	/** Add a attribute to the toolchain 
	 *  @param Attribute $attribute attribute to add to the block **/
	function addAttribute($attribute)
	{
		array_push($this->attributes, $attribute);
	}
	
	/** return a reference to the attribute with the name $name, if not found, return false
	 *  @param string $name name of the attribute enum to search
	 *  @return Attribute found attribute **/
	function getAttribute($name)
	{
		foreach($this->attributes as $attribute)
		{
			if($attribute->name==$name) return $attribute;
		}
		return null;
	}
	
	protected function parse_xml()
	{
		if(isset($this->xml['size_addr_rel'])) warning("Please update your process or io with the new conventionnal name 'pi_size_addr_rel' instead of 'size_addr_rel'",12,"Block");
		$this->pi_size_addr_rel = (int)$this->xml['pi_size_addr_rel'];
		$this->categ = (string)$this->xml['categ'];
		$this->configscriptfile = (string)$this->xml['configscriptfile'];
		$this->generatescriptfile = (string)$this->xml['generatescriptfile'];
		$this->desc = (string)$this->xml['desc'];
		
		// files
		if(isset($this->xml->files))
		{
			foreach($this->xml->files->file as $fileXml)
			{
				$this->addFile(new File($fileXml));
			}
		}
		
		// properties
		if(isset($this->xml->properties))
		{
			foreach($this->xml->properties->property as $propertyXml)
			{
				$this->addProperty(new Property($propertyXml));
			}
		}
		
		// params
		if(isset($this->xml->params))
		{
			foreach($this->xml->params->param as $paramXml)
			{
				$this->addParam(new Param($paramXml));
			}
		}
		
		// flows
		if(isset($this->xml->flows))
		{
			foreach($this->xml->flows->flow as $flowXml)
			{
				$this->addFlow(new Flow($flowXml));
			}
		}
		
		// clocks
		if(isset($this->xml->clocks))
		{
			foreach($this->xml->clocks->clock as $clockXml)
			{
				$this->addClock(new Clock($clockXml));
			}
		}
		
		// resets
		if(isset($this->xml->resets))
		{
			foreach($this->xml->resets->reset as $resetXml)
			{
				$this->addReset(new Reset($resetXml));
			}
		}
		
		// attributes
		if(isset($this->xml->attributes))
		{
			foreach($this->xml->attributes->attribute as $attribute)
			{
				$this->addAttribute(new Attribute($attribute));
			}
		}
	}
	
	public function type() {return 'block';}
	
	public function getXmlElement($xml, $format)
	{
		if($format=="project" or $format=="blockdef")
		{
			if($this->type()=="io" or $this->type()=="iocom") $typeName='io';
			elseif($this->type()=="process") $typeName="process";
			else $typeName="";
			
			if($typeName!="") $xml_element = $xml->createElement($typeName);
		}
		else
		{
			$xml_element = $xml->createElement("block");
		}
		
		// name
		$att = $xml->createAttribute('name');
		$att->value = $this->name;
		$xml_element->appendChild($att);
		
		// type
		if($format=="complete" or $format=="blockdef")
		{
			$att = $xml->createAttribute('categ');
			$att->value = $this->categ;
			$xml_element->appendChild($att);
		}
		
		// in_lib
		if($this->type()=="process" and $format!="blockdef")
		{
			$att = $xml->createAttribute('inlib');
			$att->value = $this->in_lib==1 ? "true" : "false";
			$xml_element->appendChild($att);
		}
		
		if($format!="blockdef")
		{
			// driver
			$att = $xml->createAttribute('driver');
			$att->value = $this->driver;
			$xml_element->appendChild($att);
		}
		
		// pi information & type
		if($format=="complete")
		{
			// type
			$att = $xml->createAttribute('type');
			$att->value = $this->type();
			$xml_element->appendChild($att);
			
			// addr_abs
			$att = $xml->createAttribute('addr_abs');
			$att->value = $this->addr_abs;
			$xml_element->appendChild($att);
			
			// master_count
			$att = $xml->createAttribute('master_count');
			$att->value = $this->master_count;
			$xml_element->appendChild($att);
		}
		
		if($format=="complete" or $format=="blockdef")
		{
			// pi_size_addr_rel
			$att = $xml->createAttribute('pi_size_addr_rel');
			$att->value = $this->pi_size_addr_rel;
			$xml_element->appendChild($att);
			
			// desc
			$att = $xml->createAttribute('desc');
			$att->value = $this->desc;
			$xml_element->appendChild($att);
		}
		
		// x_pos
		if(isset($this->x_pos) and $this->x_pos!=-1)
		{
			$att = $xml->createAttribute('x_pos');
			$att->value = $this->x_pos;
			$xml_element->appendChild($att);
		}
		
		// y_pos
		if(isset($this->y_pos) and $this->y_pos!=-1)
		{
			$att = $xml->createAttribute('y_pos');
			$att->value = $this->y_pos;
			$xml_element->appendChild($att);
		}
		
		if($format=="complete" or $format=="blockdef")
		{
			// files
			if(!empty($this->files))
			{
				$xml_files = $xml->createElement("files");
				foreach($this->files as $file)
				{
					$xml_files->appendChild($file->getXmlElement($xml, $format));
				}
				$xml_element->appendChild($xml_files);
			}
			
			// resets
			if(!empty($this->resets))
			{
				$xml_resets = $xml->createElement("resets");
				foreach($this->resets as $reset)
				{
					$xml_resets->appendChild($reset->getXmlElement($xml, $format));
				}
				$xml_element->appendChild($xml_resets);
			}
		
			// attributes
			if(!empty($this->attributes))
			{
				$xml_attributes = $xml->createElement("attributes");
				foreach($this->attributes as $attribute)
				{
					$xml_attributes->appendChild($attribute->getXmlElement($xml, $format));
				}
				$xml_element->appendChild($xml_attributes);
			}
		}
		
		
		// flows
		if(!empty($this->flows))
		{
			$xml_flows = $xml->createElement("flows");
			foreach($this->flows as $flow)
			{
				if($flow->type=="in" or $flow->type=="out")
				{
					$xml_flows->appendChild($flow->getXmlElement($xml, $format));
				}
			}
			$xml_element->appendChild($xml_flows);
		}
		
		// params
		$count=0;
		$xml_params = $xml->createElement("params");
		foreach($this->params as $param)
		{
			if($format=="complete" or $format=="blockdef")
			{
				$xml_params->appendChild($param->getXmlElement($xml, $format));
				$count++;
			}
			else
			{
				if($param->hard==1 and $param->name!="MASTER_ADDR_WIDTH")
				{
					$xml_params->appendChild($param->getXmlElement($xml, $format));
					$count++;
				}
			}
		}
		if($count>0) $xml_element->appendChild($xml_params);
		
		// properties
		$count=0;
		$xml_property = $xml->createElement("properties");
		foreach($this->properties as $property)
		{
			$xml_property->appendChild($property->getXmlElement($xml, $format));
			$count++;
		}
		if($count>0) $xml_element->appendChild($xml_property);
		
		// clocks
		$count=0;
		$xml_clocks = $xml->createElement("clocks");
		foreach($this->clocks as $clock)
		{
			$export=true;
			
			if($format=="project")
			{
				if($clock->direction=="out" or $clock->domain!="") $export=false;
			}
			elseif($format=="blockdef")
			{
				if($clock->name=="clk_proc") $export=false;
			}
			
			if($export)
			{
				$xml_clocks->appendChild($clock->getXmlElement($xml, $format));
				$count++;
			}
		}
		if($count>0) $xml_element->appendChild($xml_clocks);
		
		return $xml_element;
	}
	
	function saveBlockDef($file)
	{
		$xml = new DOMDocument("1.0", "UTF-8");
		$xml->preserveWhiteSpace = false;
		$xml->formatOutput = true;
		
		$xml->appendChild($this->getXmlElement($xml, "blockdef"));
		
		$xml->save($file);
	}
}

?>
