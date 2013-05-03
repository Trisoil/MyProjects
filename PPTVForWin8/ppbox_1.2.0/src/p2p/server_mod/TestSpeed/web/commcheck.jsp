<%@ page import="com.speed.comm.Util" %>
<%@include file="commnocache.jsp"%>
<%
	com.speed.bean.SysUser Constant_User =(com.speed.bean.SysUser) session.getAttribute("sys.user");
    if(Constant_User == null) {
        //System.out.println("==sys user is null");
        response.sendRedirect("login.jsp?url="+ Util.encodeUrl(Util.encodeUrl(request.getRequestURI()+ tool.SuperPage.getQueryQ(request,""))));
        return;
    }
    String Constant_AdminName = com.speed.comm.Util.getProperties("CONST_ADMINS");
    if(Constant_AdminName == null) Constant_AdminName = "";
    String Constant_UserName = Constant_User.getUserName();
    boolean Constant_isAdmin = Constant_AdminName.indexOf("[" + Constant_UserName + "]") >= 0;
    boolean Constant_isSuperAdmin = Constant_isAdmin && Constant_UserName.startsWith("admin");
%>