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

require_once("attribute.php");

/**
 * @brief Toolchain class define a toolchain for building a project.
 * @ingroup base
 */
class Toolchain
{
    /**
     * @brief Array of attributes of the toolchain (optional)
     * @var array|Attribute $attributes
     */
    public $attributes;

    /**
     * @brief constructor of Toolchain
     * 
     * Initialise all the internal members and call parse_xml if $xml is set
     * @param SimpleXMLElement|null $xml if it's different of null, call the
     * xml parser to fill members
     */
    function __construct($xml = null)
    {
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

        // attributes
        if (isset($xml->attributes))
        {
            foreach ($xml->attributes->attribute as $attribute)
            {
                $this->addAttribute(new Attribute($attribute));
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
        $xml_element = $xml->createElement("toolchain");

        // name
        $att = $xml->createAttribute('name');
        $att->value = $this->name;
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

    /**
     * @brief Configure the project node $node to be ready to be generated.
     * @param Node $node node to configure
     * @see Toolchain::generate_project
     */
    public function configure_project($node)
    {
        // must be re implemented by childrens
    }

    /**
     * @brief Create all necessary files to be compiled by the specified
     * toolchain. Files generated depends of the used toolchain.
     * @param Node $node node structure
     * @param string $path output path of the generated project
     */
    public function generate_project($node, $path)
    {
        // must be re implemented by childrens
    }

    static public function load($toolchain_name, $xml = null)
    {
        switch ($toolchain_name)
        {
            case 'hdl':
                require_once("toolchain/hdl/hdl.php");
                return new HDL_toolchain($xml);
            case 'altera_quartus':
                require_once("toolchain/altera_quartus/altera_quartus.php");
                return new Altera_quartus_toolchain($xml);
            default:
                throw new Exception('Toolchain \'' . $toolchain_name . '\' doesn\'t exists.');
        }
    }

    /** Add a attribute to the toolchain 
     *  @param Attribute $attribute attribute to add to the toolchain
     */
    function addAttribute($attribute)
    {
        array_push($this->attributes, $attribute);
    }

    /** return a reference to the attribute with the name $name, if not found,
     * return false
     *  @param string $name name of the attribute enum to search
     *  @return Attribute found attribute
     */
    function getAttribute($name)
    {
        foreach ($this->attributes as $attribute)
        {
            if ($attribute->name == $name)
                return $attribute;
        }
        return null;
    }

    /**
     * @brief This function is used for special ressources attributes request.
     * @param string $type Type of ressource requested.
     * @return array
     */
    function getRessourceAttributes($type)
    {
        // must be re implemented by childrens
        $attr = array();
        return $attr;
    }

    /**
     * @brief This function is used for special ressources assignement request.
     * @param string $type Type of ressource requested.
     * @return array
     */
    function getRessourceDeclare($type, $params)
    {
        // must be re implemented by childrens
        $declare = '';
        return $declare;
    }
}
