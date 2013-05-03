<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<%@page import="my.myorm.tool.jdbc.SQLExecutor"%>
<%@page import="my.myorm.tool.jdbc.SQLManager"%>
<%@page import="my.myorm.tool.jdbc.Pages"%>
<%@page import="tool.SuperPage"%>
<%@page import="tool.SuperString"%>
<%@page import="tool.IPSeeker"%>
<%@page import="com.speed.bean.Speed"%>
<%@page import="my.myorm.tool.jdbc.ResultList"%>
<%@page import="tool.SuperFile"%>
<%@page import="java.io.*"%>
<%@page import="java.util.*"%>
<%@include file="commcheck.jsp"%>
<%
	String curFile = SuperPage.getCurURLFile(request);
	String modFile = SuperFile.getFileName(curFile) + "mod.jsp";
	//
	String act = SuperString.notNullTrim(request.getParameter("act"));
	String s_key = SuperString.notNullTrim(request.getParameter("s_key"));
	String s_startTime = SuperString.notNullTrim(request.getParameter("s_begindate"));
	String s_endTime = SuperString.notNullTrim(request.getParameter("s_enddate"));
	String s_startTime_c = SuperString.notNullTrim(request.getParameter("s_begindate_c"));
	String s_endTime_c = SuperString.notNullTrim(request.getParameter("s_enddate_c"));
	//
    String startTimeStr=" 00:00:00";
    String endTimeStr=" 23:59:59";
    StringBuffer whereStr=new StringBuffer();
    String orderStr=" order by id desc";
    if(s_key.length()>0)whereStr.append(" and SpeedKey='"+s_key+"'");
    if(s_startTime.length()>0&&s_endTime.length()>0) {
    	whereStr.append(" and CreateTime>='"+s_startTime+startTimeStr+"' and CreateTime<='"+s_endTime+endTimeStr+"'");
    }
    if(s_startTime_c.length()>0&&s_endTime_c.length()>0) {
    	whereStr.append(" and CompleteTime>='"+s_startTime_c+startTimeStr+"' and CompleteTime<='"+s_endTime_c+endTimeStr+"'");
    }
    //
    SQLExecutor dbconn = SQLManager.getSQLExecutor();
    Pages pages = new Pages(50, "id", request);
    ResultList rs=new ResultList();
    String sql="select * from speed_list where 1=1 "+whereStr+orderStr;
	rs=dbconn.query(sql,false,pages);
	//
	String id = SuperString.notNullTrim(request.getParameter("id"));
	String note = SuperString.notNullTrim(request.getParameter("note_"+id));
	if(act.equalsIgnoreCase("addnote")){
		String sql_u="update speed_list set Note ='"+note+"' where ID ="+id;
		dbconn.execute(sql_u);
		out.print(id);
		return ;
	}
	//
	Map<String,String> serverMap=new HashMap<String, String>();
	String serverListFile=Util.getProperties("SPEED_SERVER_LIST");
	File file = new File(serverListFile);
	if(!file.exists()) {
		file = null;
	}
	if(file != null) {
		try {
			BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(serverListFile), "UTF-8"));
		    for(String inLine = reader.readLine(); inLine != null; inLine = reader.readLine()) {
			    String[] serverStr=inLine.split("	");
			    serverMap.put(serverStr[1], serverStr[3]+"--"+serverStr[2]);
		    }
		    reader.close();
		    reader = null;
		} catch(IOException e) {
	    	file = null;
		} finally {
			file = null;
		}
	}
