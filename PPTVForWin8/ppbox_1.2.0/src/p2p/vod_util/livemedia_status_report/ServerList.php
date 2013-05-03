<?php
header('Content-type: text/html;charset=utf-8');
?>
<html>

<head>
	<LINK href="styles.css" rel="stylesheet" type="text/css"/>
</head>

<body>

<div id="table-container">
	<div>
		<H3> UDP Server List</H3>
	</div>

	<table>
	   <tr class="header">		
			<th> Server </th>
			<th> Upload Speed (bytes/s)</th>
			<th> Connected Peers </th>
			<th> Uploading Peers </th>
			<th> Speed per Peer (KBps) </th>
			<th> Last Heart Beat</th>
	   </tr>
   
<?php

$timezone = "Asia/Chongqing"; 
if(function_exists('date_default_timezone_set')) date_default_timezone_set($timezone); 

foreach( glob("servers_status/*.xml") as $file_name )
{
	$ip = substr(basename($file_name), 0, -4);
	$server_page_url = 'ServerPage.php?ip=' . $ip;

	$is_server_healthy = true;
	$now = time();
	$file_update_time = filemtime($file_name);
	$file_last_update_time = date ("H:i, F d", $file_update_time);
	if ($file_update_time + 5*60 > $now)
	{
		$file_last_update_time = 'now';
	}
	else
	{
		$is_server_healthy = false;
	}
	
	$xml = simplexml_load_file($file_name);
	$global_status = $xml->xpath('/live_status/global_status/status');
	
	$count = count($global_status);
	
	$upload_speed = 0;
	$connected_peers = 0;
	$uploading_peers = 0;
	$per_peer_speed = 0;
	if ($count == 1)
	{
		$status_node = $global_status[0];
		$attributes = $status_node->attributes();
		$upload_speed = $attributes["upload_speed"];
		$connected_peers = $attributes["user_count"];
		$uploading_peers = $attributes["uploading_user_count"];
		if ($uploading_peers > 0)
		{
			$per_peer_speed = round($upload_speed/1000/$uploading_peers, 1);
		}		
	}

	if ($is_server_healthy)
	{
		echo "<tr>\n";
	}
	else
	{
		echo "<tr class='bad_state'>\n";
	}
	
	echo '  <td>' . '<a href="' . $server_page_url . '">'. $ip . '</a>' . "</td>\n";
	echo '  <td>' . $upload_speed . "</td>\n";
	echo '  <td>' . $connected_peers . "</td>\n";
	echo '  <td>' . $uploading_peers . "</td>\n";
	echo '  <td>' . $per_peer_speed . "</td>\n";
	echo '  <td>' . $file_last_update_time . "</td>\n";
	echo "</tr>\n";
}

?>

	</table>
	
	<div>注: 加亮显示的是那些没有在最近5分钟内汇报过状态的UDP Server</div>
</div>

</body>
</html>
