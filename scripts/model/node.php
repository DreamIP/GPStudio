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

require_once("board.php");
require_once("block.php");
require_once("process.php");
require_once("gpviewer.php");

/**
 * A node in GPStudio is typically a smart camera or a server. It store the list
 * of blocks and the local configuration of the node with the definition of the
 * board.
 * 
 * @brief Node is the base class container that store all the configuration of a
 * node.
 * @see Block Board
 * @ingroup base
 */
class Node
{
    /**
     * @brief The complete path of the file of the input definition
     * @var string $node_file
     */
    public $node_file;

    /**
     * @brief Name of the node
     * @var string $name
     */
    public $name;

    /**
     * @brief Board structure of the node
     * @var Board $board
     */
    public $board;

    /**
     * @brief Array of al the blocks (process or io) contain in the node
     * @var array|Block $blocks
     */
    public $blocks;

    public $xml;

    /**
     * @brief constructor of Node
     * 
     * Initialise all the internal members and call parse_config_xml if
     * $node_file is set
     * @param string $node_file Node file name
     */
    function __construct($node_file = null)
    {
        $this->blocks = array();

        if ($node_file)
            $this->parse_config_xml($node_file);
    }

    /**
     * @brief internal function to fill this instance from input xml file
     * @param string $node_file Node file name
     */
    protected function parse_config_xml($node_file)
    {
        if (!file_exists($node_file))
            error("File $node_file doesn't exist", 5, "Node");
        if (!($this->xml = simplexml_load_file($node_file)))
            error("Error when parsing $node_file", 5, "Node");
        $this->node_file = realpath($node_file);

        $this->name = (string) $this->xml['name'];

        if (isset($this->xml->board))
        {
            $this->board = new Board($this->xml->board, $this);
        }

        // process
        if (isset($this->xml->process))
        {
            foreach ($this->xml->process->process as $process)
            {
                $processBlock = new Process($process);

                // redef params
                if (isset($process->params))
                {
                    foreach ($process->params->param as $param)
                    {
                        if (isset($param['name']) and isset($param['value']))
                        {
                            if ($concerned_param = $processBlock->getParam((string) $param['name']))
                            {
                                $concerned_param->value = $param['value'];
                            }
                            else
                            {
                                warning('parameter ' . $param['name'] . " does not exist", 16, $processBlock->name);
                            }
                        }
                    }
                }

                // redef part position
                if (isset($process->parts))
                {
                    foreach ($process->parts->part as $part)
                    {
                        if (isset($part['name']) and (isset($part['x_pos']) or isset($part['y_pos'])))
                        {
                            if ($concerned_part = $processBlock->getPart((string) $part['name']))
                            {
                                $concerned_part->x_pos = $part['x_pos'];
                                $concerned_part->y_pos = $part['y_pos'];
                            }
                        }
                    }
                }

                // redef properties
                if (isset($process->properties))
                {
                    foreach ($process->properties->property as $property)
                    {
                        if (isset($property['name']) and isset($property['value']))
                        {
                            if ($concerned_property = $processBlock->getPropertyPath((string) $property['name']))
                            {
                                $concerned_property->value = $property['value'];
                            }
                            else
                            {
                                warning('property ' . $property['name'] . " does not exist", 16, $processBlock->name);
                            }
                        }
                        if (isset($property->properties))
                        {
                            foreach ($property->properties->property as $childPropertyXml)
                            {
                                if (isset($childPropertyXml['name']) and isset($childPropertyXml['value']))
                                {
                                    if ($concerned_property = $processBlock->getPropertyPath($property['name'] . '.' . (string) $childPropertyXml['name']))
                                    {
                                        $concerned_property->value = $childPropertyXml['value'];
                                    }
                                    else
                                    {
                                        warning('property ' . $property['name'] . '.' . $childPropertyXml['name'] . " does not exist", 16, $processBlock->name);
                                    }
                                }
                            }
                        }
                    }
                }

                // redef flow size
                if (isset($process->flows))
                {
                    foreach ($process->flows->flow as $flow)
                    {
                        if (isset($flow['name']) and isset($flow['size']))
                        {
                            if ($concerned_flow = $processBlock->getFlow((string) $flow['name']))
                            {
                                $concerned_flow->size = (int) $flow['size'];
                            }
                            else
                            {
                                warning('flow ' . $flow['name'] . " does not exist", 16, $processBlock->name);
                            }
                        }
                    }
                }

                // redef clock freq
                if (isset($process->clocks))
                {
                    foreach ($process->clocks->clock as $clock)
                    {
                        if (isset($clock['name']) and isset($clock['typical']))
                        {
                            if ($concerned_clock = $processBlock->getClock((string) $clock['name']))
                            {
                                $concerned_clock->typical = Clock::convert($clock['typical']);
                            }
                            else
                            {
                                warning('clock ' . $clock['name'] . " does not exist", 16, $processBlock->name);
                            }
                        }
                    }
                }

                $this->addBlock($processBlock);
            }
        }

        if (isset($this->xml->gpviewer))
        {
            $this->gpviewer = new GPViewer($this->xml->gpviewer);
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
        $xml_element = $xml->createElement("node");

        // name
        $att = $xml->createAttribute('name');
        $att->value = $this->name;
        $xml_element->appendChild($att);

        // board
        if (isset($this->board))
        {
            $xml_element->appendChild($this->board->getXmlElement($xml, $format));
        }

        if ($format == "complete")
        {
            // blocks
            $xml_blocks = $xml->createElement("blocks");
            foreach ($this->blocks as $block)
            {
                $xml_blocks->appendChild($block->getXmlElement($xml, $format));
            }
            $xml_element->appendChild($xml_blocks);
        }
        elseif ($format == "project")
        {
            // process
            $xml_blocks = $xml->createElement("process");
            foreach ($this->blocks as $block)
            {
                if ($block->type() == "process")
                    $xml_blocks->appendChild($block->getXmlElement($xml, $format));
            }
            $xml_element->appendChild($xml_blocks);

            // special blocks
            foreach ($this->blocks as $block)
            {
                if ($block->type() != "process" and $block->type() != "io" and $block->type() != "iocom")
                {
                    $xml_element->appendChild($block->getXmlElement($xml, $format));
                }
            }
        }

        // gpviewer
        if (isset($this->gpviewer))
        {
            $xml_element->appendChild($this->gpviewer->getXmlElement($xml));
        }

        return $xml_element;
    }

    /**
     * @brief Sets the board from his name in library.
     * 
     * Gets the Board block from the library or local project and add it to the
     * node.
     * @param string $boardName name of the board.
     * @see Board
     */
    function setBoard($boardName)
    {
        if (isset($this->board))
        {
            unset($this->board);
        }
        $this->board = new Board($boardName, $this);
    }

    /**
     * @brief Adds an IO from his name or path.
     * 
     * Gets the IO block from the library or local project and add it to the
     * node.
     * @param string $ioName name or path of the io to add
     */
    function addIo($ioName)
    {
        if (isset($this->board))
        {
            $this->board->addIo($ioName);
        }
    }

    /**
     * @brief Delete an IO block from his name
     * @param string $ioName name of the io block to delete
     */
    function delIo($ioName)
    {
        $i = 0;
        foreach ($this->blocks as $block)
        {
            if ($block->name == $ioName and ( $block->type() == "io" or $block->type() == "iocom"))
                unset($this->blocks[$i]);
            $i++;
        }
    }

    /**
     * @brief Adds a process from his name or path.
     * 
     * Gets the process block from the library or local project and add it to
     * the node.
     * @param string $processName name or path of the io to add
     * @param string $processDriver driver name or path of the process to add
     */
    function addProcess($processName, $processDriver)
    {
        $process = new Process($processDriver);
        $process->name = $processName;
        $this->addBlock($process);
    }

    /**
     * @brief Delete a process block from his name
     * @param string $processName name of the process block to delete
     */
    function delProcess($processName)
    {
        $i = 0;
        foreach ($this->blocks as $block)
        {
            if ($block->name == $processName and $block->type() == "process")
                unset($this->blocks[$i]);
            $i++;
        }
    }

    /**
     * @brief Save the whole structure with all data including all internal
     * children data
     * 
     * Generally used to save the 'node_generated.xml' file in build dir.
     * @param string $file file name to save
     */
    function saveXml($file)
    {
        $xml = new DOMDocument("1.0", "UTF-8");
        $xml->preserveWhiteSpace = false;
        $xml->formatOutput = true;

        $xml->appendChild($this->getXmlElement($xml, "complete"));

        $xml->save($file);
    }

    /**
     * @brief Save the project with the minimal information needed.
     * 
     * Generally used to save the '.node' project file.
     * @param string $file file name to save
     */
    function saveProject($file)
    {
        $xml = new DOMDocument("1.0", "UTF-8");
        $xml->preserveWhiteSpace = false;
        $xml->formatOutput = true;

        $xml->appendChild($this->getXmlElement($xml, "project"));

        $xml->save($file);
    }

    /**
     * @brief Add a block to the node 
     * @param Block $block block to add to the node
     */
    function addBlock($block)
    {
        $block->parentNode = $this;
        $block->toGlobalPropertyPath();

        array_push($this->blocks, $block);
    }

    /**
     * @brief return a reference to the block with the name $name, if not found,
     * return null
     * @param string $name name of the block to search
     * @return Block found block
     */
    function getBlock($name)
    {
        foreach ($this->blocks as $block)
        {
            if ($block->name == $name)
                return $block;
        }
        return null;
    }

    /**
     * @brief return a reference to the flow with the name $name, if not found,
     * return null
     * @param string $name name of the flow to search composed by the name of
     * the block followed by the name of the flow. Ex : 'block1.flowin0'
     * @return Flow found flow
     */
    function getFlow($name)
    {
        $subPath = explode('.', $name);
        if (count($subPath) != 2)
            return NULL;
        $block = $this->getBlock($subPath[0]);
        if ($block == NULL)
            return NULL;
        $flow = $block->getFlow($subPath[1]);
        return $flow;
    }
}
