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
 * FlowConnect are used in FI::flow_connects as a list.
 * @brief Define flow connection between flow interface of blocks
 * @see Block FI Flow
 * @ingroup base
 */
class FlowConnect
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
     * @brief Byte ordering can be "msb" or "lsb", default value is "msb"
     * @var string $order
     */
    public $order;

    /**
     * @brief constructor of FlowConnect
     * @param SimpleXMLElement|string|null $fromBlock if it's different of null,
     * call the xml parser to fill members
     * @param string|null $fromFlow default value contructor
     * @param string|null $toBlock default value contructor
     * @param string|null $toFlow default value contructor
     * @param string $order default value contructor
     */
    function __construct($fromBlock = NULL, $fromFlow = NULL, $toBlock = NULL, $toFlow = NULL, $order = 'msb')
    {
        $this->order = 'msb';
        if ($fromBlock != NULL)
        {
            if (is_object($fromBlock) and get_class($fromBlock) === 'SimpleXMLElement')
            {
                $xml = $fromBlock;
                $this->parse_xml($xml);
            }
            else
            {
                $this->fromblock = $fromBlock;
                $this->fromflow = $fromFlow;
                $this->toblock = $toBlock;
                $this->toflow = $toFlow;
                $this->order = $order;
            }
        }
    }

    /**
     * @brief internal function to fill this instance from input xml structure
     * 
     * Can be call only from this node into the constructor
     * @param SimpleXMLElement $xml xml element to parse
     */
    protected function parse_xml($xml)
    {
        $this->fromblock = (string) $xml['fromblock'];
        $this->fromflow = (string) $xml['fromflow'];
        $this->toblock = (string) $xml['toblock'];
        $this->toflow = (string) $xml['toflow'];
        if (isset($xml['order']))
            $this->order = (string) $xml['order'];
        else
            $this->order = 'msb';
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
        if ($format == "project")
        {
            $xml_element = $xml->createElement("connect");
        }
        else
        {
            $xml_element = $xml->createElement("flow_connect");
        }

        // fromblock
        $att = $xml->createAttribute('fromblock');
        $att->value = $this->fromblock;
        $xml_element->appendChild($att);

        // fromflow
        $att = $xml->createAttribute('fromflow');
        $att->value = $this->fromflow;
        $xml_element->appendChild($att);

        // toblock
        $att = $xml->createAttribute('toblock');
        $att->value = $this->toblock;
        $xml_element->appendChild($att);

        // toflow
        $att = $xml->createAttribute('toflow');
        $att->value = $this->toflow;
        $xml_element->appendChild($att);

        // order
        $att = $xml->createAttribute('order');
        $att->value = $this->order;
        $xml_element->appendChild($att);

        return $xml_element;
    }
}
