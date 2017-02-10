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

require_once("comconnect.php");
require_once("comparam.php");

/**
 * Contains all parameters about the software driver to use and communication
 * links and protocol declaration.
 * 
 * @brief ComDriver is the specialised implementation of IO.
 * @see IOCom ComConnect ComParam
 * @ingroup base
 */
class ComDriver
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
     * @brief Array of ComParam to give all specific parameters
     * @var array|ComParam $comParams
     * @see ComParam
     */
    public $comParams;

    /**
     * @brief constructor of ComDriver
     * 
     * Initialise all the internal members and call parse_xml if $xml is set
     * @param SimpleXMLElement|null $xml if it's different of null, call the
     * xml parser to fill members
     */
    function __construct($xml = null)
    {
        $this->comConnects = array();
        $this->comParams = array();

        if ($xml)
            $this->parse_xml($xml);
    }

    /**
     * @brief internal function to fill this instance from input xml structure
     * @param SimpleXMLElement $io_device_element element from io in lib
     * @param SimpleXMLElement $io_node_element element from the node
     */
    protected function parse_xml($xml)
    {
        $this->driverio = (string) $xml['driverio'];

        // com_connects
        if (isset($xml->com_connects))
        {
            foreach ($xml->com_connects->com_connect as $com_connectXml)
            {
                $this->addComConnect(new ComConnect($com_connectXml));
            }
        }

        // com_params
        if (isset($xml->com_params))
        {
            foreach ($xml->com_params->com_param as $comParamXml)
            {
                $this->addComParam(new ComParam($comParamXml));
            }
        }
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
        $xml_element = $xml->createElement("com_driver");

        //driverio
        $att = $xml->createAttribute('driverio');
        $att->value = $this->driverio;
        $xml_element->appendChild($att);

        if ($format == "complete" or $format == "blockdef")
        {
            // com_connects
            $xml_com_connects = $xml->createElement("com_connects");
            foreach ($this->comConnects as $comConnect)
            {
                $xml_com_connects->appendChild($comConnect->getXmlElement($xml, $format));
            }
            $xml_element->appendChild($xml_com_connects);

            // com_params
            $xml_com_params = $xml->createElement("com_params");
            foreach ($this->comParams as $comParam)
            {
                $xml_com_params->appendChild($comParam->getXmlElement($xml, $format));
            }
            $xml_element->appendChild($xml_com_params);
        }

        return $xml_element;
    }

    /**
     * @brief Add a comConnect to the comConnects
     * @param ComConnect $comConnect comConnect to add to the comConnect
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

    /**
     * @brief Add a comParam to the comParams
     * @param ComParam $comParam comParam to add to the comParam
     */
    function addComParam($comParam)
    {
        array_push($this->comParams, $comParam);
    }

    /**
     * @brief return a reference to the comConnect with the link $link, if not found,
     * return null
     * @param string $name link of the comConnect to search
     * @return ComConnect found comConnect
     */
    function getComParam($name)
    {
        foreach ($this->comParams as $comParam)
        {
            if ($comParam->name == $name)
                return $comParam;
        }
        return null;
    }
}
