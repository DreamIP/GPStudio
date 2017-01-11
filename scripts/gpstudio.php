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

/**
 * @mainpage
 *
 * This document is the complete documentation of GPStudio backend. It composed of two parts :
 * 	- the model and backend develloper documentation
 * 	- the user guide documentation
 *
 * @section modelbckend model and backend
 *
 * @section usrguide user guide
 *
 * @defgroup base Base script model
 * */

define("VERSION", "v1.20");

if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN')
{
    $txtred = ""; # Red
    $txtorange = ""; # Orange
    $txtgreen = ""; # Green
    $txtrst = "";    # Text Reset
}
else
{
    $txtred = "\e[31m"; # Red
    $txtorange = "\e[33m"; # Orange
    $txtgreen = "\e[32m"; # Green
    $txtrst = "\e[0m";    # Text Reset
}

$warningCount = 0;
$lastCtx = '';
$setVerbosityOption = true;

function setVerbosity($option)
{
    global $setVerbosityOption;
    $setVerbosityOption = $option;
}

function error($errorText, $errorCode, $errorCtx = "")
{
    global $txtrst;
    global $txtred;

    echo $txtred;
    if (!empty($errorCtx))
        echo "[$errorCtx] ";
    echo "Error ($errorCode) : " . $errorText . "\n";
    echo $txtrst;
    exit(1);
}

function warning($warningText, $warningCode, $warningCtx = "")
{
    global $txtrst;
    global $txtorange;
    global $warningCount;
    global $lastCtx;
    global $setVerbosityOption;

    if ($setVerbosityOption)
    {
        if ($lastCtx != '' and $lastCtx != $warningCtx)
            echo "\n";

        echo $txtorange;
        if (!empty($warningCtx))
            echo "[$warningCtx] ";
        echo "Warning ($warningCode) : " . $warningText . "\n";
        echo $txtrst;

        $warningCount++;
        $lastCtx = $warningCtx;
    }
}

function message($messageText, $messageCtx = "")
{
    global $txtrst;
    global $txtgreen;
    global $lastCtx;
    global $setVerbosityOption;

    if ($setVerbosityOption)
    {
        if ($lastCtx != '' and $lastCtx != $messageCtx)
            echo "\n";

        echo $txtgreen;
        if (!empty($messageCtx))
            echo "[$messageCtx] ";
        echo $messageText . "\n";
        echo $txtrst;

        $lastCtx = $messageCtx;
    }
}

function messageVerbosity($messageText, $messageCtx = "")
{
    global $argv;
    if (in_array("verbosity", $argv))
    {
        message($messageText, $messageCtx);
    }
}

function findproject()
{
    $projects = array();
    $files = scandir(getcwd());
    foreach ($files as $file)
    {
        if (substr($file, -5) === ".node")
        {
            $projects[] = $file;
        }
    }
    if (count($projects) == 1)
        return $projects[0];
    elseif (count($projects) > 1)
        error ("Your project contains more than one node file, node files must be put in separated directory.", 1);
    else
        return '';
}

function findprocess()
{
    $process = array();
    $files = scandir(getcwd());
    foreach ($files as $file)
    {
        if (substr($file, -5) === ".proc")
        {
            $process[] = $file;
        }
    }
    if (count($process) == 1)
        return $process[0];
    elseif (count($process) > 1)
        error ("Your project contains more than one process file, proc files must be put in separated directory.", 1);
    else
        return '';
}

function findio()
{
    $io = array();
    $files = scandir(getcwd());
    foreach ($files as $file)
    {
        if (substr($file, -3) === ".io")
        {
            $io[] = $file;
        }
    }
    if (count($io) == 1)
        return $io[0];
    elseif (count($io) > 1)
        error ("Your project contains more than one io file, io files must be put in separated directory.", 1);
    else
        return '';
}

function findcomp()
{
    $comp = array();
    $files = scandir(getcwd());
    foreach ($files as $file)
    {
        if (substr($file, -5) === ".comp")
        {
            $comp[] = $file;
        }
    }
    if (count($comp) == 1)
        return $comp[0];
    elseif (count($comp) > 1)
        error ("Your project contains more than one component file, comp files must be put in separated directory.", 1);
    else
        return '';
}

function listprocess()
{
    $directory = SUPPORT_PATH . "process" . DIRECTORY_SEPARATOR;
    echo $directory;
    $files = scandir($directory);
    foreach ($files as $file)
    {
        if ($file != '.' and $file != '..')
        {
            echo $file . " ";
        }
    }
}

