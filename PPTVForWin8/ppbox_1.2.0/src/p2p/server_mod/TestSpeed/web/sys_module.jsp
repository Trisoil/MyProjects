<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="tool.SuperString" %>
<%@ page import="com.speed.bean.SysModule" %>
<%@ page import="my.myorm.tool.jdbc.ResultList" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="tool.SuperFile" %>
<%@ page import="tool.SuperPage" %>
<%@include file="commcheck.jsp"%>
<%
    String curFile = SuperPage.getCurURLFile(request);
    String modFile = SuperFile.getFileName(curFile) + "mod.jsp";
    String modFileDo = SuperFile.getFileName(curFile) + "moddo.jsp";
    String curName = "系统模块";
    
    String sql = "select * from sys_module order by moduleid";
    ResultList list= SQLManager.getSQLExecutor().query(sql,false,null);
%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title><%=curName%>维护</title>
<link href="css/common.css" rel="stylesheet" type="text/css">
<script language="javascript" src="js/comm.js"></script>
<script language="javascript" src="js/tablecell.js"></script>
<script type="text/javascript" src="js/tablecell.js" language="javascript"></script>
</head>

<body>

<div class="title_text"><%=curName%>维护</div>

<div class="clear"><span/></div>

<table width="100%"  border="0" cellspacing="1" cellpadding="2">
  <tr>
    <td align="right"><input name="Submit" type="button" class="BUTTON5" value="  增加<%=curName%>  " onClick="location.href='<%=modFile%><%=SuperPage.getQueryQ(request,"act,id")%>'"></td>
  </tr>
</table>
<table width="100%" border="0" cellpadding="3" cellspacing="1" class="table_main">
  <tr align="left" class="table_main_tt">
    <td><%=curName%>维护</td>
  </tr>
  <tr class="tr_class1">
    <td><table width="100%"  border="0" cellpadding="0" cellspacing="0">
<%
    for(int i = 0; i < list.size(); i++) {
        SysModule module=list.get(i).get(SysModule.class);
        int id =module.getId();
        String moduleid =module.getModuleID();
        String modulename =module.getModuleName();
        String parentid =module.getParentID();
        String link = module.getLink();
        String linkclass =module.getLinkClass();
        int status=module.getStatus();
        String memo=module.getMemo();
        if(linkclass.length()==0) linkclass="link_menuitem";


        String spanImgText = "<img src=\"images/comm/spacer.gif\" width=\"" + (parentid.equalsIgnoreCase("0") ? 20 : 30) +
                "\" height=\"10\" align=\"absmiddle\">";
        String treeImgText = parentid.equalsIgnoreCase("0") ? "" : "<img src=\"images/comm/tree11.gif\" width=\"25\" height=\"17\" align=\"absmiddle\">&nbsp;";
        String linkclassText = linkclass.length() > 0 ? " class=" + linkclass + " " : "";
        String linkText = "<a href=\"" + (link.length() > 0 ? link : "javascript:") + "\" " + linkclassText + ">" +
                (parentid.equalsIgnoreCase("0")?"<b>"+modulename+"</b>":modulename) + "</a>";

        String statusText=status==1?"<b><font color=\"#0066FF\">[系统]</font></b>":"<b><font color=\"#FF6600\">[隐藏]</font></b>";
        if(status==0) statusText="";
        String memoText=memo.length()==0?"":" ("+memo+")";

        String trclass = i % 2 == 0 ? "tr_class1" : "tr_class2";
%>
          <tr class="<%=trclass%>" onMouseMove="cell_over(this);" onMouseOut="cell_out(this)" onClick="cell_click(this);">
            <td width="91%"><%=spanImgText%><%=treeImgText%><b><%=moduleid%></b>&nbsp; <%=linkText%><%=memoText%> &nbsp;&nbsp;&nbsp;<%=statusText%></td>
            <td width="9%" align="center"><a href="<%=modFile%>?act=mod&id=<%=id%><%=SuperPage.getQueryAnd(request,"act,id")%>">修改</a> | <a href="<%=modFileDo%>?act=del&moduleid=<%=moduleid%>&id=<%=id%><%=SuperPage.getQueryAnd(request,"act,id")%>" onClick="return delconfirm();">删除</a></td>
          </tr>
<%
    }
    if(list.size()<=0){
%>
          <tr>
            <td height="50" colspan="2" align="center">没有数据，请先添加</td>
          </tr>
<%
    }
%>
    </table></td>
  </tr>
</table>
<p>&nbsp;</p>
<p>&nbsp;</p>
</body>
</html>
