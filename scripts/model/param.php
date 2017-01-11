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

require_once("parambitfield.php");

/**
 * @brief Param handle a constant parameter (generic for VHDL, param for verilog
 * constant for C/C++) or register for hardware implementation.
 * @see Block
 * @ingroup base
 */
class Param
{
    /**
     * @brief Name of the param
     * @var string $name
     */
    public $name;

    /**
     * @brief 
     * @var string $type
     */
    public $type;

    /**
     * @brief Relative adress of the param
     * @var int $regaddr
     */
    public $regaddr;

    /**
     * @brief Current value of the param
     * @var string $value
     */
    public $value;

    /**
     * @brief Default value of the param
     * @var string $default
     */
    public $default;

    /**
     * @brief Minimum value of the param
     * @var string $min
     */
    public $min;

    /**
     * @brief Maximum value of the param
     * @var string $max
     */
    public $max;

    /**
     * @brief Generic parameter (hard = true) or dynamic one (hard = false)
     * @var bool $hard
     */
    public $hard;

    /**
     * @brief Mapping to properties (optional)
     * @var string $propertymap
     */
    public $propertymap;

    /**
     * @brief Description of the param (optional)
     * @var string $desc
     */
    public $desc;

    /**
     * @brief Reference to the associated parent block
     * @var Block $parentBlock
     */
    public $parentBlock;

    /**
     * @brief Array of bitfields if param contain different bitfield (optional)
     * @var array|ParamBitfield $parambitfields
     */
    public $parambitfields;

    function __construct($xml = null)
    {
        $this->parentBlock = null;
        $this->parambitfields = array();
        $this->paramenums = array();
        if ($xml)
            $this->parse_xml($xml);
    }

    /**
     * @brief funtion that export as string the main content of the class instance
     * @return string
     */
    public function __toString()
    {
        if ($this->hard)
            return "constant " . $this->name . " type: " . $this->type . " value: " . $this->value;
        else
            return "register " . $this->name . " regaddr: " . $this->regaddr . " propertymap: '" . $this->propertymap . "'";
    }