function listio()
{
    $directory = SUPPORT_PATH . "io" . DIRECTORY_SEPARATOR;
    echo $directory;
    $files = scandir($directory);
    foreach ($files as $file)
    {
        if ($file != '.' and $file != '..')
        {
            echo $file . " ";
        }
    }
}

function copy_with_rights($src, $dest)
{
    copy($src, $dest);
    $srcright = fileperms($src);
    if (file_exists($dest))
    {
        if (fileperms($dest) != $srcright)
            chmod($dest, $srcright);
    }
    else
        chmod($dest, $srcright);
}

function mkdir_rec($dir_path)
{
    $dir_path = str_replace("\\", "/", $dir_path);
    $dirs = explode("/", $dir_path);
    for ($i = 1; $i <= count($dirs); $i++)
    {
        $path = implode(DIRECTORY_SEPARATOR, array_slice($dirs, 0, $i));
        if (!is_dir($path) and $path != "")
            mkdir($path);
    }
}

function cpy_dir($source, $dest, $filtered=array())
{
    if (is_dir($source))
    {
        $dir_handle = opendir($source);
        while ($file = readdir($dir_handle))
        {
            if ($file != "." && $file != "..")
            {
                if (is_dir($source . DIRECTORY_SEPARATOR . $file))
                {
                    if (!is_dir($dest . DIRECTORY_SEPARATOR . $file))
                    {
                        mkdir($dest . DIRECTORY_SEPARATOR . $file);
                    }
                    cpy_dir($source . DIRECTORY_SEPARATOR . $file, $dest . DIRECTORY_SEPARATOR . $file, $filtered);
                }
                else
                {
                    if (is_link($source . DIRECTORY_SEPARATOR . $file))
                    {
                        symlink(readlink($source . DIRECTORY_SEPARATOR . $file), $dest . DIRECTORY_SEPARATOR . $file);
                    }
                    else
                    {
                        $ext = pathinfo($file, PATHINFO_EXTENSION);
                        if(!in_array($ext, $filtered))
                        {
                            copy_with_rights($source . DIRECTORY_SEPARATOR . $file, $dest . DIRECTORY_SEPARATOR . $file);
                        }
                    }
                }
            }
        }
        closedir($dir_handle);
    }
    else
    {
        copy_with_rights($source, $dest);
    }
}

function getRelativePath($path, $dir = NULL)
{
    if ($dir == NULL)
        $dir = getcwd();

    $path = realpath($path);
    $dir = realpath($dir);

    if (is_dir($path))
    {
        $dirSrc = realpath($path);
        $srcName = '';
    }
    else
    {
        $dirSrc = dirname($path);
        $srcName = basename($path);
    }

    if ($dirSrc == $dir)
    {
        return $srcName; // file in dir
    }

    if (strpos($dir, $dirSrc) === 0)
    {
        $back_dir = str_replace($dirSrc, "", $dir);
        $back_count = substr_count($back_dir, DIRECTORY_SEPARATOR);
        return str_repeat(".." . DIRECTORY_SEPARATOR, $back_count) . $srcName;
    }

    if (strpos($dirSrc, $dir) === 0)
    {
        $back_dir = str_replace($dir . DIRECTORY_SEPARATOR, "", $dirSrc);
        return $back_dir;
    }

    return $path;
}

function saveIfDifferent($fileName, $content)
{
    // save file if it's different
    $needToReplace = false;
    if (file_exists($fileName))
    {
        $handle = fopen($fileName, 'r');
        $actualContent = fread($handle, filesize($fileName));
        fclose($handle);
        if ($actualContent != $content)
            $needToReplace = true;
    }
    else
        $needToReplace = true;

    if ($needToReplace)
    {
        $handle = null;
        if (!$handle = fopen($fileName, 'w'))
            error("$fileName cannot be openned", 5, "GPStudio");
        if (fwrite($handle, $content) === FALSE)
            error("$fileName cannot be written", 5, "GPStudio");
        fclose($handle);
    }
}

function cloneSvg($src, $xml, $dest)
{
    $svgNode = $xml->createElement($src->nodeName);
    foreach ($src->attributes as $name => $attrNode)
    {
        $att = $xml->createAttribute($name);
        $att->value = $attrNode->value;
        $svgNode->appendChild($att);
    }
    foreach ($src->childNodes as $node)
    {
        if ($node->nodeType == XML_TEXT_NODE)
        {
            if(!empty($node->nodeValue) and $src->childNodes->length==1)
                $svgNode->nodeValue = $node->nodeValue;
        }
        if ($node->nodeType == XML_ELEMENT_NODE)
            cloneSvg($node, $xml, $svgNode);
    }
    $dest->appendChild($svgNode);
}
