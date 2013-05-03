<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8"%>
<%@ page import="tool.SuperPage" %>
<%@ page import="com.speed.bean.SysLog" %>
<%@ page import="my.myorm.tool.jdbc.SQLExecutor" %>
<%@ page import="my.myorm.tool.jdbc.ResultList" %>
<%@ page import="my.myorm.tool.jdbc.Pages" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="com.speed.comm.Util" %>
<%@ page import="tool.SuperString" %>
<%
    String curFile = SuperPage.getCurURLFile(request);
    String curName = "系统日志查看";

    String order = SuperString.notNullTrim(request.getParameter("order"));
    String orderby = order.length() == 0 ? " order by id desc" :
            " order by " + order;    

    SQLExecutor dbconn = SQLManager.getSQLExecutor();
    String sql="select * from sys_log "+orderby;
    Pages pages = new Pages(Util.getPropertiesInt("CONST_PAGESIZE",30), "id", request);
    ResultList rs =dbconn.query(sql,false,pages);

%>

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title><%=curName%></title>
<link href="css/common.css" rel="stylesheet" type="text/css">
<script language="javascript" src="js/comm.js"></script>
<script language="javascript" src="js/tablecell.js"></script>    
</head>

<body>
<div class="title_text"><%=curName%></div>

<div class="clear"><span/></div>

<div class="page_nav00"><%=SuperPage.generatePageNavSingle(rs.getAllCount(), request, null, pages.getPageSize(),"个")%></div>

<table width="100%" border="0" cellpadding="3" cellspacing="1" class="table_main">
  <tr class="table_main_tt">
    <td>用户</td>
    <td>操作</td>
    <td>内容</td>
    <td>IP</td>
    <td onClick="title_click('<%=Util.getOrderUrl("id",order,request)%>')">时间<%=Util.getOrderByImgHTML("id",order)%></td>
  </tr>
<%
    for(int i = 0; i < rs.size(); i++) {
        SysLog log= rs.get(i).get(SysLog.class);
        String trclass = i % 2 == 0 ? "tr_class1" : "tr_class2";
%>
  <tr class="<%=trclass%>" onMouseMove="cell_over(this);" onMouseOut="cell_out(this)" onClick="cell_click(this);">
    <td><%=log.getUserName()%></td>
    <td><%=log.getOperate()%></td>
    <td><%=log.getContent()%></td>
    <td class="font12"><%=log.getIP()%></td>
    <td class="font12"><%=log.getCreateTime_ToDateTimeString()%></td>
  </tr>
<%
    }
    if(rs.size()==0){
%>
  <tr bgcolor="#FFFFFF">
    <td height="50" colspan="5" align="center">没有数据</td>
  </tr>
  <%
    }
%>
</table>
<div class="page_nav01"><%=SuperPage.generatePageNavSingle(rs.getAllCount(), request, null, pages.getPageSize(),"个")%></div>
<div style="height:30px;"/>
</body>
</html>
