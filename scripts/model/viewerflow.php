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
 * ViewerFlow is used into the Viewer::$viewerflows to list associated flow on each viewer.
 * 
 * @brief The ViewerFlow class define a flow connection to a viewer
 * @see Viewer
 * @ingroup base
 */
class ViewerFlow
{
    /**
     * @brief Name of the associated flow
     * @var string $flowName
     */
    public $flowName;

    /**
     * @brief constructor of ViewerFlow
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
        $this->flowName = (string) $xml['flowName'];
    }

    /**
     * @brief permits to output this instance
     * 
     * Return a formated node for the node_generated file. This method call all
     * the children getXmlElement to add into this node.
     * @param DOMDocument $xml reference of the output xml document
     * @return DOMElement xml element corresponding to this current instance
     */
    public function getXmlElement($xml)
    {
        $xml_element = $xml->createElement("flow");

        // name
        $att = $xml->createAttribute('flowName');
        $att->value = $this->flowName;
        $xml_element->appendChild($att);

        return $xml_element;
    }
}
