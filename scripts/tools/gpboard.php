<?php

$handle = fopen($argv[1], "r");
if (!$handle)
    exit();

$pins = array();
$attrs = array();

// read file
while (($line = fgets($handle)) !== false)
{
    // pin declaration
    $pattern = "/set_location_assignment ([a-zA-Z0-9_]+)[ \\t]+-to[ \\t]+([^\\\"\\n\\r]*)/";
    if(preg_match($pattern, $line, $matches))
    {
        $pin_name = $matches[1];
        $name = $matches[2];
        if(!array_key_exists($name, $pins))
        {
            $pin = new stdClass();
            $pin->pin = $pin_name;
            $pin->name = $name;
            $pin->attrs = array();
            $pins[$name] = $pin;
        }
        else
        {
            $pin = $pins[$name];
            $pin->pin = $pin_name;
        }
    }
    
    // attr declaration
    $pattern = "/set_instance_assignment -name ([a-zA-Z0-9_]+)[ \\t]+\\\"([a-zA-Z0-9_\\- \\.]+)\\\"[ \\t]+\-to[ \\t]+([^\\\"\\n\\r]*)/";
    if(preg_match($pattern, $line, $matches))
    {
        $assign_name = $matches[1];
        $param = $matches[2];
        $to = $matches[3];
        $attr = new stdClass();
        $attr->name = $assign_name;
        $attr->param = $param;
        $attr->to = $to;
        if($assign_name == "IO_STANDARD")
        {
            if(!array_key_exists($to, $pins))
            {
                $pin = new stdClass();
                $pin->pin = "";
                $pin->name = $to;
                $pin->attrs = array();
                $pins[$to] = $pin;
            }
            array_push($pins[$to]->attrs, $attr);
        }
    }
}

// export file
$content = "";

// export pins map
foreach ($pins as $pin)
{
    if(!empty($pin->pin))
    {
        $content .= '<pin name="'.strtolower($pin->name).'" mapto="'.$pin->pin.'">'."\n";
        foreach ($pin->attrs as $attr)
        {
            $content .= "\t".'<attributes>'."\n";
            $content .= "\t\t".'<attribute name="IO_STANDARD" value="'.$attr->param.'" type="instance"/>'."\n";
            $content .= "\t".'</attributes>'."\n";
        }
        $content .= '</pin>'."\n";
    }
}

fclose($handle);

file_put_contents("board.dev", $content);

echo "ok\n";
