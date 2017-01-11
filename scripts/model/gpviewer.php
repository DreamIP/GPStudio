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

require_once("viewer.php");

/**
 * Structure that store all gpviewer dedicated parameters.
 * @brief List of all the configurated viewer for gpviewer.
 * @see Viewer Node
 * @ingroup base
 */
class GPViewer
{
    /**
     * @brief Array of ComConnect to give the list of all the configurated viewer for gpviewer.
     * @var array|Viewer $viewers
     * @see Viewer
     */
    public $viewers;

    /**
     * @brief internal function to fill this instance from input xml structure
     * 
     * Can be call only from this node into the constructor
     * @param SimpleXMLElement $xml xml element to parse
     */
    function __construct($xml = NULL)
    {
        $this->viewers = array();

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
        // viewers
        if (isset($xml->viewers))
        {
            foreach ($xml->viewers->viewer as $viewer)
            {
                $this->addViewer(new Viewer($viewer));
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
        $xml_element = $xml->createElement("gpviewer");

        // com_connects
        $xml_viewers = $xml->createElement("viewers");

        foreach ($this->viewers as $viewer)
        {
            $xml_viewers->appendChild($viewer->getXmlElement($xml));
        }
        $xml_element->appendChild($xml_viewers);

        return $xml_element;
    }

    /**
     * @brief Add a viewer to the list of viewers
     * @param Viewer $viewer viewer to add to the list of viewers
     */
    function addViewer($viewer)
    {
        array_push($this->viewers, $viewer);
    }

    /**
     * @brief return a reference to the viewer with the name $name, if not found,
     * return null
     * @param string $name name of the viewer to search
     * @return Viewer found viewer
     */
    function getViewer($name)
    {
        foreach ($this->viewers as $viewer)
        {
            if ($viewer->name == $name)
                return $viewer;
        }
        return null;
    }
}
