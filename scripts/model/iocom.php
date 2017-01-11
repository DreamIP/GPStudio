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
require_once("comconnect.php");

/**
 * It allows the use of communication links and protocol declaration.
 * @brief IOCom is the specialised implementation of IO.
 * @see IO ComConnect
 * @ingroup base
 */
class IOCom extends IO
{
    /**
     * @brief Name of the driver to use for etablish a communication with the board
     * @var string $driverio
     */
    public $driverio;

    /**
     * @brief Array of ComConnect to give the equivalence table between hardware flow and software id flow
     * @var array|ComConnect $comConnects
     * @see ComConnect
     */
    public $comConnects;

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
        $this->comConnects = array();

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

        if (isset($this->xml->com_connects))
        {
            $this->driverio = (string) $this->xml->com_connects['driverio'];

            // com_connects
            if (isset($this->xml->com_connects))
            {
                foreach ($this->xml->com_connects->com_connect as $com_connectXml)
                {
                    $this->addComConnect(new ComConnect($com_connectXml));
                }
            }
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
            // com_connects
            $xml_com_connects = $xml->createElement("com_connects");

            //driverio
            $att = $xml->createAttribute('driverio');
            $att->value = $this->driverio;
            $xml_com_connects->appendChild($att);

            foreach ($this->comConnects as $comConnect)
            {
                $xml_com_connects->appendChild($comConnect->getXmlElement($xml, $format));
            }
            $xml_element->appendChild($xml_com_connects);
        }

        return $xml_element;
    }

    /**
     * @brief Add a comConnect to the comParam 
     * @param ComConnect $comConnect comConnect to add to the comParam
     */
    function addComConnect($comConnect)
    {
        array_push($this->comConnects, $comConnect);
    }

    /**
     * @brief return a reference to the comConnect with the link $link, if not found,
     * return null
     * @param string $link link of the comConnect to search
     * @return ComConnect found comConnect
     */
    function getComConnect($link)
    {
        foreach ($this->comConnects as $comConnect)
        {
            if ($comConnect->link == $link)
                return $comConnect;
        }
        return null;
    }
}
