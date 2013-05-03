<?php

require_once('object_set.php');                                                                                                                                                      
$no_process = 1;
require_once('problem.php');                                                                                                                                                      

class user_bug  extends object_set
{  

		function add_item($item)
		{ 
			$problem_item = new stdClass;
			$problem_item->owner_id = $item->engineer_id;
			$problem_item->specificmatters = $item->scope;
			$problem_item->begin_time = date('Y-m-d H:i:s');
			$problem_item->instruction = $item->frequency;
			$problem_item->description = 'UserBug'.'('.$problem_item->begin_time.')';

			$problem = new problem;
			$result = $problem->add_item($problem_item);

	        if (!$result['success']) {
	            return $result;
			} 

			$item->track_id = $result['data']->id;
		    $result = parent::add_item($item);
		    if (!$result['success']) {
		       return $result;
			}

	        return $result;

	    }   
			
	    function table_name()
	    {
	        return "t_user_bug";
		}

		function auto_id()
		{
			return true;
		}
};

$user_bug = new user_bug;
$user_bug->process();

?>
