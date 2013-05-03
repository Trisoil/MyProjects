<?php

require_once('object_set.php');                                                                                                                                                      
class version extends object_set
{
		function add_item($item)
		{
			$item->publish_time = date('Y-m-d H:i:s');
			$item->engineer_id = $_SESSION['uid'];

			$result = parent::add_item($item);
			if (!$result['success'])
				return $result;

			return $result;
		}

	    function table_name()
	    {
	        return "t_version";
		}

		function auto_id()
		{
			return true;
		}
};

$version = new version;
$version->process();

?>