%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>无标题文档</title>
<link href="css/common.css" rel="stylesheet" type="text/css">
<script language="javascript" src="js/comm.js"></script>
<script type="text/javascript" src="js/WebCalendar.js" language="javascript"></script>
<script language="javascript" src="js/tablecell.js"></script>   
<script type="text/javascript" src="js/tooltips.js" language="javascript"></script>    
<script type="text/javascript" src="js/prototype.js" language="javascript"></script>    
<script type="text/javascript" language="javascript">
	function doSearch(){
		var searchForm=document.getElementById('searchForm');
		var s_key=document.getElementById('s_key').value;
		//
		var s_begindate=document.getElementById('s_begindate').value;
		var s_enddate=document.getElementById('s_enddate').value;
		//
		var s_begindate_c=document.getElementById('s_begindate_c').value;
		var s_enddate_c=document.getElementById('s_enddate_c').value;
		if(s_begindate==''&&s_enddate!=''){
		 alert('请选择开始时间!');
		 return false;
		}
		if(s_begindate!=''&&s_enddate==''){
		 alert('请选择结束时间!');
		 return false;
		}
		if(s_begindate_c==''&&s_enddate_c!=''){
		 alert('请选择开始时间!');
		 return false;
		}
		if(s_begindate_c!=''&&s_enddate_c==''){
		 alert('请选择结束时间!');
		 return false;
		}
		searchForm.action='<%=curFile%>?act=search&s_key='+s_key+'&s_begindate='+s_begindate+'&s_enddate='+s_enddate+'&s_begindate_c='+s_begindate_c+'&s_enddate_c='+s_enddate_c;
		searchForm.submit();
	}		
	function doAddNote(id) {
		var url='speed.jsp?act=addnote&id='+id;     
		var pars=Form.serialize($('noteform_'+id));     
		new Ajax.Request(     
		url,{method: 'post',parameters:pars,onComplete:showNoteDiv(id)});    
	}
	function modNote(id) {
		document.getElementById('addNoteDiv_'+id).style.display= ''; 
		document.getElementById('showNoteDiv_'+id).style.display= 'none'; 
		document.getElementById('showOkNoteDiv_'+id).style.display= 'none'; 
		document.getElementById('note_'+id).focus(); 
	}
	function cancelNote(id) {
		document.getElementById('addNoteDiv_'+id).style.display='none'; 
		document.getElementById('showNoteDiv_'+id).style.display='none'; 
		document.getElementById('showOkNoteDiv_'+id).style.display= ''; 
	}
	function showNoteDiv(id) {
		var note=document.getElementById('note_'+id).value;
		document.getElementById('addNoteDiv_'+id).style.display='none'; 
		document.getElementById('showNoteDiv_'+id).style.display='none'; 
		$('showOkNoteDiv_'+id).innerHTML=note;
		$('showOkNoteDiv_'+id).style.display='';
	}
	function searchFeedType(feedType) {
		$('feedDiv').innerHTML='<img src="/js/loading.gif">加载中......';
	 	var myAjax = new Ajax.Request('/feedtype.htm?feedType='+feedType,{method: 'post', onComplete: showFeedDiv});
	}

	function doChanageServerIpAddr(src_ip,to_id) {
		if(src_ip==''){
			alert('IP不能为空!');
			return false;
		}
		if(to_id==''){
			alert('ID不能为空!');
			return false;
		}
		var url='<%=Util.getProperties("CHANAGE_SERVER_IPADDR")%>?src_ip='+src_ip+'&to_id='+to_id    
		var myAjax = new Ajax.Request(url,{method: 'post', onComplete: showResult});
	}
	function showResult(response) {
		var isok=response.responseText;
		//alert(isok);
		if(isok==200){
			alert('跳转成功!');
		}else{
			alert('跳转失败!');
		}
	}
</script>
</head>
<body>
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td class="title_text">测 速 历 史 列 表</td>
  </tr>
</table>
<div style="float:right;padding:5px;"><span/></div>
<fieldset style="border:1px #FF8040 solid ">
<table cellspacing="0" cellpadding="1" border="0" align="center" width="90%">
	<form method="post" id="searchForm" name="searchForm">
	<tr>
      	<td nowrap="" width="70%">[<font color="#0066ff">查询</font>]
        	KEY：<input type="text" value="<%=s_key%>" size="25" title="s_key" id="s_key" name="s_key">
        	生成时间：<input name="s_begindate" type="text" id="s_begindate" title="开始时间" onclick="SelectDate(this,'yyyy-MM-dd')" value="<%=s_startTime %>" size="10"  maxlength="20" readonly="true" resetdm=true>
          	到<input name="s_enddate" type="text" id="s_enddate" title="结束时间" onclick="SelectDate(this,'yyyy-MM-dd')" value="<%=s_endTime %>" size="10"  maxlength="20" readonly="true">
        	完成时间：<input name="s_begindate_c" type="text" id="s_begindate_c" title="开始时间" onclick="SelectDate(this,'yyyy-MM-dd')" value="<%=s_startTime_c %>" size="10"  maxlength="20" readonly="true" resetdm=true>
          	到<input name="s_enddate_c" type="text" id="s_enddate_c" title="结束时间" onclick="SelectDate(this,'yyyy-MM-dd')" value="<%=s_endTime_c %>" size="10"  maxlength="20" readonly="true">
        	<input name="Submit" type="button" class="BUTTON2" value="  查 询   " onclick="doSearch();">
       	</td>
	</tr>
	</form>
