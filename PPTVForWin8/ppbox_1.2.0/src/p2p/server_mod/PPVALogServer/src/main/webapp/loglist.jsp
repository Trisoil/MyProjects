<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<%@page import="my.myorm.tool.jdbc.SQLExecutor"%>
<%@page import="my.myorm.tool.jdbc.SQLManager"%>
<%@page import="my.myorm.tool.jdbc.Pages"%>
<%@page import="my.myorm.tool.jdbc.RecordMap"%>
<%@page import="tool.SuperPage"%>
<%@page import="tool.SuperString"%>
<%@page import="tool.SuperNumber"%>
<%@page import="tool.SuperDate"%>
<%@page import="com.speed.bean.Speed"%>
<%@page import="com.PPVALog.bean.*"%>
<%@page import="my.myorm.tool.jdbc.ResultList"%>
<%@page import="my.myorm.tool.jdbc.*"%>
<%@page import="tool.SuperFile"%>
<%@page import="tool.IPSeeker"%>
<%@page import="java.io.*"%>
<%@page import="java.util.*"%>
<%@page import="java.text.SimpleDateFormat"%>
<%@page import="java.net.URLDecoder"%>

<%@include file="commcheck.jsp"%>
<%
    String curFile = SuperPage.getCurURLFile(request);
    String modFile = SuperFile.getFileName(curFile) + "mod.jsp";
    //request.setCharacterEncoding("UTF-8");
    //
    String act = SuperString.notNullTrim(request.getParameter("act"));
    //search value
    String peer_id = SuperString.notNullTrim(request.getParameter("peer_id"));
    String resource_id = SuperString.notNullTrim(request.getParameter("resource_id"));
    String query_resource_name = SuperString.notNullTrim(request.getParameter("query_resource_name"));
    if(query_resource_name!="")    query_resource_name = URLDecoder.decode(query_resource_name, "UTF-8");
    String statistics_id = SuperString.notNullTrim(request.getParameter("statistics_id"));
    String peerVersion_id = SuperString.notNullTrim(request.getParameter("peerVersion_id")); 
    
    String s_startTime = SuperString.notNullTrim(request.getParameter("s_begindate"));
    String s_endTime = SuperString.notNullTrim(request.getParameter("s_enddate"));
    
    
    SuperDate sdToday = new SuperDate();
    int s_year = SuperString.getInt(request.getParameter("s_year"),sdToday.getYear());  //要么s_year,要么当前时间
    int s_month = SuperString.getInt(request.getParameter("s_month"),sdToday.getMonth());
    int s_day = SuperString.getInt(request.getParameter("s_day"));
    String s_date = SuperString.notNullTrim(request.getParameter("s_date"));
    String s_datatype = SuperString.notNullTrim(request.getParameter("s_datatype"));
    
    SuperDate sddate=null;
    if(SuperDate.isDateString(s_date)) //如果有s_date参数
        sddate=new SuperDate(s_date,true);
    
    if(sddate!=null){
        //从sdate中分离出年、月、日
        s_year=sddate.getYear();
        s_month=sddate.getMonth();
        s_day=sddate.getDay();
    }else{
        if(s_day<=0){ //无s_day参数
            SuperDate sddefault=sdToday.getAddedDate(Calendar.DAY_OF_YEAR,-1); //得到昨天的日期作为初始化
            if(s_year==sddefault.getYear()&&s_month==sddefault.getMonth()) s_day=sddefault.getDay();
            else s_day=1;
        }
        SuperNumber.fillChar='0';
        String thedate=s_year+"-"+ SuperNumber.format(s_month,"2.0")+"-"+ SuperNumber.format(s_day,"2.0");
        if(SuperDate.isDateString(thedate)) sddate=new SuperDate(thedate,true);
        if(sddate==null) sddate=new SuperDate(sdToday.getDateString(),true);
        s_date=sddate.getDateString();
    }

    //
    String startTimeStr=" 00:00:00"; //没用
    String endTimeStr=" 23:59:59";
    
    StringBuffer whereStr=new StringBuffer();
    String orderStr=" order by id desc";
    if( peer_id.length() > 0 ) {
        whereStr.append(" and peer_guid='"+peer_id+"'");
    }
    
    if( resource_id.length() > 0 ) {
        whereStr.append(" and resource_id='"+resource_id+"'");
    }

    if( query_resource_name.length() > 0 ) {
        whereStr.append(" and resource_name like '%"+query_resource_name+"%'");
    }
    
    if( statistics_id.length() > 0 ) {
        whereStr.append(" and statistics_id='"+statistics_id+"'");
    }   
    
    if( peerVersion_id.length() > 0 ) {
        whereStr.append(" and peer_version='"+peerVersion_id+"'");
    }
    

    
    List<String> tables = new ArrayList<String>();  //table的list
    ResultList rs=new ResultList();
    SQLExecutor dbconn = SQLManager.getSQLExecutor("ppvaDataSource");

    /*else {
        String sql1 = "SHOW TABLES";
        rs=dbconn.query(sql1,false,null);
        for( int i = 0; i < rs.size(); i++ ) {
            RecordMap rm = (RecordMap)rs.get(i);
            DefaultBean dbean = new DefaultBean();
            dbean = (DefaultBean)rm.get( dbean.getClass() );
            // TODO: 这里是写死名字的,以后再改
            String s = dbean.getString("tables_in_collectionlog");
            tables.add( s );
        }
    }*/ 

    String sql = new String();
    
    boolean first = true;
    SimpleDateFormat bartDateFormat = new SimpleDateFormat("yyyy_MM_dd");
    Date start_date = null;
    Date end_date = null;

    // 在这里处理区间
    if( s_startTime.length() > 0 ) {
        // 处理开始
        s_startTime = s_startTime.replace('-' , '_');
        start_date = bartDateFormat.parse(s_startTime);
    }
    
    if( s_endTime.length() > 0 ) {
        s_endTime = s_endTime.replace('-' , '_');
        end_date = bartDateFormat.parse(s_endTime);
    }


    if(act.length() > 0 && act.equals("search")){
        Date temp_date = start_date;
        Calendar c = Calendar.getInstance();
        c.setTime(end_date);
        c.add(Calendar.DAY_OF_YEAR,1);
        Date temp_enddate=c.getTime(); 
        c.setTime(start_date);  
        while(temp_date.before (temp_enddate)){
            //System.out.println(bartDateFormat.format(temp_date));
            tables.add(bartDateFormat.format(temp_date));
            c.add(Calendar.DAY_OF_YEAR,1); 
            temp_date=c.getTime(); 
        }
    }
    else if(act.equals("datesearch")){
        if( sddate != null ) {
            tables.add( s_date.replaceAll( "-" , "_" ) );
        }
    }
    
    //System.out.println("STLOG"+"haha");
    //for( int i = tables.size() - 1; i >= 0; i-- ) {
    for( int i = 0; i < tables.size(); i++ ) {
        String tableName = (String)tables.get(i);
        //System.out.println("STLOG "+tableName);
        Date table_date = bartDateFormat.parse(tableName);
        /*
        if( start_date != null ) {
            if( table_date.before( start_date ) ) {
                continue;
            }
        }       
        if( end_date != null ) {
            if( table_date.after( end_date ) ) {
                break;
            }
        }
        */
        // 查看是否需要
        if( !first ) {
            // 加上union
            sql += "\nUNION ALL\n";
        } else {
            // 是否超过了
            
            
            first = false;
        }
        
        String cmd = "SELECT * FROM " + tableName + " where 1=1" + whereStr;
        sql += cmd;
        //System.out.println(sql);
    }
    System.out.println(sql);
    Pages pages = new Pages(20, "id", request);
