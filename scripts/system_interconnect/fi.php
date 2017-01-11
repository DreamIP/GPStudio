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
require_once("flowconnect.php");

require_once("toolchain/hdl/vhdl_generator.php");

require_once("treeconnect.php");

class FlowInterconnect extends Block
{
    /**
     * Map of computed connections, indexed by a key (blockName.flowName) with a TreeConnect object intance as value which represent the tree of flow connection
     * @var array|TreeConnect $tree_connects
     */
    public $tree_connects;

    /**
     * Flow connections between blocks
     * @var array|FlowConnect $flow_connects
     */
    public $flow_connects;

    function __construct()
    {
        parent::__construct();
        $this->name = "fi";
        $this->driver = "fi";

        $this->flow_connects = array();
    }

    static function showConnect($connect)
    {
        $str = "     in : <connect fromblock=\"$connect->fromblock\" fromflow=\"$connect->fromflow\" toblock=\"$connect->toblock\" toflow=\"$connect->toflow\"/>\n";
        return $str;
    }

    static function showFlows($block, $dir)
    {
        $str = "     Available flows $dir for $block->name : ";
        $count = 0;
        foreach ($block->flows as $flow)
        {
            if ($flow->type == $dir)
            {
                $str .= "$flow->name, ";
                $count++;
            }
        }
        if ($count == 0)
            $str .= "\tNo flow $dir available";
        return $str;
    }

    static function showBlocks($node)
    {
        $str = "     Available blocks : ";
        foreach ($node->blocks as $block)
        {
            if ($block->name != "fi" and $block->name != "pi" and $block->name != "ci" and $block->name != "ri")
            {
                $str .= "$block->name, ";
            }
        }
        return $str;
    }

    protected function parse_xml($xml=NULL, $dummy2=NULL)
    {
        if (isset($xml->flow_interconnect))
        {
            if (isset($xml->flow_interconnect->connects))
            {
                foreach ($xml->flow_interconnect->connects->connect as $connect)
                {
                    $this->addFlowConnect(new FlowConnect($connect));
                }
            }
        }
    }

