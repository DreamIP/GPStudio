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
 * Board is the class to load .dev support platform file and manage IO support
 * for the board. It also load the toolchain with attribute.
 * 
 * @brief Board is the class to load .dev support platform file.
 * @see IO Pin Toolchain
 * @ingroup base
 */
require_once("io.php");
require_once("iocom.php");
require_once("toolchain.php");

class Board
{
    private $xml;

    /**
     * @brief Complete path of the board definition
     * @var string $board_file
     */
    public $board_file;

    /**
     * @brief Name of the board
     * @var string $name
     */
    public $name;

    /**
     * @brief Specify the external file script to configure the board (optional)
     * @var string $configscriptfile
     */
    public $configscriptfile;

    /**
     * @brief Specify the external file script to generate the board files (optional)
     * @var string $generatescriptfile
     */
    public $generatescriptfile;

    /**
     * @brief Path where the root of files and define of the board is putted
     * @var string $path
     */
    public $path;

    /**
     * @brief Toolchain structure of board
     * @var Toolchain $toolchain
     */
    public $toolchain;

    /**
     * @brief Array of pin mapping of the board
     * @var array|Pin $pins
     */
    public $pins;

    /**
     * @brief Array of input clocks of the board
     * @var array|Clock $clocks
     */
    public $clocks;

    /**
     * @brief Array of input resets of the board
     * @var array|Reset $resets
     */
    public $resets;

    /**
     * @brief Reference to the associated parent node
     * @var Node $parentNode
     */
    public $parentNode;

    /**
     * @brief Constructor of the class
     * 
     * Build an board from a board node elelement and a link to a node
     * @param SimpleXMLElement|string $board_element XML element to parse if
     * $board_element is an SimpleXMLElement object, name of the board else.
     * @param Node $node node associated to the board to parse it
     */
    function __construct($board_element, $node)
    {
        $this->parentNode = $node;
        $this->pins = array();
        $this->clocks = array();
        $this->resets = array();

        if (is_string($board_element))
        {
            $board_name = $board_element;
        }
        else
        {
            $board_name = $board_element['name'];
        }

        // open device define file (.dev)
        $this->path = SUPPORT_PATH . "board" . DIRECTORY_SEPARATOR . $board_name . DIRECTORY_SEPARATOR;
        $this->board_file = $this->path . $board_name . ".dev";
        if (!file_exists($this->board_file))
            error("File $this->board_file doesn't exist", 5, "Board");
        if (!($this->xml = simplexml_load_file($this->board_file)))
            error("Error when parsing $this->board_file", 5, "Board");

        $this->parse_xml($board_element, $node);
    }

    function configure($node)
    {
        if (!empty($this->configscriptfile))
        {
            if (file_exists($this->path . $this->configscriptfile))
            {
                $script = str_replace(SUPPORT_PATH, '', $this->path . $this->configscriptfile);
                $configureBoard = (include $script);
                if ($configureBoard !== FALSE)
                {
                    $configureBoard($node);
                }
            }
        }
    }

    function generate($node, $path, $language)
    {
        if (!empty($this->generatescriptfile))
        {
            if (file_exists($this->path . $this->generatescriptfile))
            {
                $script = str_replace(SUPPORT_PATH, '', $this->path . $this->generatescriptfile);
                $generateBoard = (include $script);
                if ($generateBoard !== FALSE)
                {
                    $generateBoard($node, $path, $language);
                }
            }
        }
    }

    /**
     * @brief internal function to fill this instance from input xml structure
     * 
     * Can be call only from this node into the constructor
     * @param SimpleXMLElement $xml xml element to parse
     */
    private function parse_xml($board_element, $node)
    {
        $this->name = (string) $this->xml['name'];
        $this->configscriptfile = (string) $this->xml['configscriptfile'];
        $this->generatescriptfile = (string) $this->xml['generatescriptfile'];

        $this->toolchain = Toolchain::load($this->xml->toolchain['name'], $this->xml->toolchain);

        $this->parse_ios($board_element, $node);

        // clocks
        if (isset($this->xml->global->clocks))
        {
            foreach ($this->xml->global->clocks->clock as $clockXml)
            {
                $this->addClock(new Clock($clockXml));
            }
        }

        // resets
        if (isset($this->xml->global->resets))
        {
            foreach ($this->xml->global->resets->reset as $resetXml)
            {
                $this->addReset(new Reset($resetXml));
            }
        }

        // pins
        if (isset($this->xml->global->pins))
        {
            foreach ($this->xml->global->pins->pin as $pin)
            {
                $this->addPin(new Pin($pin));
            }
        }
    }

