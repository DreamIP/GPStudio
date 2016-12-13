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

require_once("block.php");
require_once("vhdl_generator.php");

class Block_generator
{
    public $block;

    private $slave_block;

    private $process_block;

    public $slave_generator;

    public $block_generator;

    public $process_generator;

    function __construct($block = NULL)
    {
        $this->slave_block = NULL;
        $this->process_block = NULL;

        $this->block_generator = NULL;
        $this->slave_generator = NULL;
        $this->process_generator = NULL;

        if ($block != NULL)
            $this->fromBlock($block);
        else
            $block = NULL;
    }

    static function hex($value, $width)
    {
        return 'x"' . str_pad(strtoupper(dechex($value)), ceil($width / 4.0), '0', STR_PAD_LEFT) . '"';
    }

    static function bin($value, $width)
    {
        return '"' . str_pad(strtoupper(decbin($value)), $width, '0', STR_PAD_LEFT) . '"';
    }

    function fromBlock($block)
    {
        // top block creation
        $this->block = $block;

        if ($this->block == NULL)
            return;

        // slave block creation
        if(isset($this->block->pi_size_addr_rel))
        {
            if ($this->block->pi_size_addr_rel > 0)
            {
                array_push($this->block->interfaces, new InterfaceBus("bus_sl", $this->block->name, "pi_slave", $this->block->pi_size_addr_rel));

                $this->slave_block = new IO();
                $this->slave_block->name = $this->block->name . '_slave';
                $this->slave_block->driver = $this->block->name . '_slave';
                $this->slave_block->clocks = array();
                $this->slave_block->interfaces = $this->block->interfaces;
            }
        }

        // slave block creation
        $this->process_block = new IO();
        $this->process_block->name = $this->block->name . '_process';
        $this->process_block->driver = $this->block->name . '_process';
        $this->process_block->clocks = array();
        $this->process_block->flows = $this->block->flows;
        $this->process_block->params = $this->block->params;

        // clocks copy
        foreach ($this->block->clocks as $clock)
        {
            $nclock = new Clock();
            $nclock->name = $clock->name;
            $nclock->net = $clock->name;
            $this->process_block->addClock($nclock);
            if ($this->slave_block != NULL)
                $this->slave_block->addClock($nclock);
        }

        // resets copy
        foreach ($this->block->resets as $reset)
        {
            $nreset = new Reset();
            $nreset->name = $reset->name;
            $nreset->group = $reset->name;
            $this->process_block->addReset($nreset);
            if ($this->slave_block != NULL)
                $this->slave_block->addReset($nreset);
        }

        // registers
        if ($this->slave_block != NULL)
        {
            foreach ($this->block->params as $param)
            {
                if ($param->hard == false)
                {
                    if (empty($param->parambitfields))
                    {
                        $size = 32;

                        // port connexion
                        $port_param_in = new Port();
                        $port_param_in->name = $param->name;
                        $port_param_in->type = "in";
                        $port_param_in->size = $size;
                        $this->process_block->addExtPort($port_param_in);

                        $port_param_out = new Port();
                        $port_param_out->name = $param->name;
                        $port_param_out->type = "out";
                        $port_param_out->size = $size;
                        $this->slave_block->addExtPort($port_param_out);
                    }
                    else
                    {
                        foreach ($param->parambitfields as $parambitfields)
                        {
                            $size = count($parambitfields->bitfieldlist);

                            if ($size > 0)
                            {
                                // port connexion
                                $port_param_in = new Port();
                                $port_param_in->name = $param->name . '_' . $parambitfields->name;
                                $port_param_in->type = "in";
                                $port_param_in->size = $size;
                                $this->process_block->addExtPort($port_param_in);

                                $port_param_out = new Port();
                                $port_param_out->name = $param->name . '_' . $parambitfields->name;
                                $port_param_out->type = "out";
                                $port_param_out->size = $size;
                                $this->slave_block->addExtPort($port_param_out);
                            }
                        }
                    }
                }
            }
        }
    }

