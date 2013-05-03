<?php

require_once('object_set.php');                                                                                                                                                      
class equipment_type  extends object_set
{
	    function table_name()
	    {
	        return "t_equipment_type";
		}

		function auto_id()
		{
			return true;
		}
};

$equipment_type = new equipment_type;
$equipment_type->process();

?>
