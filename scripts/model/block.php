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

require_once("component.php");
require_once("property.php");

/**
 * Block is a specialisation of component. 
 * 
 * In addition of attribute of component, it contains bus interfaces for PI
 * slave and master Block::$interfaces, addr and master count.
 * 
 * @brief Block is the base block definition for all blocks (IO, IOCom and process).
 * @see Component IO Process
 * @ingroup base
 */
class Block extends Component
{
    /**
     * @brief Specify if the block is defined in the library or not
     * @var bool $in_lib
     */
    public $in_lib;

    /**
     * @brief Specify the name of the driver for the block
     * @var string $driver
     */
    public $driver;

    /**
     * @brief The absolute adress of the block on BI
     * @var int $addr_abs
     */
    public $addr_abs;

    /**
     * @brief Size of relative adress bus
     * @var int $pi_size_addr_rel
     */
    public $pi_size_addr_rel;

    /**
     * @brief Number of master in the block on BI
     * @var int $master_count
     */
    public $master_count;

    /**
     * @brief Specify the external file script to configure the block (optional)
     * @var string $configscriptfile
     */
    public $configscriptfile;

    /**
     * @brief Specify the external file script to generate the block (optional)
     * @var string $generatescriptfile
     */
    public $generatescriptfile;

    /**
     * @brief Description of the block (optional)
     * @var string $desc
     */
    public $desc;

    /**
     * @brief Array of property class specify the high level properties
     * @var array|Property $properties
     */
    public $properties;

    /**
     * @brief Reference to the associated parent node
     * @var Node $parentNode
     */
    public $parentNode;

    /**
     * @brief Constructor of the class
     * 
     * Build an empty Block with the default clock, 'clk_proc'.
     */
    function __construct()
    {
        parent::__construct();
        
        $this->properties = array();

        $this->addr_abs = -1;
        $this->master_count = 0;
        $this->configscriptfile = '';
        $this->generatescriptfile = '';

        $this->in_lib = false;

        $clock = new Clock();
        $clock->name = 'clk_proc';
        $clock->domain = 'clk_proc';
        $this->addClock($clock);

        /* $reset = new Reset();
          $reset->name='reset';
          $reset->group='reset';
          $this->addReset($reset); */
    }

    /**
     * @brief Method to print a box corresponding to this block with all flow
     * interface.
     * 
     * Ex :
     * -------------
     * |    mt9    |  out (8)
     * |           |---------->
     * -------------
     */
    function print_flow()
    {
        // count flow in and out
        $flowIn = array();
        $maxInLenght = 0;
        $flowOut = array();
        $maxOutLenght = 0;
        foreach ($this->flows as $flow)
        {
            if ($flow->type == "in")
            {
                $flowIn[] = $flow;
                $maxInLenght = max($maxInLenght, strlen($flow->name . $flow->size) + 3);
            }
            if ($flow->type == "out")
            {
                $flowOut[] = $flow;
                $maxOutLenght = max($maxOutLenght, strlen($flow->name . $flow->size) + 3);
            }
        }
        $lenghtBlock = strlen($this->name) + 8;
        $maxInLenght+=4;
        $maxOutLenght+=4;

        echo str_repeat(' ', $maxInLenght) . str_repeat('-', $lenghtBlock + 2) . str_repeat(' ', $maxOutLenght) . "\n";
        $maxFlow = max(count($flowIn), count($flowOut));
        if ($maxFlow == 0)
            $maxFlow = 1;
        for ($i = 0; $i < $maxFlow; $i++)
        {
            // first in line
            if ($i < count($flowIn))
                echo str_pad($flowIn[$i]->name . ' (' . $flowIn[$i]->size . ')', $maxInLenght, ' ', STR_PAD_BOTH);
            else
                echo str_repeat(' ', $maxInLenght);

            // block 1
            echo '|';
            if ($i == floor($maxFlow / 2))
                echo str_pad($this->name, $lenghtBlock, ' ', STR_PAD_BOTH);
            else
                echo str_repeat(' ', $lenghtBlock);
            echo '|';

            // first out line
            if ($i < count($flowOut))
                echo str_pad($flowOut[$i]->name . ' (' . $flowOut[$i]->size . ')', $maxOutLenght, ' ', STR_PAD_BOTH);
            else
                echo str_repeat(' ', $maxOutLenght);
            echo "\n";

            // second in line
            if ($i < count($flowIn))
                echo str_repeat('-', $maxInLenght - 1) . '>';
            else
                echo str_repeat(' ', $maxInLenght);

            // block 2
            echo '|';
            echo str_repeat(' ', $lenghtBlock);
            echo '|';

            // second out line
            if ($i < count($flowOut))
                echo str_repeat('-', $maxOutLenght - 1) . '>';
            else
                echo str_repeat(' ', $maxOutLenght);
            echo "\n";
        }
        echo str_repeat(' ', $maxInLenght) . str_repeat('-', $lenghtBlock + 2) . str_repeat(' ', $maxOutLenght) . "\n";
    }

