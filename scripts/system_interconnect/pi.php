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

require_once("toolchain/hdl/vhdl_generator.php");
/**
 * It internally contain bus interconnect to all block that have a PI interface.
 * The generation of the block choose adress for each block.
 * 
 * @brief ParamInterconnect is the generated block to manage all the parameter interfaces.
 * @see Block Parameter
 * @ingroup base
 */
class ParamInterconnect extends Block
{
    public $addr_bus_width;

    function __construct()
    {
        parent::__construct();
        $this->name = "pi";
        $this->driver = "pi";
    }

    static function hex($value, $width)
    {
        return '0x' . str_pad(strtoupper(dechex($value)), ceil($width / 4.0), '0', STR_PAD_LEFT);
    }

    function configure($node, $block)
    {
        $reset = new Reset();
        $reset->name = 'reset';
        $reset->direction = 'in';
        $reset->group = 'reset_n';
        $this->addReset($reset);

        // compute the size of the memory bus
        $sum_space_addr_rel = 0;
        foreach ($node->blocks as $block)
        {
            if ($block->pi_size_addr_rel > 0)
            {
                $size_addr_space = pow(2, $block->pi_size_addr_rel);
                $sum_space_addr_rel += $size_addr_space;
            }
        }
        $this->addr_bus_width = ceil(log($sum_space_addr_rel, 2));

        // compute the adress relative of each block
        $max_addr = pow(2, $this->addr_bus_width) - 1;
        if ($max_addr > 0)
            $addr_space = array_fill(0, $max_addr, -1);
        else
            $addr_space = array();
        foreach ($node->blocks as $block)
        {
            if ($block->pi_size_addr_rel > 0)
            {
                $size_addr_space = pow(2, $block->pi_size_addr_rel);
                for ($addr = 0; $addr < $max_addr; $addr+=$size_addr_space)
                {
                    if ($addr_space[$addr] == -1)
                    {
                        $block->addr_abs = $addr;
                        $block->addInterface(new InterfaceBus("bus_sl", $block->name, "pi_slave", $block->pi_size_addr_rel));
                        $this->addInterface(new InterfaceBus("bus_sl_$block->name", $block->name, "pi_slave_conn", $block->pi_size_addr_rel));
                        for ($i = 0; $i < $size_addr_space; $i++)
                            $addr_space[$addr + $i] = 0;
                        break;
                    }
                }
            }
            if ($block->master_count > 0)
            {
                for ($i = 0; $i < $block->master_count; $i++)
                {
                    $block->addInterface(new InterfaceBus("bus_master", $block->name, "pi_master", $this->addr_bus_width));
                    $this->addInterface(new InterfaceBus("bus_master_$block->name", $block->name, "pi_master_conn", $this->addr_bus_width));

                    $param = new Param();
                    $param->name = 'MASTER_ADDR_WIDTH';
                    $param->value = $this->addr_bus_width;
                    $param->hard = true;
                    $block->addParam($param);
                }
            }
        }

        /* echo "\n" . '// =============== Slaves addr ===============' . "\n";
          foreach($node->blocks as $block)
          {
          if($block->pi_size_addr_rel>0)
          {
          echo $block->name . ' : ' . $block->addr_abs . "\n";
          }
          } */
    }