</table>
</fieldset>
<div style="height:30px;"/>
<table width="100%"  border="0" cellspacing="1" cellpading="2">
  <tr>
    <td align="left" width="5%"><input name="Submit" type="button" class="BUTTON2" value=" 新 增 测 速 连 接 " onClick="location.href='<%=modFile%>'"></td>
    <td width="90%"><div class="page_nav01"><%=SuperPage.generatePageNavSingle(rs.getAllCount(), request, null, pages.getPageSize(),"个")%></div></td>
  </tr>
</table>
<table width="100%" border="0" cellpadding="3" cellspacing="1" class="table_bord">
  <tr class="table_bord_ttl">
    <td nowrap width="2%">ID</td>
    <td nowrap width="10%">KEY</td>
    <td nowrap width="10%">生成时间</td>
    <td nowrap width="15%">结果ID</td>
    <td nowrap width="8%">结果值</td>
    <td nowrap width="10%">返回时间</td>
    <td nowrap width="10%">操作用户</td>
    <td nowrap width="5%">用户IP</td>
    <td nowrap width="15%">注释</td>
    <td nowrap width="15%">操作</td>
  </tr>
<%
    for(int i = 0; i < rs.size(); i++) {
        Speed speed=rs.get(i).get(Speed.class);
        String trclass = i % 2 == 0 ? "tr_class1" : "tr_class2";
%>
   <tr class="<%=trclass%>" onMouseMove="cell_over(this);" onMouseOut="cell_out(this)" onClick="cell_click(this);">
    <td nowrap class="font22"><%=speed.getId()%></td>
    <td nowrap class="font22"><a href="speeddetail.jsp?speedKey=<%=speed.getSpeedKey()%>"><%=speed.getSpeedKey()%></a></td>
    <td nowrap class="font22"><%=speed.getCreateTime_ToDateTimeString() %></td>
    <td nowrap class="font22"><%=speed.getResultID() %><%if(speed.getResultID().length()>0){%>--<%=serverMap.get(speed.getResultID())%><%}%></td>
    <td nowrap class="font22"><%=speed.getResultValue()/1024 %>KB/S</td>
    <td nowrap class="font22"><%=speed.getCompleteTime_ToDateTimeString() %></td>
    <td nowrap class="font22"><%=speed.getOperator() %></td>
    <td nowrap class="font22"><%=speed.getUserIp() %></td>
	<td nowrap class="font22" id="noteDiv_<%=speed.getId()%>">
	<form name="noteform_<%=speed.getId()%>" id="noteform_<%=speed.getId()%>" method="post" style="margin:0">
	<div id="showNoteDiv_<%=speed.getId()%>">
  		<%=speed.getNote() %>
	</div>
	<div id="showOkNoteDiv_<%=speed.getId()%>" style="display: none;">
		<%=speed.getNote() %>
	</div>
	<div id="addNoteDiv_<%=speed.getId()%>" style="display: none;">
		<textarea name="note_<%=speed.getId()%>" id="note_<%=speed.getId()%>" rows="3" cols="10" style="height:50px;width:200px;font-size:small;overflow:hidden;"><%=speed.getNote() %></textarea><br>
		<input name="addNotebu" type="button" class="BUTTON2" value="保存" onclick="doAddNote('<%=speed.getId()%>');">
		<input name="cancelNotebu" type="button" class="BUTTON2" value="取消" onclick="cancelNote(<%=speed.getId()%>);">
	</div>
	</form>
	</td>
    <td nowrap class="font22">
		<input name="huibao" type="button" class="BUTTON2" value="修改跳转中心" onclick="doChanageServerIpAddr('<%=speed.getUserIp()%>','<%=speed.getResultID()%>');">
		<input name="notebu" type="button" class="BUTTON2" value="修改注释" onclick="modNote(<%=speed.getId()%>);">
	</td>
  </tr>
<%} %>
<%if(rs.size()==0){ %>
  <tr bgcolor="#FFFFFF" class="tr_class1">
    <td height="50" colspan="9"><div align="center">没有数据</div></td>
  </tr>
<%} %>
</table>
<div class="page_nav01"><%=SuperPage.generatePageNavSingle(rs.getAllCount(), request, null, pages.getPageSize(),"个")%></div>
<div style="height:30px;"/>
<p>&nbsp;</p>
<p>&nbsp;</p>
</div>
</body>
</html>