    /**
     * @brief Call the configure script if exist
     * @param Node $node node container
     * @param Block $block current block
     * @see $this->configscriptfile
     * @see configscriptfile
     */
    function configure($node, $block)
    {
        if (!empty($this->configscriptfile))
        {
            if (file_exists($this->path . $this->configscriptfile))
            {
                $script = str_replace(SUPPORT_PATH, '', $this->path . $this->configscriptfile);
                $configureBlock = (include $script);
                if ($configureBlock !== FALSE)
                {
                    $configureBlock($node, $block);
                }
            }
        }
    }

    /**
     * @brief Call the generate script if exist
     * @param Node $node node container
     * @param Block $block current block
     * @param string $path path of the generated IP
     * @param string $language language of IP
     * @see $generatescriptfile
     */
    function generate($node, $block, $path, $language)
    {
        if (!empty($this->generatescriptfile))
        {
            if (file_exists($this->path . $this->generatescriptfile))
            {
                $script = str_replace(SUPPORT_PATH, '', $this->path . $this->generatescriptfile);
                $generateBlock = (include $script);
                if ($generateBlock !== FALSE)
                {
                    $generateBlock($node, $block, $path, $language);
                }
            }
        }
    }

    /** @brief return a reference to the bitfield with the path $path, if not
     * found, return null
     *  @param string $path path of the parambitfield to search (param.parambitfield)
     *  @param bool $casesens take care or not of the case of the name
     *  @return ParamBitfield found bitfield 
     */
    function getParamBitField($path, $casesens = true)
    {
        $subPath = explode('.', $path);
        if (count($subPath) != 2)
            return NULL;
        $param = $this->getParam($subPath[0], $casesens);
        if ($param == NULL)
            return NULL;
        $paramBitField = $param->getParambitfield($subPath[1], $casesens);
        return $paramBitField;
    }

    /** @brief return a reference to the bitfield with the path $path, if not
     * found, return null
     *  @param string $path path of the parambitfield to search (param.parambitfield)
     *  @return ParamBitfield found bitfield
     */
    function delParamBitField($path)
    {
        $subPath = explode('.', $path);
        if (count($subPath) != 2)
            return NULL;
        $param = $this->getParam($subPath[0]);
        if ($param == NULL)
            return NULL;
        return $param->delParambitfield($subPath[1]);
    }

    /** @brief Add a property to the block 
     *  @param Property $property property to add to the block
     */
    function addProperty($property)
    {
        $property->parentBlock = $this;
        array_push($this->properties, $property);
    }

    /** @brief return a reference to the property with the name $name, if not
     * found, return null
     *  @param string $name name of the property to search
     *  @param bool $casesens take care or not of the case of the name
     *  @return Property found property
     */
    function getProperty($name, $casesens = true)
    {
        if ($casesens)
        {
            foreach ($this->properties as $property)
            {
                if ($property->name == $name)
                    return $property;
            }
        }
        else
        {
            foreach ($this->properties as $property)
            {
                if (strcasecmp($property->name, $name) == 0)
                    return $property;
            }
        }
        return null;
    }

