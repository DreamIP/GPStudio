<?php

/** 
 * Flow are stored in Block::flows as a list.
 * @brief Define flow interface in a Block.
 * @see Block FI
 * @ingroup base
 */

class Flow
{
	/**
	* Name of the flow
	* @var string $name
	*/
	public $name;

	/**
	* Type of the flow, can be "in" or "out"
	* @var string $type
	*/
	public $type;

	/**
	* Size in bit of the flow
	* @var int $size
	*/
	public $size;

	/**
	* Description of the flow (optional)
	* @var string $desc
	*/
	public $desc;

	/**
	* Reference to the associated parent block
	* @var Block $parentBlock
	*/
	public $parentBlock;
	
	
	/**
	* Array of property class specify the high level properties
	* @var array|Property $properties
	*/
	public $properties;
	
	function __construct($xml=null)
	{
		$this->properties = array();
		
		if($xml) $this->parse_xml($xml);
	}
	
	public function __toString()
    {
        return "'".$this->name."' direction: '".$this->type."' size: '".$this->size."' desc: '".$this->desc."'";
    }
	
	protected function parse_xml($xml)
	{
		$this->parentBlock = null;
		$this->name = (string)$xml['name'];
		$this->type = (string)$xml['type'];
		$this->desc = (string)$xml['desc'];
		if(!empty($xml['size'])) $this->size = (int)$xml['size']; else $this->size = 16; // TODO change this hard coded default value
		
		// properties
		if(isset($xml->properties))
		{
			foreach($xml->properties->property as $propertyXml)
			{
				$this->addProperty(new Property($propertyXml));
			}
		}
	}
	
	public function getXmlElement($xml, $format)
	{
		$xml_element = $xml->createElement("flow");
		
		// name
		$att = $xml->createAttribute('name');
		$att->value = $this->name;
		$xml_element->appendChild($att);
		
		// size
		$att = $xml->createAttribute('size');
		$att->value = $this->size;
		$xml_element->appendChild($att);
		
		if($format=="complete" or $format=="blockdef")
		{
			// type
			$att = $xml->createAttribute('type');
			$att->value = $this->type;
			$xml_element->appendChild($att);
			
			// desc
			$att = $xml->createAttribute('desc');
			$att->value = $this->desc;
			$xml_element->appendChild($att);
			
			// properties
			if(!empty($this->properties))
			{
				$xml_property = $xml->createElement("properties");
				foreach($this->properties as $property)
				{
					$xml_property->appendChild($property->getXmlElement($xml, $format));
				}
				$xml_element->appendChild($xml_property);
			}
		}
		
		return $xml_element;
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
	 *  @param bool $casesens take care or not of the case of the name
	 *  @return Property found property **/
	function getProperty($name, $casesens=true)
	{
		if($casesens)
		{
			foreach($this->properties as $property)
			{
				if($property->name==$name) return $property;
			}
		}
		else
		{
			foreach($this->properties as $property)
			{
				if(strcasecmp($property->name,$name)==0) return $property;
			}
		}
		return null;
	}
	
	/** alias to getProperty($name, $casesens)
	 *  @param string $name name of the property enum to search
	 *  @param bool $casesens take care or not of the case of the name
	 *  @return Property found property **/
	function getSubProperty($name, $casesens=true)
	{
		return $this->getProperty($name, $casesens);
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
	}
}

?>
