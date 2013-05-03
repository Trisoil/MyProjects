<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="java.util.Calendar" %>
<%@ page import="java.io.File" %>
<%@ page import="java.util.List" %>
<%@ page import="java.util.ArrayList" %>
<%@ page import="tool.*" %>
<%@ page import="com.speed.bean.SysModule" %>
<%@ page import="my.myproperties.tool.PropUtil" %>
<%@include file="commcheck.jsp"%>
<%@page import="my.myorm.tool.jdbc.SQLExecutor"%>
<%@page import="my.myorm.tool.jdbc.SQLManager"%>
<%@page import="java.sql.*"%>
<%@page import="org.apache.commons.io.FileSystemUtils"%>

<%
String runState = "200";
String path = PropUtil.getInstance("constant").getProperty("PPVALogPath");
File tempFile = new File(path);
if(tempFile.exists()){
	try{
		tempFile= new File(path+"/a.temp");
		tempFile.delete();
		System.out.println("测试文件系统成功");
	}catch (Exception e){
		runState="500";	
	}
}else{
	runState="500";
}

try {
	Connection mysqlConnection;
    mysqlConnection = DriverManager.getConnection(
         PropUtil.getInstance("constant").getProperty("PPVALogMySqlUrl"),
         PropUtil.getInstance("constant").getProperty("PPVALogMySqlUserName"),
         PropUtil.getInstance("constant").getProperty("PPVALogMySqlPassword"));
    if(!mysqlConnection.isClosed())
        System.out.println("测试数据库成功");
    mysqlConnection.close(); 
    
} catch(Exception e) {
	runState="500";
}

try {
    long freeSpace = FileSystemUtils.freeSpace(path)/1024/1024/1024;
    System.out.println(path+" "+freeSpace);
    if(freeSpace < 20){  runState="500";}
   // System.out.println(runState);
}catch (Exception e){
	runState="500";
}

if(runState.equals("500"))
    response.sendError(500, "请求页面存在错误");


%>
<html>
<head>
<meta  http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>运行状态</title>
<link href="css/common.css" rel="stylesheet" type="text/css">
</head>

<body>
成功
</body>
</html>
