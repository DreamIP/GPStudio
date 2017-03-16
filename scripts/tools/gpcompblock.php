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

define("LIB_PATH", realpath(dirname(__FILE__) . "/../..") . DIRECTORY_SEPARATOR);
define("SUPPORT_PATH", LIB_PATH . "support" . DIRECTORY_SEPARATOR);
set_include_path(get_include_path() . PATH_SEPARATOR . LIB_PATH);
set_include_path(get_include_path() . PATH_SEPARATOR . LIB_PATH . DIRECTORY_SEPARATOR . 'scripts');
set_include_path(get_include_path() . PATH_SEPARATOR . LIB_PATH . DIRECTORY_SEPARATOR . 'scripts' . DIRECTORY_SEPARATOR . 'model');
set_include_path(get_include_path() . PATH_SEPARATOR . LIB_PATH . DIRECTORY_SEPARATOR . 'scripts' . DIRECTORY_SEPARATOR . 'system_interconnect');
set_include_path(get_include_path() . PATH_SEPARATOR . LIB_PATH . DIRECTORY_SEPARATOR . 'support');

require_once("process.php");
require_once("io.php");
require_once("iocom.php");
require_once("gpstudio.php");
require_once('toolchain' . DIRECTORY_SEPARATOR . 'hdl' . DIRECTORY_SEPARATOR . "block_generator.php");
require_once('toolchain' . DIRECTORY_SEPARATOR . 'hdl' . DIRECTORY_SEPARATOR . "vhdl_extractor.php");

$options = getopt("a:");
if (array_key_exists('a', $options))
    $action = $options['a'];
else
    error("You should specify an action with -a", 1);

switch ($action)
{
    // =========================== global commands =====================
    case "-h":
    case "--help":
        echo "# " . TOOL . " command line tool to manage a gpstudio " . KIND . " (" . VERSION . ")" . "\n";
        echo "# === global ===" . "\n";
        echo TOOL . " new -n <" . KIND . "-name>" . "\n";
        echo TOOL . " showblock" . "\n";
        echo TOOL . " generatetop [-o <dir>]" . "\n";
        if (TOOL != "gpcomp")
        {
            echo TOOL . " generateslave [-o <dir>]" . "\n";
            echo TOOL . " generateprocess [-o <dir>]" . "\n";
        }
        echo TOOL . " generate [-o <dir>]" . "\n";
        echo TOOL . " sethelp [-n <instance-name>] -v <help-text>" . "\n";
        echo TOOL . " setdraw -v <svg-draw-content>" . "\n";
        echo TOOL . " setdraw -f <svg-file>}" . "\n";
        echo TOOL . " setcateg -v <categ>" . "\n";
        echo TOOL . " setinfo -n <info-name> -v <info-value/content>" . "\n";
        echo TOOL . " info [-n <info-name>]" . "\n";
        echo "" . "\n";
        echo "# === files ===" . "\n";
        echo TOOL . " addfile -p <path> -t <type> -g <group>" . "\n";
        echo TOOL . " delfile -p <path>" . "\n";
        echo TOOL . " showfile" . "\n";
        echo "" . "\n";
        echo "# === flows ===" . "\n";
        echo TOOL . " addflow -n <flow-name> -d <direction> -s <size>" . "\n";
        echo TOOL . " delflow -n <flow-name>" . "\n";
        echo TOOL . " showflow" . "\n";
        echo TOOL . " renameflow -n <flow-name> -v <new-name>" . "\n";
        echo TOOL . " setflow -n <flow-name> [-d <direction>] [-s <size>]" . "\n";
        echo "" . "\n";
        echo "# === params ===" . "\n";
        echo TOOL . " addparam -n <param-name> [-t <type>] [-r <relative-address>] [-v <default-value>] [-m <property-map>]" . "\n";
        echo TOOL . " delparam -n <param-name>" . "\n";
        echo TOOL . " showparam" . "\n";
        echo TOOL . " renameparam -n <param-name> -v <new-name>" . "\n";
        echo TOOL . " setparam -n <param-name> [-t <type>] [-r <relative-address>] [-v <default-value>]" . "\n";
        echo TOOL . " fixparam -n <param-name> -v <true/false>" . "\n";
        echo TOOL . " setpisizeaddr -v <pisizeaddr>" . "\n";
        echo TOOL . " setpimastercount -v <pimastercount>" . "\n";
        echo "" . "\n";
        if (TOOL != "gpcomp")
        {
            echo "# === bitfields ===" . "\n";
            echo TOOL . " addbitfield -n <param-name.bitfield-name> -b <bitfield> [-m <property-map>]" . "\n";
            echo TOOL . " delbitfield -n <param-name.bitfield-name>" . "\n";
            echo TOOL . " showbitfield -n <param-name>" . "\n";
            echo TOOL . " renamebitfield -n <param-name.bitfield-name> -v <new-name>" . "\n";
            echo TOOL . " setbitfield -n <bitfield-name> -b <bitfield>" . "\n";
            echo "" . "\n";
            echo "# === properties ===" . "\n";
            echo TOOL . " addproperty -n <property-name> -t <type> [-v <default-value>] [-m <property-map>]" . "\n";
            echo TOOL . " delproperty -n <property-name>" . "\n";
            echo TOOL . " showproperty [-n <property/flow-name>]" . "\n";
            echo TOOL . " renameproperty -n <property-name> -v <new-name>" . "\n";
            echo TOOL . " setproperty -n <property-name> [-l <label>] [-t <type>] [-v <default-value>] [-r <min:max>] [-s <step>]" . "\n";
            echo TOOL . " setpropertymap -n <param/property/bitfield-name> -v <property-map>" . "\n";
            echo "" . "\n";
            echo "# === enums ===" . "\n";
            echo TOOL . " addenum -n <enum-name> -v <value> [-l <label>]" . "\n";
            echo TOOL . " delenum -n <enum-name>" . "\n";
            echo TOOL . " showenum [-n <property-name>]" . "\n";
            echo "" . "\n";
        }
        echo "# === resets ===" . "\n";
        echo TOOL . " addreset -n <reset-name> -d <direction> -g <group>" . "\n";
        echo TOOL . " delreset -n <reset-name>" . "\n";
        echo TOOL . " showreset" . "\n";
        echo TOOL . " renamereset -n <reset-name> -v <new-name>" . "\n";
        echo TOOL . " setreset -n <reset-name> [-d <direction>] [-g <group>]" . "\n";
        echo "" . "\n";
        echo "# === clocks ===" . "\n";
        echo TOOL . " addclock -n <clock-name> -d <direction> [-g <clock-domain>]" . "\n";
        echo TOOL . " delclock -n <clock-name>" . "\n";
        echo TOOL . " showclock" . "\n";
        echo TOOL . " renameclock -n <clock-name> -v <new-name>" . "\n";
        echo TOOL . " setclock -n <clock-name> [-d <direction>] [-g <clock-domain> [-m <multiplier>]] [-f <clock-frequency>] [-r <min:max>]" . "\n";
        if (TOOL == "gpdevice")
        {
            echo "" . "\n";
            echo "# === external ports ===" . "\n";
            echo TOOL . " addextport -n <port-name> -t <type> -s <size>" . "\n";
            echo TOOL . " delextport -n <port-name>" . "\n";
            echo TOOL . " showextport" . "\n";
        }
        echo "# === components ===" . "\n";
        echo TOOL . " addcomponent -n <component-name>" . "\n";
        echo TOOL . " delcomponent -n <component-name>" . "\n";
        echo TOOL . " showcomponent" . "\n";
        exit(0);
        break;
    case "-v":
    case "--version":
        echo "# " . TOOL . " command line tool to manage a gpstudio " . KIND . " (" . VERSION . ")" . "\n";
        exit(0);
        break;
}

