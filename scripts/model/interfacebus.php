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
 * InterfaceBus are used in Block::interfaces as a list. An interface as slave,
 * (pi_slave) or master (pi_master), slave connection (pi_slave_conn) or master
 * connection (pi_master_conn)
 * @brief Define a bus interface for PI
 * @see Block PI
 * @ingroup base
 */
class InterfaceBus
{
    /**
     * @brief Name of the interface
     * @var string $name
     */
    public $name;

    /**
     * @brief Name of the block
     * @var string $blockname
     */
    public $blockname;

    /**
     * bi_master bi_slave bi_master_conn bi_slave_conn
     * @brief Type of connection
     * @var string $type
     */
    public $type;

    /**
     * @brief Sife of the adress bus interface
     * @var int $size_addr
     */
    public $size_addr;

    /**
     * 
     * @param string $name default value contructor
     * @param string $blockname default value contructor
     * @param string $type default value contructor
     * @param int $size_addr default value contructor
     */
    function __construct($name, $blockname, $type, $size_addr)
    {
        $this->name = $name;
        $this->blockname = $blockname;
        $this->type = $type;
        $this->size_addr = $size_addr;
    }
}
