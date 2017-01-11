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

require_once("propertyenum.php");

/**
 * Property is contained into the Block::$properties and Flow::$properties
 * @brief The Property class is used to define high level properties
 * @see Block Param ParamBitfield Flow
 * @ingroup base
 */
class Property
{
    /**
     * @brief Name of the property
     * @var string $name
     */
    public $name;

    /**
     * @brief Caption of the property for high level interface
     * @var string $caption
     */
    public $caption;

    /**
     * @brief Type of property
     * @var string $type
     */
    public $type;

    /**
     * @brief Current value of the property
     * @var string $value
     */
    public $value;

    /**
     * @brief Minimum value of the property
     * @var string $min
     */
    public $min;

    /**
     * @brief Maximum value of the property
     * @var string $max
     */
    public $max;

    /**
     * @brief Step value of the property
     * @var string $step
     */
    public $step;

    /**
     * @brief Check if the property is good (optional)
     * @var string $assert
     */
    public $assert;

    /**
     * @brief Mapping to properties (optional)
     * @var string $propertymap
     */
    public $propertymap;

    /**
     * @brief Code to execute if the property change (optional)
     * @var string $onchange
     */
    public $onchange;

    /**
     * @brief Description of the param (optional)
     * @var string $desc
     */
    public $desc;

    /**
     * @brief Array of enums if param property have different enums (optional)
     * @var array|PropertyEnum $propertyenums
     */
    public $propertyenums;

    /**
     * @brief Array of property class specify the high level properties
     * @var array|Property $properties
     */
    public $properties;

    /**
     * @brief Reference to the associated parent block (don't set if parentProperty is set)
     * @var Block $parentBlock
     */
    public $parentBlock;

    /**
     * @brief Reference to the associated parent Property (don't set if parentBlock is set)
     * @var Property $parentProperty
     */
    public $parentProperty;

