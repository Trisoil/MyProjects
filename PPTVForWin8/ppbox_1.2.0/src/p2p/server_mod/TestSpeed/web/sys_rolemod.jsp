<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="tool.SuperString" %>
<%@ page import="com.speed.comm.Util" %>
<%@ page import="com.speed.bean.SysRole" %>
<%@ page import="tool.SuperPage" %>
<%@ page import="my.myorm.tool.jdbc.SQLExecutor" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="tool.SuperFile" %>
<%@ page import="com.speed.dao.DAOManager" %>
<%@include file="commcheck.jsp"%>
<%session.setAttribute("cache","y");%>
<%
    String act = SuperString.notNullTrim(request.getParameter("act"));
    if(!act.equalsIgnoreCase("mod")) act = "add";
    boolean isAdd = act.equalsIgnoreCase("add");
    boolean isMod = act.equalsIgnoreCase("mod");
    String titletext = isAdd ? "新增" : "修改";

    String curFile = SuperPage.getCurURLFile(request);
    String modFileDo = SuperFile.getFileName(curFile) + "do.jsp";
    String curName = "系统角色";

    int id = SuperString.getInt(request.getParameter("id"));

    if(!isAdd && id <= 0) {
        Util.callInfo("错误的调用方法！", request, response);
        return;
    }

    SysRole role= isMod? DAOManager.sysRoleDao.findSysRole(id):new SysRole();
    if(isMod && role==null){
        Util.callInfo("数据不存在！", request, response);
        return;
    }
    if(!Constant_isAdmin && role.getRoleName().indexOf("管理员") >= 0) {
        Util.callInfo("没有权限！", request, response);
        return;
    }

    String strOnSubmit = SuperPage.getOnSubmitCode("rolename,R", "");

%>

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>无标题文档</title>
<link href="css/common.css" rel="stylesheet" type="text/css">
<script language="javascript" src="js/comm.js"></script>
</head>

<body>
<div class="title_text"><%=curName%>维护</div>

<div class="clear"><span/></div>

<table width="100%"  border="0" cellspacing="1" cellpadding="2">
  <tr>
    <td align="right"><input name="Submit22" type="button" class="BUTTON5" value="   返回上页   " onClick="history.go(-1);"></td>
  </tr>
</table>
<table width="100%" border="0" cellpadding="3" cellspacing="1" class="table_main">
  <tr class="table_main_tt">
    <td><%=titletext+curName%></td>
  </tr>
  <tr class="tr_class1">
    <td><table width="100%"  border="0" cellspacing="1" cellpadding="2">
	<form name="form" method="post" action="<%=modFileDo%><%=SuperPage.getQueryQ(request,"")%>" <%=strOnSubmit%>>
        <input type="hidden" name="rolename_old" value="<%=role.getRoleName()%>">
      <tr>
        <td width="13%">&nbsp;</td>
        <td width="81%">&nbsp;</td>
        <td width="6%">&nbsp;</td>
      </tr>
      <tr>
        <td height="22" align="center">系统角色名称：</td>
        <td><input name="rolename" type="text" maxlength="20" id="rolename" value="<%=role.getRoleName()%>" title="系统角色名称"></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td align="center">&nbsp;</td>
        <td height="30"> <input name="Submit" type="submit" class="BUTTON2" value=" 提 交 ">
          <input name="Submit" type="button" class="BUTTON2" value=" 取 消 " onClick="history.back(1)"></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td align="center">&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
	  </form>
    </table></td>
  </tr>
</table>
<br>
</body>
</html>
