<?php

require_once('object_set.php');                                                                                                                                                      
class staff_type extends object_set
{
	    function table_name()
	    {
	        return "t_staff_type";
		}

		function auto_id()
		{
			return true;
		}
};

$staff_type = new staff_type;
$staff_type->process();

?>
