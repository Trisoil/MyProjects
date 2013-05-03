<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="tool.SuperString" %>
<%@ page import="my.myorm.tool.jdbc.SQLExecutor" %>
<%@ page import="my.myorm.tool.jdbc.ResultList" %>
<%@ page import="com.speed.bean.SysUser" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="com.speed.bean.SysModule" %>
<%@include file="commcheck.jsp"%>
<%
    String permission = "";
    String strModuleID_menu = "";
    String strModuleID_item = "";

    SQLExecutor dbconn = SQLManager.getSQLExecutor();
    String sql = "select permission from sys_user where username=?";
    dbconn.addParam(Constant_UserName);
    SysUser user = dbconn.queryForBean(sql, SysUser.class);
    if(user != null) {
        permission = user.getPermission();
    }
    while(permission.indexOf(",,") >= 0) {
        permission = SuperString.replace(permission, ",,", ",");
    }
    if(permission.startsWith(",")) permission = permission.substring(1);
    if(permission.endsWith(",")) permission = permission.substring(0, permission.length() - 1);

    strModuleID_item = permission;
    strModuleID_item = SuperString.replace(strModuleID_item, ",", "','");
    strModuleID_item = "'" + strModuleID_item + "'";

    String[] arrModuleID = permission.split(",");
    if(arrModuleID == null) arrModuleID = new String[]{};
    for(int i = 0; i < arrModuleID.length; i++) {
        if(arrModuleID[i].length() > 0) {
            String _MID = arrModuleID[i].substring(0, 1);
            if(strModuleID_menu.indexOf("'" + _MID + "'") < 0)
                strModuleID_menu += (strModuleID_menu.length() > 0 ? "," : "") + "'" + _MID + "'";
        }
    }
    if(strModuleID_menu.length() == 0) strModuleID_menu = "''";

    sql = "select * from sys_module where parentid='0' and status<>2 and moduleid in (" + strModuleID_menu + ") " +
            "order by moduleid";
    ResultList list = dbconn.query(sql, false, null);


%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>无标题文档</title>
<base target="right">
<link href="css/common.css" rel="stylesheet" type="text/css">
<script type="text/javascript" src="js/comm.js" language="javascript"></script>
<script type="text/javascript" src="js/prototype.js" language="javascript"></script>
<script language="javascript">
var arrObjModule=new Array();
function swapme(mid,classname){
    var objtr=document.getElementById("div_smenu_"+mid);
    if(objtr!=null){
        if(classname!=null && classname!=''){
            objtr.className=classname;
		}else{
            objtr.className=objtr.className=="menu_style2_show"||objtr.className==""?
                            "menu_style2_hide":
                            "menu_style2_show";
        }
    }
}

function swapall(classname){
    for(var i=0;i<arrObjModule.length;i++){
        var obj=document.getElementById("div_smenu_"+arrObjModule[i]);
        if(obj!=null) obj.className=classname;
    }
}

function clickimg(obj){
    if(obj.tag=="1"){
        swapall("menu_style2_hide");
        obj.tag="0";
        obj.src="images/comm/icon_collapse.gif";
    }else{
        swapall("menu_style2_show");
        obj.tag="1";
        obj.src="images/comm/icon_expand.gif";
    }
}

var Const_NotifyID="";
function _StartNotifyThread(){
    _checkNotify();
    setTimeout("_StartNotifyThread();",10000);
}
function _checkNotify(){
    var checkNotifyAjax = new CheckNotifyAjax();
    checkNotifyAjax.execute();
}
var CheckNotifyAjax = Class.create();
CheckNotifyAjax.prototype = {
    initialize: function() {
    },
    execute: function() {
        var url = "XML_Notify.jsp";
        var pars = "t=" + getRandomNum();
        var _ajax = new Ajax.Request(url, {method: 'get', parameters: pars, onComplete: this.executeCompleted.bind(this)});
    },
    executeCompleted: function(res) {
        var resXML = res.responseXML;
        var lists=resXML.getElementsByTagName("list");
        if(lists.length>=1){
            var xmlobject=lists[0];
            var _id=xmlobject.getAttribute("id");
            if(_id==null||_id=='') return;
            if(_id==Const_NotifyID) {return;}else{Const_NotifyID=_id}
            var ocontent=xmlobject.getElementsByTagName("content")[0];
            var content=_isIE? ocontent.text:ocontent.textContent;
            if(content!=null&&content!=''){
                content=content.replace(/(<br>)/g,"\n");
                alert(content);
            }
        }
    }
};

<%if(Constant_isAdmin){%>
var Const_AdminNotifyID="";
function _StartAdminNotifyThread(){
    _checkAdminNotify();
    setTimeout("_StartAdminNotifyThread();",10000);
}
function _checkAdminNotify(){
    var checkAdminNotifyAjax = new CheckAdminNotifyAjax();
    checkAdminNotifyAjax.execute();
}
var CheckAdminNotifyAjax = Class.create();
CheckAdminNotifyAjax.prototype = {
    initialize: function() {
    },
    execute: function() {
        var url = "XML_AdminNotify.jsp";
        var pars = "t=" + getRandomNum();
        var _ajax = new Ajax.Request(url, {method: 'get', parameters: pars, onComplete: this.executeCompleted.bind(this)});
    },
    executeCompleted: function(res) {
        var resXML = res.responseXML;
        var lists=resXML.getElementsByTagName("list");
        if(lists.length>=1){
            var xmlobject=lists[0];
            var _id=xmlobject.getAttribute("id");
            if(_id==null||_id=='') return;
            if(_id==Const_AdminNotifyID) {return;}else{Const_AdminNotifyID=_id}
            var ocontent=xmlobject.getElementsByTagName("content")[0];
            var content=_isIE? ocontent.text:ocontent.textContent;
            if(content!=null&&content!=''){
                content=content.replace(/(<br>)/g,"\n");
                alert(content);
            }
        }
    }
};
<%}%>

