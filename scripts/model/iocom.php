<?php
/*
 * Copyright (C) 2014-2017 Dream IP
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

require_once("io.php");
require_once("comdriver.php");

/**
 * It allows the use of communication links and protocol declaration.
 * 
 * @brief IOCom is the specialised implementation of IO.
 * @see IO ComConnect
 * @ingroup base
 */
class IOCom extends IO
{
    /**
     * @brief comdriver contains all information about the driver to this iocom
     * @var ComDriver $driverio
     */
    public $comdriver;

    /**
     * @brief constructor of IO
     * 
     * Initialise all the internal members and call parse_xml if
     * $io_device_element is an SimpleXMLElement object. Else, it open the file
     * with the path $io_device_element as string or the io with the name
     * $io_device_element in library
     * @param SimpleXMLElement|string|null $io_device_element if it's different
     * of null, call the xml parser to fill members
     * @param SimpleXMLElement|null $io_node_element if it's different of null,
     * call the xml parser to fill members
     */
    function __construct($io_device_element, $io_node_element = null)
    {
        parent::__construct($io_device_element, $io_node_element);
    }

    /**
     * @brief internal function to fill this instance from input xml structure
     * @param SimpleXMLElement $io_device_element element from io in lib
     * @param SimpleXMLElement $io_node_element element from the node
     */
    protected function parse_xml($io_device_element=NULL, $io_node_element=NULL)
    {
        parent::parse_xml($io_device_element, $io_node_element);

        if (isset($this->xml->com_driver))
        {
            $this->comdriver = new ComDriver($this->xml->com_driver);
        }
    }

    /**
     * @brief Returns the type of the block as string, redefined by children.
     * @return string type of the block.
     */
    public function type()
    {
        return 'iocom';
    }

    /**
     * @brief permits to output this instance
     * 
     * Return a formated node for the node_generated file. This method call all
     * the children getXmlElement to add into this node.
     * @param DOMDocument $xml reference of the output xml document
     * @param string $format desired output file format
     * @return DOMElement xml element corresponding to this current instance
     */
    public function getXmlElement($xml, $format)
    {
        $xml_element = parent::getXmlElement($xml, $format);

        if ($format == "complete" or $format == "blockdef")
        {
            $xml_element->appendChild($this->comdriver->getXmlElement($xml, $format));
        }

        return $xml_element;
    }
}
