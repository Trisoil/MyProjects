<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="tool.SuperString" %>
<%@ include file="commnocache.jsp" %>
<%
    String commMessage = SuperString.notNull((String) request.getAttribute("commMessage"));
    if(commMessage.length() == 0) commMessage = "系统维护中，请稍后再试！";

    String showback = SuperString.notNullTrim((String) request.getAttribute("showback"));
    String showback_url=SuperString.notNullTrim((String) request.getAttribute("showback_url"));
    if(showback_url.length()==0) showback_url="javascript:history.go(-1);";
%>
<html>
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <title>系统信息显示</title>
    <link href="css/common.css" rel="stylesheet" type="text/css">
</head>

<body>
<table width="100%" border="0" cellpadding="10" cellspacing="0" bgcolor="#FFFFFF">
    <tr>
        <td><br>
            <br>
            <br>
            <br>
            <br>
            <br>
            <br> <center>
            <table width="450" border="0" cellpadding="2" cellspacing="1" class="table_main">
                <tr class="table_main_tt">
                    <td align="center"><b>系统信息</b></td>
              </tr>
                <tr>
                    <td align="center" bgcolor="#FFFFFF" style="padding:10px;"><br><font color="FF0000"><%=commMessage%></font><br> <br>
                        <%
                            if(showback.equalsIgnoreCase("")) {
                        %>
                        <a href="<%=showback_url%>">&lt;&lt;返回&lt;&lt;</a>
                        <%
                            }
                        %>
                        <br>
                        &nbsp;
                    </td>
                </tr>
            </table>
            <br>
            <br>
            <br>
            <br>
        </center>
        </td>
    </tr>
</table>
</body>
</html>