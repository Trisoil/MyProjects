<?php

require_once('object_set.php');                                                                                                                                                     

class problem extends object_set
{    


	function mod_item($item)
	{
		$item->complete_time = date('Y-m-d H:i:s');
  
		$result = parent::mod_item($item);
		if (!$result['success'])
			return $result;

		return $result;
	}

	function add_item($item)
	{
		$item->complete_time = date('Y-m-d H:i:s');
		$item->begin_time = date('Y-m-d H:i:s');  
		$result = parent::add_item($item);
		if (!$result['success'])
			return $result;

		return $result;
	}

	function table_name()
	{   
	     return "t_problem";
	}

	function auto_id()
	{ 
		return true;
	}

};

if (!isset($no_process)) {
$problem = new problem;
$problem->process();
}

?>
