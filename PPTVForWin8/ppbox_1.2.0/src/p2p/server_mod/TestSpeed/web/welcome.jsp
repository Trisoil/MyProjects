<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="java.util.Calendar" %>
<%@ page import="java.io.File" %>
<%@ page import="java.util.List" %>
<%@ page import="java.util.ArrayList" %>
<%@ page import="tool.*" %>
<%@ page import="com.speed.bean.SysModule" %>
<%@include file="commcheck.jsp"%>
<%
    String clientInfo= SuperString.notNull(request.getHeader("User-Agent"));

    session.removeAttribute("xihttp_vod_online_rid");

%>
<html>
<head>
<meta  http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>欢迎进入管理中心</title>
<link href="css/common.css" rel="stylesheet" type="text/css">
</head>

<body>
<script type="text/javascript" src="js/calendar.js"></script>
<div id="divpop" style="border:1px solid #000000; width:200px; height:200px; background-color:bisque; position:absolute; visibility: hidden;">这是一个弹出层</div>
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td class="title_text">欢迎进入管理中心！</td>
  </tr>
</table>
<table width="100%"  border="0" cellspacing="1" cellpadding="2">
  <tr>
    <td height="8" bgcolor="#F3F3f3"></td>
  </tr>
  <tr>                               
    <td height="10"></td>
  </tr>
  <tr>
    <td><fieldset style="border:1px #FF8040 solid ">
      <legend><font color="#666666"><b>当前系统设置</b></font></legend>
      <table width="100%"  border="0" cellspacing="1" cellpadding="2">
        <tr>
          <td><table width="98%"  border="0" align="center" cellpadding="2" cellspacing="1">
            <tr>
              <td nowrap>&nbsp;</td>
              <td class="font23">&nbsp;</td>
              <td>&nbsp;</td>
            </tr>
            <tr>
              <td nowrap><font color="#0066CC">当前登录用户：</font></td>
              <td class="font23"><%=Constant_UserName%></td>
              <td>&nbsp;</td>
            </tr>
            <tr>
              <td width="13%" nowrap><font color="#0066CC">登录IP地址：</font></td>
              <td width="82%" class="font23"><%=request.getRemoteAddr()%></td>
              <td width="5%">&nbsp;</td>
            </tr>
          </table></td>
        </tr>
      </table>
    </fieldset></td>
  </tr>
  <tr>
    <td align="center">&nbsp;</td>
  </tr>
  <tr class="hide">
    <td align="center"><font color="#FF0000" size="+2" face="微软雅黑">从1月7日起，DAC系统移至：<a href="http://60.28.216.218:8080/dac/" target="_blank">http://60.28.216.218:8080/dac/</a><br>
    用户名不变，密码统一为pplive.com!，请登录后自行更改密码。</font></td>
  </tr>
  <tr class="hide">
    <td><b><font color="#FF0000" size="+1" face="微软雅黑">注意：18日数据不准确，正在重新统计中.......</font></b></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td><table width="100%"  border="0" cellspacing="2" cellpadding="0" style="border-top:1px dotted #999999;border-bottom:1px dotted #999999; ">
      <tr>
        <td align="center" nowrap><font color="#0000CC">使用本系统的要求：分辨率 1024×768 / 浏览器 IE6.0以上版本 / Flash插件支持</font></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
</table>
</body>
</html>