    public function generateTb($path)
    {
        if ($this->block == NULL)
            return;

        $this->block_generator = new VHDL_generator($this->block->name . '_tb');
        array_push($this->block_generator->libs, "use std.textio.all;");

        // clocks generation
        $codeClks = '';
        foreach ($this->block->clocks as $clock)
        {
            $clock->net = $clock->name;
            $period_name = $clock->name . "_period";

            $this->block_generator->addConstantComment(str_pad(' ' . $clock->name . ' clock constant ', 55, '-', STR_PAD_BOTH));
            $this->block_generator->addConstant($clock->name . "_FREQ", "INTEGER", "1000000");  // TODO give real freq
            $this->block_generator->addConstant($period_name, "TIME", "1 ns");                  // TODO give real freq
            $this->block_generator->addSignalComment(str_pad(' ' . $clock->name . ' clock signal ', 55, '-', STR_PAD_BOTH));
            $this->block_generator->addSignal($clock->name, 1, "std_logic");

            $codeClks.="	" . $clock->name . "_gen_process : process" . "\r\n";
            $codeClks.="	begin" . "\r\n";
            $codeClks.="		" . $clock->name . " <= '1';" . "\r\n";
            $codeClks.="		wait for " . $period_name . " / 2;" . "\r\n";
            $codeClks.="		" . $clock->name . " <= '0';" . "\r\n";
            $codeClks.="		wait for " . $period_name . " / 2;" . "\r\n";
            $codeClks.="		if(endtb='1') then" . "\r\n";
            $codeClks.="                    wait;" . "\r\n";
            $codeClks.="		end if;" . "\r\n";
            $codeClks.="	end process;" . "\r\n";
            $codeClks.="" . "\r\n";
        }
        $this->block_generator->addCode($codeClks);
        
        // flow signals
        foreach ($this->block->flows as $flow)
        {
            $this->block_generator->addConstantComment(str_pad(' ' . $flow->name . ' flow constant ', 55, '-', STR_PAD_BOTH));
            $this->block_generator->addConstant(strtoupper($flow->name) . "_SIZE", "INTEGER", $flow->size);
            $this->block_generator->addSignalComment(str_pad(' ' . $flow->name . ' flow signals ', 55, '-', STR_PAD_BOTH));
            $this->block_generator->addSignal($this->block->name ."_". $flow->name . "_dv_s", 1, "std_logic");
            $this->block_generator->addSignal($this->block->name ."_". $flow->name . "_fv_s", 1, "std_logic");
            $this->block_generator->addSignal($this->block->name ."_". $flow->name . "_data_s", strtoupper($flow->name) . "_SIZE", "std_logic_vector");
            
            if($flow->type == "out")
                $this->block_generator->declare .= "    file " . $flow->name . "_data_file : text is out \"" . $flow->name . ".pgm\";" . "\r\n";
            else
                $this->block_generator->declare .= "    file " . $flow->name . "_data_file : text is in \"" . $flow->name . ".stim\";" . "\r\n";
        }
        
        // resets signals
        foreach ($this->block->resets as $reset)
        {
            $this->block_generator->addSignalComment(str_pad(' ' . $reset->name . ' reset signal ', 55, '-', STR_PAD_BOTH));
            $this->block_generator->addSignal($reset->name, 1, "std_logic");
        }
        
        // slave connection
        if($this->block->pi_size_addr_rel>0)
        {
            $this->block_generator->addSignalComment(str_pad(' pi signals ', 55, '-', STR_PAD_BOTH));
            $this->block_generator->addSignal($this->block->name ."_addr_rel_s", $this->block->pi_size_addr_rel, "std_logic_vector");
            $this->block_generator->addSignal($this->block->name ."_wr_s", 1, "std_logic");
            $this->block_generator->addSignal($this->block->name ."_rd_s", 1, "std_logic");
            $this->block_generator->addSignal($this->block->name ."_datawr_s", 32, "std_logic_vector");
            $this->block_generator->addSignal($this->block->name ."_datard_s", 32, "std_logic_vector");
            
        }
        
        // tb_specific
        $this->block_generator->addSignalComment(str_pad(' test bench specific ', 55, '-', STR_PAD_BOTH));
        $this->block_generator->addSignal('starttb', 1, "std_logic");
        $this->block_generator->addSignal('endtb', 1, "std_logic");
        
        // reset generation
        $rstCode = '';
        $rstCode .= "	rst_proc: process" . "\r\n";
        if($this->block->pi_size_addr_rel>0)
        {
            $rstCode .= "       procedure write_param (addr,val : in integer) is" . "\r\n";
            $rstCode .= "       begin" . "\r\n";
            $rstCode .= "           " . $this->block->name . "_addr_rel_s <= std_logic_vector(to_unsigned(addr,4));" . "\r\n";
            $rstCode .= "           " . $this->block->name . "_datawr_s <= std_logic_vector(to_unsigned(val,32));" . "\r\n";
            $rstCode .= "           wait for clk_proc_period;" . "\r\n";
            $rstCode .= "           " . $this->block->name . "_wr_s <= '1';" . "\r\n";
            $rstCode .= "           wait for clk_proc_period;" . "\r\n";
            $rstCode .= "           " . $this->block->name . "_wr_s <= '0';" . "\r\n";
            $rstCode .= "       end procedure;" . "\r\n";
        }
        $rstCode .= "	begin" . "\r\n";
        $rstCode .= "		reset_n <= '0';" . "\r\n";
        $rstCode .= "		starttb <= '0';" . "\r\n";
        $rstCode .= "		wait for clk_proc_period;" . "\r\n";
        $rstCode .= "		reset_n <= '1';" . "\r\n";
        $rstCode .= "		wait for clk_proc_period;" . "\r\n";
        if($this->block->pi_size_addr_rel>0)
        {
            $rstCode .= "" . "\r\n";
            $rstCode .= "		-- write_param(0, 1); -- write parameters to initialise component" . "\r\n";
            $rstCode .= "" . "\r\n";
        }
        $rstCode .= "		starttb <= '1';" . "\r\n";
        $rstCode .= "		wait;" . "\r\n";
        $rstCode .= "	end process;" . "\r\n" . "\r\n";
        $this->block_generator->addCode($rstCode);

        // stim input
        $codeStimIn = '';
        foreach ($this->block->flows as $flow)
        {
            if ($flow->type == "in")
            {
                $codeStimIn .= "    -- Stimuli for " . $flow->name . " flow" . "\r\n";
                $codeStimIn .= "    stim_proc: process" . "\r\n";
                $codeStimIn .= "		variable " . $flow->name . "_line : line;" . "\r\n";
                $codeStimIn .= "		variable " . $flow->name . "_pixelFromFile : INTEGER;" . "\r\n";
                $codeStimIn .= "		variable " . $flow->name . "_pixel_ok : BOOLEAN;" . "\r\n";
                $codeStimIn .= "	" . "\r\n";
                $codeStimIn .= "	begin" . "\r\n";
                $codeStimIn .= "		" . $this->block->name ."_". $flow->name . "_dv_s <= '0';" . "\r\n";
                $codeStimIn .= "		" . $this->block->name ."_". $flow->name . "_fv_s <= '0';" . "\r\n";
                $codeStimIn .= "		" . $this->block->name ."_". $flow->name . "_data_s <= (others=>'0');" . "\r\n";
                $codeStimIn .= "		" . "\r\n";
                $codeStimIn .= "		wait until starttb = '1';" . "\r\n";
                $codeStimIn .= "		wait until rising_edge(clk_proc);" . "\r\n";
                $codeStimIn .= "		wait until rising_edge(clk_proc);" . "\r\n";
                $codeStimIn .= "		" . "\r\n";
                $codeStimIn .= "		" . $this->block->name ."_". $flow->name . "_dv_s <= '1';" . "\r\n";
                $codeStimIn .= "		" . $this->block->name ."_". $flow->name . "_fv_s <= '1';" . "\r\n";
                $codeStimIn .= "		" . "\r\n";
                $codeStimIn .= "		while not endfile(" . $flow->name . "_data_file) loop" . "\r\n";
                $codeStimIn .= "			readline(in_data_file, " . $flow->name . "_line);" . "\r\n";
                $codeStimIn .= "			read(" . $flow->name . "_line, in_pixelFromFile, " . $flow->name . "_pixel_ok);" . "\r\n";
                $codeStimIn .= "			while " . $flow->name . "_pixel_ok loop" . "\r\n";
                $codeStimIn .= "				" . $this->block->name ."_". $flow->name . "_data_s <= std_logic_vector(to_unsigned(" . $flow->name . "_pixelFromFile, IN_SIZE));" . "\r\n";
                $codeStimIn .= "				wait until rising_edge(clk_proc);" . "\r\n";
                $codeStimIn .= "				read(" . $flow->name . "_line, " . $flow->name . "_pixelFromFile, " . $flow->name . "_pixel_ok);" . "\r\n";
                $codeStimIn .= "			end loop;" . "\r\n";
                $codeStimIn .= "		end loop;" . "\r\n";
                $codeStimIn .= "		" . "\r\n";
                $codeStimIn .= "		" . $this->block->name ."_". $flow->name . "_dv_s <= '0';" . "\r\n";
                $codeStimIn .= "		" . $this->block->name ."_". $flow->name . "_fv_s <= '0';" . "\r\n";
                $codeStimIn .= "		" . "\r\n";
                $codeStimIn .= "		wait;" . "\r\n";
                $codeStimIn .= "	end process;" . "\r\n" . "\r\n";
            }
        }
        $this->block_generator->addCode($codeStimIn);

        // stim input
        $codeDataOut = '';
        foreach ($this->block->flows as $flow)
        {
            if ($flow->type == "out")
            {
                $codeDataOut .= "    -- Data save for " . $flow->name . " flow" . "\r\n";
                $codeDataOut .= "    " . $flow->name . "_process: process" . "\r\n";
                $codeDataOut .= "		variable " . $flow->name . "_line : line;" . "\r\n";
                $codeDataOut .= "		variable x : integer := 0;" . "\r\n";
                $codeDataOut .= "	" . "\r\n";
                $codeDataOut .= "	begin" . "\r\n";
                $codeDataOut .= "		endtb <= '0';" . "\r\n";
                $codeDataOut .= "		write(" . $flow->name . "_line, string'(\"P2\"));" . "\r\n";
                $codeDataOut .= "		writeline(" . $flow->name . "_data_file, " . $flow->name . "_line);" . "\r\n";
                $codeDataOut .= "		" . "\r\n";
                $codeDataOut .= "		write(" . $flow->name . "_line, string'(\"128 128\"));  -- change this to modify size of output picture" . "\r\n";
                $codeDataOut .= "		writeline(" . $flow->name . "_data_file, " . $flow->name . "_line);" . "\r\n";
                $codeDataOut .= "		" . "\r\n";
                $codeDataOut .= "		write(" . $flow->name . "_line, string'(\"255\"));" . "\r\n";
                $codeDataOut .= "		writeline(" . $flow->name . "_data_file, " . $flow->name . "_line);" . "\r\n";
                $codeDataOut .= "		" . "\r\n";
                $codeDataOut .= "		wait until " . $this->block->name ."_". $flow->name . "_fv_s = '1';" . "\r\n";
                $codeDataOut .= "		while " . $this->block->name ."_". $flow->name . "_fv_s='1' loop" . "\r\n";
                $codeDataOut .= "			wait until rising_edge(clk_proc);" . "\r\n";
                $codeDataOut .= "			if(" . $this->block->name ."_". $flow->name . "_dv_s='1') then" . "\r\n";
                $codeDataOut .= "				write(" . $flow->name . "_line, to_integer(unsigned(" . $this->block->name ."_". $flow->name . "_data_s)));" . "\r\n";
                $codeDataOut .= "				write(" . $flow->name . "_line, string'(\" \"));" . "\r\n";
                $codeDataOut .= "				x := x + 1;" . "\r\n";
                $codeDataOut .= "				if(x>=128) then" . "\r\n";
                $codeDataOut .= "					writeline(" . $flow->name . "_data_file, " . $flow->name . "_line);" . "\r\n";
                $codeDataOut .= "					x := 0;" . "\r\n";
                $codeDataOut .= "				end if;" . "\r\n";
                $codeDataOut .= "			end if;" . "\r\n";
                $codeDataOut .= "		end loop;" . "\r\n";
                $codeDataOut .= "		writeline(" . $flow->name . "_data_file, " . $flow->name . "_line);" . "\r\n";
                $codeDataOut .= "		endtb <= '1';" . "\r\n";
                $codeDataOut .= "		assert false report \"end of " . $flow->name . "\" severity note;" . "\r\n";
                $codeDataOut .= "		wait;" . "\r\n";
                $codeDataOut .= "	end process;" . "\r\n";
            }
        }
        $this->block_generator->addCode($codeDataOut);

        // test bench top level generation
        $this->block_generator->addblock($this->block);
        $this->block_generator->save_as_ifdiff($path . DIRECTORY_SEPARATOR . $this->block_generator->name . '.vhd');

        // === makefile generation ===
        $mfContent = "";
        $mfContent .= "-include Makefile.local" . "\r\n";
        $mfContent .= "" . "\r\n";
        $mfContent .= "PROCNAME = " . $this->block->name . "\r\n";
        $mfContent .= "TBNAME = $(PROCNAME)_tb" . "\r\n";
        $mfContent .= "OUTDIR = simbuild" . "\r\n";
        $mfContent .= "GHDL = ghdl" . "\r\n";
        $mfContent .= "GHDLFLAGS = --ieee=synopsys -fexplicit" . "\r\n";
        $mfContent .= "" . "\r\n";
        $mfContent .= "QUARTUS_LIB = altera/13.1/quartus/eda/sim_lib/" . "\r\n";
        $mfContent .= "USEALTERA_MF = 0" . "\r\n";
        $mfContent .= "" . "\r\n";
        $mfContent .= "ifeq ($(USEALTERA_MF),1)" . "\r\n";
        $mfContent .= "  SRC += altera_mf_components.vhd" . "\r\n";
        $mfContent .= "  SRC += altera_mf.vhd" . "\r\n";
        $mfContent .= "  GHDLFLAGS += --work=altera_mf --ieee=synopsys -fexplicit" . "\r\n";
        $mfContent .= "  vpath %.vhd \${QUARTUS_LIB}" . "\r\n";
        $mfContent .= "endif" . "\r\n";
        $mfContent .= "" . "\r\n";

        // source list
        $pathList = array();
        foreach ($this->block->files as $file)
        {
            if($file->group == "hdl")
            {
                $path = str_replace("hwlib:", SUPPORT_PATH . "component" . DIRECTORY_SEPARATOR, $file->path);
                $mfContent .= "SRC += " . basename($path) . "\r\n";
                if(!in_array(dirname($path), $pathList))
                    array_push($pathList, dirname($path));
            }
        }
        $mfContent .= "SRC += " . $this->block->name . "_tb.vhd" . "\r\n";
        $mfContent .= "" . "\r\n";
        foreach ($pathList as $path)
        {
            $mfContent .= "vpath %.vhd " . $path . "\r\n";
        }
        $mfContent .= "" . "\r\n";
        $mfContent .= "OBJECTS := $(addprefix $(OUTDIR)/, $(notdir $(SRC:.vhd=.o)))" . "\r\n";
        $mfContent .= "" . "\r\n";
        $mfContent .= "all: run" . "\r\n";
        $mfContent .= "" . "\r\n";
        $mfContent .= "Makefile: " . $this->block->name . ".proc" . "\r\n";
        $mfContent .= "\tgpproc generatetb" . "\r\n";
        $mfContent .= "" . "\r\n";
        $mfContent .= "$(OUTDIR)/%.stim: %.png" . "\r\n";
        $mfContent .= "\t@test -d $(OUTDIR) || mkdir -p $(OUTDIR)" . "\r\n";
        $mfContent .= "\tgpproc convert -i $< -o $@" . "\r\n";
        $mfContent .= "" . "\r\n";
        $mfContent .= "$(OUTDIR)/%.o: %.vhd" . "\r\n";
        $mfContent .= "\t@test -d $(OUTDIR) || mkdir -p $(OUTDIR)" . "\r\n";
        $mfContent .= "\t$(GHDL) -a --workdir=$(OUTDIR) $(GHDLFLAGS) $<" . "\r\n";
        $mfContent .= "" . "\r\n";
        $mfContent .= "$(OUTDIR)/$(TBNAME): $(OBJECTS)" . "\r\n";
        $mfContent .= "\tcd $(OUTDIR) && $(GHDL) -e $(GHDLFLAGS) $(TBNAME)" . "\r\n";
        $mfContent .= "" . "\r\n";

        // input flow list
        $inputStim = "";
        foreach ($this->block->flows as $flow)
        {
            if ($flow->type == "in")
                $inputStim .= "$(OUTDIR)/" . $flow->name . ".stim ";
        }
        $mfContent .= "# GHDLRUNFLAGS = --stop-time=500ns # uncomment to limit simulation time" . "\r\n";
        $mfContent .= "run : $(OUTDIR)/$(TBNAME) " . $inputStim . "\r\n";
        $mfContent .= "\tcd $(OUTDIR) && $(GHDL) -r $(TBNAME) $(GHDLRUNFLAGS) --vcd=$(PROCNAME).vcd" . "\r\n";
        $mfContent .= "" . "\r\n";
        $mfContent .= "view: run" . "\r\n";
	$mfContent .= "\tgtkwave $(TBNAME).vcd" . "\r\n";
        $mfContent .= "" . "\r\n";
        $mfContent .= ".PHONY: clean" . "\r\n";
        $mfContent .= "clean:" . "\r\n";
        $mfContent .= "\trm -rf $(OUTDIR)" . "\r\n";
        $mfContent .= "" . "\r\n";

        saveIfDifferent("Makefile", $mfContent);
    }

