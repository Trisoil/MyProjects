<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="tool.SuperPage" %>
<%@ page import="tool.SuperString" %>
<%@ page import="my.myorm.tool.jdbc.ResultList" %>
<%@ page import="com.speed.bean.SysUser" %>
<%@ page import="tool.SuperFile" %>
<%@ page import="com.speed.dao.DAOManager" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="com.speed.bean.SysRole" %>
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
    String curName = "系统用户";

    int id = SuperString.getInt(request.getParameter("id"));

    if(!isAdd && id <= 0) {
        Util.callInfo("错误的调用方法！", request, response);
        return;
    }

    SysUser user = isMod? DAOManager.sysUserDao.findSysUser(id):new SysUser();
    if(isMod && user ==null){
        Util.callInfo("数据不存在！", request, response);
        return;
    }
    if(!Constant_isAdmin && ("admin".equalsIgnoreCase(user.getUserName())
            || Constant_UserName.equalsIgnoreCase(user.getUserName()))) {
        Util.callInfo("没有权限！", request, response);
        return;
    }

    String sql = "select * from sys_role " +
            (Constant_isSuperAdmin ? "" : " where rolename not like '%管理员' ") +
            " order by id";
    ResultList list = SQLManager.getSQLExecutor().query(sql, false, null);

    String strOnSubmit = SuperPage.getOnSubmitCode("username,R" +
            (act.equalsIgnoreCase("add") ? ";password,R;password2,R" : "") + ";roleid,R", "");

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
        <input type="hidden" name="username_old" value="<%=user.getUserName()%>">
        <input type="hidden" name="roleid_old" value="<%=user.getRoleID()%>">
      <tr>
        <td width="13%">&nbsp;</td>
        <td width="81%">&nbsp;</td>
        <td width="6%">&nbsp;</td>
      </tr>
      <tr>
        <td height="22" align="center">用户名称：</td>
        <td><input name="username" type="text" maxlength="30" id="username" value="<%=user.getUserName()%>" title="用户名称"></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td height="22" align="center">登录密码：</td>
        <td><input name="password" type="password" maxlength="30" id="password" title="登录密码">
          &nbsp; <%if(act.equalsIgnoreCase("mod")){%><font color="#FF3300">密码不填写则不修改</font><%}%></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td height="22" align="center">再输一次密码：</td>
        <td><input name="password2" type="password" maxlength="30" id="password2" title="再输一次密码"></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td height="22" align="center">所属角色：</td>
        <td><select name="roleid" id="roleid">
<%
    for(int i = 0; i < list.size(); i++) {
        SysRole role=list.get(i).get(SysRole.class);
        out.println("<option value=\""+role.getId() +"\" "+(role.getId()==user.getRoleID()?"selected":"")+">"+
                role.getRoleName()+"</option>");
    }
%>
        </select>
          <%if(isMod){%><input name="reloadper" type="checkbox" id="reloadper" value="1">
          <b><font color="#FF6600">重载角色权限</font></b>
          <%}%></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td align="center">&nbsp;</td>
        <td height="50"> <input name="Submit" type="submit" class="BUTTON2" value="  提 交  ">
          <input name="Submit" type="button" class="BUTTON2" value="  取 消  " onClick="history.back(1)"></td>
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
</body>
</html>
