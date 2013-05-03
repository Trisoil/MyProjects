<?php
require_once('object_set.php');

class company extends object_set
{    
    function table_name()
    {   
        return "t_company";
	} 

    function auto_id()
	{
		return true;
	}	
};

$company = new company;
$company->process();
?>
