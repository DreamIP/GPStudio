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

require_once("treeitem.php");

/**
 * Reset is used into the Component::$resets to list all the reset of the block.
 * Compared to FlowConnect, this interface is processed to obtain all the connection
 * with the generated MUX.
 * 
 * @brief The TreeConnect class define a connection between two flows.
 * @see PI Flow
 * @ingroup base
 */
class TreeConnect
{
    /**
     * @brief Name of the block sink of the flow
     * @var string $toblock
     */
    public $toblock;

    /**
     * @brief Name of the flow on the block sink of the flow
     * @var string $toflow
     */
    public $toflow;

    /**
     * @brief Size of connect in bit
     * @var string $size
     */
    public $size;

    /**
     * @brief Byte ordering can be "msb" or "lsb", default value is "msb"
     * @var string $order
     */
    public $order;

    /**
     * @brief Property name of the MUX
     * @var string $muxname
     */
    public $muxname;

    /**
     * @brief List of all the source flow can be chosen for this flow input
     * @var array|TreeItem $treeitems
     */
    public $treeitems;

    function __construct($toblock = '', $toflow = '', $size = 8, $order = 'msb', $muxname = '')
    {
        $this->toblock = $toblock;
        $this->toflow = $toflow;
        $this->size = $size;
        $this->order = $order;
        $this->muxname = $muxname;

        $this->treeitems = array();
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
        $xml_element = $xml->createElement("tree_connect");

        // toblock
        $att = $xml->createAttribute('toblock');
        $att->value = $this->toblock;
        $xml_element->appendChild($att);

        // toflow
        $att = $xml->createAttribute('toflow');
        $att->value = $this->toflow;
        $xml_element->appendChild($att);

        // size
        $att = $xml->createAttribute('size');
        $att->value = $this->size;
        $xml_element->appendChild($att);

        // order
        $att = $xml->createAttribute('order');
        $att->value = $this->order;
        $xml_element->appendChild($att);

        // muxname
        $att = $xml->createAttribute('muxname');
        $att->value = $this->muxname;
        $xml_element->appendChild($att);

        // treeitems
        $xml_treeitems = $xml->createElement("tree_items");
        foreach ($this->treeitems as $treeitem)
        {
            $xml_treeitems->appendChild($treeitem->getXmlElement($xml, $format));
        }
        $xml_element->appendChild($xml_treeitems);

        return $xml_element;
    }
}
