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

require_once("viewerflow.php");

/**
 * Viewer is used into the Block::$viewers to list all the viewer of the block.
 * 
 * @brief The Viewer class define a viewer input or viewer provider.
 * @see Property
 * @ingroup base
 */
class Viewer
{
    /**
     * @brief Name of the viewer
     * @var string $name
     */
    public $name;

    /**
     * @brief Array of ViewerFlow associated to this viewer
     * @var array|ViewerFlow $viewerFlows
     * @see ViewerFlow
     */
    public $viewerFlows;

    /**
     * @brief constructor of Viewer
     * 
     * Initialise all the internal members and call parse_xml if $xml is set
     * @param SimpleXMLElement|null $xml if it's different of null, call the
     * xml parser to fill members
     */
    function __construct($xml = null)
    {
        $this->viewerFlows = array();

        if ($xml)
            $this->parse_xml($xml);
    }

    /** @brief Add a flow to the viewer 
     *  @param ViewerFlow $flow property to add to the block
     */
    function addViewerFlow($flow)
    {
        array_push($this->viewerFlows, $flow);
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
        
        // viewers
        if (isset($xml->flows))
        {
            foreach ($xml->flows->flow as $flow)
            {
                $this->addViewerFlow(new ViewerFlow($flow));
            }
        }
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
        $xml_element = $xml->createElement("viewer");

        // name
        $att = $xml->createAttribute('name');
        $att->value = $this->name;
        $xml_element->appendChild($att);

        // parambitfields
        if (!empty($this->viewerFlows))
        {
            $xml_flows = $xml->createElement("flows");
            foreach ($this->viewerFlows as $viewerflow)
            {
                $xml_flows->appendChild($viewerflow->getXmlElement($xml));
            }
            $xml_element->appendChild($xml_flows);
        }

        return $xml_element;
    }
}
