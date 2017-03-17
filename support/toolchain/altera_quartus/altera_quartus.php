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
require_once("toolchain/hdl/hdl.php");

require_once("pll.php");

function pgcd($a, $b)
{
    for ($c = $a % $b; $c != 0; $c = $a % $b) :
        $a = $b;
        $b = $c;
    endfor;

    return $b;
}

class Altera_quartus_toolchain extends HDL_toolchain
{
    private $nopartitions;

    private $noqsf;
    
    private $componentsIP;

    public function configure_project($node)
    {
        global $argv;
        parent::configure_project($node);

        if (in_array("nopartitions", $argv))
            $this->nopartitions = 1;
        else
            $this->nopartitions = 0;
        if (in_array("noqsf", $argv))
            $this->noqsf = 1;
        else
            $this->noqsf = 0;
    }

    public function generate_project($node, $path)
    {
        parent::generate_project($node, $path);
        $this->generate_project_file($node, $path);
        $this->copy_ip_files($node, $path);
        $this->generate_tcl($node, $path);
        $this->generate_makefile($node, $path);
    }

    protected function copy_ip_files($node, $path)
    {
        // createIpList
        $this->componentsIP = array();
        foreach ($node->blocks as $block)
        {
            $this->componentsIP = array_merge($this->componentsIP, $block->componentsList());
        }

        $fileList = array();
        // copy all files for block declared in library
        foreach ($this->componentsIP as $component)
        {
            foreach ($component->files as $file)
            {
                if ($file->generated === true)
                    continue;

                // process source file path and destination path
                if (strpos($file->path, "hwlib:") === 0)
                {
                    $filepath = str_replace("hwlib:", SUPPORT_PATH . "component" . DIRECTORY_SEPARATOR, $file->path);
                    $subpath = 'IP' . DIRECTORY_SEPARATOR . dirname(str_replace("hwlib:", "", $file->path));
                }
                else
                {
                    $filepath = $component->path . $file->path;
                    $subpath = 'IP' . DIRECTORY_SEPARATOR . $component->driver;
                    if (dirname($file->path) != ".")
                        $subpath .= DIRECTORY_SEPARATOR . dirname($file->path);
                }

                // check if file ever added
                if (!in_array($filepath, $fileList))
                {
                    $fileList[] = $filepath;

                    if (!file_exists($filepath))
                    {
                        warning("$filepath doesn't exists", 5, "Altera TCL");
                        continue;
                    }

                    // create directory
                    if (!is_dir($path . DIRECTORY_SEPARATOR . $subpath))
                        mkdir($path . DIRECTORY_SEPARATOR . $subpath, 0777, true);

                    if ($file->type != "directory")
                    {
                        // check if copy is needed
                        $needToCopy = false;
                        if (file_exists($path . DIRECTORY_SEPARATOR . $subpath . DIRECTORY_SEPARATOR . $file->name))
                        {
                            if (filemtime($filepath) > filemtime($path . DIRECTORY_SEPARATOR . $subpath . DIRECTORY_SEPARATOR . $file->name))
                            {
                                $needToCopy = true;
                            }
                        }
                        else
                        {
                            $needToCopy = true;
                        }

                        // copy if need
                        if ($needToCopy)
                        {
                            if (!copy($filepath, $path . DIRECTORY_SEPARATOR . $subpath . DIRECTORY_SEPARATOR . $file->name))
                            {
                                warning("failed to copy $file->name", 5, $component->name);
                            }
                        }
                    }
                    else
                    {
                        $dirtoCopy = $path . DIRECTORY_SEPARATOR . $subpath . DIRECTORY_SEPARATOR . $file->name;
                        if (!file_exists($dirtoCopy))
                            mkdir($dirtoCopy);
                        cpy_dir($filepath, $path . DIRECTORY_SEPARATOR . $subpath . DIRECTORY_SEPARATOR . $file->name);
                    }
                }

                // update the path file to the new copy path relative to project
                $file->path = $subpath . DIRECTORY_SEPARATOR . $file->name;
            }
        }
    }

