<?php

require_once('object_set.php');                                                                                                                                                      
class problem_type  extends object_set
{
	    function table_name()
	    {
	        return "t_problem_type";
		}

		function auto_id()
		{
			return true;
		}
};

$problem_type = new problem_type;
$problem_type->process();

?>
