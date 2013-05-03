<?php

require_once('object_set.php');                                                                                                                                                      
class equipment extends object_set
{
	    function table_name()
	    {
	        return "t_equipment";
		}

		function auto_id()
		{
			return true;
		}
};

$equipment = new equipment;
$equipment->process();

?>
