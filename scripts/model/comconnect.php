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
 * It links the id of the software protocol to the name of the harware
 * communication flow or PI interface
 * 
 * @brief The ComConnect to define mapping between hardware interfaces and high
 * level software tool
 * @see IOCom
 * @ingroup base
 */
class ComConnect
{
    /**
     * @brief Attribute name
     * @var string $link
     */
    public $link;

    /**
     * @brief Value of the id
     * @var string $id
     */
    public $id;

    /**
     * @brief Type of connection, could be "flow", "paramin" or "paramout"
     * @var string $type
     */
    public $type;

    /**
     * @brief constructor of ComConnect
     * 
     * Initialise all the internal members and call parse_xml if $xml is set
     * @param SimpleXMLElement|null $xml if it's different of null, call the
     * xml parser to fill members
     */
    function __construct($xml = null)
    {
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
        $this->link = (string) $xml['link'];
        $this->id = (string) $xml['id'];
        $this->type = (string) $xml['type'];
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
        $xml_element = $xml->createElement("com_connect");

        // link
        $att = $xml->createAttribute('link');
        $att->value = $this->link;
        $xml_element->appendChild($att);

        // id
        $att = $xml->createAttribute('id');
        $att->value = $this->id;
        $xml_element->appendChild($att);

        // type
        $att = $xml->createAttribute('type');
        $att->value = $this->type;
        $xml_element->appendChild($att);

        return $xml_element;
    }
}
