<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="tool.SuperString" %>
<%@ page import="com.speed.comm.Util" %>
<%@ page import="com.speed.bean.SysRole" %>
<%@ page import="tool.SuperDate" %>
<%@ page import="com.speed.bean.SysLog" %>
<%@ page import="tool.SuperPage" %>
<%@ page import="my.myorm.tool.jdbc.SQLExecutor" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="com.speed.dao.DAOManager" %>
<%@ page import="java.util.Date" %>
<%@ page import="my.myorm.tool.jdbc.ResultList" %>
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
    String curName = "系统角色";

    int id = SuperString.getInt(request.getParameter("id"));
    String rolename = SuperString.notNullTrim(request.getParameter("rolename"));
    String rolename_old = SuperString.notNullTrim(request.getParameter("rolename_old"));

    if(!isDel && ("".equals(rolename))) {
        Util.callInfo("请填写完整！", request, response);
        return;
    }
    if(!isAdd && id <= 0) {
        Util.callInfo("错误的调用方法！", request, response);
        return;
    }
    if(!Constant_isAdmin && rolename.indexOf("管理员") >= 0) {
        Util.callInfo("请不要使用带有\"管理员\"字样的角色名称！", request, response);
        return;
    }

    SQLExecutor dbconn = SQLManager.getSQLExecutor();
    String sql = "";

    if(!Constant_isAdmin && id > 0) {
        SysRole role = DAOManager.sysRoleDao.findSysRole(id);
        if(role == null) {
            Util.callInfo("角色不存在！", request, response);
            return;
        }
        if(role.getRoleName().indexOf("管理员") >= 0) {
            Util.callInfo("没有权限！", request, response);
            return;
        }
    }

    if((isAdd || isMod) && !rolename.equalsIgnoreCase(rolename_old)) {
        sql = "select * from sys_role where rolename=? and rolename<>?";
        dbconn.addParam(rolename);
        dbconn.addParam(rolename_old);
        SysRole role = dbconn.queryForBean(sql, SysRole.class);
        if(role != null) {
            Util.callInfo("该系统角色名称已经存在，请重新填写！", request, response);
            return;
        }
    }

    if(isAdd) {
        SysRole role = new SysRole();
        role.setRoleName(rolename);
        role.setPermission("");
        role.setCreateTime(new Date());
        DAOManager.sysRoleDao.save(role);
    }

    if(isMod) {
        SysRole role = new SysRole();
        role.setId(id);
        role.setRoleName(rolename);
        DAOManager.sysRoleDao.update(role);
    }

    if(isDel) {
        sql = "select * from sys_user where roleid=?";
        dbconn.addParam(id);
        ResultList list = dbconn.query(sql, false, null);
        if(list.size() > 0) {
            Util.callInfo("该系统角色下面存在用户，不能删除！", request, response);
            return;
        }
        SysRole role = new SysRole();
        role.setId(id);
        DAOManager.sysRoleDao.delete(role);
    }

    LogUtil.writeLog(Constant_UserName, Util.getActText(act) + curName,
            id + "|" + rolename, request, null);

    response.sendRedirect(backFile + SuperPage.getQueryQ(request, "act,id"));
    return;
%>