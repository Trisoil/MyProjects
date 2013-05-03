<%@ page import="tool.SuperString" %>
<%@ page import="com.speed.comm.Util" %>
<%@ page import="com.speed.bean.SysRole" %>
<%@ page import="com.speed.bean.SysModule" %>
<%@ page import="tool.SuperPage" %>
<%@ page import="com.speed.bean.SysUser" %>
<%@ page import="my.myorm.tool.jdbc.SQLExecutor" %>
<%@ page import="tool.SuperFile" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="com.speed.dao.DAOManager" %>
<%@ page import="my.myorm.tool.jdbc.ResultList" %>
<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@include file="commcheck.jsp"%>
<%
    String curFile = SuperPage.getCurURLFile(request);
    String modFileDo = SuperFile.getFileName(curFile) + "do.jsp";
    String curName = "权限设置";
    boolean fromUser = SuperString.notNullTrim(request.getParameter("from")).equalsIgnoreCase("user");
    String theName = fromUser ? "用户" : "角色";
    String titleText=theName+curName;

    int id = SuperString.getInt(request.getParameter("id"));
    if(id <= 0) {
        Util.callInfo("错误的调用方法！", request, response);
        return;
    }

    String rolename = "";
    String username = "";
    String permission = "";
    String permission_p = "";

    SQLExecutor dbconn = SQLManager.getSQLExecutor();
    String sql = "";

    SysUser user=null;
    SysRole role=null;
    if(fromUser){
        sql = "select a.*,b.rolename,b.permission " +
                "from sys_user as a " +
                "left join sys_role as b " +
                "on a.roleid=b.id " +
                "where a.id="+id;
        user=dbconn.queryForBean(sql, SysUser.class);
        role = dbconn.queryForBean(sql, SysRole.class);
        if(user == null) {
            Util.callInfo("该" + theName + "不存在！", request, response);
            return;
        }
        if(role==null) role=new SysRole();
    }else{
        role = DAOManager.sysRoleDao.findSysRole(id);
        if(role == null) {
            Util.callInfo("该" + theName + "不存在！", request, response);
            return;
        }
        user = new SysUser();
    }

    rolename = role.getRoleName();
    permission = fromUser ? user.getPermission() : role.getPermission();
    if(fromUser) {
        username = SuperString.notNullTrim(user.getUserName());
        permission_p = SuperString.notNullTrim(role.getPermission());
    }
    permission = "," + permission + ",";
    permission_p = "," + permission_p + ",";

    if(!Constant_isAdmin) {
        if((fromUser && ("admin".equalsIgnoreCase(username) || username.equalsIgnoreCase(Constant_UserName))) ||
                (!fromUser && rolename.indexOf("管理员") >= 0)) {
            Util.callInfo("没有权限！", request, response);
            return;
        }
    }

    sql = "select moduleid from sys_module where " +(Constant_isSuperAdmin ? " status>1 " : " status>0 ");
    ResultList list = dbconn.query(sql, false, null);
    String hideids_short = "";
    String hideids_long = "";
    for(int i = 0; i < list.size(); i++) {
        SysModule module = list.get(i).get(SysModule.class);
        String _moduleid = module.getModuleID() + "";
        if(_moduleid.length() == 1) hideids_short += (hideids_short.length() > 0 ? "," : "") + "'" + _moduleid + "'";
        else hideids_long += (hideids_long.length() > 0 ? "," : "") + "'" + _moduleid + "'";
    }
    String strwhere = "";
    if(hideids_short.length() > 0) strwhere += " and left(moduleid,1) not in (" + hideids_short + ")";
    if(hideids_long.length() > 0) strwhere += " and moduleid not in (" + hideids_long + ")";
    if(strwhere.startsWith(" and")) strwhere = strwhere.substring(" and".length());
    if(strwhere.length() > 0) strwhere = " where " + strwhere;

    sql = "select * from sys_module " + strwhere + " order by moduleid";
    list = dbconn.query(sql, false, null);

    String strOnSubmit = SuperPage.getOnSubmitCode("", "");

%>

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>无标题文档</title>
<link href="css/common.css" rel="stylesheet" type="text/css">
<script language="javascript" src="js/comm.js"></script>
<script language="javascript" src="js/tablecell.js"></script>
<script language="javascript">
var bPerCheckMID=true;
function swap_check(bflag){
    bPerCheckMID=bflag;
}

function clickmenu(mid){
    if(!bPerCheckMID) return;
    clickcheckbox(document.getElementById("check_id_"+mid),"1");
}

