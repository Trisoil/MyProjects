<?php

require_once('object_set.php');                                                                                                                                                     
class project extends object_set
{


		function  mod_item($item)
		{
			$item->complete_time = date('Y-m-d H:i:s');

			$result = parent::mod_item($item);
			if (!$resule['success'])
				return $result;

			return $result;
		}
		
		function  add_item($item)
		{
			$item->begin_time = date('Y-m-d H:i:s');
			$item->complete_time = date('Y-m-d H:i:s');

			$result = parent::add_item($item);
			if (!$resule['success'])
				return $result;

			return $result;
		
		}
		function table_name()
	    {
	        return "t_project";
		}

		function auto_id()
		{
			return true;
		}
};

$project = new project;
$project->process();

?>
