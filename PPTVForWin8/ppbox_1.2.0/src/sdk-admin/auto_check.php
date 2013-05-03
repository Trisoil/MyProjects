<?php

	$con =  mysql_connect("localhost","hkm","123456");
	if (!$con)
	{
		die ('could not connect:'.mysql_error());
	}

	mysql_select_db("ppbox",$con);

	$sql = "select * from t_version where name = ''";
	$result = mysql_query($sql);
	while($row = mysql_fetch_array($result))
	{
		$sql = "select * from t_project where id = $row['project_id']";
		$result = mysql_query($sql);
		$res = mysql_fetch_array($result);

		$sql = "select * from t_platform where id = $res['platform_id']";
		$result = mysql_query($sql);
		$res1 = mysql_fetch_array($result);
		$platform = $res1['name'];

		$sql = "select * from t_strategy where id = $res['strategy_id ']";
		$result = mysql_query($sql);
		$res2 = mysql_fetch_array($result);
		$strategy = $res2['name'];

		$sql = "select * from t_branch where id = $row['branch_id']";
		$result = mysql_query($sql);
		$res3 = mysql_fetch_array($result);
		$branch = $res3['name'];

		$sql = "select * from t_domain_strategy where id = $res['domain_strategy_id']";
		$result = mysql_query($sql);
		$res4 = mysql_fetch_array($result);
		$domain_strategy = $res4['name'];

		$handle = popen("./shell/auto_compile.sh " . $platform . " " . $branch . " " . $strategy . " " . $domain_strategy, "r");
		$out = fgets($handle);

		$sql = "update t_version set name = $out where id = $row['id'] ";
		mysql_query($sql);
	}

	mysql_close($con);

?>

