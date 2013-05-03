<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="tool.*" %>
<%@ page import="com.speed.comm.Util" %>
<%@ page import="my.myorm.tool.jdbc.SQLExecutor" %>
<%@ page import="my.myproperties.tool.PropUtil" %>
<%@ page import="com.speed.bean.SysLog" %>
<%@ page import="com.speed.bean.SysUser" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="java.util.Date" %>
<%@ page import="com.speed.dao.DAOManager" %>
<%@ page import="com.speed.service.LogUtil" %>
<%@ page import="com.speed.comm.Constant" %>
<%

    if(request.getMethod().equalsIgnoreCase("post")) {
        //登录
        String username = SuperString.notNull(request.getParameter("username"));
        String password = SuperString.notNull(request.getParameter("password"));
        String url = SuperString.notNull(request.getParameter("url"));
        if(username.length() == 0 || password.length() == 0) {
            Util.callInfo("请输入帐号和口令！", request, response);
            return;
        }
        SQLExecutor dbconn = SQLManager.getSQLExecutor();
        String sql = "";
        sql = "select * from sys_user where username=? and password=? ";
        dbconn.addParam(username);
        dbconn.addParam(DESUtil.encodeString(password, Constant.DES_KEY));
        SysUser user = dbconn.queryForBean(sql, SysUser.class);
        if(user == null) {
            Util.callInfo("帐号或口令错误！", request, response);
            return;
        }

        LogUtil.writeLog(user.getUserName(),"登录系统","",request,null);

        session.setAttribute("sys.user", user);

        String gourl = "admin.jsp" + (!"".equals(url) ? "?url=" + Util.encodeUrl(url) : "");
        url = Util.decodeUrl(url);
        if(url.indexOf("admin.jsp") >= 0) gourl = url;
        response.sendRedirect(gourl);
        return;
    }


%>
