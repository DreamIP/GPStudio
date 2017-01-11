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

require_once("lib.php");
require_once("gpstudio.php");

$options = getopt("a:");
if (array_key_exists('a', $options))
    $action = $options['a'];
else
    error("You should specify an action with -a" . "\n", 1);

$lib = new Lib(SUPPORT_PATH);

switch ($action)
{
    // =========================== global commands ===========================
    case "-h":
    case "--help":
        echo "# gplib command line tool to manage gpstudio lib (" . VERSION . ")" . "\n";
        echo "gplib listprocess" . "\n";
        echo "gplib listio" . "\n";
        echo "gplib listboard" . "\n";
        echo "gplib listtoolchain" . "\n";
        echo "gplib listcomponent" . "\n";
        echo "gplib checklib" . "\n";
        break;
    case "-v":
    case "--version":
        echo "# gplib command line tool to manage gpstudio lib (" . VERSION . ")" . "\n";
        break;

    // =========================== list commands ===========================
    case "listprocess":
        $lib->listprocess();
        echo "\n";
        break;

    case "listio":
        $lib->listio();
        echo "\n";
        break;

    case "listboard":
        $lib->listboard();
        echo "\n";
        break;

    case "listtoolchain":
        $lib->listtoolchain();
        echo "\n";
        break;

    case "listcomponent":
        $lib->listcomponent();
        echo "\n";
        break;

    case "checklib":
        $lib->checklib();
        echo "\n";
        break;

    default:
        error("Action $action is unknow.", 1);
}
