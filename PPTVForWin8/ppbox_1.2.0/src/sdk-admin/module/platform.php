<?php

require_once('object_set.php');                                                                                                                                                

class platform extends object_set
{    
	    function table_name()
	    {   
	        return "t_platform";
		}
	   
	    function auto_id()
		{
		    return true;
		}	
};

$platform = new platform;
$platform->process();

?>
