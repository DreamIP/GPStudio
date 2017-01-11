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
require_once("clockdomain.php");

require_once("toolchain/hdl/vhdl_generator.php");

require_once("pll.php");

class ClockInterconnect extends Block
{
    public $clock_providers;

    public $clock_receivers;

    public $virtual_pll;

    public $plls;

    public $domains;

    function __construct()
    {
        parent::__construct();
        $this->clock_providers = array();
        $this->clock_receivers = array();
        $this->virtual_pll = array();
        $this->domains = array();
        $this->name = "ci";
        $this->driver = "ci";
    }

    protected function parse_xml($xml=NULL, $dummy2=NULL)
    {
        if (isset($xml->clock_interconnect))
        {
            if (isset($xml->clock_interconnect->domains))
            {
                foreach ($xml->clock_interconnect->domains->domain as $domain)
                {
                    $this->addClockDomain(new ClockDomain($domain));
                }
            }
        }
    }

    function configure($node, $block)
    {
        $this->parse_xml($node->xml);

        $reset = new Reset();
        $reset->name = 'reset';
        $reset->direction = 'in';
        $reset->group = 'reset_n';
        $this->addReset($reset);

        $available_freq = array();
        $needed_freq = array();

        // construct list of clock providers and clock receivers
        foreach ($node->board->clocks as $clock)
        {
            if ($clock->direction == "out")
                array_push($this->clock_providers, $clock);
            else
                array_push($this->clock_receivers, $clock);
        }
        foreach ($node->blocks as $block)
        {
            foreach ($block->clocks as $clock)
            {
                if ($clock->direction == "out")
                    array_push($this->clock_providers, $clock);
                else
                    array_push($this->clock_receivers, $clock);
            }
        }

        // construct list of available clocks
        $inFreqs = array();
        foreach ($this->clock_providers as $clock_provider)
        {
            array_push($available_freq, array($clock_provider->typical, $clock_provider->shift));
            array_push($inFreqs, $clock_provider->typical);

            $clock = new Clock();
            $clock->name = $clock_provider->name;
            $clock->domain = $clock_provider->domain;
            $clock->typical = $clock_provider->typical;
            $clock->shift = $clock_provider->shift;

            if ($clock->shift == 0)
            {
                $clock->net = 'clk_' . Clock::hdlFreq($clock->typical);
                $clock_provider->net = $clock->net;
            }
            else
            {
                $clock->net = 'clk_' . Clock::hdlFreq($clock->typical) . '_' . $clock->shift;
                $clock_provider->net = $clock->net;
            }

            $clock->direction = "in";
            $this->addClock($clock);
        }

        // construct list of plls
        $this->plls = array();
        $pllAttr = $node->board->toolchain->getRessourceAttributes('pll');
        $maxPLL = $pllAttr['maxPLL'];
        $clkByPLL = $pllAttr['clkByPLL'];
        $vcomin = $pllAttr['vcomin'];
        $vcomax = $pllAttr['vcomax'];
        $mulmax = $pllAttr['mulmax'];
        $divmax = $pllAttr['divmax'];
        $pllclkcanbechain = $pllAttr['pllclkcanbechain'];
        for ($i = 0; $i < $maxPLL; $i++)
        {
            $pll = new PLL();
            $pll->inFreqs = $inFreqs;
            $pll->clkByPLL = $clkByPLL;
            $pll->vcomin = $vcomin;
            $pll->vcomax = $vcomax;
            $pll->mulmax = $mulmax;
            $pll->divmax = $divmax;
            $pll->canbechain = $pllclkcanbechain;
            array_push($this->plls, $pll);
        }

        // construct list of needed clocks for clock with a defined freq
        foreach ($this->clock_receivers as $clock)
        {
            if (isset($clock->typical) and $clock->typical != 0)
            {
                if (!in_array(array($clock->typical, $clock->shift), $available_freq))
                {
                    array_push($needed_freq, array($clock->typical, $clock->shift));
                    array_push($available_freq, array($clock->typical, $clock->shift));
                }
            }
        }

        // construct list of needed clocks for clock with min max interval
        foreach ($this->clock_receivers as $clock)
        {
            if (isset($clock->min) and isset($clock->max) and ( !isset($clock->typical) or $clock->typical == 0))
            {
                $find = false;
                foreach ($available_freq as $freq)
                {
                    $clkFreq = $freq[0];
                    $clkShift = $freq[1];

                    // if an available clock enter in the interval, set the typical clock to this frequency
                    if ($clkFreq >= $clock->min and $clkFreq <= $clock->max and $clkShift == $clock->shift)
                    {
                        $clock->typical = $clkFreq;
                        $clock->shift = $clkShift;
                        $find = true;
                    }
                }
                if (!$find)
                {
                    // if no clock is ok, add a the minimal frequency to the needed clock frequency
                    $clock->typical = $clock->min;
                    array_push($needed_freq, array($clock->typical, $clock->shift));
                    array_push($available_freq, array($clock->typical, $clock->shift));
                }
            }
        }

        // resolve domain with ratio clock
        foreach ($this->clock_receivers as $clock)
        {
            if (!isset($clock->typical))
            {
                if (array_key_exists($clock->domain, $this->domains))
                {
                    $clock->typical = $this->domains[$clock->domain]->typical * $clock->ratio;
                    if (!in_array(array($clock->typical, $clock->shift), $available_freq))
                    {
                        array_push($needed_freq, array($clock->typical, $clock->shift));
                        array_push($available_freq, array($clock->typical, $clock->shift));
                    }
                }
                else
                {
                    $clock->typical = $this->clock_providers[0]->typical;

                    // TODO print this warning at generate step
                    //warning("No clock specified in .node file for domain '".$clock->domain."', automaticaly choose : ".Clock::formatFreq($clock->typical),20,'CI');

                    $domain = new ClockDomain();
                    $domain->name = $clock->domain;
                    $domain->typical = $clock->typical / $clock->ratio;
                    $this->addClockDomain($domain);
                }
            }
        }

        // add clock provider foreach needed clocks
        foreach ($needed_freq as $needed_freq_item)
        {
            $clkFreq = $needed_freq_item[0];
            $clkShift = $needed_freq_item[1];

            $clockProvider = new Clock();
            $clockProvider->typical = $clkFreq;
            $clockProvider->shift = $clkShift;
            if ($clockProvider->shift == 0)
            {
                $clockProvider->net = 'clk_' . Clock::hdlFreq($clkFreq);
            }
            else
            {
                $clockProvider->net = 'clk_' . Clock::hdlFreq($clkFreq) . '_' . $clkShift;
            }
            $clockProvider->name = $clockProvider->net;

            $clockProvider->domain = "clk_" . $clkFreq;
            $clockProvider->direction = "out";

            $this->addClock($clockProvider);
            array_push($this->clock_providers, $clockProvider);
        }

        // create clocks foreach clock provider
        foreach ($this->clock_providers as $clock_provider)
        {
            // add the clock source to the first pll can accept the clock
            if ($clock_provider->parentBlock == $this)
            {
                $ok = false;
                foreach ($this->plls as $pll)
                {
                    if ($pll->canBeAdded($clock_provider))
                    {
                        $pll->addFreq($clock_provider);
                        $ok = true;

                        // remove clock from needed_freq list
                        unset($needed_freq[array_search(array($clock_provider->typical, $clock_provider->shift), $needed_freq)]);
                        break;
                    }
                }
                //if(!$ok) error("Cannot provide clock at ".Clock::formatFreq($clockProvider->typical),20,"CI");
            }
        }

        // create virtual pll (counter...) for remaining clocks
        $this->virtual_pll = $needed_freq;

        // associate clock net
        foreach ($this->clock_receivers as $clockin)
        {
            if ($clockin->typical == 0)
                $clockin->net = "'0'";
            elseif (empty($clockin->net))
            {
                foreach ($this->clock_providers as $clocksrc)
                {
                    if ($clocksrc->typical == $clockin->typical)
                    {
                        $clockin->net = $clocksrc->net;
                    }
                }
            }
        }
    }

