<html>
	<head>
		<LINK href="styles.css" rel="stylesheet" type="text/css"/>
	</head>

	<body>
		<div id="server_details">
			<div id="top-link">
				<a href="ServerList.php">UDP Servers List</a>
			</div>

<?php
	$timezone = "Asia/Chongqing"; 
	if(function_exists('date_default_timezone_set')) date_default_timezone_set($timezone); 

	$query_string = $_SERVER['QUERY_STRING'];
	parse_str($query_string, $query_string_array);

	if (false == array_key_exists('ip', $query_string_array))
	{
		echo "no ip";
		exit();
	}
	
	$ip = $query_string_array['ip'];
	$file_name = 'servers_status/' . $ip . '.xml';
	if (false == file_exists($file_name))
	{
		echo 'Specified Server not found.';
		exit();
	}
	
	echo '<div>UDP Server: ' . $ip . "</div>\n";
	
	$file_last_update_time = date("H:i, F d", filemtime($file_name));
	echo '<div>Last Report: ' . $file_last_update_time . "</div>\n";
	
	$xsl = new DomDocument;
	$xsl->load('ServerStatus.xsl');
  
	$xslt_processor = new XsltProcessor();
	$xslt_processor->importStylesheet($xsl);

	$xml_doc = new DomDocument;
	$xml_doc->load($file_name);
	
	if ($html = $xslt_processor->transformToXML($xml_doc)) 
	{
      echo $html;
	} 
	else 
	{
		echo "An error occurred while renderring server page.";
	}

?>

		<div id="bottom-link">
			<a href="ServerList.php">UDP Servers List</a>
		</div>
	</div>
	</body>
</html>
