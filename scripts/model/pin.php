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
 * @brief Pin is the physical mapping between external port of an IO block and 
 * chip pins.
 * @see IO Board Port
 * @ingroup base
 */
class Pin
{
    /**
     * @brief Pin name to map
     * @var string $name
     */
    public $name;

    /**
     * @brief Name of the external pin to map
     * @var string $mapto
     */
    public $mapto;

    /**
     * @brief Array of attributes of the pin (optional)
     * @var array|Attribute $attributes
     */
    public $attributes;

    /**
     * @brief Reference to the associated parent block
     * @var Block $parentBlock
     */
    public $parentBlock;

    /**
     * @brief constructor of Pin
     * 
     * Initialise all the internal members and call parse_xml if $xml is set
     * @param SimpleXMLElement|null $xml if it's different of null, call the
     * xml parser to fill members
     */
    function __construct($xml = null)
    {
        $this->parentBlock = null;
        $this->attributes = array();

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
        $this->mapto = (string) $xml['mapto'];

        // attributes
        if (isset($xml->attributes))
        {
            foreach ($xml->attributes->attribute as $attribute)
            {
                array_push($this->attributes, new Attribute($attribute));
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
        $xml_element = $xml->createElement("pin");

        // name
        $att = $xml->createAttribute('name');
        $att->value = $this->name;
        $xml_element->appendChild($att);

        // mapto
        $att = $xml->createAttribute('mapto');
        $att->value = $this->mapto;
        $xml_element->appendChild($att);

        // attributes
        if (!empty($this->attributes))
        {
            $xml_attributes = $xml->createElement("attributes");
            foreach ($this->attributes as $attribute)
            {
                $xml_attributes->appendChild($attribute->getXmlElement($xml, $format));
            }
            $xml_element->appendChild($xml_attributes);
        }

        return $xml_element;
    }
}
