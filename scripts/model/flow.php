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
 * Flow are stored in Block::flows as a list.
 * @brief Define flow interface in a Block.
 * @see Block FI
 * @ingroup base
 */
class Flow
{
    /**
     * @brief Name of the flow
     * @var string $name
     */
    public $name;

    /**
     * @brief Type of the flow, can be "in" or "out"
     * @var string $type
     */
    public $type;

    /**
     * @brief Size in bit of the flow
     * @var int $size
     */
    public $size;

    /**
     * @brief Description of the flow (optional)
     * @var string $desc
     */
    public $desc;

    /**
     * @brief Reference to the associated parent block
     * @var Block $parentBlock
     */
    public $parentBlock;

    /**
     * @brief Array of property class specify the high level properties
     * @var array|Property $properties
     */
    public $properties;

    /**
     * @brief constructor of Flow
     * 
     * Initialise all the internal members and call parse_xml if $xml is set
     * @param SimpleXMLElement|null $xml if it's different of null, call the
     * xml parser to fill members
     */
    function __construct($xml = null)
    {
        $this->properties = array();

        if ($xml)
            $this->parse_xml($xml);
    }

    /**
     * @brief funtion that export as string the main content of the class instance
     * @return string
     */
    public function __toString()
    {
        return "'" . $this->name . "' direction: '" . $this->type . "' size: '" . $this->size . "' desc: '" . $this->desc . "'";
    }

    /**
     * @brief internal function to fill this instance from input xml structure
     * 
     * Can be call only from this node into the constructor
     * @param SimpleXMLElement $xml xml element to parse
     */
    protected function parse_xml($xml)
    {
        $this->parentBlock = null;
        $this->name = (string) $xml['name'];
        $this->type = (string) $xml['type'];
        $this->desc = (string) $xml['desc'];
        if (!empty($xml['size']))
            $this->size = (int) $xml['size'];
        else
            $this->size = 16;

        // properties
        if (isset($xml->properties))
        {
            foreach ($xml->properties->property as $propertyXml)
            {
                $this->addProperty(new Property($propertyXml));
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
        $xml_element = $xml->createElement("flow");

        // name
        $att = $xml->createAttribute('name');
        $att->value = $this->name;
        $xml_element->appendChild($att);

        // size
        $att = $xml->createAttribute('size');
        $att->value = $this->size;
        $xml_element->appendChild($att);

        if ($format == "complete" or $format == "blockdef")
        {
            // type
            $att = $xml->createAttribute('type');
            $att->value = $this->type;
            $xml_element->appendChild($att);

            // desc
            if (!empty($this->desc))
            {
                $att = $xml->createAttribute('desc');
                $att->value = $this->desc;
                $xml_element->appendChild($att);
            }

            // properties
            if (!empty($this->properties))
            {
                $xml_property = $xml->createElement("properties");
                foreach ($this->properties as $property)
                {
                    $xml_property->appendChild($property->getXmlElement($xml, $format));
                }
                $xml_element->appendChild($xml_property);
            }
        }

        return $xml_element;
    }

    /** Add a property to the block 
     *  @param Property $property property to add to the block * */
    function addProperty($property)
    {
        $property->parentBlock = $this;
        array_push($this->properties, $property);
    }

    /** return a reference to the property with the name $name, if not found,
     * return null
     *  @param string $name name of the property to search
     *  @param bool $casesens take care or not of the case of the name
     *  @return Property found property * */
    function getProperty($name, $casesens = true)
    {
        if ($casesens)
        {
            foreach ($this->properties as $property)
            {
                if ($property->name == $name)
                    return $property;
            }
        }
        else
        {
            foreach ($this->properties as $property)
            {
                if (strcasecmp($property->name, $name) == 0)
                    return $property;
            }
        }
        return null;
    }

    /** alias to getProperty($name, $casesens)
     *  @param string $name name of the property enum to search
     *  @param bool $casesens take care or not of the case of the name
     *  @return Property found property * */
    function getSubProperty($name, $casesens = true)
    {
        return $this->getProperty($name, $casesens);
    }

    /** delete a property from his name
     *  @param string $name name of the property to delete  * */
    function delProperty($name)
    {
        $i = 0;
        foreach ($this->properties as $property)
        {
            if ($property->name == $name)
            {
                unset($this->properties[$i]);
                return;
            }
            $i++;
        }
    }
}
