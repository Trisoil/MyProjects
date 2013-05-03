<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="tool.SuperPage" %>
<%@ page import="tool.SuperString" %>
<%@ page import="my.myorm.tool.jdbc.SQLExecutor" %>
<%@ page import="com.speed.bean.SysUser" %>
<%@ page import="com.speed.dao.DAOManager" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="com.speed.bean.SysRole" %>
<%@ page import="java.util.Date" %>
<%@ page import="tool.DESUtil" %>
<%@ page import="com.speed.comm.Constant" %>
<%@ page import="com.speed.service.LogUtil" %>
<%@ include file="commcheck.jsp" %>
<%
    String act = SuperString.notNullTrim(request.getParameter("act"));
    if(!act.equalsIgnoreCase("mod") && !act.equalsIgnoreCase("del")) act = "add";
    boolean isAdd = act.equalsIgnoreCase("add");
    boolean isMod = act.equalsIgnoreCase("mod");
    boolean isDel = act.equalsIgnoreCase("del");
    String curFile = SuperPage.getCurURLFile(request);
    String backFile = SuperString.replace(curFile, "moddo", "");
    String curName = "系统用户";

    int id = SuperString.getInt(request.getParameter("id"));
    String username = SuperString.notNullTrim(request.getParameter("username"));
    String username_old = SuperString.notNullTrim(request.getParameter("username_old"));
    int roleid = SuperString.getInt(request.getParameter("roleid"));
    int roleid_old = SuperString.getInt(request.getParameter("roleid_old"));
    String password = SuperString.notNullTrim(request.getParameter("password"));
    String password2 = SuperString.notNullTrim(request.getParameter("password2"));
    int reloadper = SuperString.getInt(request.getParameter("reloadper"));

    boolean bReloadPermission = false;
    if(reloadper == 1) bReloadPermission = true;
    if(!isDel && roleid != roleid_old) bReloadPermission = true;

    if(!isDel && ("".equals(username) || roleid == 0)) {
        Util.callInfo("请填写完整！", request, response);
        return;
    }
    if(isAdd && ("".equals(password) || "".equals(password2))) {
        Util.callInfo("请将密码填写完整！", request, response);
        return;
    }
    if(!isDel && !password.equalsIgnoreCase(password2)) {
        Util.callInfo("两次输入的密码不一致，请重新输入！", request, response);
        return;
    }

    if(!isAdd && id <= 0) {
        Util.callInfo("错误的调用方法！", request, response);
        return;
    }

    String permission = "";

    SQLExecutor dbconn = SQLManager.getSQLExecutor();
    String sql = "";

    if(id > 0 && !Constant_isAdmin) {
        SysUser user = DAOManager.sysUserDao.findSysUser(id);
        if(user == null) {
            Util.callInfo("用户不存在！", request, response);
            return;
        }
        if(user.getUserName().equalsIgnoreCase("admin") || user.getUserName().equalsIgnoreCase(Constant_UserName)) {
            Util.callInfo("没有权限！", request, response);
            return;
        }
    }


    if((isAdd || isMod) && !username.equalsIgnoreCase(username_old)) {
        sql = "select * from sys_user where username=? and username<>?";
        dbconn.addParam(username);
        dbconn.addParam(username_old);
        SysUser user = dbconn.queryForBean(sql, SysUser.class);
        if(user != null) {
            Util.callInfo("该系统用户名称已经存在，请重新填写！", request, response);
            return;
        }
    }

    if(bReloadPermission) {
        SysRole role = DAOManager.sysRoleDao.findSysRole(roleid);
        if(role != null) permission = role.getPermission();
    }

    if(isAdd) {
        SysUser user = new SysUser();
        user.setUserName(username);
        user.setPassword(DESUtil.encodeString(password, Constant.DES_KEY));
        user.setRoleID(roleid);
        user.setPermission(permission);
        user.setCreateTime(new Date());
        DAOManager.sysUserDao.save(user);
    }

    if(isMod) {
        SysUser user = new SysUser();
        user.setId(id);
        user.setUserName(username);
        user.setRoleID(roleid);
        if(!"".equals(password)) user.setPassword(DESUtil.encodeString(password, Constant.DES_KEY));
        if(bReloadPermission) user.setPermission(permission);
        DAOManager.sysUserDao.update(user);
    }

    if(isDel) {
        SysUser user = new SysUser();
        user.setId(id);
        DAOManager.sysUserDao.delete(user);
    }


    LogUtil.writeLog(Constant_UserName, Util.getActText(act) + curName,
            id + "|" + username, request, null);

    response.sendRedirect(backFile + SuperPage.getQueryQ(request, "act,id"));
    return;    
%>