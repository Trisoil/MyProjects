<?php

require_once('object_set.php');

class target extends object_set
{    
    function table_name()
    {
        return "t_target";
    }

    function auto_id()
    {
        return true;
    }    
};

$target = new target;
$target->process();

?>