    public function generateTopBlock($path)
    {
        if ($this->block == NULL)
            return;

        $this->block_generator = new VHDL_generator($this->block->name);

        // connexions between slave and process
        if ($this->slave_block != NULL)
        {
            foreach ($this->block->params as $param)
            {
                if ($param->hard == false)
                {
                    if (empty($param->parambitfields))
                    {
                        $this->block_generator->addSignal($param->name, 32, 'std_logic_vector');
                    }
                    else
                    {
                        foreach ($param->parambitfields as $parambitfields)
                        {
                            $size = count($parambitfields->bitfieldlist);

                            if ($size == 1)
                                $type = "std_logic";
                            else
                                $type = "std_logic_vector";
                            if ($size > 0)
                                $this->block_generator->addSignal($param->name . '_' . $parambitfields->name, $size, $type);
                        }
                    }
                }
            }
        }

        // block top level generation
        $this->block_generator->fromBlock($this->block);
        $this->block_generator->addblock($this->process_block, TRUE);
        if ($this->slave_block != NULL)
            $this->block_generator->addblock($this->slave_block, TRUE);
        $this->block_generator->save_as_ifdiff($path . DIRECTORY_SEPARATOR . $this->block_generator->name . '.vhd');
    }

    public function generateProcess($path)
    {
        if ($this->block == NULL)
            return;

        $this->process_generator = new VHDL_generator($this->block->name . '_process');

        $code = '';
        $code.="	data_process : process (clk_proc, reset_n)" . "\r\n";
        $code.="	begin" . "\r\n";
        $code.="		if(reset_n='0') then" . "\r\n";
        $code.="			out_data <= (others => '0');" . "\r\n";
        $code.="			out_dv <= '0';" . "\r\n";
        $code.="			out_fv <= '0';" . "\r\n";
        $code.="		elsif(rising_edge(clk_proc)) then" . "\r\n";
        $code.="			out_dv <= in_dv;" . "\r\n";
        $code.="			out_fv <= in_fv;" . "\r\n";
        $code.="\r\n";
        $code.="			if(in_dv='1' and in_fv='1') then" . "\r\n";
        $code.="				out_data <= in_data;" . "\r\n";
        $code.="			end if;" . "\r\n";
        $code.="		end if;" . "\r\n";
        $code.="	end process;";

        $this->process_generator->addCode($code);

        // process generation
        $this->process_generator->fromBlock($this->process_block, TRUE);
        $this->process_generator->save_as_ifdiff($path . DIRECTORY_SEPARATOR . $this->process_generator->name . '.vhd');
    }

