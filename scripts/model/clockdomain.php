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
 * ClockDomain permits to save typical frequency of a clock domain. Use it
 * in addition to clock with shift or ratio. Clock defined with the same
 * clockdomain ensure to be syncronized.
 * 
 * @brief ClockDomain permits to save typical frequency of a clock domain.
 * @see Clock ClockInterconnect
 * @ingroup base
 */
class ClockDomain
{
    /**
     * @brief Name of the clock
     * @var string $name
     */
    public $name;

    /**
     * @brief Typical value for this clock in Hz, could be written like this :
     * "14.2M" or "18.7k" or "1500"
     * @var float $typical
     */
    public $typical;

    /**
     * @brief constructor of ClockDomain
     * 
     * Initialise all the internal members and call parse_xml if $xml is set
     * @param SimpleXMLElement|null $name if it's different of null, call the
     * xml parser to fill members, in case of $name is a string, init member
     * with this value
     * @param int|null $typical
     */
    function __construct($name = null, $typical = null)
    {
        if (is_object($name) and get_class($name) === 'SimpleXMLElement')
        {
            $xml = $name;
            $this->parse_xml($xml);
        }
        else
        {
            if ($name != null)
                $this->name = $name;

            if ($typical != null)
                $this->typical = $typical;
        }
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
        $this->typical = Clock::convert($xml['typical']);
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
        $xml_element = $xml->createElement("domain");

        // name
        $att = $xml->createAttribute('name');
        $att->value = $this->name;
        $xml_element->appendChild($att);

        // typical
        $att = $xml->createAttribute('typical');
        $att->value = $this->typical;
        $xml_element->appendChild($att);

        return $xml_element;
    }
}
