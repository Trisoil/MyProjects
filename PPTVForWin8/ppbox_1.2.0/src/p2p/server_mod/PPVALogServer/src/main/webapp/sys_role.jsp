<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="com.speed.bean.SysRole" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="my.myorm.tool.jdbc.ResultList" %>
<%@ page import="tool.SuperFile" %>
<%@ page import="tool.SuperPage" %>
<%@include file="commcheck.jsp"%>
<%
    String curFile = SuperPage.getCurURLFile(request);
    String modFile = SuperFile.getFileName(curFile) + "mod.jsp";
    String modFileDo = SuperFile.getFileName(curFile) + "moddo.jsp";
    String modFilePermission="sys_rolepermission.jsp";
    String curName = "系统角色";
    
    String sql = "select * from sys_role " +
            (Constant_isSuperAdmin ? "" : " where rolename not like '%管理员' ") +
            "order by id";
    ResultList list = SQLManager.getSQLExecutor().query(sql, false, null);

%>

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>无标题文档</title>
<link href="css/common.css" rel="stylesheet" type="text/css">
<script language="javascript" src="js/comm.js"></script>
<script language="javascript" src="js/tablecell.js"></script>    
</head>

<body>
<div class="title_text"><%=curName%>维护</div>

<div class="clear"><span/></div>

<table width="100%"  border="0" cellspacing="1" cellpadding="2">
  <tr>
    <td align="right"><input name="Submit" type="button" class="BUTTON5" value="  增加<%=curName%>  " onClick="location.href='<%=modFile%><%=SuperPage.getQueryQ(request,"act,id")%>'"></td>
  </tr>
</table>
<table width="100%" border="0" cellpadding="3" cellspacing="1" class="table_main">
  <tr class="table_main_tt">
    <td>ID</td>
    <td>系统角色名称</td>
    <td>操作</td>
  </tr>
<%
    for(int i = 0; i < list.size(); i++) {
        SysRole role=list.get(i).get(SysRole.class);
        
        String trclass = i % 2 == 0 ? "tr_class1" : "tr_class2";
%>
  <tr class="<%=trclass%>" onMouseMove="cell_over(this);" onMouseOut="cell_out(this)" onClick="cell_click(this);">
    <td><%=role.getId()%></td>
    <td><%=role.getRoleName()%></td>
    <td><a href="<%=modFilePermission%>?id=<%=role.getId()%>">权限设置</a> | <a href="<%=modFile%>?act=mod&id=<%=role.getId()%><%=SuperPage.getQueryAnd(request,"act,id")%>">修改</a> | <a href="<%=modFileDo%>?act=del&id=<%=role.getId()%><%=SuperPage.getQueryAnd(request,"act,id")%>" onClick="return delconfirm();">删除</a></td>
  </tr>
<%
    }
    if(list.size()==0){
%>
  <tr class="tr_class1">
    <td height="50" colspan="3" align="center">没有数据</td>
  </tr>
<%
    }
%>
</table>
<p>&nbsp;</p>
</body>
</html>