    public function generateSlave($path)
    {
        if ($this->slave_block == NULL)
            return;

        $this->slave_generator = new VHDL_generator($this->block->name . '_slave');

        // registers
        usort($this->block->params, "Param::cmp_raddr");

        $this->slave_generator->addSignalComment(" Internal registers");
        $this->slave_generator->addConstantComment(" Registers address");

        $code_rst = "";
        foreach ($this->block->params as $param)
        {
            if ($param->hard == false)
            {
                if ($param->regaddr != '')
                    $regaddr = $param->regaddr;
                else
                    $regaddr = 0;
                $this->slave_generator->addConstant(strtoupper($param->name) . "_REG_ADDR", "natural", $regaddr);

                if (empty($param->parambitfields))
                {
                    $size = 32;

                    // slave internal
                    $this->slave_generator->addSignal($param->name . "_reg", $size, 'std_logic_vector');
                    $code_rst.="			" . $param->name . "_reg <= " . Block_generator::hex($param->value, $size) . ";" . "\r\n";
                }
                else
                {
                    foreach ($param->parambitfields as $parambitfield)
                    {
                        $size = count($parambitfield->bitfieldlist);

                        if ($size > 0)
                        {
                            if ($size == 1)
                                $type = "std_logic";
                            else
                                $type = "std_logic_vector";

                            // slave internal
                            $this->slave_generator->addSignal($param->name . "_" . $parambitfield->name . "_reg", $size, $type);
                            if ($size == 1)
                            {
                                $code_rst.="			" . $param->name . "_" . $parambitfield->name . "_reg <= '" . (int) $parambitfield->value . "';" . "\r\n";
                            }
                            else
                            {
                                $code_rst.="			" . $param->name . "_" . $parambitfield->name . "_reg <= " . Block_generator::bin($parambitfield->value, $size) . ";" . "\r\n";
                            }
                        }
                    }
                }
            }
        }

        $size_master = $this->block->pi_size_addr_rel;

        // write reg code
        $code_wr = "";
        $code_wr.="	write_reg : process (clk_proc, reset_n)" . "\r\n";
        $code_wr.="	begin" . "\r\n";
        $code_wr.="		if(reset_n='0') then" . "\r\n";
        $code_wr.=$code_rst;
        $code_wr.="		elsif(rising_edge(clk_proc)) then" . "\r\n";
        $code_wr.="			if(wr_i='1') then" . "\r\n";
        $code_wr.="				case to_integer(unsigned(addr_rel_i)) is" . "\r\n";
        foreach ($this->block->params as $param)
        {
            if ($param->hard == false)
            {
                $contant_reg_addr = strtoupper($param->name) . "_REG_ADDR";
                $code_wr.="					when " . $contant_reg_addr . " =>" . "\r\n";

                if (empty($param->parambitfields))
                {
                    $size = 32;
                    $reg_name = $param->name . "_reg";
                    $code_wr.="						" . $reg_name . " <= datawr_i;" . "\r\n";
                }
                else
                {
                    foreach ($param->parambitfields as $parambitfield)
                    {
                        $size = count($parambitfield->bitfieldlist);

                        if ($size > 0)
                        {
                            $bf_name = $param->name . "_" . $parambitfield->name . "_reg";

                            $code_wr.="						" . $bf_name . " <= ";
                            $count = 0;
                            foreach ($parambitfield->bitfieldlist as $bit)
                            {
                                if ($count > 0)
                                    $code_wr.=" & ";
                                $code_wr.="datawr_i(" . $bit . ")";
                                $count++;
                            }
                            $code_wr.=";" . "\r\n";
                        }
                    }
                }
            }
        }

        $code_wr.="					when others=>" . "\r\n";
        $code_wr.="				end case;" . "\r\n";
        $code_wr.="			end if;" . "\r\n";
        $code_wr.="		end if;" . "\r\n";
        $code_wr.="	end process;" . "\r\n";
        $code_wr.="" . "\r\n";

        // read reg code
        $code_rd = "";
        $code_rd.="	read_reg : process (clk_proc, reset_n)" . "\r\n";
        $code_rd.="	begin" . "\r\n";
        $code_rd.="		if(reset_n='0') then" . "\r\n";
        $code_rd.="			datard_o <= (others => '0');" . "\r\n";
        $code_rd.="		elsif(rising_edge(clk_proc)) then" . "\r\n";
        $code_rd.="			if(rd_i='1') then" . "\r\n";
        $code_rd.="				case to_integer(unsigned(addr_rel_i)) is" . "\r\n";
        foreach ($this->block->params as $param)
        {
            if ($param->hard == false)
            {
                $contant_reg_addr = strtoupper($param->name) . "_REG_ADDR";
                $code_rd.="					when " . $contant_reg_addr . " =>" . "\r\n";

                if (empty($param->parambitfields))
                {
                    $size = 32;

                    $reg_name = $param->name . "_reg";

                    $code_rd.="						datard_o <= " . $reg_name . ";" . "\r\n";
                }
                else
                {
                    $bits = array();
                    foreach ($param->parambitfields as $parambitfield)
                    {
                        $size = count($parambitfield->bitfieldlist);

                        $bf_name = $param->name . "_" . $parambitfield->name . "_reg";

                        if($size==1)
                        {
                            $bits[$parambitfield->bitfieldlist[0]] = $bf_name;
                        }
                        else
                        {
                            $count = $size;
                            foreach ($parambitfield->bitfieldlist as $bit)
                            {
                                $bits[$bit] = $bf_name . "(" . ($count - 1) . ")";
                                $count--;
                            }
                        }
                    }

                    $code_rd.="						datard_o <= ";
                    $prev = 32;
                    $count = 0;
                    ksort($bits);
                    $bits = array_reverse($bits, true);
                    foreach ($bits as $bitpos => $bitname)
                    {
                        if ($count > 0)
                            $code_rd.=" & ";
                        if ($bitpos < $prev - 1)
                            $code_rd.="\"" . str_repeat('0', $prev - $bitpos - 1) . "\" & ";
                        $code_rd.=$bitname;
                        $prev = $bitpos;
                        $count++;
                    }
                    $code_rd.=";" . "\r\n";
                }
            }
        }


        $code_rd.="					when others=>" . "\r\n";
        $code_rd.="						datard_o <= (others => '0');" . "\r\n";
        $code_rd.="				end case;" . "\r\n";
        $code_rd.="			end if;" . "\r\n";
        $code_rd.="		end if;" . "\r\n";
        $code_rd.="	end process;" . "\r\n";
        $code_rd.="" . "\r\n";

        // output connexions
        $code_connect = '';
        foreach ($this->block->params as $param)
        {
            if ($param->hard == false)
            {
                if (empty($param->parambitfields))
                {
                    $reg_name = $param->name . "_reg";
                    $code_connect.="	$param->name <= $reg_name;" . "\r\n";
                }
                else
                {
                    foreach ($param->parambitfields as $parambitfield)
                    {
                        $size = count($parambitfield->bitfieldlist);

                        if ($size > 0)
                        {
                            $bf_name = $param->name . "_" . $parambitfield->name;

                            $code_connect.="	" . $bf_name . " <= " . $bf_name . "_reg;" . "\r\n";
                        }
                    }
                }
            }
        }

        $this->slave_generator->addCode($code_wr);
        $this->slave_generator->addCode($code_rd);
        $this->slave_generator->addCode($code_connect);

        // slave generation
        $this->slave_generator->fromBlock($this->slave_block, TRUE);
        $this->slave_generator->save_as_ifdiff($path . DIRECTORY_SEPARATOR . $this->slave_generator->name . '.vhd');
    }

