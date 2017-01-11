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

/**
 * @brief Port is external port definition for IO block.
 * @see IO Board Pin
 * @ingroup base
 */
class Port
{
    /**
     * @brief Name of the interface
     * @var string $name
     */
    public $name;

    /**
     * @brief Type of the port
     * @var string $type
     */
    public $type;

    /**
     * @brief Size of the port in bit
     * @var int $size
     */
    public $size;

    /**
     * @brief Description of the port (optional)
     * @var string $desc
     */
    public $desc;

    /**
     * @brief Reference to the associated parent block
     * @var Block $parentBlock
     */
    public $parentBlock;

    /**
     * @brief constructor of Port
     * 
     * Initialise all the internal members and call parse_xml if $xml is set
     * @param SimpleXMLElement|null $xml if it's different of null, call the
     * xml parser to fill members
     */
    function __construct($xml = null)
    {
        $this->parentBlock = null;
        if ($xml)
            $this->parse_xml($xml);
    }

    /**
     * @brief funtion that export as string the main content of the class instance
     * @return string
     */
    public function __toString()
    {
        return "port " . $this->name . " type: " . $this->type . " size: " . $this->size;
    }

    /**
     * @brief internal function to fill this instance from input xml structure
     * 
     * Can be call only from this node into the constructor
     * @param SimpleXMLElement $xml xml element to parse
     */
    protected function parse_xml($xml)
    {
        $this->name = (string) $xml['name'];
        $this->type = (string) $xml['type'];
        $this->size = (string) $xml['size'];
        $this->desc = (string) $xml['desc'];
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
        $xml_element = $xml->createElement("port");

        // name
        $att = $xml->createAttribute('name');
        $att->value = $this->name;
        $xml_element->appendChild($att);

        // type
        $att = $xml->createAttribute('type');
        $att->value = $this->type;
        $xml_element->appendChild($att);

        // size
        $att = $xml->createAttribute('size');
        $att->value = $this->size;
        $xml_element->appendChild($att);

        // desc
        if (!empty($this->desc))
        {
            $att = $xml->createAttribute('desc');
            $att->value = $this->desc;
            $xml_element->appendChild($att);
        }

        return $xml_element;
    }
}
