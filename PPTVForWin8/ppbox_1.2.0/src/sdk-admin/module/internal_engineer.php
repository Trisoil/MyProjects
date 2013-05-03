<?php                                                                                                                                                                              

require_once('object_set.php');

class internal_engineer extends object_set
{    
	    function table_name()
	    {   
	        return "t_internal_engineer";
		}
	   
	   
	    function auto_id()
	    {   
	        return true;
	    }          	
};

$internal_engineer = new internal_engineer;
$internal_engineer->process();

?>
