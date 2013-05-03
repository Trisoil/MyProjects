<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ page import="com.speed.bean.EggCard" %>
<%@ page import="tool.jdbc.SQLExecutor" %>
<%@ page import="tool.jdbc.ResultList" %>
<%@ page import="com.speed.comm.Util" %>
<%@ page import="com.speed.service.EggService" %>
<%
    response.setBufferSize(0);
    out.flush();
    out.println("<font style=\"font-size:12px;font-family:tahoma\">");


    SQLExecutor dbconn = null;
    String sql = "";
    try {
        dbconn = new SQLExecutor(true);

        sql="select * from egg_cardno where updatetime<'2010-04-01 00:00:00' order by createtime";
        ResultList rs = dbconn.query(sql, false, null);
        out.println("<br>共"+rs.size()+"个数据需要处理:<br>----------------------------<br>");
        for(int i = 0; i < rs.size(); i++) {
            EggCard eggCard=rs.get(i).get(EggCard.class);
            out.println("["+(i+1)+"]"+eggCard.getUpdateTime().getDateTimeString()+ " | ");
            out.println("<font color="+(eggCard.getType()==2?"red":"blue")+">");
            out.println(EggService.getEggAward(eggCard.getType(),true).getAwardName()+
                    " | "+eggCard.getCardNo());
            out.println("</font>");
            out.println(" | " +eggCard.getUserName()+" | "+eggCard.getStatus()+" .... ");

            if(eggCard.getType()==2){ //银蛋，删除
                eggCard.delete(dbconn);
                out.println("<font color=red>已删除</font>");
            }else if(eggCard.getType()==3){ //彩蛋，还原
                eggCard.setUserName(null);
                eggCard.setStatus(-1);
                eggCard.setUsed(0);
                eggCard.setUpdateTime(null);
                eggCard.update(dbconn);
                out.println("<font color=blue>update!</font>");
            }

            out.println("<br>");
            out.flush();
        }

        sql="delete from egg_log where createtime<'2010-04-01'";
        dbconn.execute(sql);
        out.println("<br><br>delete egg_log ... OK!");
        
        dbconn.commit();
    } catch(Exception e) {
        if(dbconn!=null) dbconn.rollBack();
        e.printStackTrace();
        Util.callInfo("数据库错误，请联系系统管理员！", request, response);
        return;
    } finally {
        if(dbconn!=null) dbconn.close();
    }


    out.println("<br><br>----------------------Over<bR><br>");
%>