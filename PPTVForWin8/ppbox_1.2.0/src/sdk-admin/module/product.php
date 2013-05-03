<?php

require_once('object_set.php');                                                                                                                                                     
class product extends object_set
{    
	    function table_name()
        {   
	        return "t_product";
		}

		function auto_id()
		{
			return true;
		}	
};

$product = new product;
$product->process();

?>