//    String sql="select * from 2011_06_26 where 1=1 "+whereStr;
    try {
        if( sql.length() != 0 )
            rs=dbconn.query(sql,false,pages);
        else {
            rs.data.clear();
        }
    } catch( Exception e ) {
        // 可能是由于表不存在,这里忽略掉
        System.out.println("可能是由于表不存在");
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
<script type="text/javascript" src="js/prototype.js" language="javascript"></script>    
<script type="text/javascript" language="javascript">
    function doSearch(){
        var searchForm=document.getElementById('searchForm');
        //getvalue
        var peer_id=document.getElementById('peer_id').value;
        var resource_id=document.getElementById('resource_id').value;
        var s_query_resource_name=document.getElementById('query_resource_name').value;     
        var statistics_id=document.getElementById('statistics_id').value;
        var peerVersion_id=document.getElementById('peerVersion_id').value;     
        //alert(peer_id+" "+resource_id+" "+s_query_resource_name);
        
        //gettime
        var s_begindate=document.getElementById('s_begindate').value;
        var s_enddate=document.getElementById('s_enddate').value;       
        var s_date=document.getElementById('s_date').value;
        
        //alert(s_begindate + " " + s_enddate + " " +s_date);       
        if(s_begindate==''&&s_enddate!=''){
            alert('请选择开始时间!');
            return false;
        }else if(s_begindate!=''&&s_enddate==''){
            alert('请选择结束时间!');
            return false;
        }else if(s_begindate==''&&s_enddate==''){
            s_begindate=s_date;
            s_enddate=s_date;
        }

        var action_url = '<%=curFile%>?act=search&peer_id='+peer_id+'&resource_id='+resource_id
                +'&query_resource_name='+encodeURI(encodeURI(s_query_resource_name))+'&statistics_id='+statistics_id
                +'&peerVersion_id='+peerVersion_id;
        if( s_begindate!='' ) {
            action_url += '&s_begindate='+s_begindate+'&s_enddate='+s_enddate;
        } else {
            if(s_date != '') {
                action_url += "&s_date="+s_date;
            }           
        }
        searchForm.action=action_url;
        searchForm.submit();
    }
    function doClear(){
        document.getElementById('peer_id').value="";
        document.getElementById('resource_id').value="";
        document.getElementById('query_resource_name').value="";     
        document.getElementById('statistics_id').value="";
        document.getElementById('peerVersion_id').value="";     
        document.getElementById('s_begindate').value="";
        document.getElementById('s_enddate').value="";   
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
function _click_ymlist(){
    location.href="<%=curFile%>?s_year="+($('s_year').value||'')+"&s_month="+
            ($('s_month').value||'')+"<%=SuperPage.getQueryAnd(request,"s_year,s_month,s_date")%>";
}
function _click_date(){
    location.href="<%=curFile%>?s_date="+($('s_date').value||'')+
            "<%=SuperPage.getQueryAnd(request,"s_year,s_month,s_date")%>";
}
function _overdate(obj){
    if((obj.getAttribute('issel')||'')!='1') obj.style.backgroundColor="#ddd";
}
function _outdate(obj){
    obj.style.backgroundColor="";
}
function _click_to_line(){
    location.href="traffic_channel_catalog_day_line.jsp?s_date="+($('s_date').value||'')+
    "<%=SuperPage.getQueryAnd(request,"s_year,s_month,s_date")%>";
}   
</script>
</head>
<body>
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td class="title_text">PPVA 日 志 系 统 列 表</td>
  </tr>
</table>
<div style="float:right;padding:5px;"><span></span></div>
<fieldset style="border:1px #FF8040 solid ">
<table cellspacing="0" cellpadding="1" border="0" align="center" width="90%">
    <form method="post" id="searchForm" name="searchForm">
    <tr>
        <td nowrap="" width="70%">[<font color="#0066ff">查询</font>]
            PeerGuid：<input type="text" value="<%=peer_id%>" size="15" title="peer_id" id="peer_id" name="peer_id">
            ResourceID：<input type="text" value="<%=resource_id%>" size="15" id="resource_id">
            Peer版本号: <input type="text" value="<%=peerVersion_id%>" size="15" id="peerVersion_id">
            StatisticsID: <input type="text" value="<%=statistics_id%>" size="15" id="statistics_id">
            资源名：<input type="text" value="<%=query_resource_name%>" size="25" id="query_resource_name">
        </br>生成时间：<input name="s_begindate" type="text" id="s_begindate" title="开始时间" onclick="SelectDate(this,'yyyy-MM-dd')" value="<%=s_startTime %>" size="10"  maxlength="20" readonly="true" resetdm=true>
                    到  <input name="s_enddate" type="text" id="s_enddate" title="结束时间" onclick="SelectDate(this,'yyyy-MM-dd')" value="<%=s_endTime %>" size="10"  maxlength="20" readonly="true">
            <input name="Submit" type="button" class="BUTTON2" value="  查 询   " onclick="doSearch();">
            <input name="Clear" type="button" class="BUTTON2" value="  清空   " onclick="doClear();">
        </td>
    </tr>
    <tr>
    </tr>
    </form>
</table>

<table width="100%"  border="0" cellspacing="1" cellpadding="2" style="margin:5px 0;">
   <tr>
    <td>选择月份：
      <select name="s_year" id="s_year">
<%
    for(int i = new SuperDate().getYear(); i >=2007; i--) {
        out.println("<option value=\""+i+"\""+
                (i== s_year ?" selected class2=\"input_list_01sel\"":"")+
                ">"+ i+"</option>");
    }
%>
      </select>
      年
      <select name="s_month" id="s_month">
<%
    for(int i = 1; i <=12; i++) {
        out.println("<option value=\""+i+"\""+
                (i== s_month ?" selected class2=\"input_list_01sel\"":"")+
                ">"+ i+"</option>");
    }
%>
      </select>
     月&nbsp; <input name="Submit" type="button" class="BUTTON3" value="确定" onClick="_click_ymlist();">
     &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <font color="#CC0000">直接指定日期：</font><font color="#333333">
     <input name="s_date" type="text" class="input_date" id="s_date" value="<%=s_date%>" onclick="SelectDate(this,'yyyy-MM-dd')" size="12" maxlength="10" readonly="true" AUTOCOMPLETE="off">
     <input name="Submit2" type="button" class="BUTTON3" value="确定" onClick="_click_date();">
     </font>　
  </tr>
   <tr>
     <td><table border="0" cellpadding="0" cellspacing="1" class="bordday">
       <tr align="center">
<%
    SuperDate sdbegin= SuperDate.getFirstDayInMonthDate(sddate);
    SuperDate sdend=sdbegin.getAddedDate(Calendar.MONTH,1);
    while(sdbegin.before(sdend)){
        boolean issel=sdbegin.getDay()==sddate.getDay();
        boolean isweekend=sdbegin.getWeekValue()>=6;
        boolean istoday=sdbegin.getDateString().equals(sdToday.getDateString());
        String weektext=(istoday?"今天 ":"")+"周"+sdbegin.getWeekText();
        out.println("<td class=\"bordday_td"+(istoday?" bordday_today":"")+
                (issel?" bordday_td_sel":"")+(isweekend?" bordday_weekend":"")+
                "\" onmouseover=\"_overdate(this);\" onmouseout=\"_outdate(this);\" "+
                "onclick=\"location.href='"+
                curFile+"?act=datesearch&s_date="+sdbegin.getDateString()+
                SuperPage.getQueryAnd(request,"s_year,s_month,s_day,s_date")+
                "';\" "+
                "style=\"cursor:hand;\" "+(issel?"issel=\"1\"":"")+">"+sdbegin.getDay()+"</td>");
        sdbegin.add(Calendar.DAY_OF_YEAR,1);
    }
%>
       </tr>
     </table></td>
   </tr>
</table>

</fieldset>
<div style="height:30px;" >
</div>
<table width="100%" border="0" cellpadding="3" cellspacing="1" class="table_bord">
  <tr class="table_bord_ttl">
    <td nowrap width="2%" align='center'>PeerMonitor</td>
    <td nowrap width="2%" align='center'>下载链接</td>
    <td nowrap width="2%" align='center'>ID</td>
    <td nowrap width="10%" align='center'>IP地址</td>
    <td nowrap width="10%" align='center'>Peer版本号</td>
    <td nowrap width="15%" align='center'>提交时间</td>
    <td nowrap width="8%" align='center'>ResourceID</td>
    <td nowrap width="10%" align='center'>ResourceName</td>
    <td nowrap width="10%" align='center'>StatisticsID</td>
    <td nowrap width="15%" align='center'>PeerGuid</td>
  </tr>
<%
    for(int i = 0; i < rs.size(); i++) {
        LogItem item=rs.get(i).get(LogItem.class);
        String trclass = i % 2 == 0 ? "tr_class1" : "tr_class2";
        String dataStr = item.getTime_ToDateString();
        dataStr = dataStr.replace("-" , "_");
        String userLocation = "(" + IPSeeker.getInstance().getArea( item.getAddress() ) + "," + IPSeeker.getInstance().getCountry( item.getAddress() ) + ")";
        String downloadUrl = "Download.jsp?date=" + item.getTime_ToDateString().replace("-" , "_") + "&file_id=" + item.getLog_file_md5();
        String chartShowUrl = "ChartCreate.jsp?date=" + item.getTime_ToDateString().replace("-" , "_") + "&file_id=" + item.getLog_file_md5() + "&index_id=" + item.getId() + "&statistics_id="+item.getStatistics_id();
%>
   <tr class="<%=trclass%>" onMouseMove="cell_over(this);" onMouseOut="cell_out(this)" onClick="cell_click(this);">
    <td nowrap class="font22" align='center'><a href="<%=chartShowUrl%>">趋势图</a></td>
    <td nowrap class="font22" align='center'><a href="<%=downloadUrl%>">下载</a></td>
    <td nowrap class="font22" align='center'><%=item.getId()%></td>
    <td nowrap class="font22" align='center'><%=item.getAddress()  + userLocation %></td>
    <td nowrap class="font22" align='center'><%=item.getPeer_version() %></td>
    <td nowrap class="font22" align='center'><%=item.getTime_ToDateTimeString() %></td>
    <td nowrap class="font22" align='center'><%=item.getResource_id() %></td>
    <td nowrap class="font22" align='center'><%=item.getResource_name() %></td>
    <td nowrap class="font22" align='center'><%=item.getStatistics_id() %></td>
    <td nowrap class="font22" align='center'><%=item.getPeer_guid() %></td>
  </tr>
<%} %>
<%if(rs.size()==0){ %>
  <tr bgcolor="#FFFFFF" class="tr_class1">
    <td height="50" colspan="10"><div align="center">没有数据</div></td>
  </tr>
<%} %>
</table>
<div class="page_nav01"><%=SuperPage.generatePageNavSingle(rs.getAllCount(), request, null, pages.getPageSize(),"个")%></div>
<div style="height:30px;" >
<p>&nbsp;</p>
<p>&nbsp;</p>
</div>
</body>
</html>
