<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="tool.SuperString" %>
<%@ page import="tool.SuperPage" %>
<%@ page import="com.speed.bean.SysRole" %>
<%@ page import="com.speed.bean.SysUser" %>
<%@ page import="com.speed.dao.DAOManager" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="my.myorm.tool.jdbc.SQLExecutor" %>
<%@ page import="com.speed.service.LogUtil" %>
<%@ include file="commcheck.jsp" %>
<%
    String curFile = SuperPage.getCurURLFile(request);
    String curName = "权限设置";
    boolean fromUser = SuperString.notNullTrim(request.getParameter("from")).equalsIgnoreCase("user");
    String theName = fromUser ? "用户" : "角色";
    String titleText = theName + curName;

    int id = SuperString.getInt(request.getParameter("id"));
    if(id <= 0) {
        Util.callInfo("错误的调用方法！", request, response);
        return;
    }

    boolean resetAllUserRole = SuperString.getInt(request.getParameter("resetuser")) == 1;

    String[] arrids_str = request.getParameterValues("check_id");
    if(arrids_str == null) arrids_str = new String[]{};
    String permission = "";
    for(int i = 0; i < arrids_str.length; i++) {
        if(arrids_str[i].length() > 1) permission += arrids_str[i] + ",";
    }
    if(permission.endsWith(",")) permission = permission.substring(0, permission.length() - 1);

    SQLExecutor dbconn = SQLManager.getSQLExecutor();
    String sql = "";

    if(fromUser) {
        SysUser user = new SysUser();
        user.setId(id);
        user.setPermission(permission);
        DAOManager.sysUserDao.update(user);
    } else {
        SysRole role = new SysRole();
        role.setId(id);
        role.setPermission(permission);
        DAOManager.sysRoleDao.update(role);
    }

    if(!fromUser && resetAllUserRole) {
        sql = "update sys_user set permission=? where roleid=?";
        dbconn.addParam(permission);
        dbconn.addParam(id);
        dbconn.execute(sql);
    }

    LogUtil.writeLog(Constant_UserName, titleText,
            id +"", request, null);

    //成功
    String backfile = fromUser ? "sys_user.jsp" : "sys_role.jsp";
    request.setAttribute("showback", "n");
    String showInfo = "<br><font color=#008000 style='font-size:14px'>" +
            "权限设置已经成功保存！" +
            "</font>" +
            "<br><br><a href=\"" + backfile + SuperPage.getQueryQ(request, "act,id,from") +
            "\">&lt;&lt;返回系统" + theName + "列表&lt;&lt;</a>";
    Util.callInfo(showInfo, request, response);
%>