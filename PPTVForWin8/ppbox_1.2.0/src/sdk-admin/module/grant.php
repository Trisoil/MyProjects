<?php

require_once('object_set.php');

class grant extends object_set
{    
    function table_name()
    {
        return "t_grant";
    }
};

$grant = new grant;
$grant->process();

?>
