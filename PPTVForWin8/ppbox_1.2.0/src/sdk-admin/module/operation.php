<?php

require_once('object_set.php');

class operation extends object_set
{    

    function table_name()
    {
        return "t_operation";
    }

    function auto_id()
    {
        return true;
    }    
};

$operation = new operation;
$operation->process();

?>
