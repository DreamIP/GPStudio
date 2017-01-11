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

require_once("ci.php");

class PLL
{
    public $inFreqs;

    public $outFreqs;

    public $vco;

    public $mul;

    public $clkByPLL;

    public $vcomin;

    public $vcomax;

    public $mulmax;

    public $divmax;

    public $dummyClc;

    public $canbechain;

    function __construct()
    {
        $this->inFreqs = array();
        $this->outFreqs = array();
        $this->clksshift = array();

        $this->divmax = array();

        $this->clocks = array();

        $this->dummyClc = 0;
        $this->canbechain = false;
    }

    function freqIn()
    {
        $inFreq = array_values($this->inFreqs);
        $inFreq = $inFreq[0];
        $this->mul = ceil($this->vco / $inFreq);
        return $inFreq;
    }

    function isempty()
    {
        return empty($this->outFreqs);
    }

    function canBeAdded($clock)
    {
        $idPll = count($this->outFreqs) + $this->dummyClc;
        if ($idPll >= $this->clkByPLL)
            return false;

        $find = false;
        foreach ($this->inFreqs as $inFreq)
        {
            $vco = ClockInterconnect::ppcm_array(array_merge($this->outFreqs, array($clock->typical, $inFreq)));
            if ($vco < $this->vcomin)
            {
                $vco = ceil(($this->vcomin) / $vco) * $vco;
            }
            //if($vco>$this->vcomax) return false;
            if ($vco <= $this->vcomax)
                $find = true;
        }
        if ($find == false)
            return false;

        $div = ceil($vco / $clock->typical);
        if ($this->canbechain)
        {
            if ($div > $this->divmax[$idPll] * $this->divmax[$idPll])
                return false;
            if ($div > $this->divmax[$idPll])
            {
                if (count($this->outFreqs) + $this->dummyClc + 1 >= $this->clkByPLL)
                    return false;
            }
        }
        else
        {
            if ($div > $this->divmax[$idPll])
                return false;
        }

        $can = false;
        foreach ($this->inFreqs as $inFreq)
        {
            $mul = ceil($vco / $inFreq);
            if ($mul < $this->mulmax)
                $can = true;
        }

        return $can;
    }

    function addFreq($clock)
    {
        $idPll = count($this->outFreqs) + $this->dummyClc;
        if ($idPll >= $this->clkByPLL)
            return;

        $vco = ClockInterconnect::ppcm_array(array_merge($this->outFreqs, array($clock->typical)));
        if ($vco < $this->vcomin)
        {
            $vco = ceil($this->vcomin / $vco) * $vco;
        }
        if ($vco > $this->vcomax)
            return;

        foreach ($this->inFreqs as $inFreq)
        {
            $vco = ClockInterconnect::ppcm_array(array_merge($this->outFreqs, array($clock->typical, $inFreq)));
            if ($vco < $this->vcomin)
            {
                $vco = ceil($this->vcomin / $vco) * $vco;
            }
            if ($vco > $this->vcomax)
            {
                if (($key = array_search($inFreq, $this->inFreqs)) !== false)
                    unset($this->inFreqs[$key]);
            }

            $mul = ceil($vco / $inFreq);
            if ($mul > $this->mulmax)
            {
                if (($key = array_search($inFreq, $this->inFreqs)) !== false)
                    unset($this->inFreqs[$key]);
            }
        }

        $this->vco = $vco;

        array_push($this->outFreqs, $clock->typical);
        array_push($this->clksshift, array($clock->typical, $clock->shift));
        array_push($this->clocks, $clock);

        $div = ceil($vco / $clock->typical);
        if ($div > $this->divmax[$idPll])
        {
            $this->dummyClc++;
        }
    }
}
