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

require_once("block.php");

/**
 * Compared to Block, this class does not contain additionnal members. It just
 * contains library parser and helper for process.
 * @brief Process is the specialised implementation of Block for processes.
 * @see Block
 * @ingroup base
 */
class Process extends Block
{

    /**
     * @brief constructor of Process
     * 
     * Initialise all the internal members and call parse_xml if
     * $process_node_element is an SimpleXMLElement object. Else, it open the
     * file with the path $process_node_element as string or the process with
     * the name $process_node_element in library
     * @param SimpleXMLElement|string|null $process_node_element if it's
     * different of null, call the xml parser to fill members In case of string
     * type, loads the process in lib from the name or from the path in project.
     */
    function __construct($process_node_element = NULL)
    {
        parent::__construct();

        $process_file = "";

        if (is_object($process_node_element) and get_class($process_node_element) === 'SimpleXMLElement')
        {
            $inlib = false;

            if (isset($process_node_element['inlib']))
            {
                if ($process_node_element['inlib'] == 'false')
                    $inlib = false;
                else
                    $inlib = true;
            }
            if (isset($process_node_element['driver']))
                $this->driver = (string) $process_node_element['driver'];
            else
                $this->driver = (string) $process_node_element['name'];

            if (!isset($process_node_element['name']))
                $this->name = $this->driver;
            else
                $this->name = (string) $process_node_element['name'];

            $this->in_lib = $inlib;
            if ($this->in_lib)
            {
                // process defined in library
                if (!isset($this->driver))
                    $this->driver = $this->name;

                $this->path = SUPPORT_PATH . "process" . DIRECTORY_SEPARATOR . $this->driver . DIRECTORY_SEPARATOR;
                $process_file = $this->path . $this->driver . '.proc';

                if (!file_exists($process_file))
                    error("File $process_file doesn't exist", 5, "Process");
                if (!($this->xml = simplexml_load_file($process_file)))
                    error("Error when parsing $process_file", 5, "Process");

                $this->parse_xml($this->xml);
            }
            else
            {
                if (!isset($process_node_element['files']))
                {
                    // process defined in external .proc local to the projet
                    if (isset($process_node_element['path']))
                    {
                        $this->path = getcwd() . DIRECTORY_SEPARATOR . (string) $process_node_element['path'] . DIRECTORY_SEPARATOR;
                    }
                    else
                    {
                        $this->path = getcwd() . DIRECTORY_SEPARATOR;
                    }

                    $process_file = $this->path . str_replace(".proc", "", $this->driver) . '.proc';

                    if (!file_exists($process_file))
                        error("File $process_file doesn't exist", 5, "Process");
                    if (!($this->xml = simplexml_load_file($process_file)))
                        error("Error when parsing $process_file", 5, "Process");

                    $this->parse_xml($this->xml);
                }
                else
                {
                    // process defined inside .node
                    $this->parse_xml($process_node_element);
                }
            }

            if (isset($process_node_element['x_pos']) or isset($process_node_element['y_pos']))
            {
                if (count($this->parts)>0)
                    $part = $this->parts[0];
                else
                {
                    $part = new ComponentPart();
                    $part->name = "main";
                    $this->addPart($part);
                }
                $part->x_pos = (int) $process_node_element['x_pos'];
                $part->y_pos = (int) $process_node_element['y_pos'];
            }
        }
        elseif (is_string($process_node_element))
        {
            if (strpos($process_node_element, "/") === false and strpos($process_node_element, "\\") === false and strpos($process_node_element, ".proc") === false)
            {
                $this->driver = $process_node_element;
                $this->path = SUPPORT_PATH . "process" . DIRECTORY_SEPARATOR . $this->driver . DIRECTORY_SEPARATOR;
                $process_file = $this->path . $this->driver . '.proc';
                $this->in_lib = true;
            }
            else
            {
                $process_file = $process_node_element;
                $this->driver = basename($process_node_element);
                $this->path = getRelativePath(dirname($process_node_element));
                $this->in_lib = false;
                $this->name = str_replace(".proc", "", basename($process_node_element));
            }

            if (!file_exists($process_file))
                error("File $process_file doesn't exist", 5, "Process");
            if (!($this->xml = simplexml_load_file($process_file)))
                error("Error when parsing $process_file", 5, "Process");

            $this->parse_xml($this->xml);
            $this->path = realpath(dirname($process_file));
        }
        else
        {
            // nothing to do
        }

        if ($process_file != "" and $this->getFile(basename($process_file)) == NULL)
        {
            $file_config = new File();
            $file_config->name = basename($process_file);
            $file_config->path = getRelativePath($process_file, $this->path);
            $file_config->type = "proc";
            $file_config->group = "blockdef";
            $this->addFile($file_config);
        }
    }

    /**
     * @brief internal function to fill this instance from input xml structure
     * @param SimpleXMLElement $process_node_element element from io in lib
     * @param NULL $dummy dummy parameter to ensuze PHP7 object parent compatibility
     */
    protected function parse_xml($process_node_element=NULL, $dummy=NULL)
    {
        $this->xml = $process_node_element;
        parent::parse_xml();
    }

    /**
     * @brief Returns the type of the block as string, redefined by children.
     * @return string type of the block.
     */
    public function type()
    {
        return 'process';
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
        $xml_element = parent::getXmlElement($xml, $format);

        return $xml_element;
    }
}