    function generate($node, $block, $path, $language)
    {
        $generator = new VHDL_generator('ci');

        if (!$ci = $node->getBlock('ci'))
            return;
        $generator->fromBlock($ci);

        array_push($generator->libs, "library altera_mf;");
        array_push($generator->libs, "use altera_mf.all;");

        $declare = '';
        $params = array();
        $params['pll'] = $this->plls[0];
        $declare.=$node->board->toolchain->getRessourceDeclare('pll', $params);
        $generator->declare = $declare;

        $code = '';

        // instantiate plls
        $pllId = 0;
        foreach ($this->plls as $pll)
        {
            if (!$pll->isempty())
            {
                $generator->addSignal('pll' . $pllId . '_in', 2, 'std_logic_vector');
                $generator->addSignal('pll' . $pllId . '_out', $pll->clkByPLL, 'std_logic_vector');

                $params['pllname'] = 'pll' . $pllId;
                $params['pll'] = $pll;

                $params['clkin'] = $pll->freqIn();
                $params['clks'] = $pll->clksshift;
                $code.=$node->board->toolchain->getRessourceInstance('pll', $params);

                $clkId = 0;
                foreach ($pll->clksshift as $clockNeed)
                {
                    $clockFreq = $clockNeed[0];
                    $clockShift = $clockNeed[1];

                    if ($clockShift == 0)
                        $net = 'clk_' . Clock::hdlFreq($clockFreq);
                    else
                        $net = 'clk_' . Clock::hdlFreq($clockFreq) . '_' . $clockShift;

                    $code.='	' . $net . ' <= pll' . $pllId . '_out(' . $clkId . ');' . "\n";

                    messageVerbosity('Generate Clock at ' . Clock::formatFreq($clockFreq) . ' from ' . Clock::formatFreq($params['clkin']) . ' in pll' . $pllId, 'CI');

                    $clkId++;
                }
                $code.='	pll' . $pllId . '_in(0) <= ' . $this->providerByFreq($params['clkin'])->name . ';' . "\n" . "\n";

                $pllId++;
            }
        }

        // generate virtual plls
        if (!empty($this->virtual_pll))
        {
            $code.="	-- virtual plls" . "\r\n";

            // each virtual pll is a process counter, all based on the SAME input freq
            $input_clock = $this->clock_providers[0]->typical;
            foreach ($this->virtual_pll as $virtual_pll)
            {
                $clock = $virtual_pll[0];
                $shift = $virtual_pll[1];

                $code.='	-- clock counter at ' . Clock::formatFreq($virtual_pll[0]) . " with $shift degree phase shift" . "\r\n";

                if ($shift == 0)
                    $clockname = 'clk_' . Clock::hdlFreq($clock);
                else
                    $clockname = 'clk_' . Clock::hdlFreq($clock) . '_' . $shift;

                $clock_div = ceil($input_clock / $clock) / 2;
                $counter_size = ceil(log($clock_div, 2));
                $clock_div -= 1;

                if ($shift < 0 or $shift > 360)
                    error("invalid clock shift $shift", 32, "CI");
                //~ if($shift>=180) {$beginvalue = 1; $shift-=180;} else {$beginvalue = 0;}
                //~ $beginphase = ceil(($shift / 180) * $clock_div);
                $beginphase = 0;
                $beginvalue = 0;

                $signal_clock_name = $clockname . '_s';
                $signal_counter_name = $clockname . '_counter_s';

                $generator->addSignalComment(" signal for $clockname");
                $generator->addSignal($signal_clock_name, 1, 'std_logic');
                $generator->addSignal($signal_counter_name, $counter_size, 'unsigned');

                $code.="	virtual_pll_$clockname : process (clk_proc, reset)" . "\n";
                $code.="	begin" . "\n";
                $code.="		if(reset='0') then" . "\n";
                $code.="			$signal_clock_name <= '$beginvalue';" . "\n";
                $code.="			$signal_counter_name <= to_unsigned($beginphase, $counter_size);" . "\n";
                $code.="		elsif(rising_edge(clk_proc)) then" . "\n";
                $code.="			if($signal_counter_name<$clock_div) then" . "\n";
                $code.="				$signal_counter_name <= $signal_counter_name + 1;" . "\n";
                $code.="			else" . "\n";
                $code.="				$signal_counter_name <= to_unsigned(0, $counter_size);" . "\n";
                $code.="				$signal_clock_name <= not($signal_clock_name);" . "\n";
                $code.="			end if;" . "\n";
                $code.="		end if;" . "\n";
                $code.="	end process;" . "\n";
                $code.="	$clockname <= $signal_clock_name;" . "\n";
                $code.="\n";
            }
        }

        $generator->code = $code;

        $file = new File();
        $file->name = 'ci.vhd';
        $file->path = 'ci.vhd';
        $file->group = 'hdl';
        $file->type = 'vhdl';
        $file->generated = true;
        $ci->addFile($file);

        $this->path = $path . DIRECTORY_SEPARATOR;

        $filename = $path . DIRECTORY_SEPARATOR . $file->name;
        $generator->save_as_ifdiff($filename);

        $this->create_sdc($path);
    }

