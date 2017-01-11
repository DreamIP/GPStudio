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

class TreeItem
{
    /**
     * @brief Name of the block source of the flow
     * @var string $fromblock
     */
    public $fromblock;

    /**
     * @brief Name of the flow on the block source of the flow
     * @var string $fromflow
     */
    public $fromflow;

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
     * @brief Value of the property to choose this connexion
     * @var string $muxvalue
     */
    public $muxvalue;

    function __construct($fromblock = '', $fromflow = '', $size = 8, $order = 'msb', $muxvalue = '')
    {
        $this->fromblock = $fromblock;
        $this->fromflow = $fromflow;
        $this->size = $size;
        $this->order = $order;
        $this->muxvalue = $muxvalue;
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
        $xml_element = $xml->createElement("tree_item");

        // fromblock
        $att = $xml->createAttribute('fromblock');
        $att->value = $this->fromblock;
        $xml_element->appendChild($att);

        // fromflow
        $att = $xml->createAttribute('fromflow');
        $att->value = $this->fromflow;
        $xml_element->appendChild($att);

        // size
        $att = $xml->createAttribute('size');
        $att->value = $this->size;
        $xml_element->appendChild($att);

        // order
        $att = $xml->createAttribute('order');
        $att->value = $this->order;
        $xml_element->appendChild($att);

        // muxvalue
        $att = $xml->createAttribute('muxvalue');
        $att->value = $this->muxvalue;
        $xml_element->appendChild($att);

        return $xml_element;
    }
}
