<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="com.speed.bean.SysUser" %>
<%@ page import="com.speed.service.LogUtil" %>
<%
    SysUser user = (SysUser) session.getAttribute("sys.user");
    if(user == null) user = new SysUser();
    if(!"".equals(user.getUserName())){
        LogUtil.writeLog(user.getUserName(),"退出系统","",request,null);
    }
    session.removeAttribute("sys.user");
    response.sendRedirect("index.htm");
%>
