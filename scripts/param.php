<?php

require_once("parambitfield.php");

class Param
{
	/**
	* Name of the param
	* @var string $name
	*/
	public $name;

	/**
	* 
	* @var string $type
	*/
	public $type;

	/**
	* Relative adress of the param
	* @var int $regaddr
	*/
	public $regaddr;

	/**
	* Current value of the param
	* @var string $value
	*/
	public $value;

	/**
	* Default value of the param
	* @var string $default
	*/
	public $default;

	/**
	* Minimum value of the param
	* @var string $min
	*/
	public $min;

	/**
	* Maximum value of the param
	* @var string $max
	*/
	public $max;

	/**
	* Generic parameter (hard = true) or dynamic one (hard = false)
	* @var bool $hard
	*/
	public $hard;

	/**
	* Description of the param (optional)
	* @var string $desc
	*/
	public $desc;

	
	/**
	* Array of bitfields if param contain different bitfield (optional)
	* @var array|ParamBitfield $parambitfields
	*/
	public $parambitfields;

	/**
	* Array of enums if param contain different enum (optional)
	* @var array|ParamEnum $paramenums
	*/
	public $paramenums;
	
	function __construct($xml=null)
	{
		$this->parambitfields	= array();
		$this->paramenums		= array();
		if($xml) $this->parse_xml($xml);
	}
	
	protected function parse_xml($xml)
	{
		$this->name		= (string)$xml['name'];
		$this->type		= (string)$xml['type'];
		$this->regaddr	= (string)$xml['regaddr'];
		$this->value	= (string)$xml['value'];
		$this->default	= (string)$xml['default'];
		$this->min		= (string)$xml['min'];
		$this->max		= (string)$xml['max'];
		$this->desc		= (string)$xml['desc'];
		
		if((string)$xml['hard']=="1" or (string)$xml['hard']=="true") $this->hard=true; else $this->hard=false;
		
		// parambitfields
		if(isset($xml->bitfields))
		{
			foreach($xml->bitfields->bitfield as $bitfield)
			{
				array_push($this->parambitfields, new ParamBitfield($bitfield));
			}
		}
		
		// enums
		if(isset($xml->enums))
		{
			foreach($xml->enums->enum as $enum)
			{
				array_push($this->paramenums, new ParamEnum($enum));
			}
		}
	}
	
	public function getXmlElement($xml)
	{
		$xml_element = $xml->createElement("param");
		
		// name
		$att = $xml->createAttribute('name');
		$att->value = $this->name;
		$xml_element->appendChild($att);
		
		// type
		$att = $xml->createAttribute('type');
		$att->value = $this->type;
		$xml_element->appendChild($att);
		
		// regaddr
		$att = $xml->createAttribute('regaddr');
		$att->value = $this->regaddr;
		$xml_element->appendChild($att);
		
		// value
		$att = $xml->createAttribute('value');
		$att->value = $this->value;
		$xml_element->appendChild($att);
		
		// default
		$att = $xml->createAttribute('default');
		$att->value = $this->default;
		$xml_element->appendChild($att);
		
		// min
		$att = $xml->createAttribute('min');
		$att->value = $this->min;
		$xml_element->appendChild($att);
		
		// max
		$att = $xml->createAttribute('max');
		$att->value = $this->max;
		$xml_element->appendChild($att);
		
		// hard
		$att = $xml->createAttribute('hard');
		$att->value = $this->hard;
		$xml_element->appendChild($att);
		
		// desc
		$att = $xml->createAttribute('desc');
		$att->value = $this->desc;
		$xml_element->appendChild($att);
		
		// parambitfields
		if(!empty($this->parambitfields))
		{
			$xml_parambitfields = $xml->createElement("parambitfields");
			foreach($this->parambitfields as $parambitfield)
			{
				$xml_parambitfields->appendChild($parambitfield->getXmlElement($xml));
			}
			$xml_element->appendChild($xml_parambitfields);
		}
		
		// paramenums
		if(!empty($this->paramenums))
		{
			$xml_paramenums = $xml->createElement("paramenums");
			foreach($this->paramenums as $paramenum)
			{
				$xml_paramenums->appendChild($paramenum->getXmlElement($xml));
			}
			$xml_element->appendChild($xml_paramenums);
		}
		
		return $xml_element;
	}
}

?>