    static function convertImg2stim($input_image, $output_file)
    {
        $path_parts = pathinfo($input_image);
        $ext = strtolower($path_parts['extension']);
        try
        {
            switch ($ext)
            {
                case "png":
                    $img = imagecreatefrompng($input_image);
                    break;
                case "jpg":
                case "jpeg":
                    $img = imagecreatefromjpeg($input_image);
                    break;
                case "gif":
                    $img = imagecreatefromgif($input_image);
                    break;
                case "bmp":
                    $img = imagecreatefromwbmp($input_image);
                    break;
                default:
                    $img = NULL;
                    break;
            }
        }
        catch (Exception $e)
        {
            error("GD extension is not installed with your PHP distribution, please install it and retry.");
        }
        if ($img == NULL)
            error("Cannot open image file '" . $input_image . "'", 1);

        // imagefilter($img, IMG_FILTER_GRAYSCALE);

        $handle = fopen($output_file, "wb");

        for ($y = 0; $y < imagesy($img); $y++)
        {
            for ($x = 0; $x < imagesx($img); $x++)
            {
                $rgb = imagecolorat($img, $x, $y);
                if($rgb<255)
                {
                    $gray = $rgb;
                }
                else
                {
                    $r = ($rgb >> 16) & 0xFF;
                    $g = ($rgb >> 8) & 0xFF;
                    $b = $rgb & 0xFF;
                    $gray = 0.30 * $r + 0.59 * $g + 0.11 * $b;
                }

                fwrite($handle, (int) $gray . " ");
            }
            fwrite($handle, "\n");
        }

        fclose($handle);
        imagedestroy($img);
    }

    static function convertData2Img($in, $out)
    {
        
    }
}
