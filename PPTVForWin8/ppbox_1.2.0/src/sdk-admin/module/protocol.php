<?php

require_once('object_set.php');                                                                                                                                                      
class protocol extends object_set
{
	    function table_name()
	    {
	        return "t_protocol";
		}

		function auto_id()
		{
			return true;
		}
};

$protocol = new protocol;
$protocol->process();

?>
