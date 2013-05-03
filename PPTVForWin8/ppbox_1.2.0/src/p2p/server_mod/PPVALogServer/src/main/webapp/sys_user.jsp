<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="tool.SuperPage" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="tool.SuperFile" %>
<%@ page import="my.myorm.tool.jdbc.ResultList" %>
<%@ page import="com.speed.bean.SysUser" %>
<%@ page import="com.speed.bean.SysRole" %>
<%@include file="commcheck.jsp"%>
<%
    String curFile = SuperPage.getCurURLFile(request);
    String modFile = SuperFile.getFileName(curFile) + "mod.jsp";
    String modFileDo = SuperFile.getFileName(curFile) + "moddo.jsp";
    String modFilePermission = "sys_rolepermission.jsp";
    String curName = "系统用户";

    String sql = "select a.*,b.rolename from sys_user as a " +
            "left join sys_role as b " +
            "on a.roleid=b.id " +
            "where 1=1 " +
            (Constant_isSuperAdmin ? "" : " and a.username<>'" +
                    Constant_UserName + "' and a.username not like '%admin' ") +
            "order by b.id,a.username";
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
    <td>系统用户名称</td>
    <td>所属角色</td>
    <td>操作</td>
  </tr>
<%
    for(int i = 0; i < list.size(); i++) {
        SysUser user=list.get(i).get(SysUser.class);
        SysRole role=list.get(i).get(SysRole.class);

        String trclass = i % 2 == 0 ? "tr_class1" : "tr_class2";
%>
  <tr class="<%=trclass%>" onMouseMove="cell_over(this);" onMouseOut="cell_out(this)" onClick="cell_click(this);">
    <td><%=user.getUserName()%></td>
    <td><%=role.getRoleName()%></td>
    <td><a href="<%=modFilePermission%>?from=user&id=<%=user.getId()%>">权限设置</a> | <a href="<%=modFile%>?act=mod&id=<%=user.getId()%><%=SuperPage.getQueryAnd(request,"act,id")%>">修改</a> | <a href="<%=modFileDo%>?act=del&id=<%=user.getId()%><%=SuperPage.getQueryAnd(request,"act,id")%>" onClick="return delconfirm();">删除</a></td>
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
