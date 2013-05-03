<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<%@ taglib prefix="any" uri="http://www.anychart.com" %>
<%@page import="my.myorm.tool.jdbc.SQLExecutor"%>
<%@page import="my.myorm.tool.jdbc.SQLManager"%>
<%@page import="my.myorm.tool.jdbc.Pages"%>
<%@page import="my.myorm.tool.jdbc.RecordMap"%>
<%@page import="tool.SuperPage"%>
<%@page import="tool.SuperString"%>
<%@page import="tool.SuperNumber"%>
<%@page import="tool.SuperDate"%>
<%@page import="com.speed.bean.Speed"%>
<%@page import="com.PPVALog.bean.base.*"%>
<%@page import="com.PPVALog.bean.*"%>

<%@page import="my.myorm.tool.jdbc.ResultList"%>
<%@page import="my.myorm.tool.jdbc.*"%>
<%@page import="tool.SuperFile"%>
<%@page import="tool.IPSeeker"%>
<%@page import="java.io.*"%>
<%@page import="java.util.*"%>
<%@page import="java.text.SimpleDateFormat"%>

<%@page import="my.myproperties.tool.PropUtil"%>

<%@page import="tool.XMLUtil"%>
<%@page import="org.jdom.Document"%>
<%@page import="org.jdom.Element"%>
<%@page import="org.jdom.JDOMException"%>

