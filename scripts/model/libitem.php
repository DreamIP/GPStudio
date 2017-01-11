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

/**
 * @brief LibItem is an item of the Lib container
 * @ingroup base
 * @see Lib Board IO Process Toolchain
 */
class LibItem
{
    /**
     * @brief name of LibItem
     * @var string $name
     */
    public $name;

    /**
     * @brief file path of the LibItem containing file, path and extension.
     * @var string $filePath 
     */
    public $filePath;

    /**
     * @brief Constructor of a LibItem
     * @param string $name name of LibItem
     * @param string $filePath file path of the LibItem containing file, path
     * and extension.
     */
    function __construct($name, $filePath)
    {
        $this->name = $name;
        $this->filePath = $filePath;
    }
}