    /**
     * @brief constructor of Property
     * 
     * Initialise all the internal members and call parse_xml if $xml is set
     * @param SimpleXMLElement|null $xml if it's different of null, call the
     * xml parser to fill members
     */
    function __construct($xml = null)
    {
        $this->parentBlock = null;
        $this->parentProperty = null;
        $this->propertyenums = array();
        $this->properties = array();
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
                . "' type:'" . $this->type
                . "' value:'" . $this->value
                . "' min:'" . $this->min
                . "' max:'" . $this->max
                . "' step:'" . $this->step
                . "' desc:'" . $this->desc
                . "'";
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
        if (strpos($this->name, '.') !== false)
            error("Property name cannot contains . (dot) in \"$this->name\"", 5, "Property");

        if (isset($xml['caption']))
            $this->caption = (string) $xml['caption'];
        else
            $this->caption = $this->name;

        $this->type = (string) $xml['type'];
        $this->value = (string) $xml['value'];
        $this->min = (string) $xml['min'];
        $this->max = (string) $xml['max'];
        $this->step = (string) $xml['step'];
        $this->assert = (string) $xml['assert'];
        $this->propertymap = (string) $xml['propertymap'];
        $this->onchange = (string) $xml['onchange'];
        $this->desc = (string) $xml['desc'];

        // enums
        if (isset($xml->enums))
        {
            foreach ($xml->enums->enum as $enumXml)
            {
                $this->addPropertyEnum(new PropertyEnum($enumXml));
            }
        }

        // properties
        if (isset($xml->properties))
        {
            foreach ($xml->properties->property as $propertyXml)
            {
                $this->addSubProperty(new Property($propertyXml));
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
        $xml_element = $xml->createElement("property");

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
            if ($this->caption != $this->name)
            {
                // caption
                $att = $xml->createAttribute('caption');
                $att->value = $this->caption;
                $xml_element->appendChild($att);
            }

            // type
            $att = $xml->createAttribute('type');
            $att->value = $this->type;
            $xml_element->appendChild($att);

            // value
            if (!empty($this->value) or $this->value=="0")
            {
                $att = $xml->createAttribute('value');
                $att->value = $this->value;
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

            // step
            if (!empty($this->step) or $this->step=="0")
            {
                $att = $xml->createAttribute('step');
                $att->value = $this->step;
                $xml_element->appendChild($att);
            }

            // assert
            if (!empty($this->assert))
            {
                $att = $xml->createAttribute('assert');
                $att->value = $this->assert;
                $xml_element->appendChild($att);
            }

            // propertymap
            if (!empty($this->propertymap))
            {
                $att = $xml->createAttribute('propertymap');
                $att->value = $this->propertymap;
                $xml_element->appendChild($att);
            }

            // onchange
            if (!empty($this->onchange))
            {
                $att = $xml->createAttribute('onchange');
                $att->value = htmlentities($this->onchange);
                $xml_element->appendChild($att);
            }

            // desc
            if (!empty($this->desc))
            {
                $att = $xml->createAttribute('desc');
                $att->value = $this->desc;
                $xml_element->appendChild($att);
            }

            // propertyenums
            if (!empty($this->propertyenums))
            {
                $xml_propertyenums = $xml->createElement("enums");
                foreach ($this->propertyenums as $property)
                {
                    $xml_propertyenums->appendChild($property->getXmlElement($xml, $format));
                }
                $xml_element->appendChild($xml_propertyenums);
            }
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

        return $xml_element;
    }

    /**
     * @brief Add a property enum to the block 
     * @param PropertyEnum $propertyenum property enum to add to the property
     */
    function addPropertyEnum($propertyenum)
    {
        $propertyenum->parentProperty = $this;
        array_push($this->propertyenums, $propertyenum);
    }

    /**
     * @brief return a reference to the property enum with the name $name, if not
     * found, return false
     * @param string $name name of the property enum to search
     * @param bool $casesens take care or not of the case of the name
     * @return PropertyEnum found property enum
     */
    function getPropertyEnum($name, $casesens = true)
    {
        if ($casesens)
        {
            foreach ($this->propertyenums as $propertyenum)
            {
                if ($propertyenum->name == $name)
                    return $propertyenum;
            }
        }
        else
        {
            foreach ($this->propertyenums as $propertyenum)
            {
                if (strcasecmp($propertyenum->name, $name) == 0)
                    return $propertyenum;
            }
        }
        return null;
    }

    /**
     * @brief delete a property enum from his name
     * @param string $name name of the property enum to delete 
     */
    function delPropertyEnum($name)
    {
        $i = 0;
        foreach ($this->propertyenums as $propertyenum)
        {
            if ($propertyenum->name == $name)
            {
                unset($this->propertyenums[$i]);
                return;
            }
            $i++;
        }
    }

    /**
     * @brief Add a sub-property to the property 
     * @param Property $property sub-property to add to the property
     */
    function addSubProperty($property)
    {
        $property->parentProperty = $this;
        array_push($this->properties, $property);
    }

    /**
     * @brief alias to addSubProperty($property)
     * @param Property $property sub-property to add to the property
     */
    function addProperty($property)
    {
        $this->addSubProperty($property);
    }

    /**
     * @brief return a reference to the property with the name $name, if not found, return false
     * @param string $name name of the property enum to search
     * @param bool $casesens take care or not of the case of the name
     * @return Property found property
     */
    function getSubProperty($name, $casesens = true)
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

    /**
     * @brief alias to getSubProperty($name, $casesens)
     * @param string $name name of the property enum to search
     * @param bool $casesens take care or not of the case of the name
     * @return Property found property
     */
    function getProperty($name, $casesens = true)
    {
        return $this->getSubProperty($name, $casesens);
    }

    /**
     * @brief delete a property from his name
     * @param string $name name of the property to delete 
     */
    function delSubProperty($name)
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

    /**
     * @brief alias to delSubProperty($name)
     * @param string $name name of the property to delete 
     */
    function delProperty($name)
    {
        return $this->delSubProperty($name);
    }

    /**
     * @brief return the path of the property (without the name of the block)
     * @return string path of the property
     */
    function path()
    {
        if ($this->parentProperty == NULL)
            return $this->name;
        else
            return $this->parentProperty->path() . "." . $this->name;
    }

    /**
     * @brief Returns the complete path for the property with the name of the
     * block.
     * 
     * If the property is a subproperty of another property, the call is
     * recursive.
     * @return string complete path of the property
     */
    function completePath()
    {
        if ($this->parentProperty == NULL)
        {
            if (get_class($this->parentBlock) == "Flow")
                return $this->parentBlock->parentBlock->name . "." . $this->parentBlock->name . "." . $this->name;
            else
                return $this->parentBlock->name . "." . $this->name;
        }
        else
            return $this->parentProperty->path() . "." . $this->name;
    }

    /**
     * @brief Returns all the depending property of the expression $expression
     * @param string $expression expression that need to be analysed
     * @return array|string array of property path
     */
    static function dependsProperties($expression)
    {
        preg_match_all("/([a-zA-Z_]+[a-zA-Z0-9_]*\\.?)+/x", $expression, $matches);
        $depends = array();
        foreach ($matches[0] as $match)
        {
            if (substr($match, -6) === ".value" or substr($match, -5) === ".bits")
                array_push($depends, $match);
        }
        return array_unique($depends);
    }

    /**
     * @brief Transforms an expression into a global expression in the context
     * of $blockName
     * 
     * This function use Property::dependsProperties function.
     * @param string $expression expression to convert
     * @param string $blockName context of expression (name of the block)
     * @return string processed expression in global context
     */
    static function localToGlobalPropertyPath($expression, $blockName)
    {
        foreach (Property::dependsProperties($expression) as $depend)
        {
            $expression = str_replace($depend, $blockName . '.' . $depend, $expression);
        }
        return $expression;
    }

    /**
     * @brief Transforms the propertymap expression into a global expression in
     * the context of $blockName
     * @param string $blockName context of expression (name of the block)
     */
    function toGlobalPropertyPath($blockName)
    {
        $this->propertymap = Property::localToGlobalPropertyPath($this->propertymap, $blockName);
        $this->onchange = Property::localToGlobalPropertyPath($this->onchange, $blockName);

        foreach ($this->properties as $property)
        {
            $property->toGlobalPropertyPath($blockName);
        }
    }
}