    function create_header_file($node, $filename)
    {
        $content = '';

        $content.='#define MASTER_ADDR_WIDTH ' . ParamInterconnect::hex($this->addr_bus_width, 8) . "\t\t// size of bus master adress in bits" . "\n";

        $used_drivers = array();
        foreach ($node->blocks as $block)
        {
            if ($block->pi_size_addr_rel > 0)
            {
                $content.="\n" . '// ' . str_pad(' ' . $block->name . ' at ' . ParamInterconnect::hex($block->addr_abs, $this->addr_bus_width) . ' (' . $block->addr_abs . ')' . ' : ' . $block->driver . ' ', 71, '=', STR_PAD_BOTH) . "\n";
                if (!empty($block->driver))
                    $used_drivers[$block->driver] = 0;

                // base
                $paramname = strtoupper($block->name . '_REG_BASE');
                $value = $block->addr_abs;
                $content.="#define " . str_pad($paramname, 35) . "\t" . str_pad(ParamInterconnect::hex($value, $this->addr_bus_width), 10);
                $content.="// (" . $value . ") ";
                if (!empty($block->desc))
                    $content.=$block->desc;
                $content.="\n\n";

                // generics
                $count = 0;
                foreach ($block->params as $param)
                {
                    if ($param->hard == true)
                    {
                        if ($count++ == 0)
                            $content.="//----- generics \n";
                        $value = $param->value;
                        $paramname = strtoupper($block->name . '_' . $param->name . '_GENERIC_VALUE');
                        $content.="#define " . str_pad($paramname, 35) . "\t" . str_pad($value, 10);
                        $content.="// (" . $value . ") ";
                        if (!empty($block->desc))
                            $content.=$block->desc;
                        $content.="\n";
                    }
                }
                if ($count > 0)
                    $content.="\n";

                // params
                $content.="//----- params \n";
                foreach ($block->params as $param)
                {
                    if ($param->hard == false)
                    {
                        // offset
                        $paramname = strtoupper($block->name . '_' . $param->name . '_REG_OFFSET');
                        $value = $param->regaddr;
                        $content.="#define " . str_pad($paramname, 35) . "\t" . str_pad(ParamInterconnect::hex($value, $this->addr_bus_width), 10);
                        $content.="// (" . $value . ") ";
                        if (!empty($block->desc))
                            $content.=$block->desc;
                        $content.="\n";

                        foreach ($param->parambitfields as $parambitfield)
                        {
                            $mask = 0;
                            foreach ($parambitfield->bitfieldlist as $bitfield)
                                $mask += pow(2, $bitfield);
                            $content.="	#define " . str_pad(strtoupper($paramname . '_' . $parambitfield->name . '_MASK'), 40) . "\t" . ParamInterconnect::hex($mask, 32) . "\n";

                            foreach ($parambitfield->paramenums as $paramenum)
                            {
                                $content.="		#define " . str_pad(strtoupper($paramname . '_' . $paramenum->name), 40) . "\t" . ParamInterconnect::hex($paramenum->value, 32);
                                if (!empty($enum->desc))
                                    $content.="\t// " . $paramenum->desc;
                                $content.="\n";
                            }
                        }
                        if (!empty($param->parambitfields))
                            $content.="\n";
                    }
                }

                foreach ($block->properties as $property)
                {
                    foreach ($property->propertyenums as $propertyenum)
                    {
                        $content.="	#define " . str_pad(strtoupper($property->name . '_' . $propertyenum->name), 40) . "\t" . ParamInterconnect::hex($propertyenum->value, 32);
                        if (!empty($enum->desc))
                            $content.="\t// " . $paramenum->desc;
                        $content.="\n";
                    }
                }
            }
        }

        // used drivers
        $content.="\n";
        $content.="// " . str_pad(' used drivers ', 71, '=', STR_PAD_BOTH) . "\n";
        foreach ($used_drivers as $driver => $val)
        {
            $content.="#define " . strtoupper($driver) . " 1" . "\n";
        }

        // save file if it's different
        $needToReplace = false;

        if (file_exists($filename))
        {
            if (filesize($filename) == 0)
            {
                $needToReplace = true;
            }
            else
            {
                $handle = fopen($filename, 'r');
                $actualContent = fread($handle, filesize($filename));
                fclose($handle);
                if ($actualContent != $content)
                    $needToReplace = true;
            }
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
        $generator = new VHDL_generator('pi');

        if (!$bi = $node->getBlock('pi'))
            return;
        $generator->fromBlock($bi);

        // find the master name
        $name_master = '';
        $master_count = 0;
        $size_master_addr = 0;
        foreach ($bi->interfaces as $interface)
        {
            if ($interface->type == 'pi_master_conn')
            {
                $name_master = $interface->blockname;
                $size_master_addr = $interface->size_addr;
                $master_count++;
            }
        }

        if ($master_count == 0)
            warning("No master on PI, you can not change parameters.", 65, "PI");
        if ($master_count > 1)
            error("Multi master not supported yet, but you can implement me.", 65, "PI");

        $code = '';

        $code.="\n";
        if ($master_count == 1)
        {
            // generate for only ONE master
            foreach ($bi->interfaces as $interface)
            {
                if ($interface->type == 'pi_slave_conn')
                {
                    $addr_slave_abs = $node->getBlock($interface->blockname)->addr_abs;
                    $addr_slave_abs_masked = $addr_slave_abs >> $interface->size_addr;

                    $name_cs = 'cs_' . $interface->blockname . '_s';
                    $name_addr_rel = $interface->blockname . '_addr_rel_o';

                    $name_wr = $interface->blockname . '_wr_o';
                    $name_rd = $interface->blockname . '_rd_o';
                    $name_datawr = $interface->blockname . '_datawr_o';

                    $name_addr_master = $name_master . '_master_addr_i';

                    $code.='-- ' . $interface->blockname . ' slave at addr : ' . $addr_slave_abs . ' to ' . ($addr_slave_abs + pow(2, $interface->size_addr) - 1) . "\n";
                    // rd/wr slave
                    $code.=$name_wr . ' <= ' . $name_cs . ' and ' . $name_master . '_master_wr_i;' . "\n";
                    $code.=$name_rd . ' <= ' . $name_cs . ' and ' . $name_master . '_master_rd_i;' . "\n";
                    // addr relative slave
                    if ($interface->size_addr == 1)
                    {
                        $code.=$name_addr_rel . ' <= ' . $name_addr_master . '(0);' . "\n";
                    }
                    else
                    {
                        $code.=$name_addr_rel . ' <= ' . $name_addr_master . '(' . ($interface->size_addr - 1) . ' downto 0);' . "\n";
                    }
                    // write slave
                    $code.=$name_datawr . ' <= ' . $name_master . '_master_datawr_i;' . "\n";

                    // decode address slave
                    $code.="\n";
                    $code.='decode_' . $interface->blockname . ' : process(' . $name_master . '_master_addr_i)' . "\n";
                    $code.='begin' . "\n";
                    $code.='	if(' . $name_addr_master . '(' . ($size_master_addr - 1) . ' downto ' . ($interface->size_addr) . ') = std_logic_vector(to_unsigned(' . $addr_slave_abs_masked . ', ' . ($size_master_addr - $interface->size_addr) . '))) then' . "\n";
                    $code.='		' . $name_cs . ' <= \'1\';' . "\n";
                    $code.='	else' . "\n";
                    $code.='		' . $name_cs . ' <= \'0\';' . "\n";
                    $code.='	end if;' . "\n";
                    $code.='end process;' . "\n";

                    $code.="\n";
                }
            }

            // data mux foreach master
            $code.="-- data mux foreach master" . "\n";
            //$code.='data_rd_'.$name_master.' : process('.$name_master.'_master_addr_i)'."\n";
            //$code.='begin'."\n";
            $code.='' . $name_master . '_master_datard_o <=' . "\n";
            foreach ($bi->interfaces as $interface)
            {
                if ($interface->type == 'pi_slave_conn')
                {
                    $addr_slave_abs = $node->getBlock($interface->blockname)->addr_abs;
                    $addr_slave_abs_masked = $addr_slave_abs >> $interface->size_addr;

                    $name_addr_rel = $interface->blockname . '_addr_rel_o';
                    $name_datard = $interface->blockname . '_datard_i';

                    $conditional = $name_addr_master . '(' . ($size_master_addr - 1) . ' downto ' . ($interface->size_addr) . ') = std_logic_vector(to_unsigned(' . $addr_slave_abs_masked . ', ' . ($size_master_addr - $interface->size_addr) . '))';

                    $code.='	' . $name_datard . ' when (' . $conditional . ') else' . "\n";
                }
            }
            $code.='	(others => \'0\');' . "\n";
        }
        //$code.='end process;'."\n";

        $generator->code = $code;

        $file = new File();
        $file->name = 'pi.vhd';
        $file->path = 'pi.vhd';
        $file->group = 'hdl';
        $file->type = 'vhdl';
        $file->generated = true;
        $bi->addFile($file);

        $this->path = $path . DIRECTORY_SEPARATOR;

        $filename = $path . DIRECTORY_SEPARATOR . $file->name;
        $generator->save_as_ifdiff($filename);

        $this->create_header_file($node, $path . DIRECTORY_SEPARATOR . 'params.h');
    }

    public function type()
    {
        return 'pi';
    }

    public function getXmlElement($xml, $format)
    {
        if ($format == "complete")
        {
            $xml_element = parent::getXmlElement($xml, $format);

            return $xml_element;
        }
        elseif ($format == "project")
        {
            $xml_element = $xml->createElement("params_interconnect");
            return $xml_element;
        }
        return NULL;
    }
}