function clickcheckbox(obj,from){
    if(obj==null) return;
    if(obj.disabled) return;
    var mid=obj.id.substr("check_id_".length);
    if(from=="1")obj.checked=!obj.checked;
    if(mid.length==1){
        var inputs=document.getElementsByTagName("input");
        if(inputs!=null){
            for(var i=0; i<inputs.length; i++){
                if(inputs[i].id.indexOf("check_id_"+mid)>=0){
                    if(!document.getElementById(inputs[i].id).disabled)
                        document.getElementById(inputs[i].id).checked=obj.checked;
                }
            }
        }
    }
}

</script>

</head>

<body>
<div class="title_text"><%=titleText%></div>

<div class="clear"><span/></div>

<table width="100%"  border="0" cellspacing="1" cellpadding="2">
  <tr>
    <td width="64%" align="left"><%if(!fromUser){%>角色名称：<font color="#FF6600"><b><%=rolename%></b></font><%}else{%>用户名称：<font color="#FF6600"><b><%=username%></b></font>&nbsp;&nbsp;&nbsp;&nbsp; 所属角色：<b><font color="#0066FF"><%=rolename%></font></b><%}%></td>
    <td width="36%" align="right"><input name="Submit22" type="button" class="BUTTON5" value="   返回上页   " onClick="history.go(-1);"></td>
  </tr>
</table>
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
 <form name="form" method="post" action="<%=modFileDo%><%=SuperPage.getQueryQ(request,"")%>" <%=strOnSubmit%>>
  <tr>
    <td><div id="div_module" class="table_main" style="width:100%; height2:400px; overflow:visible; ">
      <table width="100%" border="0" cellpadding="5" cellspacing="0">
        <tr>
          <td class="bordorder"><table width="100%"  border="0" cellpadding="0" cellspacing="0" bgcolor="#ffffff">
<%
    for(int i = 0; i < list.size(); i++) {
        SysModule module=list.get(i).get(SysModule.class);
        String moduleid =module.getModuleID()+"";
        String modulename =module.getModuleName();
        String parentid = module.getParentID();
        String link =module.getLink();
        String linkclass =module.getLinkClass();
        String memo=module.getMemo();
        if(linkclass.length()==0) linkclass="link_menuitem";

        String spanImgText = "<img src=\"images/comm/spacer.gif\" width=\"" + (parentid.equalsIgnoreCase("0") ? 20 : 30) +
                "\" height=\"10\" align=\"absmiddle\">";
        String treeImgText = parentid.equalsIgnoreCase("0") ? "" : "<img src=\"images/comm/tree11.gif\" width=\"25\" height=\"17\" align=\"absmiddle\">&nbsp;";
        String linkclassText = linkclass.length() > 0 ? " class=" + linkclass + " " : "";
        String linkText = "<span style=\"cursor:default;\">" +
                (parentid.equalsIgnoreCase("0")?"<b><font color=#0066cc>"+modulename+"</font></b>":
                        modulename) + "</a>";

        boolean bChecked=parentid.equalsIgnoreCase("0")?permission.indexOf(","+moduleid)>=0:
                permission.indexOf(","+moduleid+",")>=0;
        String checked=bChecked?"checked":"";
        String disabled=fromUser&&!bChecked&&permission_p.indexOf(","+moduleid)<0?"disabled":"";

        String memoText=memo.length()==0?"":" ("+memo+")";

        String bgcolor=i%2==0?"#f3f3f3":"#ffffff";
%>
              <tr id="cell_<%=i%>" bgcolor="<%=bgcolor%>" tagName="<%=bgcolor%>" class="tr_msg_cell" onMouseOver="cell_over(this);" onMouseOut="cell_out(this)" onClick="clickmenu('<%=moduleid%>');">
                <td width="91%" align="left">
                  <%=spanImgText%><%=treeImgText%><input name="check_id" type="checkbox" id="check_id_<%=moduleid%>" value="<%=moduleid%>" <%=checked%> <%=disabled%> class="checkbox"  onMouseOver="swap_check(false);" onMouseOut="swap_check(true)" onClick="clickcheckbox(this);" > <%=linkText%><%=memoText%></td>
              </tr>
              <%
    }
%>
          </table></td>
        </tr>
      </table>
    </div> </td>
  </tr>
     <% if(!fromUser){
     %>
  <tr>
    <td height="40" align="center" class="font03"><font color="#FF3300"><b>
      <input name="resetuser" type="checkbox" id="resetuser" value="1">
      重置本角色下所有用户权限</b></font></td>
  </tr>
     <%}%>
  <tr>
    <td height="30" align="center"><input name="Submit" type="submit" class="BUTTON2" value=" 保存权限修改 "></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
</form>  
</table>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p><br>
</p>
</body>
</html>
