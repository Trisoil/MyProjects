<?php

require_once('object_set.php');                                                                                                                                                      
class accessory_type extends object_set
{
	    function table_name()
	    {
	        return "t_accessory_type";
		}

		function auto_id()
		{
			return true;
		}
};

$accessory_type = new accessory_type;
$accessory_type->process();

?>
