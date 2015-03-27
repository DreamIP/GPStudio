<?php

define("LIB_PATH", realpath(dirname(__FILE__)).DIRECTORY_SEPARATOR);
set_include_path(get_include_path() . PATH_SEPARATOR . LIB_PATH);
set_include_path(get_include_path() . PATH_SEPARATOR . LIB_PATH.DIRECTORY_SEPARATOR.'scripts');
set_include_path(get_include_path() . PATH_SEPARATOR . LIB_PATH.DIRECTORY_SEPARATOR.'system_interconnect');

require("node.php");
require("toolchain.php");
require("gpstudio.php");

if(count($argv)<2) error("Please give a config file node as argument."."\n",1);
$config_node_file = $argv[1];
if(!file_exists($config_node_file)) error("The config file '$config_node_file' does'nt exist.",1);

// create node data structure from config file
$node = new Node($config_node_file);

// create toolchain depend of the config node
$toolchain = Toolchain::load('altera_quartus');
$toolchain->configure_project($node);
$toolchain->generate_project($node, getcwd());

$node->saveXml("node_generated.xml");

?>
