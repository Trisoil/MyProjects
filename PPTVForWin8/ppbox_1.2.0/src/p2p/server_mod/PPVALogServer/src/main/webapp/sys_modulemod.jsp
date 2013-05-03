<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="java.util.List"%>
<%@ page import="java.util.ArrayList"%>
<%@ page import="tool.SuperString" %>
<%@ page import="com.speed.bean.SysModule" %>
<%@ page import="com.speed.bean.SysRole" %>
<%@ page import="com.speed.comm.Util" %>
<%@ page import="tool.SuperPage" %>
<%@ page import="com.speed.bean.SysUser" %>
<%@ page import="my.myorm.tool.jdbc.SQLExecutor" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="my.myorm.tool.jdbc.ResultList" %>
<%@ page import="com.speed.dao.DAOManager" %>
<%@ page import="tool.SuperFile" %>
<%@include file="commcheck.jsp"%>
<%session.setAttribute("cache","y");%>
<%
    if(!Constant_isAdmin){
        Util.callInfo("没有操作权限！",request,response);
        return;
    }

    String act = SuperString.notNullTrim(request.getParameter("act"));
    if(!act.equalsIgnoreCase("mod")) act = "add";
    boolean isAdd = act.equalsIgnoreCase("add");
    boolean isMod = act.equalsIgnoreCase("mod");
    String titletext = isAdd ? "新增" : "修改";

    String curFile = SuperPage.getCurURLFile(request);
    String modFileDo = SuperFile.getFileName(curFile) + "do.jsp";
    String curName = "系统模块";

    int id=SuperString.getInt(request.getParameter("id"));
    if(act.equalsIgnoreCase("mod") && id<=0){
        Util.callInfo("错误的调用方法！",request,response);
        return;
    }

    SysModule module= isMod?DAOManager.sysModuleDao.findSysModule(id):new SysModule();
    if(isMod && module==null){
        Util.callInfo("该模块不存在！", request, response);
        return;
    }

    SQLExecutor dbconn = SQLManager.getSQLExecutor();
    String sql = "select * from sys_role";
    ResultList list = dbconn.query(sql, false, null);

    String strOnSubmit = SuperPage.getOnSubmitCode("moduleid,R;modulename,R;parentid,R", "");
%>

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>无标题文档</title>
<link href="css/common.css" rel="stylesheet" type="text/css">
<script language="javascript" src="js/comm.js"></script>
<script language="javascript">
function click_role(obj){
    if(obj==null) return;
    var inputs=document.getElementsByTagName("input");
    if(inputs!=null){
        for(var i=0; i<inputs.length; i++){
            if(inputs[i].id.indexOf(obj.id+"_")>=0){
                document.getElementById(inputs[i].id).checked=obj.checked;
            }
        }
    }
}
</script>
</head>

<body>
<div class="title_text"><%=curName%>维护</div>

<div class="clear"><span/></div>

<table width="100%"  border="0" cellspacing="1" cellpadding="2">
  <tr>
    <td align="right"><input name="Submit22" type="button" class="BUTTON5" value="   返回上页   " onClick="history.go(-1);"></td>
  </tr>
</table>
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
	<form name="form" method="post" action="<%=modFileDo%><%=SuperPage.getQueryQ(request,"")%>" <%=strOnSubmit%>>
        <input type="hidden" name="moduleid_old" value="<%=module.getModuleID()%>">
    <tr>
      <td><table width="100%" border="0" cellpadding="3" cellspacing="1" class="table_main">
        <tr align="left" class="table_main_tt">
          <td><%=titletext+curName%></td>
        </tr>
        <tr class="tr_class1">
          <td><table width="100%"  border="0" cellspacing="1" cellpadding="2">
              <tr>
                <td>&nbsp;</td>
                <td>&nbsp;</td>
                <td>&nbsp;</td>
              </tr>
              <tr>
                <td width="13%" align="center">模块ID：</td>
                <td width="81%"><input name="moduleid" maxlength="10" type="text" id="moduleid" value="<%=module.getModuleID()%>" title="模块ID">                </td>
                <td width="6%">&nbsp;</td>
              </tr>
              <tr>
                <td height="22" align="center">模块名称：</td>
                <td><input name="modulename" type="text" id="modulename" title="模块名称" value="<%=module.getModuleName()%>" size="50" maxlength="30"></td>
                <td>&nbsp;</td>
              </tr>
              <tr>
                <td align="center">父模块ID：</td>
                <td><input name="parentid" maxlength="10" type="text" id="parentid" value="<%=module.getParentID()%>" title="父模块ID"></td>
                <td>&nbsp;</td>
              </tr>
              <tr>
                <td height="22" align="center">链接地址：</td>
                <td><input name="link" type="text" id="link" title="链接地址" value="<%=module.getLink()%>" size="90" maxlength="255"></td>
                <td>&nbsp;</td>
              </tr>
              <tr>
                <td height="22" align="center">链接Class：</td>
                <td><input name="linkclass" type="text" id="linkclass" title="链接Class" value="<%=module.getLinkClass()%>" maxlength="20"></td>
                <td>&nbsp;</td>
              </tr>
              <tr>
                <td height="22" align="center">备注：</td>
                <td><input name="memo" type="text" id="memo" title="模块名称" value="<%=module.getMemo()%>" size="50" maxlength="50"></td>
                <td>&nbsp;</td>
              </tr>
              <tr>
                <td height="22" align="center">状态：</td>
                <td><input type="radio" name="status" value="0" <%if(module.getStatus()==0) out.println("checked");%>>
                  <b><font color="#333333">正常</font></b>
                    <input type="radio" name="status" value="1" <%if(module.getStatus()==1) out.println("checked");%>>
                  <b><font color="#0066FF">系统</font></b>
                  <input type="radio" name="status" value="2" <%if(module.getStatus()==2) out.println("checked");%>>
                  <b><font color="#FF6600">隐藏</font></b></td>
                <td>&nbsp;</td>
              </tr>
              <tr>
                <td align="center">&nbsp;</td>
                <td height="30"><input name="Submit" type="submit" class="BUTTON2" value=" 提 交 ">
                    <input name="Submit" type="button" class="BUTTON2" value=" 取 消 " onClick="history.back(1)"></td>
                <td>&nbsp;</td>
              </tr>
              <tr>
                <td align="center">&nbsp;</td>
                <td>&nbsp;</td>
                <td>&nbsp;</td>
              </tr>
          </table></td>
        </tr>
      </table></td>
    </tr>
    <tr>
      <td height="5"></td>
    </tr>
