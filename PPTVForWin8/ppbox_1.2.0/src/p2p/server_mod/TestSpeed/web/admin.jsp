<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="tool.SuperString" %>
<%@ page import="com.speed.comm.Util" %>
<%@include file="commcheck.jsp"%>
<%
    String url= Util.decodeUrl(SuperString.notNullTrim(request.getParameter("url")));
    if(url.indexOf("adminleft.jsp")>=0||url.indexOf("admin.jsp")>=0) url="";

    if(!"".equals(url)){
        session.setAttribute("temp_url",url);
        response.sendRedirect(request.getRequestURI());
        return;
    }

    url=SuperString.notNull((String)session.getAttribute("temp_url"));
    session.removeAttribute("temp_url");

    String welurl="welcome.jsp";
    if(!"".equals(url)) welurl=url;
    
%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>管理后台</title>
</head>

<frameset rows="*" cols="150,*" framespacing="4" frameborder="yes" border="4" bordercolor="#666666">
  <frame src="adminleft.jsp" name="left" frameborder="no" scrolling="auto" marginwidth="2" id="left">
  <frame src="<%=welurl%>" name="right" frameborder="no" id="right">
</frameset>
<noframes><body>
</body></noframes>
</html>
