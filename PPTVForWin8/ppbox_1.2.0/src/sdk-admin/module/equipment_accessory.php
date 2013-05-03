<?php

require_once('object_set.php');                                                                                                                                                      
class equipment_accessory extends object_set
{
	    function table_name()
	    {
	        return "t_equipment_accessory";
		}

};

$equipment_accessory = new equipment_accessory;
$equipment_accessory->process();

?>