<%
    if(module.getModuleID().length()>1 || isAdd){
%>
    <tr>
      <td><table width="100%"  border="0" cellspacing="1" cellpadding="2">
          <tr>
            <td width="64%" height="25" align="left"><font color="#0066FF"><b>权限设置</b></font></td>
          </tr>
      </table></td>
    </tr>
    <tr>
      <td><div id="div_module" style="width:100%; border:1px solid #0099cc; overflow:visible; ">
          <table width="100%" border="0" cellpadding="5" cellspacing="0" class="TABLE_beijing">
            <tr bgcolor="#FFFFFF" class="tr_jiange1">
              <td><table width="100%"  border="0" cellpadding="2" cellspacing="0">
<%
        for(int j = 0; j < list.size(); j++) {
            SysRole role=list.get(j).get(SysRole.class);
            int roleid=role.getId();
            String rolename= role.getRoleName();
            String permission=role.getPermission();
            String checked_role=SuperString.strToList(permission).contains(module.getModuleID())?"checked":"";
            sql="select * from sys_user where roleid="+roleid;
            ResultList userlist=dbconn.query(sql,false,null);
            List listUser=new ArrayList();
            for(int i = 0; i < userlist.size(); i++) {
                SysUser user=userlist.get(i).get(SysUser.class);
                int userid=user.getId();
                String username=user.getUserName();
                int intChecked=0;
                if(!"".equals(module.getModuleID()) && SuperString.strToList(user.getPermission()).contains(module.getModuleID())){
                    intChecked=1;
                }
                listUser.add(new String[]{String.valueOf(userid),username,String.valueOf(intChecked)});
            }

%>
                  <tr>
                    <td width="91%" bgcolor="#f3f3f3"><img src="images/comm/spacer.gif" width="20" height="10">
                      <input name="check_id" type="checkbox" id="check_id_<%=roleid%>" value="role_<%=roleid%>" <%=checked_role%> class="checkbox" onClick="click_role(this)">
                        <font color="#0066CC"><b><%=rolename%></b></font></td>
                  </tr>
<%
            if(listUser.size()>0){

%>
                  <tr>
                    <td><table width="100%"  border="0" cellspacing="0" cellpadding="0">
                      <tr>
                        <td width="7%">&nbsp;</td>
                        <td width="93%">
<%
                for(int i = 0; i < listUser.size(); i++) {
                    String[] arrstr=(String[])listUser.get(i);
                    int _userid=SuperString.getInt(arrstr[0]);
                    String _username=SuperString.notNullTrim(arrstr[1]);
                    String checked_user=SuperString.getInt(arrstr[2])==1?"checked":"";
%>
                            <input name="check_id" type="checkbox" id="check_id_<%=roleid%>_<%=_userid%>" value="<%=_userid%>" <%=checked_user%> class="checkbox"> <%=_username%> &nbsp;&nbsp;
<%
                }
%>
                        </td>
                      </tr>
                    </table></td>
                  </tr>
<%

            }
        }
%>
              </table></td>
            </tr>
          </table>
      </div></td>
    </tr>
<%
    }
%>
    <tr>
      <td height="30" align="center">&nbsp;</td>
    </tr>
  </form>
</table>
</body>
</html>