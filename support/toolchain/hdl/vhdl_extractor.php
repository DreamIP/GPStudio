<?php
/*
 * Copyright (C) 2016 Dream IP
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

require_once 'model/component.php';

class VHDLPart
{
    public $type;
    public $name;
    public $content;
    public $line;
    public $offset;

    public $parts;

    function __construct($type, $name, $content, $line, $offset)
    {
        $this->type = $type;
        $this->name = $name;
        $this->content = $content;
        $this->line = $line;
        $this->offset = $offset;

        $this->parts = array();
    }
}

class VHDL_extractor
{
    public $file;
    public $name;
    public $fileContent;

    public $signals;
    public $ports;
    public $params;

    public $entity;

    function __construct($file = "")
    {
        $this->file = $file;

        $this->signals = array();
        $this->ports = array();
        $this->params = array();
        $this->parts = array();

        if(!empty($file))
            $this->parse($file);
    }

    function parse($file)
    {
        $this->fileContent = str_replace("\r\n", "\n", file_get_contents($file));

        if (!empty($this->fileContent))
            $this->parseEntity();
    }

    function parseEntity()
    {
        // search start entity and catch entity name
        $matches = array();
        if(preg_match("/.*entity[[:space:]]+([A-Za-z_][A-Za-z_\\-0-9]+)[[:space:]]+is.*/", $this->fileContent, $matches, PREG_OFFSET_CAPTURE)===FALSE)
            return false;
        $this->name = $matches[1][0];
        $beginOffset = $matches[0][1];

        // search end entity
        if(preg_match("/.*end[[:space:]]+(".$this->name."|entity);/", $this->fileContent, $matches, PREG_OFFSET_CAPTURE, $beginOffset)===FALSE)
            return false;

        $endOffset = $matches[0][1] + strlen($matches[0][0]);
        $entityContent = substr($this->fileContent, $beginOffset, $endOffset - $beginOffset);
        $this->entity = new VHDLPart('entity', $this->name, $entityContent, $this->offset2line($beginOffset), $beginOffset);

        // parse generic content
        if(preg_match("/generic[[:space:]]*\\(/", $this->fileContent, $matches, PREG_OFFSET_CAPTURE, $beginOffset)!==FALSE)
        {
            $genericOffset = $matches[0][1] + strlen($matches[0][0]) - 1;
            $genericEndOffset = $this->nextParenthesis($genericOffset);
            $genericContent = trim(substr($this->fileContent, $genericOffset + 1, $genericEndOffset - $genericOffset - 2));
            $generic = new VHDLPart('generic', 'generic', $genericContent, $this->offset2line($genericOffset), $genericOffset);
            $this->parts['generic'] = $generic;
            $this->parseDeclare($generic);
        }

        // parse ports content
        if(preg_match("/port[[:space:]]*\\(/", $this->fileContent, $matches, PREG_OFFSET_CAPTURE, $beginOffset)!==FALSE)
        {
            $portOffset = $matches[0][1] + strlen($matches[0][0]) - 1;
            $portEndOffset = $this->nextParenthesis($portOffset);
            $portContent = trim(substr($this->fileContent, $portOffset + 1, $portEndOffset - $portOffset - 2));
            $port = new VHDLPart('port', 'port', $portContent, $this->offset2line($portOffset), $portOffset);
            $this->parts['port'] = $port;
            $this->parseDeclare($port);
        }
    }

    function parseDeclare($part)
    {
        preg_match_all("/([a-zA-Z\\_][a-zA-Z\\_0-9\\-]*)[[:space:]]*:[[:space:]]*([^;]+)[[:space:]]*;*\\)*/", $part->content, $matches, PREG_OFFSET_CAPTURE, 0);
        for($i=0; $i<count($matches[0]); $i++)
        {
            $name = $matches[1][$i][0];
            $content = $matches[0][$i][0];
            $offset = $matches[0][$i][1] + $part->offset;
            $subpart = new VHDLPart($part->type, $name, $content, $this->offset2line($offset), $offset);
            $part->parts[$name] = $subpart;
        }
    }

    function offset2line($offset)
    {
        return substr_count($this->fileContent, "\n", 0, $offset) + 1;
    }
    
    function nextParenthesis($offset)
    {
        $open = 0;
        $strlen = strlen( $this->fileContent );
        for( $i = $offset; $i <= $strlen; $i++ )
        {
            $char = substr( $this->fileContent, $i, 1 );
            if($char == '(')
                $open++;
            if($char == ')')
            {
                $open--;
                if($open==0)
                    return $i+1;
            }
        }
        return NULL;
    }
    
    function toComponent($tool)
    {
        if ($tool == "gpcomp")
            $component = new Component();
        elseif ($tool == "gpproc")
            $component = new Process();
        elseif ($tool == "gpdevice")
            $component = new IO();

        $component->name = $this->name;
        
        // param extractor
        if (array_key_exists('generic', $this->parts))
        {
            foreach ($this->parts['generic']->parts as $k => $part)
            {
                if (preg_match("/:[[:space:]]*([^\:]+)[[:space:]]*:=[[:space:]]*([^\;]+)[[:space:]]*;*/", $part->content, $matches, PREG_OFFSET_CAPTURE) === FALSE)
                    continue;
                if (empty($matches))
                {
                    if (preg_match("/:[[:space:]]*([^\;]+)[[:space:]]*;*/", $part->content, $matches, PREG_OFFSET_CAPTURE) === FALSE)
                        continue;
                }
                $type = $matches[1][0];
                if (count($matches) > 2)
                    $value = str_replace("\"", "", $matches[2][0]);
                else
                    $value = "";

                $param = new Param();
                $param->hard = true;
                $param->name = $part->name;
                $param->type = trim($type);
                $param->value = trim($value);
                $component->addParam($param);
            }
        }

        if (!array_key_exists('port', $this->parts))
            return $component;

        // flow extractor
        $flows = array();
        $portPart = $this->parts['port']->parts;
        foreach ($portPart as $k => $part)
        {
            if(substr($part->name, -3) == "_fv"
                or substr($part->name, -3) == "_dv"
                or substr($part->name, -5) == "_data")
            {
                $flowPart = explode("_", $part->name);
                $flowName = implode("_", array_slice($flowPart, 0, count($flowPart)-1));
                
                if(!array_key_exists($flowName, $flows))
                {
                    $flows[$flowName] = array();
                }
                $flows[$flowName][$flowPart[count($flowPart)-1]] = $part;
                
                unset($portPart[$k]);
            }
        }
        foreach ($flows as $flowName => $flowPart)
        {
            if(count($flowPart)!=3)
            {
                foreach ($flowPart as $part)
                    $portPart[$part->name] = $part;
                continue;
            }

            if(preg_match("/:[[:space:]]*(in|out)/", $flowPart['fv']->content, $matches, PREG_OFFSET_CAPTURE)===FALSE)
                return false;
            $dir = $matches[1][0];
            if(preg_match("/\\([[:space:]]*(.*)[[:space:]]+downto[[:space:]]+0[[:space:]]*\\)/", $flowPart['data']->content, $matches, PREG_OFFSET_CAPTURE)===FALSE)
                return false;
            $size = $matches[1][0];
            $size = preg_replace("/[[:space:]\\(\\)]*/", "", $size); // remove all spaces
            if (strpos($size, "-1") !== FALSE)
            {
                $size = str_replace("-1", "", $size);
            }
            else
            {
                $size = (int) $size + 1;
            }

            if (!is_int($size) and $component->getParam($size) == NULL)
            {
                $param = new Param();
                $param->hard = true;
                $param->name = $size;
                $param->type = "integer";
                $param->value = 8;
                $component->addParam($param);
            }

            $flow = $component->getFlow($flowName);
            if($flow == NULL)
            {
                $flow = new Flow();
                $flow->name = $flowName;
                $flow->type = $dir;
                $flow->size = $size;
                $component->addFlow($flow);
            }
        }

        // resets
        foreach ($portPart as $k => $part)
        {
            if(preg_match("/([A-Za-z_\\-0-9]*(rst|reset)[A-Za-z_\\-0-9]*)[[:space:]]*:[[:space:]]*(in|out)[[:space:]]*std_logic[[:space:];\\)]+/", $part->content, $matches, PREG_OFFSET_CAPTURE)===FALSE)
                continue;
            if(empty($matches))
                continue;
            unset($portPart[$k]);
            $reset = new Reset();
            $reset->name = $part->name;
            $reset->group = "reset_n";
            $reset->direction = $matches[3][0];
            $component->addReset($reset);
        }

        // clocks
        foreach ($portPart as $k => $part)
        {
            if(preg_match("/([A-Za-z_\\-0-9]*(clk|clock)[A-Za-z_\\-0-9]*)[[:space:]]*:[[:space:]]*(in|out)[[:space:]]*std_logic[[:space:];\\)]+/", $part->content, $matches, PREG_OFFSET_CAPTURE)===FALSE)
                continue;
            if(empty($matches))
                continue;
            unset($portPart[$k]);
            $clock = new Clock();
            $clock->name = $part->name;
            $clock->domain = "clk_proc";
            $clock->direction = $matches[3][0];
            $component->addClock($clock);
        }

        // ports
        foreach ($portPart as $k => $part)
        {
            if(preg_match("/:[[:space:]]*(in|out)[[:space:]]*(std_logic[^;]*)[[:space:];\\)]+/", $part->content, $matches, PREG_OFFSET_CAPTURE)===FALSE)
                continue;
            if(empty($matches))
                continue;
            $port = new Port();
            $port->name = $part->name;
            $port->type = trim($matches[1][0]);
            
            $typeport = trim($matches[2][0]);
            if($typeport == "std_logic")
                $port->size = 1;
            else
            {
                if(preg_match("/\\([[:space:]]*(.*)[[:space:]]+downto[[:space:]]+0[[:space:]]*\\)/", $part->content, $matches, PREG_OFFSET_CAPTURE)===FALSE)
                    return false;
                $size = $matches[1][0];
                $size = preg_replace("/[[:space:]\\(\\)]*/", "", $size); // remove all spaces
                if (strpos($size, "-1") !== FALSE)
                {
                    $size = str_replace("-1", "", $size);
                }
                else
                {
                    $size = (int)$size+1;
                }
                $port->size = $size;


                if (!is_int($size) and $component->getParam($size) == NULL)
                {
                    $param = new Param();
                    $param->hard = true;
                    $param->name = $size;
                    $param->type = "integer";
                    $param->value = 8;
                    $component->addParam($param);
                }
            }
            $component->addExtPort($port);
        }

        return $component;
    }
}