    /**
     * @brief Compare two param object address to sort it by address
     * @param Param $a first Param object to compare
     * @param Param $b second Param object to compare
     * @return int -1, 1 or 0 to say if it is smaller, bigger or equal
     */
    public static function cmp_raddr($a, $b)
    {
        if ($a->hard && $b->hard)
            return 0;
        if ($a->hard)
            return -1;
        if ($b->hard)
            return 1;
        if ($a->regaddr == $b->regaddr)
            return 0;
        return ($a->regaddr < $b->regaddr) ? -1 : 1;
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
        $this->regaddr = (string) $xml['regaddr'];
        $this->value = (string) $xml['value'];
        $this->default = (string) $xml['default'];
        $this->min = (string) $xml['min'];
        $this->max = (string) $xml['max'];
        $this->propertymap = (string) $xml['propertymap'];
        $this->desc = (string) $xml['desc'];

        if ((string) $xml['hard'] == "1" or (string) $xml['hard'] == "true")
            $this->hard = true;
        else
            $this->hard = false;

        // parambitfields
        if (isset($xml->bitfields))
        {
            foreach ($xml->bitfields->bitfield as $bitfield)
            {
                $this->addParamBitfield(new ParamBitfield($bitfield));
            }
        }

        // enums
        if (isset($xml->enums))
        {
            foreach ($xml->enums->enum as $enum)
            {
                array_push($this->paramenums, new ParamEnum($enum));
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
        $xml_element = $xml->createElement("param");

        // name
        $att = $xml->createAttribute('name');
        $att->value = $this->name;
        $xml_element->appendChild($att);

        if ($format == "project")
        {
            // value
            $att = $xml->createAttribute('value');
            $att->value = $this->value;
            $xml_element->appendChild($att);
        }

        if ($format == "complete" or $format == "blockdef")
        {
            if ($this->hard)
            {
                // type
                $att = $xml->createAttribute('type');
                $att->value = $this->type;
                $xml_element->appendChild($att);

                // hard
                $att = $xml->createAttribute('hard');
                $att->value = $this->hard;
                $xml_element->appendChild($att);
            }
            else
            {
                // regaddr
                $att = $xml->createAttribute('regaddr');
                $att->value = $this->regaddr;
                $xml_element->appendChild($att);

                // propertymap
                if (!empty($this->propertymap))
                {
                    $att = $xml->createAttribute('propertymap');
                    $att->value = $this->propertymap;
                    $xml_element->appendChild($att);
                }
            }

            // value
            if (!empty($this->value) or $this->value=="0")
            {
                $att = $xml->createAttribute('value');
                $att->value = $this->value;
                $xml_element->appendChild($att);
            }

            // default
            if (!empty($this->default) or $this->default=="0")
            {
                $att = $xml->createAttribute('default');
                $att->value = $this->default;
                $xml_element->appendChild($att);
            }

            // min
            if (!empty($this->min) or $this->min=="0")
            {
                $att = $xml->createAttribute('min');
                $att->value = $this->min;
                $xml_element->appendChild($att);
            }

            // max
            if (!empty($this->max) or $this->max=="0")
            {
                $att = $xml->createAttribute('max');
                $att->value = $this->max;
                $xml_element->appendChild($att);
            }

            // desc
            if (!empty($this->desc))
            {
                $att = $xml->createAttribute('desc');
                $att->value = $this->desc;
                $xml_element->appendChild($att);
            }

            // parambitfields
            if (!empty($this->parambitfields))
            {
                $xml_parambitfields = $xml->createElement("bitfields");
                foreach ($this->parambitfields as $parambitfield)
                {
                    $xml_parambitfields->appendChild($parambitfield->getXmlElement($xml, $format));
                }
                $xml_element->appendChild($xml_parambitfields);
            }
        }

        return $xml_element;
    }

    /**
     * @brief Add a parambitfield to the param 
     * @param ParamBitfield $parambitfield parambitfield to add to the param
     */
    function addParamBitfield($parambitfield)
    {
        $parambitfield->parentParam = $this;
        array_push($this->parambitfields, $parambitfield);
    }

    /**
     * @brief return a reference to the bitfield with the name $name, if not
     * found, return null
     * @param string $name name of the parambitfield to search
     * @param bool $casesens take care or not of the case of the name
     * @return ParamBitfield found bitfield
     */
    function getParambitfield($name, $casesens = true)
    {
        if ($casesens)
        {
            foreach ($this->parambitfields as $parambitfield)
            {
                if ($parambitfield->name == $name)
                    return $parambitfield;
            }
        }
        else
        {
            foreach ($this->parambitfields as $parambitfield)
            {
                if (strcasecmp($parambitfield->name, $name) == 0)
                    return $parambitfield;
            }
        }
        return null;
    }

    /**
     * @brief delete a bitfield from his name
     * @param string $name name of the bitfield to delete
     */
    function delParambitfield($name)
    {
        $i = 0;
        foreach ($this->parambitfields as $parambitfield)
        {
            if ($parambitfield->name == $name)
            {
                unset($this->parambitfields[$i]);
                return;
            }
            $i++;
        }
        return null;
    }

    /**
     * @brief Redefines all the contained properties to the global context by
     * adding the name of the block at the begining of all of properties map
     * @param string $blockName Name of of the block to append before all the
     * propertymap expression
     */
    function toGlobalPropertyPath($blockName)
    {
        $this->propertymap = Property::localToGlobalPropertyPath($this->propertymap, $blockName);

        foreach ($this->parambitfields as $parambitfield)
        {
            $parambitfield->propertymap = Property::localToGlobalPropertyPath($parambitfield->propertymap, $blockName);
        }
    }
}
