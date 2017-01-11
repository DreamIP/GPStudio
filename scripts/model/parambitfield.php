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
 * A param can contains multiple bitfieds and each bit field can be composed by
 * one or more bits.
 * 
 * @brief Bit field for param when param are registers
 * @see Param
 * @ingroup base
 */
class ParamBitfield
{
    /**
     * @brief Name of the bitfield
     * @var string $name
     */
    public $name;

    /**
     * @brief Type of the value
     * @var string $type
     */
    public $type;

    /**
     * @brief Current value of the bitfield
     * @var string $value
     */
    public $value;

    /**
     * @brief Bitfield expression in text format, example:
     * "2-0" "2,6,1" "15-4,0"
     * @var string $bitfield
     */
    public $bitfield;

    /**
     * @brief Mapping to properties (optional)
     * @var string $propertymap
     */
    public $propertymap;

    /**
     * @brief Description of the bitfield (optional)
     * @var string $desc
     */
    public $desc;

    /**
     * @brief Array of int for concerned bit in the param
     * @var array|int $bitfieldlist
     */
    public $bitfieldlist;

    /**
     * @brief Reference to the associated parent Param
     * @var Param $parentParam
     */
    public $parentParam;

    /**
     * @brief constructor of ParamBitfield
     * 
     * Initialise all the internal members and call parse_xml if $xml is set
     * @param SimpleXMLElement|null $xml if it's different of null, call the
     * xml parser to fill members
     */
    function __construct($xml = null)
    {
        $this->bitfieldlist = array();
        $this->paramenums = array();
        $this->parentParam = NULL;
        if ($xml)
            $this->parse_xml($xml);
    }

    /**
     * @brief funtion that export as string the main content of the class instance
     * @return string
     */
    public function __toString()
    {
        return "bitfield " . $this->name . " bitfield: " . $this->bitfield . " propertymap: '" . $this->propertymap . "'";
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
        $this->value = (string) $xml['value'];
        $this->value = (string) $xml['value'];
        $this->propertymap = (string) $xml['propertymap'];
        $this->desc = (string) $xml['desc'];

        if (isset($xml['bitfield']))
        {
            $bitfield = (string) $xml['bitfield'];
            if (!empty($bitfield) or $bitfield == "0")
            {
                $this->bitfield = $bitfield;
                $this->bitfieldlist = ParamBitfield::decodeBitField($bitfield);
            }
        }
    }

    /**
     * @brief bit field expression parser that gives the list of bits
     * @param type $string bit field expression
     * @return array list of all bits ordered
     */
    public static function decodeBitField($string)
    {
        $bitfieldlist = array();
        // bitfield support with exp like 3,0 => [3 0] or 3-0 => [3 2 1 0] or 6-4,0 => [6 5 4 0]
        preg_match_all("|([-,]?)([0-9]+)|", $string, $out, PREG_SET_ORDER);

        $prev = -1;
        $lastsymbole = '';
        foreach ($out as $bitdesc)
        {
            $bit = $bitdesc[2];
            $symbole = $bitdesc[1];

            if ($bit > 31)
                break;

            if ($symbole == ',')
            {
                if ($lastsymbole == ',' or $lastsymbole == '')
                    array_push($bitfieldlist, $prev);
            }
            elseif ($symbole == '-')
            {
                if ($prev < $bit)
                {
                    for ($i = $prev; $i <= $bit; $i++)
                        array_push($bitfieldlist, $i);
                }
                else
                {
                    for ($i = $prev; $i >= $bit; $i--)
                        array_push($bitfieldlist, $i);
                }
            }

            $prev = $bit;
            $lastsymbole = $symbole;
        }
        if (($lastsymbole == ',' or $lastsymbole == '') and $prev != -1)
            array_push($bitfieldlist, $prev);
        return $bitfieldlist;
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
        $xml_element = $xml->createElement("bitfield");

        // name
        $att = $xml->createAttribute('name');
        $att->value = $this->name;
        $xml_element->appendChild($att);

        // type
        if (!empty($this->type))
        {
            $att = $xml->createAttribute('type');
            $att->value = $this->type;
            $xml_element->appendChild($att);
        }

        // value
        if (!empty($this->value))
        {
            $att = $xml->createAttribute('value');
            $att->value = $this->value;
            $xml_element->appendChild($att);
        }

        // bitfield
        $att = $xml->createAttribute('bitfield');
        $att->value = $this->bitfield;
        $xml_element->appendChild($att);

        // propertymap
        if (!empty($this->propertymap))
        {
            $att = $xml->createAttribute('propertymap');
            $att->value = $this->propertymap;
            $xml_element->appendChild($att);
        }

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
