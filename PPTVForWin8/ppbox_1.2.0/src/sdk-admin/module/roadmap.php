<?php

require_once('object_set.php');                                                                                                                                                      
class roadmap extends object_set
{
	    function table_name()
	    {
	        return "t_roadmap";
	    }
};

$roadmap = new roadmap;
$roadmap->process();

?>