    public function create_sdc($path)
    {
        $content = "# ----------------------------------------------------------------------------" . "\r\n";
        $content .= "# Create Generated Clock" . "\r\n";
        $content .= "# ----------------------------------------------------------------------------" . "\r\n";
        $content .= "" . "\r\n";
        $content .= "derive_pll_clocks -create_base_clocks" . "\r\n";
        $content .= "" . "\r\n";
        $content .= "# Automatically calculate clock uncertainty to jitter and other effects." . "\r\n";
        $content .= "derive_clock_uncertainty" . "\r\n";

        $filename = $this->parentNode->name . '.sdc';

        // save file if it's different
        $needToReplace = false;
        if (file_exists($path . DIRECTORY_SEPARATOR . $filename))
        {
            $handle = fopen($path . DIRECTORY_SEPARATOR . $filename, 'r');
            $actualContent = fread($handle, filesize($path . DIRECTORY_SEPARATOR . $filename));
            fclose($handle);
            if ($actualContent != $content)
                $needToReplace = true;
        }
        else
            $needToReplace = true;

        if ($needToReplace)
        {
            $handle = null;
            if (!$handle = fopen($path . DIRECTORY_SEPARATOR . $filename, 'w'))
                error("$filename cannot be openned", 5, "CI");
            if (fwrite($handle, $content) === FALSE)
                error("$filename cannot be written", 5, "Vhdl Gen");
            fclose($handle);
        }

        $file = new File();
        $file->name = $filename;
        $file->path = $filename;
        $file->group = 'hdl';
        $file->type = 'sdc';
        $file->generated = true;
        $this->addFile($file);
    }

