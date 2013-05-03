<?php

require_once('object_set.php');                                                                                                                                                      
class company_type  extends object_set
{
	    function table_name()
	    {
	        return "t_company_type";
		}

		function auto_id()
		{
			return true;
		}
};

$company_type = new company_type;
$company_type->process();

?>
