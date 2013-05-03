<?php                                                                                                                                                                              

require_once('object_set.php');

class engineer extends object_set
{    
	    function table_name()
	    {   
	        return "t_engineer";
		}
	   
	   
	    function auto_id()
	    {   
	        return true;
	    }          	
};

$engineer = new engineer;
$engineer->process();

?>
