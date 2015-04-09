<?php

require_once("file.php");
require_once("param.php");
require_once("property.php");
require_once("flow.php");
require_once("clock.php");
require_once("reset.php");
require_once("pin.php");
require_once("port.php");
require_once("interface.php");

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
	* The absolute adress of the block on BI
	* @var int $addr_abs
	*/
	public $addr_abs;

	/**
	* Size of relative adress bus
	* @var int $size_addr_rel
	*/
	public $size_addr_rel;

	/**
	* Number of master in the block on BI
	* @var int $master_count
	*/
	public $master_count;

	/**
	* Specify the external file script to configure the block (optional)
	* @var string $scriptfile
	*/
	public $configscriptfile;

	/**
	* Specify the external file script to generate the block (optional)
	* @var string $generatescriptfile
	*/
	public $generatescriptfile;


	/**
	* Array of parameters class (can be Generic or dynamics parameter on BI)
	* @var array|Param $params
	*/
	public $params;
	
	/**
	* Array of property class specify the hight level properties
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
	* External pins mapping for blocks abble to comunicate with the output
	* @var array|Pin $pins
	*/
	public $pins;

	/**
	* Array of port abble to comunicate with the output
	* @var array|Port $ext_ports
	*/
	public $ext_ports;

	/**
	* Array of interfaces of the block
	* @var array|InterfaceBus $interfaces
	*/
	public $interfaces;

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
		$this->pins = array();
		$this->ext_ports = array();
		$this->interfaces = array();
		$this->addr_abs = -1;
		$this->master_count = 0;
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
	
	function configure($node, $block)
	{
		if(!empty($this->configscriptfile))
		{
			if(file_exists($this->path.$this->configscriptfile))
			{
				$script = str_replace(LIB_PATH,'',$this->path.$this->configscriptfile);
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
				$script = str_replace(LIB_PATH,'',$this->path.$this->generatescriptfile);
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
	
	/** return a reference to the parameter with the name $name, if not found, return false
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
	
	/** Add a property to the block 
	 *  @param Property $property property to add to the block **/
	function addProperty($property)
	{
		$property->parentBlock = $this;
		array_push($this->properties, $property);
	}
	
	/** return a reference to the property with the name $name, if not found, return false
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
	
	/** Add a file to the block 
	 *  @param File $file file to add to the block **/
	function addFile($file)
	{
		$file->parentBlock = $this;
		array_push($this->files, $file);
	}
	
	/** return a reference to the file with the name $name, if not found, return false
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
	
	/** Add a flow to the block 
	 *  @param Flow $flow flow to add to the block **/
	function addFlow($flow)
	{
		$flow->parentBlock = $this;
		array_push($this->flows, $flow);
	}
	
	/** return a reference to the flow with the name $name, if not found, return false
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
	
	/** Add a clock to the block 
	 *  @param Clock $clock clock to add to the block **/
	function addClock($clock)
	{
		$clock->parentBlock = $this;
		array_push($this->clocks, $clock);
	}
	
	/** return a reference to the clock with the name $name, if not found, return false
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
	
	/** Add a reset to the block 
	 *  @param Reset $reset reset to add to the block **/
	function addReset($reset)
	{
		$reset->parentBlock = $this;
		array_push($this->resets, $reset);
	}
	
	/** return a reference to the reset with the name $name, if not found, return false
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
	
	/** Add a pin to the block 
	 *  @param Pin $pin pin to add to the block **/
	function addPin($pin)
	{
		$pin->parentBlock = $this;
		array_push($this->pins, $pin);
	}
	
	/** return a reference to the pin with the name $name, if not found, return false
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
	
	/** return a reference to the external port with the name $name, if not found, return false
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
	
	/** return a reference to the interface with the name $name, if not found, return false
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
	
	protected function parse_xml()
	{
		$this->size_addr_rel = (int)$this->xml['size_addr_rel'];
		$this->configscriptfile = $this->xml['configscriptfile'];
		$this->generatescriptfile = $this->xml['generatescriptfile'];
		
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
	}
	
	public function getXmlElement($xml)
	{
		$xml_element = $xml->createElement("block");
		
		// name
		$att = $xml->createAttribute('name');
		$att->value = $this->name;
		$xml_element->appendChild($att);
		
		// type
		$att = $xml->createAttribute('type');
		$att->value = $this->type();
		$xml_element->appendChild($att);
		
		// in_lib
		$att = $xml->createAttribute('in_lib');
		$att->value = $this->in_lib;
		$xml_element->appendChild($att);
		
		// addr_abs
		$att = $xml->createAttribute('addr_abs');
		$att->value = $this->addr_abs;
		$xml_element->appendChild($att);
		
		// size_addr_rel
		$att = $xml->createAttribute('size_addr_rel');
		$att->value = $this->size_addr_rel;
		$xml_element->appendChild($att);
		
		// master_count
		$att = $xml->createAttribute('master_count');
		$att->value = $this->master_count;
		$xml_element->appendChild($att);
		
		// files
		$xml_files = $xml->createElement("files");
		foreach($this->files as $file)
		{
			$xml_files->appendChild($file->getXmlElement($xml));
		}
		$xml_element->appendChild($xml_files);
		
		// params
		$xml_params = $xml->createElement("params");
		foreach($this->params as $param)
		{
			$xml_params->appendChild($param->getXmlElement($xml));
		}
		$xml_element->appendChild($xml_params);
		
		// properties
		$xml_property = $xml->createElement("properties");
		foreach($this->properties as $property)
		{
			$xml_property->appendChild($property->getXmlElement($xml));
		}
		$xml_element->appendChild($xml_property);
		
		// flows
		$xml_flows = $xml->createElement("flows");
		foreach($this->flows as $flow)
		{
			if($flow->type=="in" or $flow->type=="out")
			{
				$xml_flows->appendChild($flow->getXmlElement($xml));
			}
		}
		$xml_element->appendChild($xml_flows);
		
		// clocks
		$xml_clocks = $xml->createElement("clocks");
		foreach($this->clocks as $clock)
		{
			$xml_clocks->appendChild($clock->getXmlElement($xml));
		}
		$xml_element->appendChild($xml_clocks);
		
		// resets
		$xml_resets = $xml->createElement("resets");
		foreach($this->resets as $reset)
		{
			$xml_resets->appendChild($reset->getXmlElement($xml));
		}
		$xml_element->appendChild($xml_resets);
		
		return $xml_element;
	}
}

?>