    public function type()
    {
        return 'ci';
    }

    public function getXmlElement($xml, $format)
    {
        if ($format == "complete")
        {
            $xml_element = parent::getXmlElement($xml, $format);

            // clock_providers
            $xml_clocks = $xml->createElement("clock_providers");
            foreach ($this->clock_providers as $clock)
            {
                $clock_element = $clock->getXmlElement($xml, $format);

                // blockName
                $att = $xml->createAttribute('blockName');
                $att->value = $clock->parentBlock->name;
                $clock_element->appendChild($att);

                $xml_clocks->appendChild($clock_element);
            }
            $xml_element->appendChild($xml_clocks);

            // clock_receivers
            $xml_clocks = $xml->createElement("clock_receivers");
            foreach ($this->clock_receivers as $clock)
            {
                $clock_element = $clock->getXmlElement($xml, $format);

                // blockName
                $att = $xml->createAttribute('blockName');
                $att->value = $clock->parentBlock->name;
                $clock_element->appendChild($att);

                $xml_clocks->appendChild($clock_element);
            }
            $xml_element->appendChild($xml_clocks);

            return $xml_element;
        }
        elseif ($format == "project")
        {
            $xml_element = $xml->createElement("clock_interconnect");

            // domains
            $xml_flow_connects = $xml->createElement("domains");
            foreach ($this->domains as $domain)
            {
                $xml_flow_connects->appendChild($domain->getXmlElement($xml, $format));
            }
            $xml_element->appendChild($xml_flow_connects);

            return $xml_element;
        }
        return NULL;
    }

    static function ppcm($nombre, $nombre2)
    {
        $res = $nombre * $nombre2;
        while ($nombre > 1)
        {
            $reste = $nombre % $nombre2;
            if ($reste == 0)
            {
                return $res / $nombre2;
            }
            $nombre = $nombre2;
            $nombre2 = $reste;
        }
        return $nombre2;
    }

    static function ppcm_array($array, $a = 0)
    {
        $b = array_pop($array);
        return ($b === null) ? (int) $a : ClockInterconnect::ppcm_array($array, ClockInterconnect::ppcm($a, $b));
    }

    function providerByFreq($freq)
    {
        foreach ($this->clock_providers as $clock)
        {
            if ($clock->typical == $freq)
                return $clock;
        }
        return null;
    }

    /** Add a clock domain to the block
     *  @param ClockDomain $domain clock domain to add to the block * */
    function addClockDomain($domain)
    {
        $this->domains[$domain->name] = $domain;
    }

    /** return a reference to the clock domain with the name $name, if not found, return null
     *  @param string $name name of the clock domain to search
     *  @return ClockDomain found clock domain * */
    function getClockDomain($name)
    {
        return $this->domain[$name];
    }
}
