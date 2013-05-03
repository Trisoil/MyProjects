<?php

require_once('object_set.php');                                                                                                                                                      
class project_company extends object_set
{
	    function table_name()
	    {
	        return "t_project_company";
		}

		function auto_id()
		{
			return true;
		}
};

$project_company = new project_company;
$project_company->process();

?>
