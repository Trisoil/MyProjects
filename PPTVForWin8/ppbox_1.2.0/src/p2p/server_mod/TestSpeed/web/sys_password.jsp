<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="tool.*" %>
<%@ page import="my.myorm.tool.jdbc.SQLExecutor" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="my.myproperties.tool.PropUtil" %>
<%@ page import="com.speed.comm.Constant" %>
<%@include file="commcheck.jsp"%>
<%
    String curFile = SuperPage.getCurURLFile(request);
    String curName = "密码修改";

    if(request.getMethod().equalsIgnoreCase("post")) {

        String password_old = SuperString.notNull(request.getParameter("password_old"));
        String password = SuperString.notNull(request.getParameter("password"));
        String password2 = SuperString.notNull(request.getParameter("password2"));

        if(password_old.length() == 0 || password.length() == 0 || password2.length() == 0) {
            Util.callInfo("请填写完整！", request, response);
            return;
        }
        if(!password.equalsIgnoreCase(password2)) {
            Util.callInfo("两次输入的新密码不一致，请重新输入！", request, response);
            return;
        }

        SQLExecutor dbconn = SQLManager.getSQLExecutor();
        String sql = "";


        sql = "select id from sys_user where username=? and password=?";
        dbconn.addParam(Constant_UserName);
        dbconn.addParam(DESUtil.encodeString(password_old, Constant.DES_KEY));
        if(dbconn.query(sql, false, null).size() <= 0) {
            Util.callInfo("旧密码错误，修改失败！", request, response);
            return;
        }

        sql = "update sys_user set password=? where username=?";
        dbconn.addParam(DESUtil.encodeString(password, Constant.DES_KEY));
        dbconn.addParam(Constant_UserName);
        dbconn.execute(sql);

        request.setAttribute("showback", "n");
        String showInfo = "<br><font color=#008000 style='font-size:14px'>" +
                "密码修改成功！" +
                "</font>" +
                "<br><br><a href=\""+curFile+"\">&lt;&lt;返回密码修改&lt;&lt;</a>";
        Util.callInfo(showInfo, request, response);
        return;
    }


    String strOnSubmit=SuperPage.getOnSubmitCode("password_old,R;password,R;password2,R","");

%>

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>无标题文档</title>
<link href="css/common.css" rel="stylesheet" type="text/css">
<script language="javascript" src="js/comm.js"></script>
</head>

<body>
<div class="title_text"><%=curName%></div>

<div class="clear"><span/></div>
<table width="100%" border="0" cellpadding="3" cellspacing="1" class="table_main">
  <tr class="table_main_tt">
    <td><%=curName%></td>
  </tr>
  <tr class="tr_class1">
    <td><table width="100%"  border="0" cellspacing="1" cellpadding="2">
	<form name="form" method="post" action="<%=curFile%><%=SuperPage.getQueryQ(request,"")%>" <%=strOnSubmit%>>
      <tr>
        <td width="15%">&nbsp;</td>
        <td width="79%">&nbsp;</td>
        <td width="6%">&nbsp;</td>
      </tr>
      <tr>
        <td height="22" align="center" nowrap>用户名：</td>
        <td><b><font color="#FF6600"><%=Constant_UserName%></font></b></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td height="22" align="center" nowrap>输入旧密码：</td>
        <td><input name="password_old" type="password" maxlength="30" id="password_old" title="旧密码"></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td height="22" align="center" nowrap>输入新密码：</td>
        <td><input name="password" type="password" maxlength="30" id="password" title="新密码">
           </td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td height="22" align="center" nowrap>再输一次新密码：</td>
        <td><input name="password2" type="password" maxlength="30" id="password2" title="密码验证"></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td align="center">&nbsp;</td>
        <td height="40"> <input name="Submit" type="submit" class="BUTTON2" value="  提 交  ">          </td>
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
