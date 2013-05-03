<?php
	function GetRealIpAddr()
	{
		if (!empty($_SERVER['HTTP_CLIENT_IP']))
		{
			$ip=$_SERVER['HTTP_CLIENT_IP'];
		}
		elseif (!empty($_SERVER['HTTP_X_FORWARDED_FOR']))
		{
			$ip=$_SERVER['HTTP_X_FORWARDED_FOR'];
		}
		else
		{
			$ip=$_SERVER['REMOTE_ADDR'];
		}
		
		return $ip;
	}

	header('HTTP/1.1 500 Internal Server Error'); 
	
	$client_ip = GetRealIpAddr();
	if (false == $client_ip)
	{
		exit();
	}
	
	$request_body = @file_get_contents('php://input');
	
	$xml_dom = DOMDocument::loadXML($request_body);
	
	if (false == $xml_dom)
	{
		exit();
	}
	
	$file_name = 'servers_status/' . $client_ip . '.xml';
	if (false == $xml_dom->save($file_name))
	{
		exit();
	}
	
	header ('HTTP/1.1 200 OK');
?>