if (!in_array($action, array("convert")))
{
    // new block creation
    if (($action == "new" or $action == "extract") and TOOL == "gpproc")
    {
        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $componentName = $options['n'];
        else
            error("You should specify a process name with -n", 1);

        $component = new Process();
        $component->name = $componentName;
        $componentName.=".proc";
    }
    elseif (($action == "new" or $action == "extract") and TOOL == "gpdevice")
    {
        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $componentName = $options['n'];
        else
            error("You should specify a device name with -n", 1);

        $component = new IO();
        $component->name = $componentName;
        $component->driver = $componentName;
        $componentName.=".io";
    }
    elseif (($action == "new" or $action == "extract") and TOOL == "gpcomp")
    {
        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $componentName = $options['n'];
        else
            error("You should specify a component name with -n", 1);

        $component = new Component();
        $component->name = $componentName;
        $component->driver = $componentName;
        $componentName.=".comp";
    }
    else
    {
        // find io or process and open it
        if (!defined("TOOL"))
            error("Cannot call this script directly", 1);
        if (TOOL == "gpproc")
        {
            $componentName = findprocess();
            if (!file_exists($componentName))
            {
                if (strpos($action, "list") === false)
                    error("Cannot find a valid block process in the current directory.", 1);
                else
                    exit(1);
            }
            else
                $component = new Process($componentName);
        }
        elseif (TOOL == "gpdevice")
        {
            $componentName = findio();
            if (!file_exists($componentName))
            {
                if (strpos($action, "list") === false)
                    error("Cannot find a valid block device in the current directory.", 1);
                else
                    exit(1);
            }
            else
            {
                $xml = simplexml_load_file($componentName);
                if (isset($xml->com_driver))
                    $component = new IOCom($componentName);
                else
                    $component = new IO($componentName);
                unset($xml);
            }
        }
        elseif (TOOL == "gpcomp")
        {
            $componentName = findcomp();
            if (!file_exists($componentName))
            {
                if (strpos($action, "list") === false)
                    error("Cannot find a valid component in the current directory.", 1);
                else
                    exit(1);
            }
            else
                $component = new Component($componentName);
        }
        else
        {
            error("Cannot call this script without a valid tool.", 1);
        }
    }

    if ($component->getReset('reset_n') == NULL and $action == "new")
    {
        // Added reset on block
        $reset = new Reset();
        $reset->name = 'reset_n';
        $reset->group = 'reset_n';
        $component->addReset($reset);
    }
}

$save = true;

