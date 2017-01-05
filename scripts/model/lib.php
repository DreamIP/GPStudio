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

require_once("libitem.php");

require_once("process.php");
require_once("io.php");

/**
 * @brief Lib is a container that store all the IPs available in library path.
 * @ingroup base
 * @see Board IO Process Toolchain
 */
class Lib
{
    /**
     * @brief Array of IOs name available in library
     * @var array|LibItem $ios
     */
    public $ios;

    /**
     * @brief Array of boards name available in library
     * @var array|LibItem $boards
     */
    public $boards;

    /**
     * @brief Array of processes name available in library
     * @var array|LibItem $process
     */
    public $processes;

    /**
     * @brief Array of toolchains name available in library
     * @var array|LibItem $toolchain
     */
    public $toolchain;

    /**
     * @brief Array of components name available in library
     * @var array|LibItem $components
     */
    public $components;

    /**
     * @brief Constructor of Lib from an array of library path.
     * @param array|string $libpaths
     */
    function __construct($libpaths)
    {
        $this->ios = array();
        $this->boards = array();
        $this->processes = array();
        $this->toolchains = array();
        $this->components = array();

        if (!is_array($libpaths))
            $libpaths = array($libpaths);

        foreach ($libpaths as $libpath)
        {
            $this->openlib($libpath);
        }
    }

    /**
     * @brief Opens the path and search all the available IP in all of them.
     * @param array|string $libpath Array of library path
     */
    protected function openlib($libpath)
    {
        // process
        $processDir = $libpath . DIRECTORY_SEPARATOR . "process" . DIRECTORY_SEPARATOR;
        $dirs = scandir($processDir);
        foreach ($dirs as $dir)
        {
            if (is_dir($processDir . $dir) and $dir != '.' and $dir != '..')
            {
                $fileName = $processDir . $dir . DIRECTORY_SEPARATOR . $dir . ".proc";
                if (file_exists($fileName))
                    $this->processes[] = new LibItem($dir, $fileName);
            }
        }

        // ios
        $ioDir = $libpath . DIRECTORY_SEPARATOR . "io" . DIRECTORY_SEPARATOR;
        $dirs = scandir($ioDir);
        foreach ($dirs as $dir)
        {
            if (is_dir($ioDir . $dir) and $dir != '.' and $dir != '..')
            {
                $fileName = $ioDir . $dir . DIRECTORY_SEPARATOR . $dir . ".io";
                if (file_exists($fileName))
                    $this->ios[] = new LibItem($dir, $fileName);
            }
        }

        // boards
        $boardDir = $libpath . DIRECTORY_SEPARATOR . "board" . DIRECTORY_SEPARATOR;
        $dirs = scandir($boardDir);
        foreach ($dirs as $dir)
        {
            if (is_dir($boardDir . $dir) and $dir != '.' and $dir != '..')
            {
                $fileName = $boardDir . $dir . DIRECTORY_SEPARATOR . $dir . ".dev";
                if (file_exists($fileName))
                    $this->boards[] = new LibItem($dir, $fileName);
            }
        }

        // toolchains
        $toolchainDir = $libpath . DIRECTORY_SEPARATOR . "toolchain" . DIRECTORY_SEPARATOR;
        $dirs = scandir($toolchainDir);
        foreach ($dirs as $dir)
        {
            if (is_dir($toolchainDir . $dir) and $dir != '.' and $dir != '..')
            {
                $fileName = $toolchainDir . $dir . DIRECTORY_SEPARATOR . $dir . ".php";
                if (file_exists($fileName))
                    $this->toolchains[] = new LibItem($dir, $fileName);
            }
        }

        // components
        $componentsDir = $libpath . DIRECTORY_SEPARATOR . "component" . DIRECTORY_SEPARATOR;
        $dirs = scandir($componentsDir);
        foreach ($dirs as $dir)
        {
            if (is_dir($componentsDir . $dir) and $dir != '.' and $dir != '..')
            {
                $this->components[] = new LibItem($dir, $componentsDir . DIRECTORY_SEPARATOR . $dir);
            }
        }
    }

    /**
     * @brief Prints the list of process (only the name)
     */
    function listprocess()
    {
        foreach ($this->processes as $process)
        {
            echo $process->name . "\n";
        }
    }

    /**
     * @brief Prints the list of devices (only the name)
     */
    function listio()
    {
        foreach ($this->ios as $io)
        {
            echo $io->name . "\n";
        }
    }

    /**
     * @brief Prints the list of board (only the name)
     */
    function listboard()
    {
        foreach ($this->boards as $board)
        {
            echo $board->name . "\n";
        }
    }

    /**
     * @brief Prints the list of toolchain (only the name)
     */
    function listtoolchain()
    {
        foreach ($this->toolchain as $toolchain)
        {
            echo $toolchain->name . "\n";
        }
    }

    /**
     * @brief Prints the list of component (only the name)
     */
    function listcomponent()
    {
        foreach ($this->components as $component)
        {
            echo $component->name . "\n";
        }
    }

    /**
     * @brief Try to open all the process
     */
    function checklib()
    {
        echo "========== Processes ===========\n";
        foreach ($this->processes as $process)
        {
            echo "Trying to open " . $process->name . "...\n";
            $proc = new Process($process->filePath);
            // check ip info
            if (count($proc->infos) < 1)
                warning("Missing IP informations", 1);

            // check flow out properties
            foreach ($proc->flows as $flow)
            {
                if (empty($flow->properties) and $flow->type == "out")
                    warning("Missing properties on output flow '" . $flow->name . "'", 2);
            }

            // check file existance
            foreach ($proc->files as $file)
            {
                if (strpos($file->path, "hwlib:") === 0)
                    $path = str_replace("hwlib:", SUPPORT_PATH . "component" . DIRECTORY_SEPARATOR, $file->path);
                else
                    $path = $proc->path . "/" . $file->path;
                if (!file_exists($path))
                    warning("File '" . $file->name . "' does not exist", 3);
            }

            // check categ not empty
            if ($proc->categ == "")
                warning("Missing IP category", 4);
            echo $proc->name . " OK \n";
        }
        echo "=========== Devices ============\n";
        foreach ($this->ios as $io)
        {
            echo "Trying to open " . $io->name . "...\n";
            $proc = new IO($io->filePath);
            echo $io->name . " OK \n";
        }
    }
}
