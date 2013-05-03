<?php

require_once('object_set.php');                                                                                                                                                      
class strategy extends object_set
{
	    function table_name()
	    {
	        return "t_strategy";
		}

		function auto_id()
		{
			return true;
		}
};

$strategy = new strategy;
$strategy->process();

?>