    private function parse_ios($board_element, $node)
    {
        $used_ios = array();

        // add all used ios
        if (isset($board_element->ios))
        {
            foreach ($board_element->ios->io as $ioXml)
            {
                $io_name = (string) $ioXml['name'];
                $used_ios[$io_name] = $ioXml;
            }
            foreach ($board_element->ios->iocom as $ioXml)
            {
                $io_name = (string) $ioXml['name'];
                $used_ios[$io_name] = $ioXml;
            }
        }
        if (isset($this->xml->ios->io))
        {
            foreach ($this->xml->ios->io as $ioXml)
            {
                $io_name = (string) $ioXml['name'];
                if (array_key_exists($io_name, $used_ios))
                {
                    if ((string) $ioXml['type'] == "communication")
                    {
                        $io = new IOCom($ioXml, $used_ios[$io_name]);
                    }
                    else
                    {
                        $io = new IO($ioXml, $used_ios[$io_name]);
                    }

                    // redef params
                    if (isset($ioXml->params))
                    {
                        foreach ($ioXml->params->param as $paramXml)
                        {
                            if (isset($paramXml['name']) and isset($paramXml['value']))
                            {
                                if ($concerned_param = $io->getParam((string) $paramXml['name']))
                                {
                                    $concerned_param->value = $paramXml['value'];
                                }
                                else
                                {
                                    warning('parameter ' . $paramXml['name'] . " does not exist", 16, $io->name);
                                }
                            }
                        }
                    }

                    // redef properties
                    if (isset($ioXml->properties))
                    {
                        foreach ($ioXml->properties->property as $propertyXml)
                        {
                            if (isset($propertyXml['name']) and isset($propertyXml['value']))
                            {
                                if ($concerned_property = $io->getPropertyPath((string) $propertyXml['name']))
                                {
                                    $concerned_property->value = $propertyXml['value'];
                                }
                                else
                                {
                                    //warning('property ' . $propertyXml['name'] . " does not exist", 16, $io->name);
                                }
                            }
                        }
                    }

                    // redef clock freq
                    if (isset($ioXml->clocks))
                    {
                        warning('clocks', 16, $io->name);
                        foreach ($ioXml->clocks->clock as $clockXml)
                        {
                            if (isset($clockXml['name']) and isset($clockXml['typical']))
                            {
                                if ($concerned_clock = $io->getClock((string) $clockXml['name']))
                                {
                                    $concerned_clock->typical = Clock::convert($clockXml['typical']);
                                }
                                else
                                {
                                    warning('clock ' . $clockXml['name'] . " does not exist", 16, $processBlock->name);
                                }
                            }
                        }
                    }

                    $node->addBlock($io);
                    unset($used_ios[$io_name]);
                }
                elseif ($ioXml['optional'] != "true")
                {
                    //$node->addBlock(new IO($io, null));
                }
            }
        }

        // warning for io does'nt exist
        foreach ($used_ios as $key => $io)
        {
            warning('io \'' . $io['name'] . "' does not exist in board " . $this->name, 16, 'Board');
        }

        // replace default param value directly in .node file
        if (isset($board_element->ios->io))
        {
            $this->redefParam($board_element->ios->io, $node);
        }
        if (isset($board_element->ios->iocom))
        {
            $this->redefParam($board_element->ios->iocom, $node);
        }
    }
    
