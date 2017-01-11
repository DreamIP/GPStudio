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
 * Attribute is contained into the Board class to define the specific attributes
 * of the board and in Pin class to define each features of pin dedicated to the
 * toolchain.
 * @brief The Attribute class is used to define special attributes for a toolchain.
 * @see Toolchain Pin Board
 * @ingroup base
 */
class Attribute
{
    /**
     * @brief Name of the attribute
     * @var string $name
     */
    public $name;

    /**
     * @brief Value of the attibute
     * @var string $value
     */
    public $value;

    /**
     * @brief Type of attribute
     * @var string $type
     */
    public $type;

    /**
     * @brief Constructor of the class
     * 
     * Build an empty Attribute if $xml is empty, fill it with $xml if set
     * @param SimpleXMLElement|null $xml XML element to parse if not null
     */
    function __construct($xml = null)
    {
        if ($xml)
            $this->parse_xml($xml);
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
        $this->value = (string) $xml['value'];
        $this->type = (string) $xml['type'];
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
        $xml_element = $xml->createElement("attribute");

        // name
        $att = $xml->createAttribute('name');
        $att->value = $this->name;
        $xml_element->appendChild($att);

        // value
        $att = $xml->createAttribute('value');
        $att->value = $this->value;
        $xml_element->appendChild($att);

        // type
        $att = $xml->createAttribute('type');
        $att->value = $this->type;
        $xml_element->appendChild($att);

        return $xml_element;
    }
}