    function configure($node, $block)
    {
        $this->parse_xml($node->xml);

        $reset = new Reset();
        $reset->name = 'reset';
        $reset->direction = 'in';
        $reset->group = 'reset_n';
        $this->addReset($reset);

        foreach ($node->blocks as $block)
        {
            foreach ($block->flows as $flow)
            {
                if ($flow->type == 'in' or $flow->type == 'out')
                {
                    $flow_interface = new Flow();
                    $flow_interface->name = $block->name . '_' . $flow->name;
                    $flow_interface->type = $flow->type . '_conn';
                    $flow_interface->size = $flow->size;

                    $this->addFlow($flow_interface);
                    //$this->addParam(new Param());
                }
            }
        }

        // compute connexion
        $tree_connects = array();
        foreach ($this->flow_connects as $connect)
        {
            // check connexion
            if (!$fromblock = $node->getBlock($connect->fromblock))
            {
                warning("Block '$connect->fromblock' doesn't exists. Please check your flow_interconnect->connects section.\n"
                        . FlowInterconnect::showConnect($connect)
                        . FlowInterconnect::showBlocks($node), 15, "FI");
                continue;
            }
            if (!$fromflow = $fromblock->getFlow($connect->fromflow))
            {
                warning("Flow '$connect->fromflow' doesn't exists in block '$connect->fromblock'. Please check your flow_interconnect->connects section." . "\n"
                        . FlowInterconnect::showConnect($connect)
                        . FlowInterconnect::showFlows($fromblock, "out"), 15, "FI");
                continue;
            }
            if (!$toblock = $node->getBlock($connect->toblock))
            {
                warning("Block '$connect->toblock' doesn't exists. Please check your flow_interconnect->connects section.\n"
                        . FlowInterconnect::showConnect($connect)
                        . FlowInterconnect::showBlocks($node), 15, "FI");
                continue;
            }
            if (!$toflow = $toblock->getFlow($connect->toflow))
            {
                warning("Flow '$connect->toflow' doesn't exists in block '$connect->toblock'. Please check your flow_interconnect->connects section.\n"
                        . FlowInterconnect::showConnect($connect)
                        . FlowInterconnect::showFlows($toblock, "in"), 15, "FI");
                continue;
            }

            // create tree of connexions
            if (!array_key_exists($toblock->name . '.' . $toflow->name, $tree_connects))
            {
                $tree_connects[$toblock->name . '.' . $toflow->name] = new TreeConnect($toblock->name, $toflow->name, $toflow->size, $connect->order);
            }
            array_push($tree_connects[$toblock->name . '.' . $toflow->name]->treeitems, new TreeItem($fromblock->name, $fromflow->name, $fromflow->size, $connect->order));
        }

        $this->tree_connects = $tree_connects;

        // create params for each mux
        $count_param = 0;
        foreach ($this->tree_connects as $treeconnect)
        {
            $in_connect_name = $treeconnect->toblock . '_' . $treeconnect->toflow;

            if (count($treeconnect->treeitems) > 1)
            {
                $property = new Property();
                $property->name = $treeconnect->toblock . '_' . $treeconnect->toflow;
                $property->type = 'enum';

                $treeconnect->muxname = $property->name;

                $param = new Param();
                $param->name = $property->name . '_param';
                $param->regaddr = $count_param;
                $param->value = 0;
                $param->default = 0;
                $param->hard = false;
                $param->desc = 'Mux control for ' . $treeconnect->toblock . '.' . $treeconnect->toflow . ' flow input';

                $parambitfield = new ParamBitfield();
                $parambitfield->name = 'muxdir';
                $parambitfield->default = 0;
                $parambitfield->propertymap = $property->name . ".bits";

                $this->addParam($param);

                $muxvalue = 0;
                foreach ($treeconnect->treeitems as $treeitem)
                {
                    $propertyenum = new PropertyEnum();
                    $propertyenum->name = $treeitem->fromblock . '.' . $treeitem->fromflow;
                    $propertyenum->caption = $propertyenum->name;
                    $propertyenum->value = $muxvalue;
                    $propertyenum->desc = $treeitem->fromblock . '.' . $treeitem->fromflow . ' as input of ' . $in_connect_name;

                    $treeitem->muxvalue = $muxvalue;

                    array_push($property->propertyenums, $propertyenum);
                    $muxvalue++;
                }

                $bitcountenum = ceil(log(count($property->propertyenums), 2));
                for ($i = 0; $i < $bitcountenum; $i++)
                    array_push($parambitfield->bitfieldlist, $i);
                if ($bitcountenum > 1)
                    $parambitfield->bitfield = ($bitcountenum - 1) . "-0";
                else
                    $parambitfield->bitfield = "0";

                array_push($param->parambitfields, $parambitfield);

                $this->addProperty($property);

                $count_param++;
            }
        }

        if ($count_param == 1)
            $this->pi_size_addr_rel = 1;
        else
            $this->pi_size_addr_rel = ceil(log($count_param, 2));

        // process properties on input flow
        foreach ($this->tree_connects as $treeconnect)
        {
            $flowIn = $node->getFlow($treeconnect->toblock . '.' . $treeconnect->toflow);

            // direct connection
            if (count($treeconnect->treeitems) == 1)
            {
                $flowOut = $node->getFlow($treeconnect->treeitems[0]->fromblock . '.' . $treeconnect->treeitems[0]->fromflow);

                foreach ($flowOut->properties as $propertyOut)
                {
                    $propertyIn = new Property();
                    $propertyIn->name = $propertyOut->name;
                    $propertyIn->type = $propertyOut->type;
                    $propertyIn->value = $propertyOut->value;
                    $propertyIn->propertymap = $propertyOut->completePath() . ".value";
                    $flowIn->addProperty($propertyIn);
                }
            }
            // multiple connection with mux
            elseif (count($treeconnect->treeitems) > 1)
            {
                $dependingPropertyName = "fi." . $treeconnect->muxname . ".bits";
                $propsFlowBase = array();
                foreach ($treeconnect->treeitems as $treeitem)
                {
                    $propsFlowBase[$treeitem->muxvalue] = '';
                }

                $propsFlow = array();

                foreach ($treeconnect->treeitems as $treeitem)
                {
                    $flowOut = $node->getFlow($treeitem->fromblock . '.' . $treeitem->fromflow);

                    foreach ($flowOut->properties as $propertyOut)
                    {
                        if (!array_key_exists($propertyOut->name, $propsFlow))
                            $propsFlow[$propertyOut->name] = $propsFlowBase;
                        $propsFlow[$propertyOut->name][$treeitem->muxvalue] = $propertyOut;
                    }
                }

                // add all properties in
                foreach ($propsFlow as $propName => $props)
                {
                    $type = '';
                    $value = '';

                    $code = "switch(" . $dependingPropertyName . "){";
                    foreach ($props as $key => $prop)
                    {
                        if (is_object($prop) and get_class($prop) === "Property")
                        {
                            $code.="case " . $key . ":" . $prop->completePath() . ".value" . "; break;";
                            $type = $prop->type;
                            $value = $prop->value;
                        }
                        else
                        {
                            $code.="case " . $key . ":" . "; break;";
                        }
                    }
                    $code.="}";

                    $propertyIn = new Property();
                    $propertyIn->name = $propName;
                    $propertyIn->type = $type;
                    $propertyIn->value = $value;
                    $propertyIn->propertymap = $code;
                    $flowIn->addProperty($propertyIn);
                }
            }
        }

        // transform toGlobalPropertyPath
        $this->toGlobalPropertyPath("fi");
    }

