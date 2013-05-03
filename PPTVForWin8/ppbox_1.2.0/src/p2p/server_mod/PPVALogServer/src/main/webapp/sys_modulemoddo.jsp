<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="com.speed.bean.SysRole" %>
<%@ page import="com.speed.bean.SysModule" %>
<%@ page import="tool.*" %>
<%@ page import="com.speed.bean.SysUser" %>
<%@ page import="com.speed.comm.Util" %>
<%@ page import="my.myorm.tool.jdbc.SQLExecutor" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="java.util.Date" %>
<%@ page import="com.speed.dao.DAOManager" %>
<%@ page import="my.myorm.tool.jdbc.ResultList" %>
<%@ page import="com.speed.service.LogUtil" %>
<%@ include file="commcheck.jsp" %>
<%!public static void resetPermission(String moduleid, int id, String table, SQLExecutor dbcon) throws Exception {
        String sql = "select id,permission from sys_" + table + " where id=" + id;
        if(table.equalsIgnoreCase("role")) {
            SysRole role = dbcon.queryForBean(sql, SysRole.class);
            if(role != null) {
                String permission = role.getPermission();
                while(permission.indexOf(",,") >= 0) {
                    permission = SuperString.replace(permission, ",,", ",");
                }
                if(permission.startsWith(",")) permission = permission.substring(1);
                if(permission.endsWith(",")) permission = permission.substring(0, permission.length() - 1);
                if(permission.indexOf(moduleid) < 0) {
                    permission += (permission.length() > 0 ? "," : "") + moduleid;
                }
                role.setPermission(permission);
                DAOManager.sysRoleDao.update(role);
            }
        } else {
            SysUser user = dbcon.queryForBean(sql, SysUser.class);
            if(user != null) {
                String permission = user.getPermission();
                while(permission.indexOf(",,") >= 0) {
                    permission = SuperString.replace(permission, ",,", ",");
                }
                if(permission.startsWith(",")) permission = permission.substring(1);
                if(permission.endsWith(",")) permission = permission.substring(0, permission.length() - 1);
                if(permission.indexOf(moduleid) < 0) {
                    permission += (permission.length() > 0 ? "," : "") + moduleid;
                }
                user.setPermission(permission);
                DAOManager.sysUserDao.update(user);
            }
        }
    }


    public static void deleteModuleFromPermission(String moduleid, SQLExecutor dbcon) throws Exception {
        String sql = "";
        sql = "update sys_role set permission=replace(permission,'," + moduleid + "','')";
        dbcon.execute(sql);
        sql = "update sys_role set permission=replace(permission,'" + moduleid + ",','')";
        dbcon.execute(sql);
        sql = "update sys_role set permission=replace(permission,'" + moduleid + "','')";
        dbcon.execute(sql);

        sql = "update sys_user set permission=replace(permission,'," + moduleid + "','')";
        dbcon.execute(sql);
        sql = "update sys_user set permission=replace(permission,'" + moduleid + ",','')";
        dbcon.execute(sql);
        sql = "update sys_user set permission=replace(permission,'" + moduleid + "','')";
        dbcon.execute(sql);

        sql = "update sys_role set permission=replace(permission,',,',',')";
        dbcon.execute(sql);
        sql = "update sys_user set permission=replace(permission,',,',',')";
        dbcon.execute(sql);
    }%>
<%
    if(!Constant_isAdmin) {
        Util.callInfo("没有操作权限！", request, response);
        return;
    }

    String act = SuperString.notNullTrim(request.getParameter("act"));
    if(!act.equalsIgnoreCase("mod") && !act.equalsIgnoreCase("del")) act = "add";
    boolean isAdd = act.equalsIgnoreCase("add");
    boolean isMod = act.equalsIgnoreCase("mod");
    boolean isDel = act.equalsIgnoreCase("del");
    String curFile = SuperPage.getCurURLFile(request);
    String backFile = SuperString.replace(curFile, "moddo", "");
    String curName = "系统模块";

    int id = SuperString.getInt(request.getParameter("id"));
    String moduleid = SuperString.notNullTrim(request.getParameter("moduleid"));
    String moduleid_old = SuperString.notNullTrim(request.getParameter("moduleid_old"));
    String modulename = SuperString.notNullTrim(request.getParameter("modulename"));
    String parentid = SuperString.notNullTrim(request.getParameter("parentid"));
    String link = SuperString.notNullTrim(request.getParameter("link"));
    String linkclass = SuperString.notNullTrim(request.getParameter("linkclass"));
    int status = SuperString.getInt(request.getParameter("status"));
    String memo = SuperString.notNullTrim(request.getParameter("memo"));

    String[] arrids_str = request.getParameterValues("check_id");
    if(arrids_str == null) arrids_str = new String[]{};

    if(!isDel && ("".equals(moduleid) || "".equals(modulename) || "".equals(parentid))) {
        Util.callInfo("请填写完整！", request, response);
        return;
    }
    if(isDel && id <= 0) {
        Util.callInfo("错误的调用方法！", request, response);
        return;
    }

    SQLExecutor dbconn = SQLManager.getSQLExecutor();
    String sql = "";

    if((isAdd || isMod) && !moduleid.equalsIgnoreCase(moduleid_old)) {
        sql = "select id from sys_module where moduleid=? and moduleid<>?";
        dbconn.addParam(moduleid);
        dbconn.addParam(moduleid_old);
        SysModule module = dbconn.queryForBean(sql, SysModule.class);
        if(module != null) {
            Util.callInfo("该模块ID已经存在，请重新填写！", request, response);
            return;
        }
    }

    if(isAdd) {
        SysModule module = new SysModule();
        module.setModuleID(moduleid);
        module.setModuleName(modulename);
        module.setParentID(parentid);
        module.setLink(link);
        module.setLinkClass(linkclass);
        module.setStatus(status);
        module.setMemo(memo);
        module.setCreateTime(new Date());
        DAOManager.sysModuleDao.save(module);
    }

    if(isMod) {
        SysModule module = new SysModule();
        module.setId(id);
        module.setModuleID(moduleid);
        module.setModuleName(modulename);
        module.setParentID(parentid);
        module.setLink(link);
        module.setLinkClass(linkclass);
        module.setStatus(status);
        module.setMemo(memo);
        module.setCreateTime(new Date());
        DAOManager.sysModuleDao.update(module);
    }

    //加入权限
    if((isAdd || isMod) && moduleid.length() > 1) {
        if(isMod) deleteModuleFromPermission(moduleid_old, dbconn);
        for(String strid : arrids_str) {
            String table = "";
            int _id = 0;
            if(strid.startsWith("role_")) {
                table = "role";
                _id = SuperString.getInt(strid.substring("role_".length()));
            } else {
                table = "user";
                _id = SuperString.getInt(strid);
            }
            resetPermission(moduleid, _id, table, dbconn);
        }

    }


    if(isDel) {
        sql = "select id from sys_module where parentid=?";
        dbconn.addParam(moduleid);
        ResultList list = dbconn.query(sql, false, null);
        if(list.size() > 0) {
            Util.callInfo("该模块下面存在子模块，不能删除，必须先删除子模块才可进行此操作！", request, response);
            return;
        }

        SysModule module = new SysModule();
        module.setId(id);
        DAOManager.sysModuleDao.delete(module);

        //将模块从权限表中删除
        if(moduleid.length() > 1) deleteModuleFromPermission(moduleid, dbconn);

    }

    LogUtil.writeLog(Constant_UserName, Util.getActText(act) + curName,
            id + "|" + modulename, request, null);

    response.sendRedirect(backFile + SuperPage.getQueryQ(request, "act,id"));
    return;
%>