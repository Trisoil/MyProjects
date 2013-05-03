<?php

require_once('object_set.php');                                                                                                                                                      
class schedules extends object_set
{
		function add_item($item)
		{
			$item->time = date('Y-m-d H:i:s');
			$item->name = $_SESSION['uid'];

			$result = parent::add_item($item);
			if (!$result['success'])
				return $result;

			return $result;
		}

	    function table_name()
	    {
	        return "t_schedules";
		}

		function auto_id()
		{
			return true;
		}
};

$schedules = new schedules;
$schedules->process();

?>