    /** @brief alias to getProperty($name, $casesens)
     *  @param string $name name of the property enum to search
     *  @param bool $casesens take care or not of the case of the name
     *  @return Property found property
     */
    function getSubProperty($name, $casesens = true)
    {
        return $this->getProperty($name, $casesens);
    }

    /** @brief return a reference to the property with the access path $path,
     * if not found, return null
     *  @param string $path path of the property to search, separed by . (dot)
     *  @param bool $casesens take care or not of the case of the name
     *  @return Property found property
     */
    function getPropertyPath($path, $casesens = true)
    {
        $subprops = explode('.', $path);
        if (count($subprops) == 0)
            return null;

        $parent = $this;
        $i = 0;
        if (($instance = $this->getFlow($subprops[0], $casesens)) != NULL)
        {
            $parent = $instance;
            $i++;
        }
        for (; $i < count($subprops); $i++)
        {
            $parent = $parent->getSubProperty($subprops[$i], $casesens);
            if ($parent == null)
                return null;
        }
        return $parent;
    }

    /** @brief delete a property from his name
     *  @param string $name name of the property to delete
     */
    function delProperty($name)
    {
        $i = 0;
        foreach ($this->properties as $property)
        {
            if ($property->name == $name)
            {
                unset($this->properties[$i]);
                return;
            }
            $i++;
        }
        return null;
    }

    /** @brief delete a property from his path
     *  @param string $path path of the property to delete
     */
    function delPropertyPath($path)
    {
        $subprops = explode('.', $path);
        if (count($subprops) == 0)
            return;

        $parent = $this;
        $i = 0;
        if (($instance = $this->getFlow($subprops[0])) != NULL)
        {
            $parent = $instance;
            $i++;
        }
        for (; $i < count($subprops) - 1; $i++)
        {
            $parent = $parent->getSubProperty($subprops[$i]);
            if ($parent == null)
                return;
        }
        $parent->delProperty($subprops[count($subprops) - 1]);
    }

    /** @brief return a reference to the property with the access path $path,
     * if not found, return null
     *  @param string $path path of the property to search, separed by . (dot)
     *  @param bool $casesens take care or not of the case of the name
     *  @return Property found property
     */
    function getPropertyEnumPath($path, $casesens = true)
    {
        $subprops = explode('.', $path);
        if (count($subprops) == 0)
            return NULL;

        $name = $subprops[count($subprops) - 1];
        $propertyPath = array_slice($subprops, 0, count($subprops) - 1);
        $propertyPath = implode('.', $propertyPath);
        $property = $this->getPropertyPath($propertyPath, $casesens);
        if ($property == NULL)
            return NULL;

        return $property->getPropertyEnum($name, $casesens);
    }

    /** @brief delete a property enum from his path
     *  @param string $path path of the property to delete
     */
    function delPropertyEnumPath($path)
    {
        $subprops = explode('.', $path);
        if (count($subprops) == 0)
            return;

        $name = $subprops[count($subprops) - 1];
        $propertyPath = array_slice($subprops, 0, count($subprops) - 1);
        $propertyPath = implode('.', $propertyPath);
        $property = $this->getPropertyPath($propertyPath);
        if ($property == NULL)
            return;

        $property->delPropertyEnum($name);
    }

    /**
     * @brief internal function to fill this instance from input xml structure
     * 
     * SimpleXMLElement $xml xml element to parse need to be specified in
     * members before to call this function
     */
    protected function parse_xml($dummy=NULL, $dummy2=NULL)
    {
        parent::parse_xml();

        if (isset($this->xml['size_addr_rel']))
            warning("Please update your process or device with the new conventionnal name 'pi_size_addr_rel' instead of 'size_addr_rel'", 12, "Block");
        $this->pi_size_addr_rel = (int) $this->xml['pi_size_addr_rel'];
        $this->configscriptfile = (string) $this->xml['configscriptfile'];
        $this->generatescriptfile = (string) $this->xml['generatescriptfile'];

        // properties
        if (isset($this->xml->properties))
        {
            foreach ($this->xml->properties->property as $propertyXml)
            {
                $this->addProperty(new Property($propertyXml));
            }
        }
    }