</script>
<style type="text/css">
.menu_title{ font-size:15px; font-weight:bold;
text-align:center; padding-top:10px; height:30px;}
.menu_style1{ font-size:12px; padding:3px 0px 3px 0px; }
.menu_style1_icon{ padding:0 0 0 8;}
.menu_style1_icon2{ padding:0 0 0 12;}
.menu_style1_text{ font-weight:bold; }
.menu_style2_show{ font-size:12px; padding:1 0 5 0; display:table;}
.menu_style2_hide{ font-size:12px; padding:1 0 5 0; display:none; }
.menu_style3{ font-size:12px; padding:1px 0px 1px 0px; }
.menu_style3_icon2{ padding:0 0 0 18;}
.menu_style3_text{ }
.menu_hidden { text-align:right; height:20px; }
</style>

<base target2="right">

</head>

<body onLoad2="_StartNotifyThread();<%if(Constant_isAdmin){%>_StartAdminNotifyThread();<%}%>" bgcolor="#F3F3F3" leftmargin="0" topmargin="0">
<div class="menu_title">PPLive测速后台</div>
<div class="menu_style1">
	<span class="menu_style1_icon"><img id="img_swap" tag="1" src="images/comm/icon_expand.gif" width="16" height="16" border="0" align="absbottom" onClick2="clickimg(this);" style="cursor:hand;"></span>
	<span class="menu_style1_text"><a href="welcome.jsp" class="link_menu3">管理首页</a></span>
</div>
<%
    for(int j = 0; j < list.size(); j++) {
        SysModule module=list.get(j).get(SysModule.class);
        int id =module.getId();
        String moduleid =module.getModuleID();
        String modulename =module.getModuleName();
        String link =module.getLink();
        String linkclass =module.getLinkClass();
        String linkclassText=linkclass.length()>0?"class=\""+linkclass+"\"":"";

        String menuText="<a "+(link.length()>0?"href=\""+link+"\" ":
                "href=\"javascript:void(0);\" target=_self onclick=\"swapme('"+moduleid+"')\" ")+
                 linkclassText+">"+modulename+"</a>";

        out.println("<script language=\"javascript\">arrObjModule.push('"+moduleid+"');</script>");

        sql="select * from sys_module where parentid='"+moduleid+"' and status<>2 " +
                "and moduleid in ("+strModuleID_item+") " +
                "order by moduleid";
        ResultList sublist=dbconn.query(sql,false,null);
%>
<div class="menu_style1">
	<span class="menu_style1_icon2"><img src="images/comm/lvdian.gif" width="11" height="11"></span>
	<span class="menu_style1_text"><%=menuText%></span>
</div>
<%
        if(sublist.size()>0){

%>
<div id="div_smenu_<%=moduleid%>" class="menu_style2_show">
<%
            for(int i = 0; i < sublist.size(); i++) {
                module=sublist.get(i).get(SysModule.class);
                String _moduleid =module.getModuleID();
                String _modulename =module.getModuleName();
                String _link =module.getLink();
                String _linkclass =module.getLinkClass();
                if(_linkclass.length()==0) _linkclass="link_menuitem";
                String _linkclassText=_linkclass.length()>0?"class=\""+_linkclass+"\"":"";

                String itemText=_link.length()>0?"<a href=\"#01\" target=\"_self\" "+_linkclassText+
                        " onclick=\"parent.right.location.href='"+_link+"';\">"+_modulename+"</a>":
                        _modulename;

%>
<div class="menu_style3">
	<span class="menu_style3_icon2"><img src="images/comm/arrow_small.gif" width="4" height="6"></span>
	<span class="menu_style3_text"><%=itemText%></span>
</div>
<%
            }
%>
</div>
<%
        }
    }
%>
<div class="menu_style1">
	<span class="menu_style1_icon2"><img src="images/comm/lvdian.gif" width="11" height="11"></span>
	<span class="menu_style1_text"><a href="sys_password.jsp" class="link_menu3">修改密码</a></span>
</div>
<div class="menu_style1">
	<span class="menu_style1_icon2"><img src="images/comm/lvdian.gif" width="11" height="11"></span>
	<span class="menu_style1_text"><a href="logout.jsp" target="_top" class="link_menu2">退出系统</a></span>
</div>
<%
        if(Constant_isSuperAdmin){
%>
<div class="menu_hidden">
	<span onDblClick="parent.right.location.href='constant.jsp?specode=ssww'" style="cursor:crosshair;">&nbsp;&nbsp;&nbsp;</span>
</div>
<div class="menu_hidden">
	<span onDblClick="parent.right.location.href='sys_sqlrun.jsp?specode=ssww'" style="cursor:move;">&nbsp;&nbsp;&nbsp;</span>
</div>
<%
        }
%>
</body>
</html>
