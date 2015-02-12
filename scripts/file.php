<?php

class File
{
	/**
	* Name of the file
	* @var string $name
	*/
	public $name;

	/**
	* File type can be : ['', 'verilog', 'vhdl', 'qip', 'sdc', 'hex']
	* @var string $type
	*/
	public $type;

	/**
	* Group can be : ['', 'hdl', ...]
	* @var string $group
	*/
	public $group;

	/**
	* Sub path of the file
	* @var string $path
	*/
	public $path;

	/**
	* Description of the file (optional)
	* @var string $desc
	*/
	public $desc;
	
	function __construct($xml=null)
	{
		if($xml) $this->parse_xml($xml);
	}
	
	protected function parse_xml($xml)
	{
		$this->name = (string)$xml['name'];
		$this->type = (string)$xml['type'];
		$this->group = (string)$xml['group'];
		$this->path = (string)$xml['path'];
		$this->desc = (string)$xml['desc'];
	}
	
	public function getXmlElement($xml)
	{
		$xml_element = $xml->createElement("file");
		
		// name
		$att = $xml->createAttribute('name');
		$att->value = $this->name;
		$xml_element->appendChild($att);
		
		// type
		$att = $xml->createAttribute('type');
		$att->value = $this->type;
		$xml_element->appendChild($att);
		
		// group
		$att = $xml->createAttribute('group');
		$att->value = $this->group;
		$xml_element->appendChild($att);
		
		// path
		$att = $xml->createAttribute('path');
		$att->value = $this->path;
		$xml_element->appendChild($att);
		
		// desc
		$att = $xml->createAttribute('desc');
		$att->value = $this->desc;
		$xml_element->appendChild($att);
		
		return $xml_element;
	}
}

?>
