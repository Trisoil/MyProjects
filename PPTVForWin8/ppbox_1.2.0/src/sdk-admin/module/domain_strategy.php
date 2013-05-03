<?php

require_once('object_set.php');                                                                                                                                                      
class domain_strategy extends object_set
{
	    function table_name()
	    {
	        return "t_domain_strategy";
		}

		function auto_id()
		{
			return true;
		}
};

$domain_strategy = new domain_strategy;
$domain_strategy->process();

?>
