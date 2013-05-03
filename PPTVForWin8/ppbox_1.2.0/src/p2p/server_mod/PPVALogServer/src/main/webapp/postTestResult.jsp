<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<%@page import="my.myorm.tool.jdbc.SQLExecutor"%>
<%@page import="my.myorm.tool.jdbc.SQLManager"%>
<%@page import="my.myorm.tool.jdbc.ResultList"%>
<%@page import="tool.SuperString"%>
<%@page import="com.speed.bean.Speed"%>
<%@page import="com.speed.comm.Util"%>
<%@page import="java.util.Date"%>
<%@page import="com.speed.dao.DAOManager"%>
<%@page import="com.speed.bean.SpeedResult"%>
<%@page import="java.util.Arrays"%>
<%@page import="java.util.Map"%>
<%@page import="java.util.HashMap"%><%
String speedKey=SuperString.notNullTrim(request.getParameter("key"));
String value=SuperString.notNullTrim(request.getParameter("value"));
String msg="";
/**
if(!request.getMethod().equalsIgnoreCase("post")) {
	msg="当前数据提交方式不是POST方式，请用post方法提交数据！";	
	out.print(msg);
	return;
}*/
try{
	if(speedKey.length()==0){
		msg="KEY不能为空！";
		out.print(msg);
		return;
	}
	if(value.length()==0){
		msg="参数不能为空！";
		out.print(msg);
		return;
	}
	SQLExecutor dbconn = SQLManager.getSQLExecutor();
	String sql="select * from speed_list where speedKey='"+speedKey+"' order by id desc";
	ResultList rs=dbconn.query(sql,false,null);
	if(rs.size()==0){
		msg="KEY不存在！";
		out.print(msg);
		return;
	}else{
		Speed speed=rs.get(0).get(Speed.class);
		int speedTime=Util.getPropertiesInt("SPEED_TIME");
		long sysTimeL=System.currentTimeMillis();
		long createTimeL=speed.getCreateTime().getTime();
		long timeL=(sysTimeL-createTimeL)/1000;
		if(timeL>speedTime){
			msg="KEY已经过期！";
			out.print(msg);
			return;
		}else{
			String[] speedValueArr=value.split(":");
			if(speedValueArr!=null&&speedValueArr.length>0){
				//先删除
			    String sql_del="delete from speed_result where SpeedKey="+speedKey;
				dbconn.execute(sql_del);
				//再保存
			    Map<String,Integer> map=new HashMap<String,Integer>();
			    int maxSpeed=0;
			    String maxSpeedStr="";
				for(String resultStr:speedValueArr){
					String[] resultArr=resultStr.split(",");
					if(resultArr.length<2){
						continue;
					}
					int temp=Integer.valueOf(resultArr[1]);
					if(maxSpeed==0){
						maxSpeed=temp;
						maxSpeedStr=resultArr[0]+","+resultArr[1];
					}else if(temp>maxSpeed){
						maxSpeed=temp;
						maxSpeedStr=resultArr[0]+","+resultArr[1];
					}
					SpeedResult speedResult=new SpeedResult();
					speedResult.setSpeedKey(speedKey);
					speedResult.setResultID(resultArr[0]);
					speedResult.setResultValue(Integer.parseInt(resultArr[1]));
					speedResult.setCompleteTime(new Date());
					speedResult.setUserIp(Util.getIpAddr(request));
					DAOManager.speedResultDao.save(speedResult);
				}
				//速度最快的更新到list
				String[] resultMaxArr=maxSpeedStr.split(",");
				Speed modSpeed=new Speed();
				modSpeed.setId(speed.getId());
				modSpeed.setResultID(resultMaxArr[0]);
				modSpeed.setResultValue(Integer.parseInt(resultMaxArr[1]));
				modSpeed.setCompleteTime(new Date());
				modSpeed.setUserIp(Util.getIpAddr(request));
				DAOManager.speedDao.update(modSpeed);
				msg="汇报测速数据成功";
				out.print(msg);
				return;
			}
		}
	}
}catch(Exception e){
	e.printStackTrace();
	msg="汇报测速数据出错了！赶快联系管理员，哈哈！！";
	out.print(msg);
	return;
}
%>