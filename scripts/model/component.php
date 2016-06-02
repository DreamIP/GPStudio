<?php

/* 
 * Copyright (C) 2016 Dream IP
 * 
 * This file is part of GPStudio.
 *
 * GPStudio is a free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** 
 * Component is the the definition of hardware components. It could be
 * used in a block to indicate the inclusion of the block or in the
 * component library to define them.
 * @brief Component is the the definition of hardware components.
 * @see Block
 * @ingroup base
 */

require_once("file.php");
require_once("param.php");
require_once("flow.php");
require_once("clock.php");
require_once("reset.php");
require_once("port.php");

class Component
{
	/**
	* Name of the component
	* @var string $name
	*/
	public $name;

	/**
	* Path where the root of files and define of the component is putted
	* @var string $path
	*/
	public $path;

	/**
	* Specify the categorie of the component eg : communication, imagesensor, descriptor...
	* @var string $categ
	*/
	public $categ;

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
	* Array of files whith define the implementation of the component
	* @var array|File $files
	*/
	public $files;
	
	/**
    * Array of flows in the component can be input flow or output
    * @var array|Flow $flows
    */
	public $flows;

	/**
	* Array of clocks to drive the component
	* @var array|Clock $clocks
	*/
	public $clocks;

	/**
	* Array of resets, can be different type of resets
	* @var array|Reset $resets
	*/
	public $resets;
	
	protected $xml;
	
	function __construct()
	{
		$this->params = array();
		$this->files = array();
		$this->flows = array();
		$this->clocks = array();
		$this->resets = array();
	}
	
	/** Add a parameter to the component 
	 *  @param Param $param parameter to add to the component **/
	function addParam($param)
	{
		array_push($this->params, $param);
	}
	
	/** return a reference to the parameter with the name $name, if not found, return null
	 *  @param string $name name of the parameter to search
	 *  @param bool $casesens take care or not of the case of the name
	 *  @return Param found parameter **/
	function getParam($name, $casesens=true)
	{
		if($casesens)
		{
			foreach($this->params as $param)
			{
				if($param->name==$name) return $param;
			}
		}
		else
		{
			foreach($this->params as $param)
			{
				if(strcasecmp($param->name,$name)==0) return $param;
			}
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
	
	/** Add a file to the component 
	 *  @param File $file file to add to the component **/
	function addFile($file)
	{
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
	
	/** Add a flow to the component 
	 *  @param Flow $flow flow to add to the component **/
	function addFlow($flow)
	{
		array_push($this->flows, $flow);
	}
	
	/** return a reference to the flow with the name $name, if not found, return null
	 *  @param string $name name of the flow to search
	 *  @param bool $casesens take care or not of the case of the name
	 *  @return Flow found flow **/
	function getFlow($name, $casesens=true)
	{
		if($casesens)
		{
			foreach($this->flows as $flow)
			{
				if($flow->name==$name) return $flow;
			}
		}
		else
		{
			foreach($this->flows as $flow)
			{
				if(strcasecmp($flow->name,$name)==0) return $flow;
			}
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
	
	/** Add a clock to the component 
	 *  @param Clock $clock clock to add to the component **/
	function addClock($clock)
	{
		array_push($this->clocks, $clock);
	}
	
	/** return a reference to the clock with the name $name, if not found, return null
	 *  @param string $name name of the clock to search
	 *  @param bool $casesens take care or not of the case of the name
	 *  @return Clock found clock **/
	function getClock($name, $casesens=true)
	{
		if($casesens)
		{
			foreach($this->clocks as $clock)
			{
				if($clock->name==$name) return $clock;
			}
		}
		else
		{
			foreach($this->clocks as $clock)
			{
				if(strcasecmp($clock->name,$name)==0) return $clock;
			}
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
	
	/** Add a reset to the component 
	 *  @param Reset $reset reset to add to the component **/
	function addReset($reset)
	{
		array_push($this->resets, $reset);
	}
	
	/** return a reference to the reset with the name $name, if not found, return null
	 *  @param string $name name of the reset to search
	 *  @param bool $casesens take care or not of the case of the name
	 *  @return Reset found reset **/
	function getReset($name, $casesens=true)
	{
		if($casesens)
		{
			foreach($this->resets as $reset)
			{
				if($reset->name==$name) return $reset;
			}
		}
		else
		{
			foreach($this->resets as $reset)
			{
				if(strcasecmp($reset->name,$name)==0) return $reset;
			}
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
	
	protected function parse_xml()
	{
		$this->name = (string)$this->xml['name'];
		$this->categ = (string)$this->xml['categ'];
		$this->desc = (string)$this->xml['desc'];
		
		// files
		if(isset($this->xml->files))
		{
			foreach($this->xml->files->file as $fileXml)
			{
				$this->addFile(new File($fileXml));
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
	
	public function getXmlElement($xml, $format)
	{
		$xml_element = $xml->createElement("component");
		
		// name
		$att = $xml->createAttribute('name');
		$att->value = $this->name;
		$xml_element->appendChild($att);
		
		// desc
		$att = $xml->createAttribute('desc');
		$att->value = $this->desc;
		$xml_element->appendChild($att);
		
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
		
		// flows
		if(!empty($this->flows))
		{
			$xml_flows = $xml->createElement("flows");
			foreach($this->flows as $flow)
			{
				if($flow->type=="in" or $flow->type=="out")
				{
					$xml_flows->appendChild($flow->getXmlElement($xml, $format));
					$count++;
				}
			}
			$xml_element->appendChild($xml_flows);
		}
		
		// params
		if(!empty($this->params))
		{
			$xml_params = $xml->createElement("params");
			foreach($this->params as $param)
			{
				$xml_params->appendChild($param->getXmlElement($xml, $format));
				$count++;
			}
			$xml_element->appendChild($xml_params);
		}
		
		// clocks
		if(!empty($this->clocks))
		{
			$xml_clocks = $xml->createElement("clocks");
			foreach($this->clocks as $clock)
			{
				$xml_clocks->appendChild($clock->getXmlElement($xml, $format));
			}
			$xml_element->appendChild($xml_clocks);
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
		
		return $xml_element;
	}
}

?>
