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
 * PropertyEnum is contained into the Property::$propertyenums when
 * property have a list of choices for value.
 * 
 * @brief The PropertyEnum can be used to list the values that can take a
 * property.
 * @see Property
 * @ingroup base
 */
class PropertyEnum
{
    /**
     * @brief Name of the enum
     * @var string $name
     */
    public $name;

    /**
     * @brief Caption of the property for high level interface (can contain
     * space)
     * @var string $caption
     */
    public $caption;

    /**
     * @brief Coresponding value of the enum when this enum is chosen
     * @var string $value
     */
    public $value;

    /**
     * @brief Description of the enum (optional)
     * @var string $desc
     */
    public $desc;

    /**
     * @brief Reference to the associated parent Property
     * @var Property $parentProperty
     */
    public $parentProperty;

    /**
     * @brief constructor of PropertyEnum
     * 
     * Initialise all the internal members and call parse_xml if $xml is set
     * @param SimpleXMLElement|null $xml if it's different of null, call the
     * xml parser to fill members
     */
    function __construct($xml = null)
    {
        $this->parentProperty = null;
        if ($xml)
            $this->parse_xml($xml);
    }

    /**
     * @brief funtion that export as string the main content of the class
     * instance
     * @return string
     */
    public function __toString()
    {
        $string = $this->name
                . " caption:'" . $this->caption
                . "' value:'" . $this->value
                . "' desc:'" . $this->desc . "'";
        return $string;
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
        $this->caption = (string) $xml['caption'];
        if ($this->caption == '')
            $this->caption = $this->name;
        $this->value = (string) $xml['value'];
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
        $xml_element = $xml->createElement("enum");

        // name
        $att = $xml->createAttribute('name');
        $att->value = $this->name;
        $xml_element->appendChild($att);

        // value
        $att = $xml->createAttribute('value');
        $att->value = $this->value;
        $xml_element->appendChild($att);

        if ($format == "complete" or $format == "blockdef")
        {
            if ($this->caption != $this->name)
            {
                // caption
                $att = $xml->createAttribute('caption');
                $att->value = $this->caption;
                $xml_element->appendChild($att);
            }

            // desc
            if (!empty($this->desc))
            {
                $att = $xml->createAttribute('desc');
                $att->value = $this->desc;
                $xml_element->appendChild($att);
            }
        }

        return $xml_element;
    }
}
