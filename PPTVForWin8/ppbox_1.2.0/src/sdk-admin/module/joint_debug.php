<?php
require_once('object_set.php');

$no_process = 1;
require_once('problem.php');

class joint_debug extends object_set
{   


	function add_item($item)
	{
		$problem_item = new stdClass;
		$problem_item->owner_id = $item->engineer_id;
		$problem_item->specificmatters = $item->description;
		$problem_item->begin_time = date('Y-m-d H:i');
		$problem_item->progress = 0;
		$problem_item->description = 'JointBug'. '('.$problem_item->begin_time.')';


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
        return "t_joint_debug";
	} 

    function auto_id()
	{
		return true;
	}	
};

$joint_debug = new joint_debug;
$joint_debug->process();

?>