    /**
     * @brief Returns the type of the block as string, redefined by children.
     * @return string type of the block.
     */
    public function type()
    {
        return 'block';
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
        if ($this->type() == "io" or $this->type() == "iocom")
            $typeName = "io";
        elseif ($this->type() == "process")
            $typeName = "process";
        else
            $typeName = "";

        if ($format == "project" or $format == "blockdef")
        {
            if ($typeName != "")
                $xml_element = $xml->createElement($typeName);
        }
        else
        {
            $xml_element = $xml->createElement("block");
        }

        // name or driver
        if ($typeName == 'io' and $format == "blockdef")
        {
            $att = $xml->createAttribute('driver');
            $att->value = $this->driver;
            $xml_element->appendChild($att);
        }
        else
        {
            $att = $xml->createAttribute('name');
            $att->value = $this->name;
            $xml_element->appendChild($att);
        }

        // type
        if ($format == "complete" or $format == "blockdef")
        {
            $att = $xml->createAttribute('categ');
            $att->value = $this->categ;
            $xml_element->appendChild($att);
        }

        // in_lib
        if ($this->type() == "process" and $format != "blockdef")
        {
            $att = $xml->createAttribute('inlib');
            $att->value = $this->in_lib == 1 ? "true" : "false";
            $xml_element->appendChild($att);

            if ($this->in_lib != 1)
            {
                $att = $xml->createAttribute('path');
                $att->value = getRelativePath($this->path);
                $xml_element->appendChild($att);
            }
        }

        if ($format != "blockdef")
        {
            // driver
            $att = $xml->createAttribute('driver');
            $att->value = $this->driver;
            $xml_element->appendChild($att);
        }

        // pi information & type
        if ($format == "complete")
        {
            // type
            $att = $xml->createAttribute('type');
            $att->value = $this->type();
            $xml_element->appendChild($att);

            // addr_abs
            $att = $xml->createAttribute('addr_abs');
            $att->value = $this->addr_abs;
            $xml_element->appendChild($att);
        }

        if ($format == "complete" or $format == "blockdef")
        {
            if (!empty($this->master_count) and $this->master_count != "0")
            {
                // master_count
                $att = $xml->createAttribute('master_count');
                $att->value = $this->master_count;
                $xml_element->appendChild($att);
            }

            // pi_size_addr_rel
            $att = $xml->createAttribute('pi_size_addr_rel');
            $att->value = $this->pi_size_addr_rel;
            $xml_element->appendChild($att);

            // desc
            $att = $xml->createAttribute('desc');
            $att->value = $this->desc;
            $xml_element->appendChild($att);
            
            // configscriptfile
            if (!empty($this->configscriptfile))
            {
                $att = $xml->createAttribute('configscriptfile');
                $att->value = $this->configscriptfile;
                $xml_element->appendChild($att);
            }
            
            // generatescriptfile
            if (!empty($this->generatescriptfile))
            {
                $att = $xml->createAttribute('generatescriptfile');
                $att->value = $this->generatescriptfile;
                $xml_element->appendChild($att);
            }
        }
        
        // parts
        if (!empty($this->parts))
        {
            $xml_parts = $xml->createElement("parts");
            foreach ($this->parts as $part)
            {
                $xml_parts->appendChild($part->getXmlElement($xml, $format));
            }
            $xml_element->appendChild($xml_parts);
        }

        if ($format == "complete" or $format == "blockdef")
        {
            // infos
            if (!empty($this->infos))
            {
                $xml_infos = $xml->createElement("infos");
                foreach ($this->infos as $info)
                {
                    $xml_infos->appendChild($info->getXmlElement($xml, $format));
                }
                $xml_element->appendChild($xml_infos);
            }

            // files
            if (!empty($this->files))
            {
                $xml_files = $xml->createElement("files");
                foreach ($this->files as $file)
                {
                    $xml_files->appendChild($file->getXmlElement($xml, $format));
                }
                $xml_element->appendChild($xml_files);
            }

            // resets
            if (!empty($this->resets))
            {
                $xml_resets = $xml->createElement("resets");
                foreach ($this->resets as $reset)
                {
                    $xml_resets->appendChild($reset->getXmlElement($xml, $format));
                }
                $xml_element->appendChild($xml_resets);
            }

            // attributes
            if (!empty($this->attributes))
            {
                $xml_attributes = $xml->createElement("attributes");
                foreach ($this->attributes as $attribute)
                {
                    $xml_attributes->appendChild($attribute->getXmlElement($xml, $format));
                }
                $xml_element->appendChild($xml_attributes);
            }
            
            // ports
            if (!empty($this->ext_ports))
            {
                $xml_ports = $xml->createElement("ports");
                foreach ($this->ext_ports as $port)
                {
                    $xml_ports->appendChild($port->getXmlElement($xml, $format));
                }
                $xml_element->appendChild($xml_ports);
            }
        }

        // components
        if (!empty($this->components))
        {
            $xml_components = $xml->createElement("components");
            foreach ($this->components as $component)
            {
                $xml_components->appendChild($component->getXmlElement($xml, $format));
            }
            $xml_element->appendChild($xml_components);
        }

        // flows
        if (!empty($this->flows))
        {
            $xml_flows = $xml->createElement("flows");
            foreach ($this->flows as $flow)
            {
                if ($flow->type == "in" or $flow->type == "out")
                {
                    $xml_flows->appendChild($flow->getXmlElement($xml, $format));
                }
            }
            $xml_element->appendChild($xml_flows);
        }

        // params
        $count = 0;
        $xml_params = $xml->createElement("params");
        foreach ($this->params as $param)
        {
            if ($format == "complete" or $format == "blockdef")
            {
                $xml_params->appendChild($param->getXmlElement($xml, $format));
                $count++;
            }
            else
            {
                if ($param->hard == 1 and $param->name != "MASTER_ADDR_WIDTH")
                {
                    $xml_params->appendChild($param->getXmlElement($xml, $format));
                    $count++;
                }
            }
        }
        if ($count > 0)
            $xml_element->appendChild($xml_params);

        // properties
        $count = 0;
        $xml_property = $xml->createElement("properties");
        foreach ($this->properties as $property)
        {
            $xml_property->appendChild($property->getXmlElement($xml, $format));
            $count++;
        }
        if ($count > 0)
            $xml_element->appendChild($xml_property);

        // clocks
        $count = 0;
        $xml_clocks = $xml->createElement("clocks");
        foreach ($this->clocks as $clock)
        {
            $export = true;

            if ($format == "project")
            {
                if ($clock->direction == "out" or $clock->domain != "")
                    $export = false;
            }
            elseif ($format == "blockdef")
            {
                if ($clock->name == "clk_proc")
                    $export = false;
            }

            if ($export)
            {
                $xml_clocks->appendChild($clock->getXmlElement($xml, $format));
                $count++;
            }
        }
        if ($count > 0)
            $xml_element->appendChild($xml_clocks);

        return $xml_element;
    }

    /**
     * @brief Helper function that save an '.io' or '.proc' file.
     * @param string $file file name to save
     */
    function saveBlockDef($file)
    {
        $xml = new DOMDocument("1.0", "UTF-8");
        $xml->preserveWhiteSpace = false;
        $xml->formatOutput = true;

        $xml->appendChild($this->getXmlElement($xml, "blockdef"));

        $xml->save($file);
    }

    /**
     * @brief Redefines all the contained properties to the global context by
     * adding the name of the block at the begining of all of properties map
     */
    function toGlobalPropertyPath()
    {
        foreach ($this->properties as $property)
        {
            $property->toGlobalPropertyPath($this->name);
        }
        foreach ($this->params as $param)
        {
            $param->toGlobalPropertyPath($this->name);
        }
        foreach ($this->flows as $flow)
        {
            foreach ($flow->properties as $property)
            {
                $property->toGlobalPropertyPath($this->name);
            }
        }
    }
}
