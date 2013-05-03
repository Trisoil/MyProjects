<?php

require_once('object_set.php');                                                                                                                                                      
class application extends object_set
{
	    function table_name()
	    {
	        return "t_application";
		}

};

$application = new application;
$application->process();

?>