    function create_dot_file($node, $filename)
    {
        $content = '';

        $content.='digraph G {' . "\n";
        foreach ($node->blocks as $block)
        {
            if ($block->name != "fi" and count($block->flows) > 0)
            {
                $ins = array();
                $outs = array();
                foreach ($block->flows as $flow)
                {
                    if ($flow->type == 'in')
                        array_push($ins, $flow);
                    if ($flow->type == 'out')
                        array_push($outs, $flow);
                }

                $content.="\t" . $block->name . '[shape = record,label = "';
                $content.='{{';
                $count = 0;
                foreach ($ins as $in)
                {
                    $content.='<' . $in->name . '> ' . $in->name . ' (' . $in->size . ')';
                    if (++$count < count($ins))
                        $content.='|';
                }
                $content.='}|' . $block->name . '|{';
                $count = 0;
                foreach ($outs as $out)
                {
                    $content.='<' . $out->name . '> ' . $out->name . ' (' . $out->size . ')';
                    if (++$count < count($outs))
                        $content.='|';
                }

                $content.='}}"];' . "\n";
            }
        }

        foreach ($this->tree_connects as $treeconnect)
        {
            foreach ($treeconnect->treeitems as $treeitem)
            {
                $from_block = $treeitem->fromblock;
                $from_flow = $treeitem->fromflow;
                $to_block = $treeconnect->toblock;
                $to_flow = $treeconnect->toflow;
                if ($treeconnect->size != $treeitem->size)
                {
                    $content.="\t" . '"' . $from_block . '":' . $from_flow . ' -> "' . $to_block . '":' . $to_flow . "[ label=\"" . $treeitem->order . "\" ]" . ';' . "\n";
                }
                else
                {
                    $content.="\t" . '"' . $from_block . '":' . $from_flow . ' -> "' . $to_block . '":' . $to_flow . ';' . "\n";
                }
            }
        }
        $content.='}' . "\n";

        // save file if it's different
        $needToReplace = false;

        if (file_exists($filename))
        {
            $handle = fopen($filename, 'r');
            $actualContent = fread($handle, filesize($filename));
            fclose($handle);
            if ($actualContent != $content)
                $needToReplace = true;
        }
        else
            $needToReplace = true;

        if ($needToReplace)
        {
            if (!$handle = fopen($filename, 'w'))
                error("$filename cannot be openned", 5, "FI");
            if (fwrite($handle, $content) === FALSE)
                error("$filename cannot be written", 5, "FI");
            fclose($handle);
        }
    }

