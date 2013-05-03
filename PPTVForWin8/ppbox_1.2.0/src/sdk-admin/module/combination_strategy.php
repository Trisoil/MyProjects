<?php                                                                                                                                                                              

require_once('object_set.php');

class combination_strategy extends object_set
{    
	    function table_name()
	    {   
	        return "t_combination_strategy";
		}  

		function auto_id()
		{
			return true;
		}	
};

$combination_strategy = new combination_strategy;
$combination_strategy->process();

?>