<%@include file="commcheck.jsp"%>
<%
  //  String clientInfo= SuperString.notNull(request.getHeader("User-Agent"));
  //  session.removeAttribute("xihttp_vod_online_rid");
  	String index_id = SuperString.notNullTrim(request.getParameter("index_id"));
	String date = SuperString.notNullTrim(request.getParameter("date"));
	
	SQLExecutor dbconn = SQLManager.getSQLExecutor("ppvaDataSource");
	String sql = "";
    sql = "select * from " + date + " where id=?";
    dbconn.addParam(index_id);
    LogItem item = dbconn.queryForBean(sql, LogItem.class);
    
    String downloadUrl = "Download.jsp?date=" + item.getTime_ToDateString().replace("-" , "_") + "&file_id=" + item.getLog_file_md5();
    String userLocation = "(" + IPSeeker.getInstance().getArea( item.getAddress() ) + "," + IPSeeker.getInstance().getCountry( item.getAddress() ) + ")";
    
    String dataXMLUrl = PropUtil.getInstance("constant").getProperty("PPVALogDir") + item.getStatistics_id() + "/" + item.getTime_ToDateString().replace("-" , "_") + "/" + item.getLog_file_md5() + "_target.xml";
	
    Document xml=XMLUtil.getDocument(dataXMLUrl);
    Element dataRootElement = XMLUtil.getRootElement(xml);
    List <Element> statisticDataList = dataRootElement.getChildren("StatisticData");
    int dataLength = statisticDataList.size();
    LogChartShowItemBase []chartShowItemList={};
    String [] typeall = PropUtil.getInstance("constant").getProperty("PPVALogType").split(",");
    //constant data
    String [] constantDataName={};
    String [] constantDataValue={};
    if(item.getStatistics_id().equals(typeall[0])){  //显示在顶端的常量
    	constantDataName = VodLogChartShowItem.getShowConstantName();
    	constantDataValue = new String [constantDataName.length];
    	for(int i1=0;i1<constantDataName.length;i1++)  constantDataValue[i1] = VodLogChartShowItem.getConstantData(i1,dataRootElement);
    }
    else if(item.getStatistics_id().equals(typeall[1])){
    	constantDataName = LiveLogChartShowItem.getShowConstantName();
        constantDataValue = new String [constantDataName.length];
        for(int i1=0;i1<constantDataName.length;i1++)  constantDataValue[i1] = LiveLogChartShowItem.getConstantData(i1,dataRootElement);
    }
    //table & chart
    if(item.getStatistics_id().equals(typeall[0])){
    	chartShowItemList = new VodLogChartShowItem[dataLength];
        for (int i1=0;i1<dataLength;i1++)   chartShowItemList[i1]= new VodLogChartShowItem();
    }
    else if(item.getStatistics_id().equals(typeall[1])){
	    chartShowItemList = new LiveLogChartShowItem[dataLength];
	    for (int i1=0;i1<dataLength;i1++)   chartShowItemList[i1]= new LiveLogChartShowItem();
    }
  	//boolean httpDownloadSpeedWrong=false,leftTimeWrong=false;
  	//boolean p2pDownloadSpeedWrong=false,stateMachineWrong=false;
  	String[][] dataSeries;
  	String[][] toolTipYValue;
  	String []titleSeries={};
  	if(item.getStatistics_id().equals(typeall[0]))
  		titleSeries=VodLogChartShowItem.getShowChartSeriesTitle();
  	else if(item.getStatistics_id().equals(typeall[1]))
  		titleSeries=LiveLogChartShowItem.getShowChartSeriesTitle();
  	
    //System.out.println("dataLength=" + dataLength);
    //System.out.println("titleSerieslength=" + titleSeries.length);
    
  	boolean []iswrongseries = new boolean[titleSeries.length];
  	for(int i1=0;i1<titleSeries.length;i1++) iswrongseries[i1]=false;
  	String allShowDataSeries="";
  	for (int i1=0;i1<dataLength;i1++)	chartShowItemList[i1].createByElement(statisticDataList.get(i1));  

  	if (dataLength>0){
  		dataSeries = new String [dataLength][titleSeries.length];
  		toolTipYValue = new String [dataLength][titleSeries.length];
  		for(int i1=0;i1<dataLength;i1++){
  			for(int i2=0;i2<titleSeries.length;i2++){
  				dataSeries[i1][i2]=chartShowItemList[i1].getChartData(i2,Integer.toString(i1));
  				toolTipYValue[i1][i2]=chartShowItemList[i1].getChartToolTipYValue(i2,Integer.toString(i1));
  				if(dataSeries[i1][i2].equals("wrong data")) 
  					iswrongseries[i2]=true;
  			}
  		}
  		
  		String []showDataSeries=new String [titleSeries.length];
    	for(int i1=1;i1<titleSeries.length;i1++){
    		if(iswrongseries[i1]) showDataSeries[i1]="";
    		else{
    			if(titleSeries[i1].equals("stateMachine")) showDataSeries[i1]= "<series name=\""+ titleSeries[i1] +"\" "+ "y_axis=\"extra_y_axis_1\">";
    			else showDataSeries[i1]= "<series name=\""+ titleSeries[i1] +"\">";
	    		for(int i2=0;i2<dataLength;i2++){
	    			showDataSeries[i1]+= "<point name=\"" + dataSeries[i2][0] + "\" y=\"" + dataSeries[i2][i1]+ "\" >"+
	    					"<tooltip><format><![CDATA[ " + titleSeries[i1] + "\n x: {%Name}{numDecimals:0}\n value: " 
	    				    + (toolTipYValue[i2][i1].length()==0?"{%YValue}":toolTipYValue[i2][i1])  + "]]></format></tooltip></point>";    	        	
	    	    }
	    		showDataSeries[i1]+= "</series>";
    		}
    		 
    	}   	 
    	for(int i1=1;i1<titleSeries.length;i1++) allShowDataSeries += showDataSeries[i1];		
  	}
    
%>
<html>
<head>
<meta  http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>欢迎进入管理中心</title>
<link href="css/common.css" rel="stylesheet" type="text/css">
<script language="javascript" src="js/comm.js"></script>
<script type="text/javascript" src="js/WebCalendar.js" language="javascript"></script>
<script language="javascript" src="js/tablecell.js"></script>    
<script type="text/javascript" src="js/prototype.js" language="javascript"></script>
</head>

<body>
<script type="text/javascript" src="js/calendar.js"></script>
<div id="divpop" style="border:1px solid #000000; width:200px; height:200px; background-color:bisque; position:absolute; visibility: hidden;">这是一个弹出层</div>
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td class="title_text">PeerMonitor</td>
  </tr>
  <tr>
  	<td width="90%">&nbsp;</td>
  	<td width="10%" class="font23"><a href= "javascript:history.back()">返回</a></td>    
  </tr>
  <tr><td class="font23"><a href="<%=downloadUrl%>">下载链接</a></td></tr>
  <tr><td class="font23">IP地址: <%=item.getAddress()  + userLocation %></td></tr>
  <tr><td class="font23">Peer版本号: <%=item.getPeer_version() %></td></tr>
  <tr><td class="font23">提交时间: <%=item.getTime_ToDateTimeString() %></td></tr>
  <tr><td class="font23">ResourceID: <%=item.getResource_id() %></td></tr>
  <tr><td class="font23">ResourceName: <%=item.getResource_name() %></td></tr>
  <tr><td class="font23">StatisticsID: <%=item.getStatistics_id() %></td></tr>
  <tr><td class="font23">PeerGuid: <%=item.getPeer_guid() %></td></tr>