    function generate($node, $block, $path, $language)
    {
        $generator = new VHDL_generator('fi');

        if (!$fi = $node->getBlock('fi'))
            return;
        $generator->fromBlock($fi);

        $code = '';

        // create mux foreach input flow and reg
        $count_reg = 0;
        foreach ($fi->tree_connects as $treeconnect)
        {
            $in_size = $treeconnect->size;
            $in_order = $treeconnect->order;
            $in_connect_name = $treeconnect->toblock . '_' . $treeconnect->toflow;

            $code.='	-- ' . $in_connect_name . ' connection' . "\n";
            if (count($treeconnect->treeitems) == 0)   // no connection
            {
                // nothing to do
            }
            elseif (count($treeconnect->treeitems) == 1)  // one out to one in ==> direct connection
            {
                $out_order = $treeconnect->treeitems[0]->order;
                $out_size = $treeconnect->treeitems[0]->size;
                $out_connect_name = $treeconnect->treeitems[0]->fromblock . '_' . $treeconnect->treeitems[0]->fromflow;

                if ($in_size == $out_size)
                {
                    $code.='	' . $in_connect_name . '_data <= ' . $out_connect_name . '_data;' . "\n";
                }
                elseif ($in_size > $out_size)
                {
                    $padding_size = $in_size - $out_size;
                    $padding = '"' . str_pad('', $padding_size, '0') . '"';
                    if ($out_order == 'msb')
                    {
                        $code.='	' . $in_connect_name . '_data <= ' . $out_connect_name . '_data & ' . $padding . ';' . "\n";
                    }
                    else
                    {
                        $code.='	' . $in_connect_name . '_data <= ' . $padding . ' & ' . $out_connect_name . '_data;' . "\n";
                    }
                }
                elseif ($in_size < $out_size)
                {
                    $padding_size = $out_size - $in_size;
                    if ($out_order == 'msb')
                    {
                        $code.='	' . $in_connect_name . '_data <= ' . $out_connect_name . '_data(' . ($out_size - 1) . ' downto ' . $padding_size . ');' . "\n";
                    }
                    else
                    {
                        $code.='	' . $in_connect_name . '_data <= ' . $out_connect_name . '_data(' . ($out_size - $padding_size - 1) . ' downto 0);' . "\n";
                    }
                    warning("Size of flow " . $out_connect_name . " ($out_size) > size of flow " . $in_connect_name . " ($in_size), " . $out_order . " connection", 10, "FI");
                }
                $code.='	' . $in_connect_name . '_fv <=  ' . $out_connect_name . '_fv;' . "\n";
                $code.='	' . $in_connect_name . '_dv <=  ' . $out_connect_name . '_dv;' . "\n" . "\n";
            }
            else          // many outputs to one in ==> mux connection
            {
                $name_reg = 'mux_' . $in_connect_name . '_reg';
                $generator->addSignal($name_reg, 32, 'std_logic_vector');

                $code.='	mux_' . $in_connect_name . ' : process (clk_proc, reset)' . "\n";
                $code.='	begin' . "\n";
                $code.='		if(reset=\'0\') then' . "\n";
                $code.='			' . $in_connect_name . '_data <= (others => \'0\');' . "\n";
                $code.='			' . $in_connect_name . '_fv <= \'0\';' . "\n";
                $code.='			' . $in_connect_name . '_dv <= \'0\';' . "\n";

                $code.='		elsif(rising_edge(clk_proc)) then' . "\n";
                $code.='			case ' . $name_reg . ' is' . "\n";

                $value_mux = 0;
                foreach ($treeconnect->treeitems as $treeitem)
                {
                    $code.='				when std_logic_vector(to_unsigned(' . $value_mux . ', 32))=>' . "\n";

                    $out_order = $treeitem->order;
                    $out_size = $treeitem->size;
                    $out_connect_name = $treeitem->fromblock . '_' . $treeitem->fromflow;

                    if ($in_size == $out_size)
                    {
                        $code.='					' . $in_connect_name . '_data <= ' . $out_connect_name . '_data;' . "\n";
                    }
                    elseif ($in_size > $out_size)
                    {
                        $padding_size = $in_size - $out_size;
                        $padding = '"' . str_pad('', $padding_size, '0') . '"';
                        if ($out_order == 'msb')
                        {
                            $code.='					' . $in_connect_name . '_data <= ' . $out_connect_name . '_data & ' . $padding . ';' . "\n";
                        }
                        else
                        {
                            $code.='					' . $in_connect_name . '_data <= ' . $padding . ' & ' . $out_connect_name . '_data;' . "\n";
                        }
                    }
                    elseif ($in_size < $out_size)
                    {
                        $padding_size = $out_size - $in_size;
                        if ($out_order == 'msb')
                        {
                            $code.='					' . $in_connect_name . '_data <= ' . $out_connect_name . '_data(' . ($out_size - 1) . ' downto ' . $padding_size . ');' . "\n";
                        }
                        else
                        {
                            $code.='					' . $in_connect_name . '_data <= ' . $out_connect_name . '_data(' . ($padding_size - 1) . ' downto 0);' . "\n";
                        }
                        warning("Size of flow " . $out_connect_name . " ($out_size) > " . $in_connect_name . " ($in_size), " . $out_order . " connection", 10, "FI");
                    }

                    $code.='					' . $in_connect_name . '_fv <= ' . $out_connect_name . '_fv;' . "\n";
                    $code.='					' . $in_connect_name . '_dv <= ' . $out_connect_name . '_dv;' . "\n";
                    $value_mux++;
                }
                $code.='				when others=>' . "\n";
                $code.='					' . $in_connect_name . '_data <= (others => \'0\');' . "\n";
                $code.='					' . $in_connect_name . '_fv <= \'0\';' . "\n";
                $code.='					' . $in_connect_name . '_dv <= \'0\';' . "\n";
                $code.='			end case;' . "\n";
                $code.='		end if;' . "\n";
                $code.='	end process;' . "\n";
                $code.="\n";
            }
        }

        // register management
        $count_reg = 0;
        foreach ($fi->tree_connects as $treeconnect)
        {
            $in_connect_name = $treeconnect->toblock . '_' . $treeconnect->toflow;

            if (count($treeconnect->treeitems) > 1)
            {
                $code.='	-- ' . $in_connect_name . ' register' . "\n";
                $name_reg = 'mux_' . $in_connect_name . '_reg';
                $code.='	slave_' . $in_connect_name . ' : process (clk_proc, reset)' . "\n";
                $code.='	begin' . "\n";
                $code.='		if(reset=\'0\') then' . "\n";
                $code.='			' . $name_reg . ' <= (others => \'0\');' . "\n"; // TODO modify default value

                $code.='		elsif(rising_edge(clk_proc)) then' . "\n";

                if ($fi->pi_size_addr_rel == 1)
                {
                    $code.='			if(wr_i=\'1\' and addr_rel_i=\'' . $count_reg . '\') then' . "\n";
                }
                else
                {
                    $code.='			if(wr_i=\'1\' and addr_rel_i=std_logic_vector(to_unsigned(' . $count_reg . ', ' . $fi->pi_size_addr_rel . '))) then' . "\n";
                }
                $code.='				' . $name_reg . ' <= datawr_i;' . "\n";
                $code.='			end if;' . "\n";
                $code.='		end if;' . "\n";
                $code.='	end process;' . "\n";
                $code.="\n";

                $count_reg++;
            }
        }

        $generator->code = $code;

        $file = new File();
        $file->name = 'fi.vhd';
        $file->path = 'fi.vhd';
        $file->group = 'hdl';
        $file->type = 'vhdl';
        $file->generated = true;

        $this->path = $path . DIRECTORY_SEPARATOR;

        array_push($fi->files, $file);
        $filename = $path . DIRECTORY_SEPARATOR . $file->name;
        $generator->save_as_ifdiff($filename);

        $this->create_dot_file($node, $path . DIRECTORY_SEPARATOR . 'fi.dot');
    }

