<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ page import="tool.SuperString" %>
<%@ page import="tool.SuperPage" %>
<%@ page import="com.speed.comm.Util" %>
<%@ page import="java.util.Date" %>
<%@ page import="java.util.Random" %>
<%@ page import="com.speed.service.RenewNotifyService" %>
<%@include file="../commnocache.jsp"%>
<%
    response.setBufferSize(0);
    out.flush();
    out.println("<font style=\"font-size:12px;font-family:Tahoma;\">");

    String username = SuperString.notNullTrim(request.getParameter("username"));
    if(request.getMethod().equalsIgnoreCase("post")){
        if("".equals(username)){
            out.println("Please input username!<br>");
            return;
        }
        RenewNotifyService renewNotity=(RenewNotifyService)tool.SpringBeanProxy.getBean("renewNotify");
        renewNotity.renewNotify(username);
        out.println("<br>put username:"+username+"....ok!<br><br>");
    }
%>

<b>Put to TT</b><br>
---------------------------<bR><br>
<form style="display:inline;" action="<%=request.getRequestURI()%>" name="form1" method="post">
    用户名:
      <input name="username" type="text" value="<%=username%>" size="20"/>
    <input type="submit" name="Submit" value="Put"/>
</form>

