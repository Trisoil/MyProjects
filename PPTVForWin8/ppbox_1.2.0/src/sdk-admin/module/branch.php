<?php

require_once('object_set.php');                                                                                                                                                      
class branch extends object_set
{
	    function table_name()
	    {
	        return "t_branch";
		}

		function auto_id()
		{
			return true;
		}
};

$branch = new branch;
$branch->process();

?>