<%
for(int i1=0;i1<constantDataName.length;i1++){
%>
  <tr><td class="font23"><%=constantDataName[i1] %>: <%=constantDataValue[i1] %></td></tr> 
<%	
}
%>
</table>
<div style="height:10px;" >
</div>

<div style="margin-left: -17px">
	<any:chart width="1000" height="480">
		<anychart>
			<charts>
		        <chart>
		            <chart_settings>
		                <title>
		                    <text><![CDATA[Peer Monitor]]></text>
		                </title>
		                <axes>
		                    <x_axis>
		                        <title>
		                            <text> <!--  <![CDATA[Months]]> --></text>
		                        </title>
		                    </x_axis>
		                    <y_axis>
		                        <title>
		                            <text> <!-- <![CDATA[Sales]]> --></text>
		                        </title>
		                    </y_axis>
		                    <extra>
								<y_axis name="extra_y_axis_1" position="Right">
									<minor_grid enabled="false"/>
									<major_grid enabled="false"/>
									<title>			
										<text> </text>
									</title>
								</y_axis>
							</extra>
		                </axes>
		                <legend enabled="true"/>
		            </chart_settings>
		            <data_plot_settings default_series_type="Spline">
						<line_series>
							<tooltip_settings enabled="true">
								<format>
Value: {%YValue}{numDecimals:2}
ID: {%Name}
								</format>
								<background>
									<border type="Solid" color="DarkColor(%Color)"/>
								</background>
								<font color="DarkColor(%Color)"/>
							</tooltip_settings>
							<marker_settings enabled="true"/>
							<line_style>
								<line thickness="3"/>
							</line_style>
						</line_series>
					</data_plot_settings>
		            <data>
		                <%=allShowDataSeries%>
		            </data>
		        </chart>
		    </charts>         
        </anychart>       
    </any:chart>
</div>

<div style="height:10px;" >
</div>
<table width="100%" border="0" cellpadding="3" cellspacing="1" class="table_bord">
  <tr class="table_bord_ttl">
<%
    if(chartShowItemList.length > 0){
    	String []showTableRowName = chartShowItemList[0].getShowTableRowName();
    	//System.out.println(showTableRowName.length+"");
    	for(int i1=0;i1<showTableRowName.length;i1++)
    	{
%>
            <td nowrap width="3%" align='center'><%=showTableRowName[i1] %></td>
<%   		
    	}	    
    }
%>
  </tr>
<%
    for(int i = 0; i < statisticDataList.size(); i++) {
    	String trclass = i % 2 == 0 ? "tr_class1" : "tr_class2";
      /*  Element el=statisticDataList.get(i);
        String totalDownloadSpeed;
        try{
        	totalDownloadSpeed=Integer.toString(Integer.parseInt(el.getChildText("httpDownloadSpeed"))/1024+Integer.parseInt(el.getChildText("p2pDownloadSpeed"))/1024);
        }catch (Exception e){
        	totalDownloadSpeed="wrong data";			
		}  */   
		LogChartShowItemBase temp = chartShowItemList[i];		
%>
   <tr class="<%=trclass%>" onMouseMove="cell_over(this);" onMouseOut="cell_out(this)" onClick="cell_click(this);">
<%
		for(int i2=0;i2<temp.getShowTableRowName().length;i2++){
%>
   <td nowrap class="font22" align='center'><%=temp.getRowData(i2,Integer.toString(i))%></td>
<%			
		}
%>

  </tr>
<%} %>
<%if(statisticDataList.size()==0){ %>
  <tr bgcolor="#FFFFFF" class="tr_class1">
    <td height="50" colspan="9"><div align="center">没有数据</div></td>
  </tr>
<%} %>
</table>

</body>
</html>
