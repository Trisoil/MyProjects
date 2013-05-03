<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<%@page import="my.myorm.tool.jdbc.SQLExecutor"%>
<%@page import="my.myorm.tool.jdbc.SQLManager"%>
<%@page import="tool.SuperString"%>
<%@page import="com.speed.bean.Speed"%>
<%@page import="my.myorm.tool.jdbc.ResultList"%>
<%@page import="com.speed.comm.Util"%>
<%@page import="java.util.Date"%>
<%
String speedKey=SuperString.notNullTrim(request.getParameter("key"));
String speedFlvFile=Util.getProperties("SPEED_FLV_FILE");
String msg="";
boolean isPlay=false;
if(speedKey.length()==0){
	msg="KEY不能为空！";
}else{
	SQLExecutor dbconn = SQLManager.getSQLExecutor();
	String sql="select * from speed_list where speedKey='"+speedKey+"' order by id desc";
	ResultList rs=dbconn.query(sql,false,null);
	if(rs.size()==0){
		msg="KEY不存在！";
	}else{
		Speed speed=rs.get(0).get(Speed.class);
		//
		int speedTime=Util.getPropertiesInt("SPEED_TIME");
		long sysTimeL=System.currentTimeMillis();
		long createTimeL=speed.getCreateTime().getTime();
		long timeL=(sysTimeL-createTimeL)/1000;
		if(timeL>speedTime){
			msg="KEY已经过期！";
		}else{
			isPlay=true;
		}
	}
}
%>
<!doctype html>
<html>
<head>
<title>PPTV服务器在线网速测试</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<style type="text/css">
html, body{margin:0;padding:0;font-size:14px;}
body{background:#def0ff;}
.swf{width:540px;height:460px;position:absolute;left:50%;top:50%;margin-left:-270px;margin-top:-230px;}
</style>
</head>
<body>
<script type="text/javascript">
function getSwf(name, method){
	var swf = window[name] || document[name];
	if (typeof swf['method'] != 'function') {
		swf = document.getElementById(name);
	}
	return swf;
}
function speedReady() {
	var swf = getSwf('speed', 'setUrls');
	var key = (location.search || 'key=testkey').split('=')[1];
	swf.setUrls({key: key, list: 'http://testspeed.synacast.com:8080/serverspeed/server_list.jsp', submit: 'http://testspeed.synacast.com:8080/postTestResult.jsp' });
}
</script>
<%if(isPlay==true){ %>
<div class="swf">
	<!--[if IE]>
	<object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" width="540" height="460" id="speed" align="middle">
		<param name="movie" value="<%=speedFlvFile%>" />
		<param name="quality" value="high" />
		<param name="bgcolor" value="#ffffff" />
		<param name="play" value="true" />
		<param name="loop" value="true" />
		<param name="wmode" value="window" />
		<param name="scale" value="showall" />
		<param name="menu" value="true" />
		<param name="devicefont" value="false" />
		<param name="salign" value="" />
		<param name="allowScriptAccess" value="always" />
		<a href="http://www.adobe.com/go/getflash">
			<img src="http://www.adobe.com/images/shared/download_buttons/get_flash_player.gif" alt="获得 Adobe Flash Player" />
		</a>
	</object>
	<![endif]-->
	
	<!--[if !IE]>-->
	<object id="speed" type="application/x-shockwave-flash" data="<%=speedFlvFile%>" width="540" height="460">
		<param name="movie" value="<%=speedFlvFile%>" />
		<param name="quality" value="high" />
		<param name="bgcolor" value="#ffffff" />
		<param name="play" value="true" />
		<param name="loop" value="true" />
		<param name="wmode" value="window" />
		<param name="scale" value="showall" />
		<param name="menu" value="true" />
		<param name="devicefont" value="false" />
		<param name="salign" value="" />
		<param name="allowScriptAccess" value="always" />
		<a href="http://www.adobe.com/go/getflash">
			<img src="http://www.adobe.com/images/shared/download_buttons/get_flash_player.gif" alt="获得 Adobe Flash Player" />
		</a>
	</object>
	<!--<![endif]-->
</div>
<%}else{ %>
	<font color="red" size="4"><%=msg %></font>
<%} %>
</body>
</html>