    protected function generate_project_file($node, $path)
    {
        $content = '';

        $content.="QUARTUS_VERSION = \"13.1\"" . "\n";
        $content.="PROJECT_REVISION = \"$node->name\"" . "\n";

        // save file if it's different
        $filename = $path . DIRECTORY_SEPARATOR . "$node->name.qpf";
        $needToReplace = false;

        if (!file_exists($filename))
            $needToReplace = true;

        if ($needToReplace)
        {
            if (!$handle = fopen($filename, 'w'))
                error("$filename cannot be openned", 5, "Altera toolchain");
            if (fwrite($handle, $content) === FALSE)
                error("$filename cannot be written", 5, "Altera toolchain");
            fclose($handle);
        }
    }

    protected function generate_tcl($node, $path)
    {
        $content = '';

        // global board attributes
        $content.="# ========================= global assignement =========================\n";
        $content.="set_global_assignment -name TOP_LEVEL_ENTITY " . $node->name . "\n";
        foreach ($node->board->toolchain->attributes as $attribute)
        {
            $value = $attribute->value;
            if (strpos($value, ' ') !== false and strpos($value, '-section_id') === false)
                $value = '"' . $value . '"';
            $content.='set_' . $attribute->type . '_assignment -name ' . $attribute->name . ' ' . $value . "\n";
        }
        $content.="\nset_global_assignment -name REPORT_PARAMETER_SETTINGS OFF\n";
        $content.="set_global_assignment -name REPORT_SOURCE_ASSIGNMENTS OFF\n";
        $content.="set_global_assignment -name REPORT_CONNECTIVITY_CHECKS OFF\n";
        $content.="\n# ------------ pins ------------\n";
        foreach ($node->board->pins as $pin)
        {
            if (!empty($pin->mapto))
                $content.='set_location_assignment ' . $pin->name . ' -to ' . $pin->mapto . "\n";
            foreach ($pin->attributes as $attribute)
            {
                $value = $attribute->value;
                if (strpos($value, ' ') !== false and strpos($value, '-section_id') === false)
                    $value = '"' . $value . '"';
                $content.='set_' . $attribute->type . '_assignment -name ' . $attribute->name . ' ' . $value . ' -to ' . $pin->name . "\n";
            }
        }
        $content.="\n# ----------- files ------------\n";
        $content.='set_global_assignment -name VHDL_FILE ' . $node->name . '.vhd' . "\n";

        if ($this->nopartitions == 0)
        {
            $content.="\n# -------- partitions --------\n";
            $content.="set_global_assignment -name IGNORE_PARTITIONS ON\n";      // disabled for web edition
            $content.='set_instance_assignment -name PARTITION_HIERARCHY root_partition -to | -section_id ' . $node->name . "\n";
        }

        // blocks assignement
        $fileList = array();
        $componentUsed = array();
        $content.="\n\n# ========================= blocks assignements ========================\n";
        foreach ($this->componentsIP as $component)
        {
            if($component->type()=="component")
            {
                if(in_array($component->driver, $componentUsed))
                    continue;
                $componentUsed[] = $component->driver;
            }
            $content.="\n# " . str_pad(" $component->name ($component->driver) ", 70, '*', STR_PAD_BOTH);

            // pins
            if (!empty($component->pins))
            {
                $content.="\n# ------------ pins ------------\n";
                foreach ($component->pins as $pin)
                {
                    if (!empty($pin->mapto))
                        $content.='set_location_assignment ' . $pin->name . ' -to ' . $component->name . '_' . $pin->mapto . "\n";
                    foreach ($pin->attributes as $attribute)
                    {
                        $value = $attribute->value;
                        if (strpos($value, ' ') !== false and strpos($value, '-section_id') === false)
                            $value = '"' . $value . '"';
                        $content.='set_' . $attribute->type . '_assignment -name ' . $attribute->name . ' ' . $value . ' -to ' . $pin->name . "\n";
                    }
                }
            }

            // files
            $fileEmpty = true;
            foreach ($component->files as $file)
            {
                if ($file->group == "hdl" and $file->type != "directory")
                {
                    $type = '';
                    if ($file->type == "verilog")
                    {
                        $type = 'VERILOG_FILE';
                    }
                    elseif ($file->type == "vhdl")
                    {
                        $type = 'VHDL_FILE';
                    }
                    elseif ($file->type == "qip")
                    {
                        $type = 'QIP_FILE';
                    }
                    elseif ($file->type == "sdc")
                    {
                        $type = 'SDC_FILE';
                    }
                    elseif ($file->type == "hex")
                    {
                        $type = 'HEX_FILE';
                    }

                    $file_path = str_replace("\\", '/', $file->path);
                    $file_path = str_replace("/./", '/', $file_path);

                    if (!in_array($file_path, $fileList) and $type != "")
                    {
                        $fileList[] = $file_path;

                        if($fileEmpty)
                        {
                            $content.="\n# ----------- files ------------\n";
                            $fileEmpty = false;
                        }
                        $content.="set_global_assignment -name $type " . $file_path . "\n";
                    }
                }
            }

            // attributes
            if (!empty($component->attributes))
            {
                $content.="\n# --------- attributes ---------\n";
                foreach ($component->attributes as $attribute)
                {
                    $value = $attribute->value;
                    if (strpos($attribute->type, "location") === false)
                    {
                        $content.='set_' . $attribute->type . '_assignment -name ' . $attribute->name . ' ' . $value . "\n";
                    }
                    else
                    {
                        $content.='set_' . $attribute->type . '_assignment ' . $attribute->name . ' -to ' . $value . "\n";
                    }
                }
            }

            // partitions
            if ($this->nopartitions == 0 && $component->type()!="component")
            {
                $content.="\n# --------- partitions ---------\n";
                $driver = str_replace(".proc", "", $component->driver);
                if ($component->name == $driver)
                {
                    $instance = $driver . ':' . $component->name . '_inst';
                }
                else
                {
                    $instance = $driver . ':' . $component->name;
                }
                $partition_name = substr(str_replace('_', '', $driver), 0, 4) . '_' . substr(md5($node->name . '/' . $instance), 0, 4);
                $content.="set_instance_assignment -name PARTITION_HIERARCHY $partition_name -to \"$instance\" -section_id \"$instance\"" . "\n";
                $content.="set_global_assignment -name PARTITION_NETLIST_TYPE POST_SYNTH -section_id \"$instance\"" . "\n";
                $content.="set_global_assignment -name PARTITION_FITTER_PRESERVATION_LEVEL PLACEMENT_AND_ROUTING -section_id \"$instance\"" . "\n";
            }
        }

        if ($this->noqsf == 0)
        {
            // save file if it's different
            $filename = $path . DIRECTORY_SEPARATOR . "$node->name.qsf";
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
                    error("$filename cannot be openned", 5, "Altera toolchain");
                if (fwrite($handle, $content) === FALSE)
                    error("$filename cannot be written", 5, "Altera toolchain");
                fclose($handle);
            }
        }
    }

    public function generate_makefile($node, $path)
    {
        if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN')
        {
            $win = true;
            $rm = 'del';
            $rmreq = 'rmdir /s /q';
        }
        else
        {
            $win = false;
            $rm = 'rm -f';
            $rmreq = 'rm -rf';
        }

        $nodename = $node->name;

        $content = "-include Makefile.local" . "\r\n";
        $content .= "" . "\r\n";
        $content .= "ifndef GPS_LIB" . "\r\n";
        $content .= "	$(error Define GPS_LIB in Makefile.local)" . "\r\n";
        $content .= "endif" . "\r\n";
        $content .= "" . "\r\n";
        $content .= "all: generate compile send view" . "\r\n";
        $content .= "" . "\r\n";

        // generate
        $content .= "generate: " . str_replace("\\", '/', getRelativePath($node->node_file, $path)) . "\r\n";
        if (dirname($node->node_file) == realpath($path))
            $content .= "	gpnode generate \${OPT}" . "\r\n";
        else
            $content .= "	cd \"" . str_replace("\\", '/', getRelativePath(dirname($node->node_file), $path)) . "\" && gpnode generate -o \"" . str_replace("\\", '/', getRelativePath($path, dirname($node->node_file))) . "\" \${OPT}" . "\r\n";
        $content .= "" . "\r\n";

        // compile
        $content .= "compile:" . "\r\n";
        $content .= "	$(QUARTUS_TOOLS_PATH)quartus_sh --flow compile $nodename.qpf" . "\r\n";

        // send
        $content .= "send:" . "\r\n";
        $content .= "	$(QUARTUS_TOOLS_PATH)quartus_pgm -m jtag -c1 -o 'P;output_files/" . $node->name . ".sof'" . "\r\n";

        // view
        $content .= "view:" . "\r\n";
        if ($win)
            $content .= "	$(GPS_VIEWER)/gpviewer node_generated.xml" . "\r\n";
        else
            $content .= "	LD_LIBRARY_PATH=$(GPS_VIEWER) $(GPS_VIEWER)gpviewer node_generated.xml" . "\r\n";
        $content .= "" . "\r\n";

        // clean
        $content .= "clean:" . "\r\n";
        $content .= "	$rmreq output_files incremental_db db IP greybox_tmp" . "\r\n";
        $content .= "	$rm " . $node->name . ".vhd pi.vhd fi.vhd ci.vhd" . "\r\n";
        $content .= "	$rm node_generated.xml PLLJ_PLLSPE_INFO.txt fi.dot fi.png params.h" . "\r\n";
        $content .= "	$rm $nodename.qpf $nodename.qsf $nodename.qws $nodename.sdc $nodename.cdf" . "\r\n";

        // printfi
        $content .= "printfi:" . "\r\n";
        $content .= "	dot -Tpng -o fi.png fi.dot" . "\r\n";
        $content .= "	xdg-open fi.png" . "\r\n";

        $filename = "Makefile";
        saveIfDifferent($path . DIRECTORY_SEPARATOR . $filename, $content);

        // makefile local
        $content = "GPS_LIB=" . str_replace("\\", '/', LIB_PATH) . "\r\n";
        $content .= "GPS_VIEWER=" . str_replace("\\", '/', LIB_PATH) . "bin/\r\n";
        $content .= "QUARTUS_TOOLS_PATH=" . "\r\n";
        $content .= "" . "\r\n";

        $filename = "Makefile.local";
        if (!file_exists($path . DIRECTORY_SEPARATOR . $filename))
        {
            $handle = null;
            if (!$handle = fopen($path . DIRECTORY_SEPARATOR . $filename, 'w'))
                error("$filename cannot be openned", 5, "Toolchain");
            if (fwrite($handle, $content) === FALSE)
                error("$filename cannot be written", 5, "Vhdl Gen");
            fclose($handle);
        }

        // makefile src
        $content = "OUT_PWD := build/" . "\r\n";
        // generate
        $content .= "generate: " . $node->node_file . "\r\n";
        $content .= "	gpnode generate -o $(OUT_PWD) \${OPT}" . "\r\n" . "\r\n";

        // compile
        $content .= "compile:" . "\r\n";
        $content .= "	cd $(OUT_PWD) && make -f Makefile compile" . "\r\n" . "\r\n";

        // send
        $content .= "send:" . "\r\n";
        $content .= "	cd $(OUT_PWD) && make -f Makefile send" . "\r\n" . "\r\n";

        // view
        $content .= "view:" . "\r\n";
        $content .= "	cd $(OUT_PWD) && make -f Makefile view" . "\r\n" . "\r\n";

        // clean
        $content .= "clean:" . "\r\n";
        $content .= "	cd $(OUT_PWD) && make -f Makefile clean" . "\r\n" . "\r\n";

        // printfi
        $content .= "printfi:" . "\r\n";
        $content .= "	cd $(OUT_PWD) && make -f Makefile printfi" . "\r\n" . "\r\n";

        $filename = "Makefile";
        if (!file_exists(dirname($node->node_file) . DIRECTORY_SEPARATOR . $filename))
        {
            $handle = null;
            if (!$handle = fopen(dirname($node->node_file) . DIRECTORY_SEPARATOR . $filename, 'w'))
                error("$filename cannot be openned", 5, "Toolchain");
            if (fwrite($handle, $content) === FALSE)
                error("$filename cannot be written", 5, "Vhdl Gen");
            fclose($handle);
        }
    }

    function getRessourceAttributes($type)
    {
        $attr = array();
        $family = $this->getAttribute("FAMILY")->value;
        $device = $this->getAttribute("DEVICE")->value;

        switch ($family)
        {
            case 'MAX 10': // 10M50SAE144C8GES
                preg_match_all("|(10M[0-9]{2})([SD][CFA])([VEMUF])([0-9]+)([CIA])([6-8]).*|", $device, $out, PREG_SET_ORDER);
                $deviceMember = $out[0][1];
                $speedgrade = $out[0][6];
                $package = $out[0][3];
                break;
            case 'Cyclone III':
                preg_match_all("|(EP3)([A-Z]{1,3}[0-9]+)([A-Z]{1,3}[0-9]+)([A-Z][0-9]+[A-Z]*).*|", $device, $out, PREG_SET_ORDER);
                $deviceMember = $out[0][2];
                $speedgrade = $out[0][4];
                break;
            case 'Cyclone V': // 5CSEMA5F31C6
                preg_match_all("|(5C)(S[XE]{0,1})([FM]{0,1})([A-Z][0-9])([A-Z][0-9][0-9])([CIA][6-8])|", $device, $out, PREG_SET_ORDER);
                $deviceMember = $out[0][4];
                $speedgrade = $out[0][6];
                break;
            case 'Stratix IV': // EP4SE820F43C3
                preg_match_all("|(EP4)(SE{0,1})([0-9]+)([F]{0,1})([0-9]+)([A-Z][0-9]).*|", $device, $out, PREG_SET_ORDER);
                $deviceMember = $out[0][4];
                $speedgrade = $out[0][5];
                break;
            default:
                warning("family $family does'nt exist in toolchain", 5, 'Altera toolchain');
        }

        switch ($type)
        {
            case 'pll':
                switch ($family)
                {
                    case 'MAX 10':
                        $attr['maxPLL'] = 1;
                        $attr['clkByPLL'] = 5;
                        $attr['vcomin'] = 600000000;
                        $attr['vcomax'] = 1300000000;
                        $attr['mulmax'] = 512;
                        $attr['divmax'] = array(512, 512, 512, 512, 512);
                        $attr['pllclkcanbechain'] = true;
                        break;
                    case 'Cyclone III':
                        if ($deviceMember == 'C5' or $deviceMember == 'C10')
                            $attr['maxPLL'] = 2;
                        else
                            $attr['maxPLL'] = 4;
                        $attr['clkByPLL'] = 5;
                        $attr['vcomin'] = 300000000; //with divide by 2 mode
                        $attr['vcomax'] = 1300000000;
                        $attr['mulmax'] = 512;
                        $attr['divmax'] = array(512, 512, 512, 512, 512);
                        $attr['pllclkcanbechain'] = true;
                        break;
                    case 'Cyclone IV':
                        $attr['maxPLL'] = 4;
                        $attr['clkByPLL'] = 5;
                        $attr['vcomin'] = 600000000;
                        $attr['vcomax'] = 1300000000;
                        $attr['mulmax'] = 512;
                        $attr['divmax'] = array(512, 512, 512, 512, 512);
                        $attr['pllclkcanbechain'] = true;
                        break;
                    case 'Cyclone V':
                        if ($deviceMember == 'A9' or $deviceMember == 'C9' or $deviceMember == 'D9')
                            $attr['maxPLL'] = 8;
                        elseif ($deviceMember == 'A7' or $deviceMember == 'C7' or $deviceMember == 'D7')
                            $attr['maxPLL'] = 7;
                        elseif ($deviceMember == 'A5' or $deviceMember == 'C5' or $deviceMember == 'C6' or $deviceMember == 'C4' or $deviceMember == 'D5')
                            $attr['maxPLL'] = 6;
                        else
                            $attr['maxPLL'] = 4;

                        $attr['clkByPLL'] = 9;
                        $attr['vcomin'] = 600000000;

                        if ($speedgrade == 'C6')
                            $attr['vcomax'] = 1600000000;
                        elseif ($speedgrade == 'C7' or $speedgrade == 'I7')
                            $attr['vcomax'] = 1400000000;
                        elseif ($speedgrade == 'C8' or $speedgrade == 'A7')
                            $attr['vcomax'] = 1300000000;

                        $attr['mulmax'] = 4096;
                        $attr['divmax'] = array(4096, 1024, 1024, 512, 512, 512);
                        $attr['pllclkcanbechain'] = false;
                        break;
                    case 'Stratix IV':
                        // TODO complete
                        $attr['maxPLL'] = 4;
                        $attr['clkByPLL'] = 5;
                        $attr['vcomin'] = 600000000;
                        if ($speedgrade == 'C2')
                            $attr['vcomax'] = 1600000000;
                        else
                            $attr['vcomax'] = 1300000000;
                        $attr['mulmax'] = 512;
                        $attr['divmax'] = array(512, 512, 512, 512, 512);
                        $attr['pllclkcanbechain'] = true;
                        break;
                    default:
                        error("family $family does'nt exist in toolchain", 5, 'Altera toolchain');
                }

                break;
            default:
                warning("resource $type does'nt exist", 5, 'Altera toolchain');
        }
        return $attr;
    }

    function getRessourceDeclare($type, $params)
    {
        $declare = '';

        switch ($type)
        {
            case 'pll':
                $clkByPLL = $params['pll']->clkByPLL;
                $declare.='	COMPONENT altpll' . "\n";
                $declare.='	GENERIC (' . "\n";
                $declare.='		bandwidth_type		: STRING;' . "\n";
                for ($i = 0; $i < $clkByPLL; $i++)
                {
                    $declare.='		clk' . $i . '_divide_by		: NATURAL;' . "\n";
                    $declare.='		clk' . $i . '_duty_cycle		: NATURAL;' . "\n";
                    $declare.='		clk' . $i . '_multiply_by		: NATURAL;' . "\n";
                    $declare.='		clk' . $i . '_phase_shift		: STRING;' . "\n";
                }
                $declare.='		compensate_clock		: STRING;' . "\n";
                $declare.='		inclk0_input_frequency		: NATURAL;' . "\n";
                $declare.='		intended_device_family		: STRING;' . "\n";
                $declare.='		lpm_hint		: STRING;' . "\n";
                $declare.='		lpm_type		: STRING;' . "\n";
                $declare.='		operation_mode		: STRING;' . "\n";
                $declare.='		pll_type		: STRING;' . "\n";
                $declare.='		port_activeclock		: STRING;' . "\n";
                $declare.='		port_areset		: STRING;' . "\n";
                $declare.='		port_clkbad0		: STRING;' . "\n";
                $declare.='		port_clkbad1		: STRING;' . "\n";
                $declare.='		port_clkloss		: STRING;' . "\n";
                $declare.='		port_clkswitch		: STRING;' . "\n";
                $declare.='		port_configupdate		: STRING;' . "\n";
                $declare.='		port_fbin		: STRING;' . "\n";
                $declare.='		port_inclk0		: STRING;' . "\n";
                $declare.='		port_inclk1		: STRING;' . "\n";
                $declare.='		port_locked		: STRING;' . "\n";
                $declare.='		port_pfdena		: STRING;' . "\n";
                $declare.='		port_phasecounterselect		: STRING;' . "\n";
                $declare.='		port_phasedone		: STRING;' . "\n";
                $declare.='		port_phasestep		: STRING;' . "\n";
                $declare.='		port_phaseupdown		: STRING;' . "\n";
                $declare.='		port_pllena		: STRING;' . "\n";
                $declare.='		port_scanaclr		: STRING;' . "\n";
                $declare.='		port_scanclk		: STRING;' . "\n";
                $declare.='		port_scanclkena		: STRING;' . "\n";
                $declare.='		port_scandata		: STRING;' . "\n";
                $declare.='		port_scandataout		: STRING;' . "\n";
                $declare.='		port_scandone		: STRING;' . "\n";
                $declare.='		port_scanread		: STRING;' . "\n";
                $declare.='		port_scanwrite		: STRING;' . "\n";
                for ($i = 0; $i < $clkByPLL; $i++)
                    $declare.='		port_clk' . $i . '		: STRING;' . "\n";
                for ($i = 0; $i < $clkByPLL; $i++)
                    $declare.='		port_clkena' . $i . '		: STRING;' . "\n";
                $declare.='		port_extclk0		: STRING;' . "\n";
                $declare.='		port_extclk1		: STRING;' . "\n";
                $declare.='		port_extclk2		: STRING;' . "\n";
                $declare.='		port_extclk3		: STRING;' . "\n";
                $declare.='		width_clock		: NATURAL' . "\n";
                $declare.='	);' . "\n";
                $declare.='	PORT (' . "\n";
                $declare.='			areset : IN STD_LOGIC;' . "\n";
                $declare.='			clk	: OUT STD_LOGIC_VECTOR (' . ($clkByPLL - 1) . ' DOWNTO 0);' . "\n";
                $declare.='			inclk	: IN STD_LOGIC_VECTOR (1 DOWNTO 0)' . "\n";
                $declare.='	);' . "\n";
                $declare.='	END COMPONENT;' . "\n";
                break;
            default:
                warning("Ressource $type does'nt exist", 5, 'Altera toolchain');
        }
        return $declare;
    }

    function getRessourceInstance($type, $params)
    {
        $instance = '';
        $family = $this->getAttribute("FAMILY")->value;

        switch ($type)
        {
            case 'pll':
                $clkByPLL = $params['pll']->clkByPLL;
                $clkin = $params['clkin'];
                $pllname = $params['pllname'];

                $instance.='	' . $pllname . ' : altpll' . "\n";
                $instance.='	GENERIC MAP (' . "\n";
                $instance.='		bandwidth_type => "AUTO",' . "\n";
                $instance.='		compensate_clock => "CLK0",' . "\n";
                $instance.='		intended_device_family => "' . $family . '",' . "\n";
                $instance.='		lpm_hint => "CBX_MODULE_PREFIX=pll",' . "\n";
                $instance.='		lpm_type => "altpll",' . "\n";
                $instance.='		operation_mode => "NORMAL",' . "\n";
                $instance.='		pll_type => "AUTO",' . "\n";
                $instance.='		port_inclk0 => "PORT_USED",' . "\n";

                $instance.='		port_activeclock => "PORT_UNUSED",' . "\n";
                $instance.='		port_areset => "PORT_USED",' . "\n";
                $instance.='		port_clkbad0 => "PORT_UNUSED",' . "\n";
                $instance.='		port_clkbad1 => "PORT_UNUSED",' . "\n";
                $instance.='		port_clkloss => "PORT_UNUSED",' . "\n";
                $instance.='		port_clkswitch => "PORT_UNUSED",' . "\n";
                $instance.='		port_configupdate => "PORT_UNUSED",' . "\n";
                $instance.='		port_fbin => "PORT_UNUSED",' . "\n";
                $instance.='		port_inclk1 => "PORT_UNUSED",' . "\n";
                $instance.='		port_locked => "PORT_UNUSED",' . "\n";
                $instance.='		port_pfdena => "PORT_UNUSED",' . "\n";
                $instance.='		port_phasecounterselect => "PORT_UNUSED",' . "\n";
                $instance.='		port_phasedone => "PORT_UNUSED",' . "\n";
                $instance.='		port_phasestep => "PORT_UNUSED",' . "\n";
                $instance.='		port_phaseupdown => "PORT_UNUSED",' . "\n";
                $instance.='		port_pllena => "PORT_UNUSED",' . "\n";
                $instance.='		port_scanaclr => "PORT_UNUSED",' . "\n";
                $instance.='		port_scanclk => "PORT_UNUSED",' . "\n";
                $instance.='		port_scanclkena => "PORT_UNUSED",' . "\n";
                $instance.='		port_scandata => "PORT_UNUSED",' . "\n";
                $instance.='		port_scandataout => "PORT_UNUSED",' . "\n";
                $instance.='		port_scandone => "PORT_UNUSED",' . "\n";
                $instance.='		port_scanread => "PORT_UNUSED",' . "\n";
                $instance.='		port_scanwrite => "PORT_UNUSED",' . "\n";
                for ($i = 0; $i < $clkByPLL; $i++)
                    $instance.='		port_clkena' . $i . ' => "PORT_UNUSED",' . "\n";
                $instance.='		port_extclk0 => "PORT_UNUSED",' . "\n";
                $instance.='		port_extclk1 => "PORT_UNUSED",' . "\n";
                $instance.='		port_extclk2 => "PORT_UNUSED",' . "\n";
                $instance.='		port_extclk3 => "PORT_UNUSED",' . "\n";

                $instance.='		inclk0_input_frequency => ' . ceil(1000000000000 / $clkin) . ',' . "\n";

                $clkId = 0;
                foreach ($params['pll']->clksshift as $clk)
                {
                    $clockFreq = $clk[0];
                    $clockShift = $clk[1];

                    $div = $clkin / pgcd($clockFreq, $clkin);
                    $mul = floor($clockFreq / ($clkin / $div));
                    if ($clockShift == 0)
                        $shift = 0;
                    else
                        $shift = ($clockShift / 360) * 10000000;

                    $instance.='		-- clk' . $clkId . ' at ' . Clock::formatFreq($clockFreq) . ' ' . $clockShift . '° shifted' . "\n";
                    $instance.='		port_clk' . $clkId . ' => "PORT_USED",' . "\n";
                    $instance.='		clk' . $clkId . '_duty_cycle => 50,' . "\n";
                    $instance.='		clk' . $clkId . '_divide_by => ' . $div . ',' . "\n";
                    $instance.='		clk' . $clkId . '_multiply_by => ' . $mul . ',' . "\n";
                    $instance.='		clk' . $clkId . '_phase_shift => "' . $shift . '",' . "\n";

                    $clkId++;
                }
                for ($i = $clkId; $i < $clkByPLL; $i++)
                {
                    $instance.='		port_clk' . $i . ' => "PORT_UNUSED",' . "\n";
                    $instance.='		clk' . $i . '_duty_cycle => 50,' . "\n";
                    $instance.='		clk' . $i . '_divide_by => 1,' . "\n";
                    $instance.='		clk' . $i . '_multiply_by => 1,' . "\n";
                    $instance.='		clk' . $i . '_phase_shift => "0",' . "\n";
                }
                $instance.='		width_clock => ' . $clkByPLL . "\n";
                $instance.='	)' . "\n";
                $instance.='	PORT MAP (' . "\n";
                $instance.='		areset => not(reset),' . "\n";
                $instance.='		inclk => ' . $pllname . '_in,' . "\n";
                $instance.='		clk => ' . $pllname . '_out' . "\n";
                $instance.='	);' . "\n";

                break;
            default:
                warning("Ressource $type does'nt exist", 5, 'Altera toolchain');
        }
        return $instance;
    }
}
