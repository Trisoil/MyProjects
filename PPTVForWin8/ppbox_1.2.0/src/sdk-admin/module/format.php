<?php

require_once('object_set.php');                                                                                                                                                      
class format extends object_set
{
	    function table_name()
	    {
	        return "t_format";
		}

		function auto_id()
		{
			return true;
		}
};

$format = new format;
$format->process();

?>