    public function redefParam($ios, $node)
    {
        foreach ($ios as $io)
        {
            if ($concerned_block = $node->getBlock($io['name']))
            {
                // params
                if (isset($io->params))
                {
                    foreach ($io->params->param as $param)
                    {
                        if (isset($param['name']) and isset($param['value']))
                        {
                            if ($concerned_param = $concerned_block->getParam((string) $param['name']))
                            {
                                $concerned_param->value = $param['value'];
                            }
                            else
                            {
                                warning('parameter ' . $param['name'] . " does not exist", 16, $concerned_block->name);
                            }
                        }
                    }
                }

                // redef part position
                if (isset($io->parts))
                {
                    foreach ($io->parts->part as $part)
                    {
                        if (isset($part['name']) and (isset($part['x_pos']) or isset($part['y_pos'])))
                        {
                            if ($concerned_part = $concerned_block->getPart((string) $part['name']))
                            {
                                $concerned_part->x_pos = $part['x_pos'];
                                $concerned_part->y_pos = $part['y_pos'];
                            }
                        }
                    }
                }

                // redef properties
                if (isset($io->properties))
                {
                    foreach ($io->properties->property as $property)
                    {
                        if (isset($property['name']) and isset($property['value']))
                        {
                            if ($concerned_property = $concerned_block->getPropertyPath((string) $property['name']))
                            {
                                $concerned_property->value = $property['value'];
                            }
                            else
                            {
                                //warning('property ' . $property['name'] . " does not exist", 16, $concerned_block->name);
                            }
                        }
                        if (isset($property->properties))
                        {
                            foreach ($property->properties->property as $childPropertyXml)
                            {
                                if (isset($childPropertyXml['name']) and isset($childPropertyXml['value']))
                                {
                                    if ($concerned_property = $concerned_block->getPropertyPath($property['name'] . '.' . (string) $childPropertyXml['name']))
                                    {
                                        $concerned_property->value = $childPropertyXml['value'];
                                    }
                                    else
                                    {
                                        //warning('property ' . $property['name'] . '.' . $childPropertyXml['name'] . " does not exist", 16, $io->name);
                                    }
                                }
                            }
                        }
                    }
                }

                // flow size
                if (isset($io->flows))
                {
                    foreach ($io->flows->flow as $flow)
                    {
                        if (isset($flow['name']) and isset($flow['size']))
                        {
                            if ($concerned_flow = $concerned_block->getFlow((string) $flow['name']))
                            {
                                $concerned_flow->size = (int) $flow['size'];
                            }
                            else
                            {
                                warning('flow ' . $flow['name'] . " does not exist", 16, $concerned_block->name);
                            }
                        }
                    }
                }

                // clocks
                if (isset($io->clocks))
                {
                    foreach ($io->clocks->clock as $clock)
                    {
                        if (isset($clock['name']) and isset($clock['typical']))
                        {
                            if ($concerned_clock = $concerned_block->getClock((string) $clock['name']))
                            {
                                $concerned_clock->typical = Clock::convert($clock['typical']);
                            }
                            else
                            {
                                warning('clock ' . $clock['name'] . " does not exist", 16, $concerned_block->name);
                            }
                        }
                    }
                }
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
        $xml_element = $xml->createElement("board");

        // name
        $att = $xml->createAttribute('name');
        $att->value = $this->name;
        $xml_element->appendChild($att);

        if ($format == "complete")
        {
            $xml_element->appendChild($this->toolchain->getXmlElement($xml, $format));
        }
        elseif ($format == "project")
        {
            // process
            $xml_blocks = $xml->createElement("ios");
            foreach ($this->parentNode->blocks as $block)
            {
                if ($block->type() == "io" or $block->type() == "iocom")
                    $xml_blocks->appendChild($block->getXmlElement($xml, $format));
            }
            $xml_element->appendChild($xml_blocks);
        }

        return $xml_element;
    }

    /** Add a clock to the block 
     *  @param Clock $clock clock to add to the block * */
    function addClock($clock)
    {
        $clock->parentBlock = $this;
        $clock->direction = "out";
        array_push($this->clocks, $clock);
    }

    /** return a reference to the clock with the name $name, if not found, return
     * null
     *  @param string $name name of the clock to search
     *  @return Clock found clock * */
    function getClock($name)
    {
        foreach ($this->clocks as $clock)
        {
            if ($clock->name == $name)
                return $clock;
        }
        return null;
    }

    /** Add a reset to the block 
     *  @param Reset $reset reset to add to the block * */
    function addReset($reset)
    {
        $reset->parentBlock = $this;
        array_push($this->resets, $reset);
    }

    /** return a reference to the reset with the name $name, if not found, return
     * null
     *  @param string $name name of the reset to search
     *  @return Reset found reset * */
    function getReset($name)
    {
        foreach ($this->resets as $reset)
        {
            if ($reset->name == $name)
                return $reset;
        }
        return null;
    }

    /** Add a pin to the block 
     *  @param Pin $pin pin to add to the block * */
    function addPin($pin)
    {
        $pin->parentBlock = $this;
        array_push($this->pins, $pin);
    }

    /** return a reference to the pin with the name $name, if not found, return
     * false
     *  @param string $name name of the pin to search
     *  @return Pin found pin * */
    function getPin($name)
    {
        foreach ($this->pins as $pin)
        {
            if ($pin->name == $name)
                return $pin;
        }
        return null;
    }

    function addIo($ioName)
    {
        if (isset($this->xml->ios->io))
        {
            foreach ($this->xml->ios->io as $ioXml)
            {
                if ($ioName == (string) $ioXml['name'])
                {
                    $io = new IO($ioName, (string) $ioXml['driver']);
                    $this->parentNode->addBlock($io);
                }
            }
        }
    }

    function availableIosName()
    {
        $result = array();
        if (isset($this->xml->ios->io))
        {
            foreach ($this->xml->ios->io as $ioXml)
            {
                $result[] = (string) $ioXml['name'];
            }
        }
        return $result;
    }
}
