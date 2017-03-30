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

require_once("toolchain.php");

require_once("vhdl_generator.php");

require_once("fi.php");
require_once("pi.php");
require_once("ci.php");

class HDL_toolchain extends Toolchain
{

    public function configure_project($node)
    {
        $node->board->configure($node);
        foreach ($node->blocks as $block)
        {
            $block->configure($node, $block);
        }

        // add flow interconnect with config
        $flowInterconnect = new FlowInterconnect();
        $node->addBlock($flowInterconnect);

        // add clock interconnect with config
        $clockInterconnect = new ClockInterconnect();
        $node->addBlock($clockInterconnect);

        // add bus interconnect
        $paramInterconnect = new ParamInterconnect();
        $node->addBlock($paramInterconnect);

        $flowInterconnect->configure($node, $flowInterconnect);
        $paramInterconnect->configure($node, $paramInterconnect);
        $clockInterconnect->configure($node, $clockInterconnect);
    }

    public function generate_project($node, $path)
    {
        $node->board->generate($node, $path, 'hdl');
        foreach ($node->blocks as $block)
        {
            $block->generate($node, $block, $path, 'hdl');
        }
        $this->generate_top_level($node, $path);
    }

    protected function generate_top_level($node, $path)
    {
        $generator = new VHDL_generator($node->name);

        // clocks
        foreach ($node->board->clocks as $clock)
        {
            if ($clock->direction == "in")
            {
                $generator->addPort($clock->name, 1, 'out');
            }
            else
            {
                $generator->addPort($clock->name, 1, 'in');
            }
        }

        // resets
        foreach ($node->board->resets as $reset)
        {
            $generator->addPort($reset->name, 1, $reset->direction);
        }

        // external ios
        foreach ($node->blocks as $block)
        {
            if (!empty($block->ext_ports))
            {
                $generator->addPortComment(str_pad(' ' . $block->name . ' flow ', 55, '-', STR_PAD_BOTH));
                foreach ($block->ext_ports as $port)
                {
                    $size = $port->size;
                    $type = "";
                    if (!is_numeric($size))
                    {
                        $size = (int) $block->getParam($size)->value;
                        $type = "std_logic_vector";
                    }
                    $generator->addPort($block->name . '_' . $port->name, $size, $port->type, $type);
                }
            }
        }

        // signals for clocks
        $generator->addSignalComment(str_pad(' clocks part ', 55, '=', STR_PAD_BOTH));
        $clocks = array();
        foreach ($node->getBlock('ci')->clocks as $clock)
        {
            if ($clock->net != "" and ! in_array($clock->net, $clocks)/* and $clock->direction=='out' */)
            {
                $generator->addSignal($clock->net, 1, 'std_logic');
                array_push($clocks, $clock->net);
            }
        }

        // signals for resets
        $generator->addSignalComment(str_pad(' resets part ', 55, '=', STR_PAD_BOTH));
        $resets = array();
        foreach ($node->blocks as $block)
        {
            foreach ($block->resets as $reset)
            {
                if (!in_array($reset->group, $resets))
                    array_push($resets, $reset->group);
            }
        }
        foreach ($resets as $reset)
        {
            $generator->addSignal($reset, 1, 'std_logic', "'1'");
        }

        // signals for flows interconnect
        $generator->addSignalComment(str_pad(' flows part ', 55, '=', STR_PAD_BOTH));
        foreach ($node->blocks as $block)
        {
            if (!empty($block->flows) and $block->name != 'fi')
            {
                $generator->addSignalComment(str_pad(' ' . $block->name . ' ', 55, '-', STR_PAD_BOTH));
                foreach ($block->flows as $flow)
                {
                    $generator->addSignal($block->name . '_' . $flow->name . '_data_s', $flow->size, 'std_logic_vector');
                    $generator->addSignal($block->name . '_' . $flow->name . '_fv_s', 1, 'std_logic');
                    $generator->addSignal($block->name . '_' . $flow->name . '_dv_s', 1, 'std_logic');
                }
            }
        }

        // signals for bus interconnect
        $generator->addSignalComment(str_pad(' bus part ', 55, '=', STR_PAD_BOTH));
        foreach ($node->blocks as $block)
        {
            if (!empty($block->interfaces))
            {
                foreach ($block->interfaces as $interface)
                {
                    if ($interface->type == 'pi_slave')
                    {
                        $generator->addSignalComment(str_pad(' ' . $block->name . ' ' . $interface->name . ' ', 55, '-', STR_PAD_BOTH));
                        if ($interface->size_addr == 1)
                        {
                            $generator->addSignal($block->name . '_addr_rel_s', $interface->size_addr, 'std_logic');
                        }
                        else
                        {
                            $generator->addSignal($block->name . '_addr_rel_s', $interface->size_addr, 'std_logic_vector');
                        }
                        $generator->addSignal($block->name . '_wr_s', 1, 'std_logic');
                        $generator->addSignal($block->name . '_rd_s', 1, 'std_logic');
                        $generator->addSignal($block->name . '_datawr_s', 32, 'std_logic_vector');
                        $generator->addSignal($block->name . '_datard_s', 32, 'std_logic_vector');
                    }
                    if ($interface->type == 'pi_master')
                    {
                        $generator->addSignalComment(str_pad(' ' . $block->name . ' ' . $interface->name . ' ', 55, '-', STR_PAD_BOTH));
                        if ($interface->size_addr == 1)
                        {
                            $generator->addSignal($block->name . '_master_addr_s', $interface->size_addr, 'std_logic');
                        }
                        else
                        {
                            $generator->addSignal($block->name . '_master_addr_s', $interface->size_addr, 'std_logic_vector');
                        }
                        $generator->addSignal($block->name . '_master_wr_s', 1, 'std_logic');
                        $generator->addSignal($block->name . '_master_rd_s', 1, 'std_logic');
                        $generator->addSignal($block->name . '_master_datawr_s', 32, 'std_logic_vector');
                        $generator->addSignal($block->name . '_master_datard_s', 32, 'std_logic_vector');
                    }
                }
            }
        }

        foreach ($node->blocks as $block)
            $generator->addBlock($block);

        $code = "";
        foreach ($node->board->clocks as $clock)
        {
            $code.='	' . $clock->net . '	<=	' . $clock->name . ";\n";
        }

        $ci = $node->getBlock('ci');
        //print_r($ci->domains);

        $generator->code = $code;
        $generator->save_as_ifdiff($path . DIRECTORY_SEPARATOR . $node->name . '.vhd');
    }
}