    public function type()
    {
        return 'fi';
    }

    public function getXmlElement($xml, $format)
    {
        if ($format == "complete")
        {
            $xml_element = parent::getXmlElement($xml, $format);

            // flow_connects
            $xml_flow_connects = $xml->createElement("flow_connects");
            foreach ($this->flow_connects as $flow_connect)
            {
                $xml_flow_connects->appendChild($flow_connect->getXmlElement($xml, $format));
            }
            $xml_element->appendChild($xml_flow_connects);

            // tree_connects
            $xml_tree_connects = $xml->createElement("tree_connects");
            foreach ($this->tree_connects as $treeconnect)
            {
                $xml_tree_connects->appendChild($treeconnect->getXmlElement($xml, $format));
            }
            $xml_element->appendChild($xml_tree_connects);

            return $xml_element;
        }
        elseif ($format == "project")
        {
            $xml_element = $xml->createElement("flow_interconnect");

            // flow_connects
            $xml_flow_connects = $xml->createElement("connects");
            foreach ($this->flow_connects as $flow_connect)
            {
                $xml_flow_connects->appendChild($flow_connect->getXmlElement($xml, $format));
            }
            $xml_element->appendChild($xml_flow_connects);

            return $xml_element;
        }
        return NULL;
    }

    /** Add a flow connection to the block 
     *  @param FlowConnect $flow_connect flow connection to add to the block * */
    function addFlowConnect($flow_connect)
    {
        $flow_connect->parentBlock = $this;
        array_push($this->flow_connects, $flow_connect);
    }