switch ($action)
{
    // =========================== project commands ====================
    case "new":
        // nothing to do
        break;

    case "readwrite":
        $save = true;
        break;

    case "generate":
        $options = getopt("a:o:");
        if (array_key_exists('o', $options))
            $outDir = $options['o'];
        else
            $outDir = getcwd();
        mkdir_rec($outDir);

        $component_generator = new Block_generator($component);

        $component_generator->generateTopBlock($outDir);
        message($component_generator->block_generator->name . '.vhd' . ' generated');

        $component_generator->generateSlave($outDir);
        if (isset($component_generator->slave_generator))
        {
            message($component_generator->slave_generator->name . '.vhd' . ' generated');
        }

        $component_generator->generateProcess($outDir);
        message($component_generator->process_generator->name . '.vhd' . ' generated');

        $save = false;
        break;

    case "generatetop":
        $options = getopt("a:o:");
        if (array_key_exists('o', $options))
            $outDir = $options['o'];
        else
            $outDir = getcwd();
        mkdir_rec($outDir);

        $component_generator = new Block_generator($component);
        $component_generator->generateTopBlock($outDir);
        message($component_generator->block_generator->name . '.vhd' . ' generated');

        $save = false;
        break;

    case "generateslave":
        $options = getopt("a:o:");
        if (array_key_exists('o', $options))
            $outDir = $options['o'];
        else
            $outDir = getcwd();
        mkdir_rec($outDir);

        $component_generator = new Block_generator($component);
        $component_generator->generateSlave($outDir);
        message($component_generator->slave_generator->name . '.vhd' . ' generated');

        $save = false;
        break;

    case "generateprocess":
        $options = getopt("a:o:");
        if (array_key_exists('o', $options))
            $outDir = $options['o'];
        else
            $outDir = getcwd();
        mkdir_rec($outDir);

        $component_generator = new Block_generator($component);
        $component_generator->generateProcess($outDir);
        message($component_generator->process_generator->name . '.vhd' . ' generated');

        $save = false;
        break;

    case "generatetb":
        $options = getopt("a:o:");
        if (array_key_exists('o', $options))
            $outDir = $options['o'];
        else
            $outDir = getcwd();
        mkdir_rec($outDir);

        $component_generator = new Block_generator($component);
        $component_generator->generateTb($outDir);
        message($component_generator->block_generator->name . '.vhd' . ' generated');
        message('Makefile' . ' generated');

        $save = false;
        break;

    case "convert":
        $options = getopt("a:i:o:");
        if (array_key_exists('i', $options))
            $input = $options['i'];
        else
            error("You should specify an input file to convert with -i", 1);
        $infos = pathinfo($input);
        $extension = strtolower($infos['extension']);

        if (array_key_exists('o', $options))
            $output = $options['o'];
        else   
        {
            if (in_array($extension, array("jpg", "jpeg", "bmp", "gif", "png")))
                $output = $input . ".stim";
            elseif ($extension == "data")
                $output = $infos['filename'] . ".png";
            else
                error("You should specify an output file to convert with -o", 1);
        }
        if (in_array($extension, array("jpg", "jpeg", "bmp", "gif", "png")))
            Block_generator::convertImg2stim($input, $output);
        elseif ($extension == "data")
            Block_generator::convertData2Img($input, $output);
        else
            error("Bad input format", 2);
        message($output . ' generated');
        $save = false;
        break;

    case "extract":
        $name = str_replace(".comp", "", str_replace(".proc", "", str_replace(".io", "", $componentName)));
        $extractor = new VHDL_extractor($name);
        $component = $extractor->toComponent(TOOL);
        $componentName = str_replace(".vhd", "", $name);
        break;

    case "showblock":
        $component->print_flow();
        break;

    // =========================== files commands ======================
    case "addfile":
        $options = getopt("a:p:t:g:");
        if (array_key_exists('p', $options))
            $path = str_replace("\\", "/", $options['p']);
        else
            error("You should specify a path for the file with -p", 1);
        if (array_key_exists('t', $options))
            $type = $options['t'];
        else
            error("You should specify a type for the file with -t", 1);
        if (array_key_exists('g', $options))
            $group = $options['g'];
        else
            error("You should specify a group for the file with -g", 1);

        if ($component->getFileByPath($path) != NULL)
            error("This file already exists added with the same path.", 1);
        if (!file_exists($path))
            warning("This file '" . $path . "' does not exist, you should create it.", 4);

        $file = new File();
        $file->name = basename($path);
        $file->path = $path;
        $file->type = $type;
        $file->group = $group;

        $component->addFile($file);
        break;

    case "delfile":
        $options = getopt("a:p:");
        if (array_key_exists('p', $options))
            $path = $options['p'];
        else
            error("You should specify a path for the file with -p", 1);

        if ($component->getFileByPath($path) == NULL)
            error("A file does not exist with the path '$path'.", 1);

        $component->delFileByPath($path);
        break;

    case "showfile":
        echo "files :" . "\n";
        foreach ($component->files as $file)
        {
            echo "  + " . $file . "\n";
        }
        $save = false;
        break;

    // ======================= flows interface commands ================
    case "addflow":
        $options = getopt("a:n:d:s:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the flow with -n", 1);
        if (array_key_exists('d', $options))
            $direction = $options['d'];
        else
            error("You should specify a direction for the flow with -d [in-out]", 1);
        if (array_key_exists('s', $options))
            $size = (int) $options['s'];
        else
            error("You should specify a size for the flow with -s", 1);

        if (!preg_match("/^[A-Za-z][0-9A-Za-z_]*$/", $name))
            echo error("This name '$name' does not respect the naming convention ([A-Za-z][0-9A-Za-z_]*).", 1);
        if ($component->getInstance($name, false) != NULL)
            error("This instance name already exists (insensitive case)." . " '" . $name . "'", 1);
        if ($direction != "in" and $direction != "out")
            error("You should specify a direction for the flow with -d [in-out]", 1);

        $flow = new Flow();
        $flow->name = $name;
        $flow->type = $direction;
        $flow->size = $size;

        $component->addFlow($flow);
        break;

    case "delflow":
        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the flow with -n", 1);

        if ($component->getFlow($name) == NULL)
            error("A flow does not exist with the name '$name'.", 1);

        $component->delFlow($name);
        break;

    case "showflow":
        echo "flows :" . "\n";
        foreach ($component->flows as $flow)
        {
            echo "  + " . $flow . "\n";
        }
        $save = false;
        break;

    case "renameflow":
        $options = getopt("a:n:v:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the flow with -n", 1);
        if (array_key_exists('v', $options))
            $newname = $options['v'];
        else
            error("You should specify a new name for the flow with -v", 1);

        if (!preg_match("/^[A-Za-z][0-9A-Za-z_]*$/", $newname))
            echo error("This name '$newname' does not respect the naming convention ([A-Za-z][0-9A-Za-z_]*).", 1);
        if (($flow = $component->getFlow($name)) == NULL)
            error("A flow does not exist with the name '$name'.", 1);
        $check = $component->getFlow($newname, false);
        if ($check != $flow and $check != NULL)
            error("This flow name '$check->name' already exists (insensitive case).", 1);

        $flow->name = $newname;

        break;

    case "setflow":
        $options = getopt("a:n:d:s:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the flow with -n", 1);

        if (($flow = $component->getFlow($name)) == NULL)
            error("A flow does not exist with the name '$name'.", 1);

        if (array_key_exists('d', $options))
        {
            if ($options['d'] != "in" and $options['d'] != "out")
                error("You should specify a direction for the flow with -d [in-out]", 1);
            else
                $direction = $options['d'];
        }
        else
            $direction = $flow->type;
        if (array_key_exists('s', $options))
            $size = (int) $options['s'];
        else
            $size = $flow->size;

        $flow->type = $direction;
        $flow->size = $size;

        break;

    // =========================== params commands =====================
    case "addparam":
        $options = getopt("a:n:t:v:r:m:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the param with -n", 1);

        if (!preg_match("/^[A-Za-z][0-9A-Za-z_]*$/", $name))
            echo error("This name '$name' does not respect the naming convention ([A-Za-z][0-9A-Za-z_]*).", 1);
        if ($component->getInstance($name) != NULL)
            error("This instance name already exists." . " '" . $name . "'", 1);

        if (array_key_exists('t', $options))
            $type = $options['t'];
        else
        {
            if (TOOL == "gpcomp")
                error("Component param type have to be specified.", 1);
            $type = '';
        }
        if (array_key_exists('v', $options))
            $value = $options['v'];
        else
            $value = '';
        if (array_key_exists('r', $options))
        {
            if (TOOL == "gpcomp")
                error("Component cannot have register.", 1);
            $regaddr = $options['r'];
        }
        else
            $regaddr = '';
        if (array_key_exists('m', $options))
            $propertymap = $options['m'];
        else
            $propertymap = '';

        if (array_key_exists('t', $options) and array_key_exists('r', $options))
            error("You could not define a relative address and a type for a parameter.", 1);

        $param = new Param();
        $param->name = $name;
        if ($type != '')
            $param->hard = true;
        else
            $param->hard = false;
        $param->default = $value;
        $param->value = $value;
        $param->type = $type;
        $param->regaddr = $regaddr;
        $param->propertymap = $propertymap;

        if ($regaddr != '' and $param->hard == false)
        {
            if (log($regaddr, 2) > $component->pi_size_addr_rel or $component->pi_size_addr_rel == 0)
            {
                warning("Your relative adress is greater than the range of relative address ($component->pi_size_addr_rel bits).\nPlease specify a new PI size address with :\n" . TOOL . " setpisizeaddr -v " . ( ($regaddr == 0) ? 1 : (floor(log($regaddr, 2)) + 1)), 1);
            }
        }

        $component->addParam($param);
        break;

    case "delparam":
        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the param with -n", 1);

        if ($component->getParam($name) == NULL)
            error("A param does not exist with the name '$name'.", 1);

        $component->delParam($name);
        break;

    case "showparam":
        echo "param :" . "\n";
        foreach ($component->params as $param)
        {
            echo "  + " . $param . "\n";
            if (!empty($param->parambitfields))
            {
                foreach ($param->parambitfields as $parambitfield)
                {
                    echo "    * " . $parambitfield . "\n";
                }
            }
        }
        $save = false;
        break;

    case "renameparam":
        $options = getopt("a:n:v:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the param with -n", 1);
        if (array_key_exists('v', $options))
            $newname = $options['v'];
        else
            error("You should specify a new name for the param with -v", 1);

        if (!preg_match("/^[A-Za-z][0-9A-Za-z_]*$/", $newname))
            echo error("This name '$newname' does not respect the naming convention ([A-Za-z][0-9A-Za-z_]*).", 1);
        if (($param = $component->getParam($name)) == NULL)
            error("A param does not exist with the name '$name'.", 1);
        $check = $component->getParam($newname, false);
        if ($check != $param and $check != NULL)
            error("This param name '$check->name' already exists (insensitive case).", 1);

        $param->name = $newname;

        break;

    case "setparam":
        $options = getopt("a:n:t:v:r:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the param with -n", 1);

        if (($param = $component->getParam($name)) == NULL)
            error("A param does not exist with the name '$name'.", 1);

        if (array_key_exists('t', $options))
            $type = $options['t'];
        else
            $type = $param->type;
        if (array_key_exists('v', $options))
            $value = $options['v'];
        else
            $value = $param->value;
        if (array_key_exists('r', $options))
            $regaddr = $options['r'];
        else
            $regaddr = $param->regaddr;

        if (array_key_exists('t', $options) and array_key_exists('r', $options))
            error("You could not define a relative address and a type for a parameter.", 1);

        if (isset($regaddr) and $param->hard == false)
        {
            if (log($regaddr, 2) > $component->pi_size_addr_rel or $component->pi_size_addr_rel == 0)
            {
                warning("Your relative adress is greater than the range of relative address ($component->pi_size_addr_rel bits).\nPlease specify a new PI size address with :\n" . TOOL . " setpisizeaddr -v " . ( ($regaddr == 0) ? 1 : (floor(log($regaddr, 2)) + 1)), 1);
            }
        }

        $param->type = $type;
        $param->default = $value;
        $param->value = $value;
        $param->regaddr = $regaddr;

        break;

    case "fixparam":
        $options = getopt("a:n:v:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the param with -n", 1);
        if (array_key_exists('v', $options))
            $hard = $options['v'];
        else
            $hard = "true";

        if ($hard == "true" or $hard == "TRUE" or $hard == "1")
            $hard = true;
        elseif ($hard == "false" or $hard == "FALSE" or $hard == "0")
            $hard = false;
        else
            error("The value of -v option need to be true/false or 0/1.", 1);

        if (($param = $component->getParam($name)) == NULL)
            error("A param does not exist with the name '$name'.", 1);

        $param->hard = $hard;

        break;

    case "setpisizeaddr":
        if (TOOL == "gpcomp")
            error("Component cannot have pi interface.", 1);

        $options = getopt("a:v:");
        if (array_key_exists('v', $options))
            $size = $options['v'];
        else
            error("You should specify a PI address relative size in bit with -v", 1);
        if (!is_numeric($size))
            error("You should specify a PI address relative size in bit with -v", 1);

        $component->pi_size_addr_rel = $size;

        break;

    case "setpimastercount":
        if (TOOL == "gpcomp")
            error("Component cannot have pi interface.", 1);

        $options = getopt("a:v:");
        if (array_key_exists('v', $options))
            $size = $options['v'];
        else
            error("You should specify a master count with -v", 1);
        if (!is_numeric($size))
            error("You should specify a master count with -v", 1);

        $component->master_count = $size;

        break;

    // ========================= bitfields commands ====================
    case "addbitfield":
        if (TOOL == "gpcomp")
            error("Component cannot have bitfields.", 1);

        $options = getopt("a:n:b:m:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the bitfield with -n", 1);
        if (array_key_exists('b', $options))
            $bitfield = (string) $options['b'];
        else
            error("You should specify a bitfield bits selection for the bitfield with -b\nExemple: 3,0 => [3 0] or 3-0 => [3 2 1 0] or 6-4,0 => [6 5 4 0]", 1);
        if (array_key_exists('m', $options))
            $propertymap = $options['m'];
        else
            $propertymap = '';

        $subPath = explode('.', $name);
        if (count($subPath) != 2)
            error("Invalide name for a bitfield (param.parambitfield).", 1);
        $param = $component->getParam($subPath[0]);
        if ($param == NULL)
            error("Unknow param name '" . $subPath[0] . "'.", 1);

        if (!preg_match("/^[A-Za-z][0-9A-Za-z_]*$/", $subPath[1]))
            echo error("This name '$name' does not respect the naming convention ([A-Za-z][0-9A-Za-z_]*).", 1);
        $paramBitField = $param->getParambitfield($subPath[1]);
        if ($paramBitField != NULL)
            error("This bitfield name already exists.", 1);

        $paramBitField = new ParamBitfield();
        $paramBitField->name = $subPath[1];
        $paramBitField->bitfield = $bitfield;
        $paramBitField->propertymap = $propertymap;

        $param->addParamBitfield($paramBitField);
        break;

    case "delbitfield":
        if (TOOL == "gpcomp")
            error("Component cannot have bitfields.", 1);

        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the bitfield with -n", 1);

        if ($component->getParamBitField($name) == NULL)
            error("A bitfield does not exist with the name '$name'.", 1);

        $component->delParamBitField($name);
        break;

    case "showbitfield":
        if (TOOL == "gpcomp")
            error("Component cannot have bitfields.", 1);

        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the bitfield with -n", 1);

        if (($param = $component->getParam($name)) == NULL)
            error("A param does not exist with the name '$name'.", 1);

        echo "bit fields of $param->name:" . "\n";
        foreach ($param->parambitfields as $parambitfield)
        {
            echo "  + " . $parambitfield . "\n";
        }
        $save = false;
        break;

    case "renamebitfield":
        if (TOOL == "gpcomp")
            error("Component cannot have bitfields.", 1);

        $options = getopt("a:n:v:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the bitfield with -n", 1);
        if (array_key_exists('v', $options))
            $newname = $options['v'];
        else
            error("You should specify a new name for the bitfield with -v", 1);

        if (!preg_match("/^[A-Za-z][0-9A-Za-z_]*$/", $newname))
            echo error("This name '$newname' does not respect the naming convention ([A-Za-z][0-9A-Za-z_]*).", 1);
        if (($bitfield = $component->getParambitfield($name)) == NULL)
            error("A bitfield does not exist with the name '$name'.", 1);
        $check = $bitfield->parentParam->getParambitfield($newname, false);
        if ($check != $bitfield and $check != NULL)
            error("This bitfield name '$check->name' already exists (insensitive case).", 1);

        $bitfield->name = $newname;

        break;

    case "setbitfield":
        if (TOOL == "gpcomp")
            error("Component cannot have bitfields.", 1);

        $options = getopt("a:n:b:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the bitfield with -n", 1);
        if (array_key_exists('b', $options))
            $bitfield = (string) $options['b'];
        else
            error("You should specify a bitfield bits selection for the bitfield with -b\nExemple: 3,0 => [3 0] or 3-0 => [3 2 1 0] or 6-4,0 => [6 5 4 0]", 1);

        if (($paramBitField = $component->getParamBitField($name)) == NULL)
            error("A bitfield does not exist with the name '$name'.", 1);

        $paramBitField->bitfield = $bitfield;

        break;

    // =========================== resets commands =====================
    case "addreset":
        $options = getopt("a:n:d:g:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the reset with -n", 1);
        if (array_key_exists('d', $options))
            $direction = $options['d'];
        else
            $direction = "in";
        if (array_key_exists('g', $options))
            $group = $options['g'];
        else
            error("You should specify a group for the reset with -g", 1);

        if (!preg_match("/^[A-Za-z][0-9A-Za-z_]*$/", $name))
            echo error("This name '$name' does not respect the naming convention ([A-Za-z][0-9A-Za-z_]*).", 1);
        if ($component->getInstance($name) != NULL)
            error("This instance name already exists." . " '" . $name . "'", 1);
        if ($direction != "in" and $direction != "out")
            error("You should specify a direction for the reset with -d [in-out]", 1);

        $reset = new Reset();
        $reset->name = $name;
        $reset->direction = $direction;
        $reset->group = $group;

        $component->addReset($reset);
        break;

    case "delreset":
        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the reset with -n", 1);

        if ($component->getReset($name) == NULL)
            error("A reset does not exist with the name '$name'.", 1);

        $component->delReset($name);
        break;

    case "showreset":
        echo "resets :" . "\n";
        foreach ($component->resets as $reset)
        {
            echo "  + " . $reset . "\n";
        }
        $save = false;
        break;

    case "renamereset":
        $options = getopt("a:n:v:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the reset with -n", 1);
        if (array_key_exists('v', $options))
            $newname = $options['v'];
        else
            error("You should specify a new name for the reset with -v", 1);

        if (!preg_match("/^[A-Za-z][0-9A-Za-z_]*$/", $newname))
            echo error("This name '$newname' does not respect the naming convention ([A-Za-z][0-9A-Za-z_]*).", 1);
        if (($reset = $component->getReset($name)) == NULL)
            error("A reset does not exist with the name '$name'.", 1);
        $check = $component->getReset($newname, false);
        if ($check != $reset and $check != NULL)
            error("This reset name '$check->name' already exists (insensitive case).", 1);

        $reset->name = $newname;

        break;

    case "setreset":
        $options = getopt("a:n:d:g:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the reset with -n", 1);

        if (($reset = $component->getReset($name)) == NULL)
            error("A reset does not exist with the name '$name'.", 1);

        if (array_key_exists('d', $options))
        {
            if ($options['d'] != "in" and $options['d'] != "out")
                error("You should specify a direction for the reset with -d [in-out]", 1);
            else
                $direction = $options['d'];
        }
        else
            $direction = $reset->direction;
        if (array_key_exists('g', $options))
            $group = $options['g'];
        else
            $group = $reset->group;

        $reset->direction = $direction;
        $reset->group = $group;

        break;

    // =========================== clocks commands =====================
    case "addclock":
        $options = getopt("a:n:d:g:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the clock with -n", 1);
        if (array_key_exists('d', $options))
            $direction = $options['d'];
        else
            $direction = "in";
        if (array_key_exists('g', $options))
            $domain = $options['g'];
        else
            $domain = "";

        if (!preg_match("/^[A-Za-z][0-9A-Za-z_]*$/", $name))
            echo error("This name '$name' does not respect the naming convention ([A-Za-z][0-9A-Za-z_]*).", 1);
        if ($component->getInstance($name) != NULL)
            error("This instance name already exists." . " '" . $name . "'", 1);
        if ($direction != "in" and $direction != "out")
            error("You should specify a direction for the clock with -d [in-out]", 1);

        $clock = new Clock();
        $clock->name = $name;
        $clock->direction = $direction;
        $clock->domain = $domain;

        $component->addClock($clock);
        break;

    case "delclock":
        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the clock with -n", 1);

        if ($component->getClock($name) == NULL)
            error("A clock does not exist with the name '$name'.", 1);

        $component->delClock($name);
        break;

    case "showclock":
        echo "clocks :" . "\n";
        foreach ($component->clocks as $clock)
        {
            echo "  + " . $clock . "\n";
        }
        $save = false;
        break;

    case "renameclock":
        $options = getopt("a:n:v:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the clock with -n", 1);
        if (array_key_exists('v', $options))
            $newname = $options['v'];
        else
            error("You should specify a new name for the clock with -v", 1);

        if (!preg_match("/^[A-Za-z][0-9A-Za-z_]*$/", $newname))
            echo error("This name '$newname' does not respect the naming convention ([A-Za-z][0-9A-Za-z_]*).", 1);
        if (($clock = $component->getClock($name)) == NULL)
            error("A clock does not exist with the name '$name'.", 1);
        $check = $component->getClock($newname, false);
        if ($check != $clock and $check != NULL)
            error("This clock name '$check->name' already exists (insensitive case).", 1);

        $clock->name = $newname;

        break;

    case "setclock":
        $options = getopt("a:n:d:g:m:f:r:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the clock with -n", 1);

        if (($clock = $component->getClock($name)) == NULL)
            error("A clock does not exist with the name '$name'.", 1);

        if (array_key_exists('d', $options))
        {
            if ($options['d'] != "in" and $options['d'] != "out")
                error("You should specify a direction for the clock with -d [in-out]", 1);
            else
                $direction = $options['d'];
        }
        else
            $direction = $clock->direction;
        if (array_key_exists('g', $options))
            $domain = $options['g'];
        else
            $domain = $clock->domain;
        if (array_key_exists('m', $options))
            $ratio = $options['m'];
        else
            $ratio = $clock->ratio;
        if (array_key_exists('f', $options))
            $typical = Clock::convert($options['f']);
        else
            $ratio = $clock->typical;
        if (array_key_exists('r', $options))
        {
            $range = explode(':', $options['r']);
            if (count($range) != 2)
                error("You should specify a range whith min:max syntax.", 1);
            $min = Clock::convert($range[0]);
            $max = Clock::convert($range[1]);
        }
        else
        {
            $min = $clock->min;
            $max = $clock->max;
        }

        $clock->direction = $direction;
        $clock->domain = $domain;
        $clock->ratio = $ratio;
        $clock->typical = $typical;
        $clock->min = $min;
        $clock->max = $max;

        break;

    // ========================== extport commands =====================
    case "addextport":
        if (TOOL != "gpdevice")
            error("This command can only be used on a device block.", 1);

        $options = getopt("a:n:t:s:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the external port with -n", 1);
        if (array_key_exists('t', $options))
            $type = $options['t'];
        else
            error("You should specify a type for the external port with -t [in-out-inout]", 1);
        if (array_key_exists('s', $options))
            $size = $options['s'];
        else
            error("You should specify a size for the external port with -s", 1);

        if (!preg_match("/^[A-Za-z][0-9A-Za-z_]*$/", $name))
            echo error("This name '$name' does not respect the naming convention ([A-Za-z][0-9A-Za-z_]*).", 1);
        if ($component->getInstance($name) != NULL)
            error("This instance name already exists." . " '" . $name . "'", 1);
        if ($type != "in" and $type != "out" and $type != "inout")
            error("You should specify a type for the external port with -t [in-out-inout]", 1);

        $port = new Port();
        $port->name = $name;
        $port->type = $type;
        $port->size = $size;

        $component->addExtPort($port);
        break;

    case "delextport":
        if (TOOL != "gpdevice")
            error("This command can only be used on a device block.", 1);

        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the file with -n", 1);

        if ($component->getExtPort($name) == NULL)
            error("A file does not exist with the path '$path'.", 1);

        $component->delExtPort($name);
        break;

    case "showextport":
        if (TOOL != "gpdevice")
            error("This command can only be used on a device block.", 1);

        echo "external ports :" . "\n";
        foreach ($component->ext_ports as $ext_port)
        {
            echo "  + " . $ext_port . "\n";
        }
        $save = false;
        break;

    // ======================= properties commands =====================
    case "addproperty":
        if (TOOL == "gpcomp")
            error("Component cannot have properties.", 1);

        $options = getopt("a:n:t:v:m:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the property with -n", 1);
        if (array_key_exists('t', $options))
            $type = $options['t'];
        else
            error("You should specify a type for the property with -t", 1);
        if (array_key_exists('v', $options))
            $value = $options['v'];
        else
            $value = '';
        if (array_key_exists('m', $options))
            $propertymap = $options['m'];
        else
            $propertymap = '';

        $subprops = explode('.', $name);
        if (count($subprops) == 0)
            error("Invalid property name '$name'.", 1);

        $parent = $component;
        $i = 0;
        if (($instance = $component->getFlow($subprops[0], false)) != NULL)
        {
            $parent = $instance;
            $i++;
            if ($instance->type != 'out')
                error("It's forbiden to add property to an input flow.", 1);
        }
        for (; $i < count($subprops); $i++)
        {
            $property = $parent->getProperty($subprops[$i], false);
            if ($property == null)
            {
                if ($i < count($subprops) - 1)
                    error("Invalid property name '$name' $parent->name.", 1);
            }
            else
            {
                if ($i == count($subprops) - 1)
                    error("This property name already exists.", 1);
                $parent = $property;
            }
        }

        $name = $subprops[count($subprops) - 1];
        if (!preg_match("/^[A-Za-z][0-9A-Za-z_]*$/", $name))
            echo error("This name '$name' does not respect the naming convention ([A-Za-z][0-9A-Za-z_]*).", 1);

        $property = new Property();
        $property->name = $subprops[count($subprops) - 1];
        $property->caption = $property->name;
        $property->type = $type;
        $property->value = $value;
        $property->propertymap = $propertymap;

        $parent->addProperty($property);
        break;

    case "delproperty":
        if (TOOL == "gpcomp")
            error("Component cannot have properties.", 1);

        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the property with -n", 1);

        if ($component->getPropertyPath($name) == NULL)
            error("A property does not exist with the name '$name'.", 1);

        $component->delPropertyPath($name);
        break;

    case "showproperty":
        if (TOOL == "gpcomp")
            error("Component cannot have properties.", 1);

        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            $name = "";

        $subprops = explode('.', $name);
        $parent = $component;
        if (($instance = $component->getFlow($subprops[0])) != NULL)
        {
            $parent = $instance;
            unset($subprops[0]);
        }

        if (count($subprops) > 0 and $subprops[0] != "")
            $parent = $parent->getProperty(implode('.', $subprops));
        if ($parent == NULL)
            error("An instance does not exist with the name '$name'.", 1);

        echo "properties :" . "\n";
        foreach ($parent->properties as $property)
        {
            echo "  + " . $property . "\n";
        }
        $save = false;
        break;

    case "renameproperty":
        if (TOOL == "gpcomp")
            error("Component cannot have properties.", 1);

        $options = getopt("a:n:v:");

        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the property with -n", 1);
        if (array_key_exists('v', $options))
            $newname = $options['v'];
        else
            error("You should specify a new name for the property with -v", 1);

        if (!preg_match("/^[A-Za-z][0-9A-Za-z_]*$/", $newname))
            echo error("This name '$newname' does not respect the naming convention ([A-Za-z][0-9A-Za-z_]*).", 1);

        if (($property = $component->getPropertyPath($name)) == NULL)
            error("A property does not exist with the name '$name'.", 1);
        if ($property->parentProperty == NULL)
        {
            if ($component->getProperty($newname, false) != NULL)
                error("This property name '$newname' already exists.", 1);
        }
        else
        {
            $newpath = $property->parentProperty->path() . "." . $newname;
            if ($component->getPropertyPath($newpath, false) != NULL)
                error("This property name '$newpath' already exists.", 1);
        }

        $property->name = $newname;

        break;

    case "setproperty":
        if (TOOL == "gpcomp")
            error("Component cannot have properties.", 1);

        $options = getopt("a:n:l:t:v:r:s:c:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the property with -n", 1);

        if (($property = $component->getPropertyPath($name)) == NULL)
            error("A property does not exist with the name '$name'.", 1);

        if (array_key_exists('l', $options))
            $caption = $options['l'];
        else
            $caption = $property->caption;
        if (array_key_exists('t', $options))
            $type = $options['t'];
        else
            $type = $property->type;
        if (array_key_exists('v', $options))
            $value = $options['v'];
        else
            $value = $property->value;
        if (array_key_exists('r', $options))
        {
            $range = explode(':', $options['r']);
            if (count($range) != 2)
                error("You should specify a range whith min:max syntax.", 1);
            $min = $range[0];
            $max = $range[1];
        }
        else
        {
            $min = $property->min;
            $max = $property->max;
        }
        if (array_key_exists('s', $options))
            $step = $options['s'];
        else
            $step = $property->step;
        if (array_key_exists('c', $options))
            $onchange = $options['c'];
        else
            $onchange = $property->onchange;

        $property->caption = $caption;
        $property->type = $type;
        $property->value = $value;
        $property->min = $min;
        $property->max = $max;
        $property->step = $step;
        $property->onchange = $onchange;
        break;

    case "setpropertymap":
        if (TOOL == "gpcomp")
            error("Component cannot have properties.", 1);

        $options = getopt("a:n:v:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the property with -n", 1);
        if (array_key_exists('v', $options))
            $propertymap = $options['v'];
        else
            error("You should specify a name for the property with -v", 1);

        $instance = $component->getPropertyPath($name);
        if ($instance == NULL)
            $instance = $component->getParam($name);
        if ($instance == NULL)
            $instance = $component->getParamBitField($name);
        if ($instance == NULL)
            error("A property/bitfield/param does not exist with the name '$name'.", 1);

        $instance->propertymap = $propertymap;

        break;

    // ========================= enums commands ========================
    case "addenum":
        if (TOOL == "gpcomp")
            error("Component cannot have enums.", 1);

        $options = getopt("a:n:v:l:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the enum with -n", 1);
        if (array_key_exists('v', $options))
            $value = $options['v'];
        else
            error("You should specify a value for the enum with -v", 1);
        if (array_key_exists('l', $options))
            $caption = $options['l'];
        else
            $caption = '';

        $subprops = explode('.', $name);
        if (count($subprops) == 0)
            error("Invalid enum name '$name'.", 1);

        $name = $subprops[count($subprops) - 1];
        $propertyPath = array_slice($subprops, 0, count($subprops) - 1);
        $propertyPath = implode('.', $propertyPath);
        $property = $component->getPropertyPath($propertyPath);
        if ($property == NULL)
            error("This property name '$propertyPath' does not exist.", 1);

        if ($property->getPropertyEnum($name, false) != NULL)
            error("This instance name already exists." . " '" . $name . "'", 1);

        if (!preg_match("/^[A-Za-z][0-9A-Za-z_]*$/", $name))
            echo error("This name '$name' does not respect the naming convention ([A-Za-z][0-9A-Za-z_]*).", 1);

        if ($caption == '')
            $caption = $name;

        $propertyEnum = new PropertyEnum();
        $propertyEnum->name = $name;
        $propertyEnum->caption = $caption;
        $propertyEnum->value = $value;

        $property->type = "enum";
        $property->addPropertyEnum($propertyEnum);
        break;

    case "delenum":
        if (TOOL == "gpcomp")
            error("Component cannot have enums.", 1);

        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the enum with -n", 1);

        if ($component->getPropertyEnumPath($name) == NULL)
            error("A enum does not exist with the name '$name'.", 1);

        $component->delPropertyEnumPath($name);
        break;

    case "showenum":
        if (TOOL == "gpcomp")
            error("Component cannot have enums.", 1);

        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name of a property with -n", 1);

        $property = $component->getPropertyPath($name);
        if ($property == NULL)
            error("A property does not exist with the name '$name'.", 1);

        echo "enums :" . "\n";
        foreach ($property->propertyenums as $propertyenum)
        {
            echo "  + " . $propertyenum . "\n";
        }

        break;

    // ========================= global commands =======================
    case "sethelp":
        $options = getopt("a:n:v:");
        if (array_key_exists('v', $options))
            $desc = $options['v'];
        else
            error("You should specify the help text with -v", 1);

        if (array_key_exists('n', $options))
        {
            $name = $options['n'];

            if (($instance = $component->getInstance($name)) == NULL)
                error("An instance does not exist with the name '$name'.", 1);
            $instance->desc = $desc;
        }
        else
            $component->desc = $desc;
        break;

    case "setdraw":
        $options = getopt("a:f:v:");
        if (array_key_exists('v', $options))
        {
            $svgcommands = $options['v'];
            $orgdoc = new DOMDocument;
            if (!$orgdoc->loadXML("<?xml version='1.0'?><svg>" . $svgcommands . "</svg>\n"))
                error("Invalid svg commands", 1);

            $component->setSvgDraw($orgdoc->firstChild);
        }
        elseif (array_key_exists('f', $options))
        {
            $svgfile = $options['f'];
            $orgdoc = new DOMDocument;
            if (!$orgdoc->load($svgfile))
                error("Invalid svg file", 1);

            $component->setSvgDraw($orgdoc->getElementsByTagName("svg")->item(0));
        }
        else
            error("You should specify a svg file with -f or svg commands with -v", 1);
        break;

    case "setcateg":
        $options = getopt("a:v:");
        if (array_key_exists('v', $options))
            $categ = $options['v'];
        else
            error("You should specify a value for the categ with -v", 1);

        $component->categ = $categ;
        break;

    case "categ":
        echo $component->categ . "\n";
        break;

    case "setinfo":
        $options = getopt("a:n:v:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name for the info with -n", 1);
        if (array_key_exists('v', $options))
            $value = $options['v'];
        else
            error("You should specify a value for the info with -v", 1);

        if (($info = $component->getInfo($name)) == NULL)
        {
            $info = new Info();
            $info->name = $name;
            $info->value = $value;
            $component->addInfo($info);
        }
        else
        {
            $info->name = $name;
            $info->value = $value;
        }
        break;

    case "info":
        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
        {
            $name = $options['n'];

            $info = $component->getInfo($name);
            if ($info == NULL)
                error("An info does not exist with the name '$name'.", 1);

            echo "\"" . $info->value . "\"\n";
        }
        else
        {
            foreach ($component->infos as $info)
                echo $info . "\n";
        }
        break;

    // ======================== components commands ====================
    case "addcomponent":
        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name driver for the component with -n", 1);

        if ($component->getComponent($name) != NULL)
            error("The component '$name' is ever added.", 32);

        $newcomp = new Component(NULL);
        $newcomp->driver = $name;
        $component->addComponent($newcomp);
        break;

    case "delcomponent":
        $options = getopt("a:n:");
        if (array_key_exists('n', $options))
            $name = $options['n'];
        else
            error("You should specify a name driver for the component with -n", 1);

        if ($component->getComponent($name) == NULL)
            error("The component '$name' is not added.", 32);

        $component->delComponent($name);
        break;

    case "showcomponent":
        echo "components :" . "\n";
        foreach ($component->components as $subcomponent)
        {
            echo "  + " . $subcomponent->driver . "\n";
        }
        $save = false;

        break;

    // ========================== list commands ========================
    case "listinfo":
        foreach ($component->infos as $info)
            echo $info->name . ' ';
        $save = false;
        break;

    case "listfile":
        foreach ($component->files as $file)
            echo $file->path . ' ';
        $save = false;
        break;

    case "listcomponent":
        foreach ($component->components as $subcomponent)
            echo $subcomponent->driver . ' ';
        $save = false;
        break;

    case "listflow":
        foreach ($component->flows as $flow)
            echo $flow->name . ' ';
        $save = false;
        break;

    case "listparam":
        foreach ($component->params as $param)
            echo $param->name . ' ';
        $save = false;
        break;

    case "listparamdot":
        foreach ($component->params as $param)
            echo $param->name . '. ';
        $save = false;
        break;

    case "listreset":
        foreach ($component->resets as $reset)
            echo $reset->name . ' ';
        $save = false;
        break;

    case "listclock":
        foreach ($component->clocks as $clock)
            echo $clock->name . ' ';
        $save = false;
        break;

    case "listword":
        $save = false;

        $options = getopt("a:w:m:");
        if (array_key_exists('w', $options))
            $word = $options['w'];
        else
            $word = "";
        if (array_key_exists('m', $options))
            $mode = $options['m'];
        else
            $mode = "";

        $wordRes = explode(".", $word);

        if ($mode == "property")
        {
            if (count($wordRes) == 1)
            {
                foreach ($component->properties as $property)
                {
                    echo $property->name . ' ';
                    if (count($property->properties) > 0)
                        echo $property->name . '. ';
                }
                foreach ($component->flows as $flow)
                {
                    if (count($flow->properties) > 0)
                        echo $flow->name . '. ';
                }
                exit(1);
            }
            else
            {
                $instance = $component->getFlow($wordRes[0]);
                if ($instance == NULL)
                    $instance = $component->getProperty($wordRes[0]);
                if ($instance == NULL)
                    exit(1);

                $prefix = $wordRes[0] . '.';
                for ($i = 1; $i < count($wordRes) - 1; $i++)
                {
                    $instance = $instance->getSubProperty($wordRes[$i]);
                    if ($instance == NULL)
                        exit(1);
                    $prefix.=$wordRes[$i] . '.';
                }

                foreach ($instance->properties as $property)
                {
                    echo $prefix . $property->name . ' ';
                    if (count($property->properties) > 0)
                        echo $prefix . $property->name . '. ';
                }
                exit(1);
            }
        }
        elseif ($mode == "bitfield")
        {
            if (count($wordRes) == 1)
            {
                foreach ($component->params as $param)
                {
                    if (count($param->parambitfields) > 0)
                        echo $param->name . '. ';
                }
                exit(1);
            }
            else
            {
                $instance = $component->getParam($wordRes[0]);
                if ($instance == NULL)
                    exit(0);

                foreach ($instance->parambitfields as $parambitfield)
                {
                    echo $instance->name . '.' . $parambitfield->name . ' ';
                }
            }
        }
        break;

    default:
        error("Action $action is unknow.", 1);
}

if ($save)
{
    if(TOOL == "gpcomp")
        $component->saveComponentDef($componentName);
    else
        $component->saveBlockDef($componentName);
}
