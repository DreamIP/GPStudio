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

require_once("componentpartflow.php");
require_once("componentpartproperty.php");

/**
 * Multiple parts could be used to define a component
 * @brief ComponentPart is the the graphical definition of hardware components.
 * @see Block
 * @ingroup base
 */
class ComponentPart
{
    /**
     * @brief Name of the component part
     * @var string $name
     */
    public $name;

    /**
     * @brief X position on schematic (optional)
     * @var int $x_pos
     */
    public $x_pos;

    /**
     * @brief Y position on schematic (optional)
     * @var int $y_pos
     */
    public $y_pos;

    /**
     * @brief SVG draw for the part
     * @var string $svg
     */
    public $svg;

    /**
     * @brief Array of PartFlow 
     * @var array|ComponentPartFlow $partflows
     */
    public $partflows;

    /**
     * @brief Array 
     * @var array|ComponentPartProperty $partproperties
     */
    public $partproperties;

    /**
     * @brief Constructor of the class
     *
     * Build an empty ComponentPart if $xml is empty, fill it with $xml if set
     * @param SimpleXMLElement|null $xml XML element to parse if not null
     */
    function __construct($xml = null)
    {
        $this->svg = NULL;
        $this->x_pos = -1;
        $this->y_pos = -1;
        $this->partflows = array();
        $this->partproperties = array();

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

        $this->x_pos = (int) $xml['x_pos'];
        $this->y_pos = (int) $xml['y_pos'];

        // svg
        if (isset($xml->svg))
        {
            $this->svg = dom_import_simplexml($xml->svg);
        }

        // partflows
        if (isset($xml->flows))
        {
            foreach ($xml->flows->flow as $flow)
            {
                $this->addPartFlow(new ComponentPartFlow($flow));
            }
        }

        // partproperties
        if (isset($xml->properties))
        {
            foreach ($xml->properties->property as $property)
            {
                $this->addPartProperty(new ComponentPartProperty($property));
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
        $xml_element = $xml->createElement("part");

        // name
        $att = $xml->createAttribute('name');
        $att->value = $this->name;
        $xml_element->appendChild($att);

        if ($format == "complete" or $format == "blockdef")
        {
            // SVG draw
            if ($this->svg != NULL)
            {
                cloneSvg($this->svg, $xml, $xml_element);
            }

            // partflows
            if (!empty($this->partflows))
            {
                $xml_partflows = $xml->createElement("flows");
                foreach ($this->partflows as $partflow)
                {
                    $xml_partflows->appendChild($partflow->getXmlElement($xml, $format));
                }
                $xml_element->appendChild($xml_partflows);
            }

            // partproperties
            if (!empty($this->partproperties))
            {
                $xml_partproperties = $xml->createElement("properties");
                foreach ($this->partproperties as $property)
                {
                    $xml_partproperties->appendChild($property->getXmlElement($xml, $format));
                }
                $xml_element->appendChild($xml_partproperties);
            }
        }

        if ($format == "complete" or $format == "project")
        {
            // x_pos
            if (isset($this->x_pos) and $this->x_pos != -1)
            {
                $att = $xml->createAttribute('x_pos');
                $att->value = $this->x_pos;
                $xml_element->appendChild($att);
            }

            // y_pos
            if (isset($this->y_pos) and $this->y_pos != -1)
            {
                $att = $xml->createAttribute('y_pos');
                $att->value = $this->y_pos;
                $xml_element->appendChild($att);
            }
        }

        return $xml_element;
    }

    /**
     * @brief Add a partflow to the param
     * @param PartFlow $partflow partflow to add to the param
     */
    function addPartFlow($partflow)
    {
        array_push($this->partflows, $partflow);
    }

    /**
     * @brief return a reference to the partflow with the name $name, if not
     * found, return null
     * @param string $name name of the partflow to search
     * @param bool $casesens take care or not of the case of the name
     * @return PartFlow found bitfield
     */
    function getPartFlow($name, $casesens = true)
    {
        if ($casesens)
        {
            foreach ($this->partflows as $partflow)
            {
                if ($partflow->name == $name)
                    return $partflow;
            }
        }
        else
        {
            foreach ($this->partflows as $partflow)
            {
                if (strcasecmp($partflow->name, $name) == 0)
                    return $partflow;
            }
        }
        return null;
    }

    /**
     * @brief delete a partflow from his name
     * @param string $name name of the partflow to delete
     */
    function delPartFlow($name)
    {
        $i = 0;
        foreach ($this->partflows as $partflow)
        {
            if ($partflow->name == $name)
            {
                unset($this->partflows[$i]);
                return;
            }
            $i++;
        }
        return null;
    }

    /**
     * @brief Add a partproperty to the param
     * @param PartProperty $partproperty partproperty to add to the param
     */
    function addPartProperty($partproperty)
    {
        array_push($this->partproperties, $partproperty);
    }

    /**
     * @brief return a reference to the partproperty with the name $name, if not
     * found, return null
     * @param string $name name of the partproperty to search
     * @param bool $casesens take care or not of the case of the name
     * @return PartProperty found bitfield
     */
    function getPartProperty($name, $casesens = true)
    {
        if ($casesens)
        {
            foreach ($this->partproperties as $partproperty)
            {
                if ($partproperty->name == $name)
                    return $partproperty;
            }
        }
        else
        {
            foreach ($this->partproperties as $partproperty)
            {
                if (strcasecmp($partproperty->name, $name) == 0)
                    return $partproperty;
            }
        }
        return null;
    }

    /**
     * @brief delete a partproperty from his name
     * @param string $name name of the partproperty to delete
     */
    function delPartProperty($name)
    {
        $i = 0;
        foreach ($this->partproperties as $partproperty)
        {
            if ($partproperty->name == $name)
            {
                unset($this->partproperties[$i]);
                return;
            }
            $i++;
        }
        return null;
    }
}