    function delFlowConnect($fromBlock, $fromFlow, $toBlock, $toFlow)
    {
        $i = 0;
        foreach ($this->flow_connects as $flow_connect)
        {
            if ($flow_connect->fromblock == $fromBlock
                    and $flow_connect->fromflow == $fromFlow
                    and $flow_connect->toblock == $toBlock
                    and $flow_connect->toflow == $toFlow)
                unset($this->flow_connects[$i]);
            $i++;
        }
        return null;
    }

    function delFlowConnectToBlock($block)
    {
        $i = 0;
        foreach ($this->flow_connects as $flow_connect)
        {
            if ($flow_connect->fromblock == $block or $flow_connect->toblock == $block)
                unset($this->flow_connects[$i]);
            $i++;
        }
        return null;
    }

    /** return a reference to the flow connection with the name $name, if not found, return false
     *  @param string $fromBlock name of the output flow block.
     *  @param string $fromFlow name of the output flow in $fromBlock.
     *  @param string $toBlock name of the input flow block.
     *  @param string $toFlow name of the input flow block in $toBlock.
     *  @return FlowConnect found flow connection * */
    function getFlowConnect($fromBlock, $fromFlow, $toBlock, $toFlow)
    {
        foreach ($this->flow_connects as $flow_connect)
        {
            if ($flow_connect->fromblock == $fromBlock
                    and $flow_connect->fromflow == $fromFlow
                    and $flow_connect->toblock == $toBlock
                    and $flow_connect->toflow == $toFlow)
                return $flow_connect;
        }
        return null;
    }
}
