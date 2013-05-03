<?php

require_once('object_set.php');                                                                                                                                                      
class project_engineer extends object_set
{
	    function table_name()
	    {
	        return "t_project_engineer";
	    }
};

$project_engineer = new project_engineer;
$project_engineer->process();

?>
