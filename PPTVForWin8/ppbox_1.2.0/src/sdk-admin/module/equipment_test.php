<?php

require_once('object_set.php');                                                                                                                                                      
class equipment_test  extends object_set
{
	    function table_name()
	    {
	        return "t_test";
		}

		function auto_id()
		{
			return true;
		}
};

$equipment_test = new equipment_test;
$equipment_test->process();

?>
