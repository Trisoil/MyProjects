<?php

require_once('object_set.php');

class role extends object_set
{    
    function table_name()
    {
        return "t_role";
    }

    function auto_id()
    {
        return true;
    }    
};

$role = new role;
$role->process();

?>
