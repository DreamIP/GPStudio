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
 * In addition to simple Block, it allows the use of pins and external port to
 * declare physical interface.
 * 
 * It only contains the list of :
 *  - pins and ports as external io IO::$pins, IO::$ext_ports
 * @brief IO is the specialised implementation of Block for sensors and
 * communication blocks.
 * @see Block
 * @ingroup base
 */
class IO extends Block
{
    /**
     * @brief External pins mapping for blocks abble to comunicate with the
     * output
     * @var array|Pin $pins
     */
    public $pins;

    /**
     * @brief constructor of IO
     * 
     * Initialise all the internal members and call parse_xml if
     * $io_device_element is an SimpleXMLElement object. Else, it open the file
     * with the path $io_device_element as string or the io with the name
     * $io_device_element in library
     * @param SimpleXMLElement|string|null $io_device_element if it's different
     * of null, call the xml parser to fill members. In case of string type,
     * loads the IO in lib from the name or from the path in project.
     * @param SimpleXMLElement|null $io_node_element if it's different of null,
     * call the xml parser to fill members
     */
    function __construct($io_device_element = null, $io_node_element = null)
    {
        $this->pins = array();

        parent::__construct();
        $io_file = "";

        if (is_object($io_device_element) and get_class($io_device_element) === 'SimpleXMLElement')
        {
            $io_name = (string) $io_device_element['name'];
            $this->name = $io_name;
            $io_driver = (string) $io_device_element['driver'];

            $this->path = SUPPORT_PATH . "io" . DIRECTORY_SEPARATOR . $io_driver . DIRECTORY_SEPARATOR;
            $this->in_lib = true;
            $io_file = $this->path . $io_driver . ".io";
        }
        elseif (is_string($io_device_element))
        {
            if (substr($io_device_element, -3) === ".io")
            {
                $io_file = $io_device_element;
                $this->name = str_replace(".io", "", basename($io_file));
                $this->driver = $this->name;
                $this->path = realpath(dirname($io_file));
            }
            elseif (strpos($io_device_element, "/") === false and strpos($io_device_element, "\\") === false)
            {
                $io_name = $io_device_element;
                $io_driver = $io_node_element;
                $this->name = $io_name;
                $this->driver = $io_driver;
                $this->path = SUPPORT_PATH . "io" . DIRECTORY_SEPARATOR . $io_driver . DIRECTORY_SEPARATOR;
                $this->in_lib = true;
                $io_file = $this->path . $io_driver . ".io";
            }
            else
            {
                $io_file = $io_device_element;
                $this->path = realpath(dirname($io_file));
            }
        }

        if ($io_device_element != NULL)
        {
            if (!file_exists($io_file))
                error("File $io_file doesn't exist", 5, "IO");
            if (!($this->xml = simplexml_load_file($io_file)))
                error("Error when parsing $io_file", 5, "IO");

            $this->parse_xml($io_device_element, $io_node_element);

            if (isset($io_node_element['x_pos']) or isset($io_node_element['y_pos']))
            {
                if (count($this->parts)>0)
                    $part = $this->parts[0];
                else
                {
                    $part = new ComponentPart();
                    $part->name = "main";
                    $this->addPart($part);
                }
                $part->x_pos = (int) $io_node_element['x_pos'];
                $part->y_pos = (int) $io_node_element['y_pos'];
            }
        }

        if ($io_file != "" and $this->getFile(basename($io_file)) == NULL)
        {
            $file_config = new File();
            $file_config->name = basename($io_file);
            $file_config->path = getRelativePath($io_file, $this->path);
            $file_config->type = "io";
            $file_config->group = "blockdef";
            $this->addFile($file_config);
        }
    }

    /**
     * @brief internal function to fill this instance from input xml structure
     * @param SimpleXMLElement $io_device_element element from io in lib
     * @param SimpleXMLElement $io_node_element element from the node
     */
    protected function parse_xml($io_device_element=NULL, $io_node_element=NULL)
    {
        parent::parse_xml();
        $this->driver = (string) $this->xml['driver'];
        $this->master_count = (int) $this->xml['master_count'];

        // pins assignement
        if (isset($io_device_element->pins))
        {
            foreach ($io_device_element->pins->pin as $pin)
            {
                $this->addPin(new Pin($pin));
            }
        }
    }

    /**
     * @brief Returns the type of the block as string, redefined by children.
     * @return string type of the block.
     */
    public function type()
    {
        return 'io';
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

        if ($format == "complete" or $format == "blockdef")
        {
            // pins
            $xml_pins = $xml->createElement("pins");
            foreach ($this->pins as $pin)
            {
                $xml_pins->appendChild($pin->getXmlElement($xml, $format));
            }
            $xml_element->appendChild($xml_pins);
        }

        return $xml_element;
    }

    /**
     * @brief Add a pin to the block 
     * @param Pin $pin pin to add to the block
     */
    function addPin($pin)
    {
        $pin->parentBlock = $this;
        array_push($this->pins, $pin);
    }

    /**
     * @brief return a reference to the pin with the name $name, if not found,
     * return null
     * @param string $name name of the pin to search
     * @param bool $casesens take care or not of the case of the name
     * @return Pin found pin
     */
    function getPin($name, $casesens = true)
    {
        if ($casesens)
        {
            foreach ($this->pins as $pin)
            {
                if ($pin->name == $name)
                    return $pin;
            }
        }
        else
        {
            foreach ($this->pins as $pin)
            {
                if (strcasecmp($pin->name, $name) == 0)
                    return $pin;
            }
        }
        return null;
    }
}
