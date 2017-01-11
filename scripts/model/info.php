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
 * Info is contained into Component class to give information for IPs like
 * author, email, version, company, licence...
 * @brief The Info class is used to indicate information for IPs.
 * @see Component
 * @ingroup base
 */
class Info
{
    /**
     * @brief Name of the info
     * @var string $name
     */
    public $name;

    /**
     * @brief Value of the info
     * @var string $value
     */
    public $value;

    /**
     * @brief Constructor of the class
     * 
     * Build an empty Info if $xml is empty, fill it with $xml if set
     * @param SimpleXMLElement|null $xml XML element to parse if not null
     */
    function __construct($xml = null)
    {
        if ($xml)
            $this->parse_xml($xml);
    }

    /**
     * @brief funtion that export as string the main content of the class instance
     * @return string
     */
    public function __toString()
    {
        return "info " . $this->name . " value: \"" . $this->value . "\"";
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
        $xml_element = $xml->createElement("info");

        // name
        $att = $xml->createAttribute('name');
        $att->value = $this->name;
        $xml_element->appendChild($att);

        // value
        $att = $xml->createAttribute('value');
        $att->value = $this->value;
        $xml_element->appendChild($att);

        return $xml_element;
    }
}
