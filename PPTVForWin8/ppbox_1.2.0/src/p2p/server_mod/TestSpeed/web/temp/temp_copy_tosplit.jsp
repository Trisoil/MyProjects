<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="my.myorm.tool.jdbc.SQLExecutor" %>
<%@ page import="my.myorm.tool.jdbc.SQLManager" %>
<%@ page import="my.myorm.tool.jdbc.DefaultBean" %>
<%@ page import="tool.SuperString" %>
<%@ page import="my.myorm.tool.jdbc.ResultList" %>
<%@ page import="com.speed.bean.Comment" %>
<%@ page import="com.speed.dao.DAOManager" %>
<%@ page import="com.speed.comm.Util" %>
<%
    long ibegin= System.currentTimeMillis();

    response.setBufferSize(0);
    out.print("<font style=\"font-size:12px;font-family:Tahoma;\">");
    out.flush();

    SQLExecutor dbconn= SQLManager.getSQLExecutor();
    String sql="";

    long lastid= SuperString.getLong(request.getParameter("lastid"));
    long maxid=SuperString.getLong(request.getParameter("maxid"));

    sql="select count(*) as thecount from ppt_comment where id>"+lastid +(maxid>0?" and id <="+maxid+" ":"");
    long allcount=dbconn.queryForBean(sql,DefaultBean.class).getLong("thecount");
    out.println("<b>共"+allcount+"条数据需要导出！</b><br>");
    out.flush();

    String sqlbase="select * from ppt_comment ";
    long sum=0;
    while(true){
        sql=sqlbase+" where id>"+lastid+ (maxid>0?" and id <="+maxid+" ":"")+ " order by id limit 10000";
        out.println("<br><br>[查询"+sum+"] "+sql+"<br>");
        out.flush();
        ResultList rs=dbconn.query(sql,true,null);
        if(rs.size()<=0){
            break;
        }
        for(int i = 0; i < rs.size(); i++) {
            Comment comment=rs.get(i).get(Comment.class);
            sql="insert into ppt_comment_"+Util.getTableSplit(comment.getChannelID())+" "+
                    "select * from ppt_comment where id="+comment.getId();
            dbconn.execute(sql);
            //DAOManager.commentDao.save(comment, "_"+Util.getTableSplit(comment.getChannelID()));
            if(i>0 && i%500==0) {
                out.println(i+". ");
                out.flush();
            }
            lastid=comment.getId();
            sum++;
        }
        /*if(sum>=2000){
            break;
        }*/
    }

    out.println("<br><br>---over, lastid:"+lastid+" total:"+sum+" spend:"+((System.currentTimeMillis()-ibegin)/1000)+" s<br><br>");
    out.flush();



%>