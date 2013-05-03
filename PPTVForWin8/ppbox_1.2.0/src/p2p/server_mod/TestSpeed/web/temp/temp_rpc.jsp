<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="com.speed.service.IPHPRPCService" %>
<%@ page import="org.phprpc.PHPRPC_Client" %>
<%@ page import="com.speed.bean.Comment" %>
<%@ page import="java.util.List" %>
<%@ page import="tool.SuperString" %>
<%@ page import="com.speed.comm.Util" %>
<%@ page import="tool.SuperPage" %>
<%@ page import="com.speed.dao.DAOManager" %>
<%
    long minid=SuperString.getLong(request.getParameter("minid"));

    //PHPRPC_Client client = new PHPRPC_Client(SuperPage.getHostURL(request)+"xihttp_phprpc_service.jsp");
    PHPRPC_Client client = new PHPRPC_Client("http://60.28.216.242:8080/xihttp_phprpc_service.jsp");
    client.setTimeout(50000);   

    IPHPRPCService iphprpcService = (IPHPRPCService) client.useService(IPHPRPCService.class);

    String face_url= SuperString.notNullTrim(Util.getProperties("PP_USERFACE_URL"));

    String act=SuperString.notNullTrim(request.getParameter("act"));
    List<Comment> list=null; 
    if("hall".equals(act)) list=iphprpcService.getCommentForHall(0,0,10,true);
    else list=iphprpcService.getCommentForChannel(34478,(int)minid,10,false);

    System.out.println((int)minid);
    out.println(list.size()+"<br>");
    long _minid=0l;
    for(int i = 0; i < list.size(); i++) {
        Comment comment=list.get(i);
        out.println("["+(i+1)+"] "+ comment.getId()+"|"+comment.getRootID()+"|"+comment.getUserName()+
                "<img src=\""+face_url+comment.getUserHeadPic()+"\" width=20>"+
                "|"+comment.getCreateTime_ToDateTimeString()+"|"+ comment.getChannelName()+"|"+
                comment.getContent()+"<br>");
        out.println("<font style='font-size:12px;color=#0066cc'>"+DAOManager.commentDao.toJSON(comment).toString()+"</font><br>");
        if(_minid==0||comment.getId()<_minid) _minid=comment.getId();
        for(Comment _comment:comment.getListReplyComments()){
            out.println("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;→"+_comment.getId()+"|"+_comment.getRootID()+"|"+
                    _comment.getUserName()+
                    "<img src=\""+face_url+_comment.getUserHeadPic()+"\" width=20>"+
                "|"+_comment.getCreateTime_ToDateTimeString()+"|"+
                    _comment.getChannelName()+"|"+
                    _comment.getContent()+"<br>");
            if(_minid==0||_comment.getId()<_minid) _minid=_comment.getId();
        }
    }
    //iphprpcService.postComment(111,1,1,1,1,"cname","fb","username","headpic",1,"192.198.1.1","太好看了");
    if(_minid>0) {
        out.println("<br><a href=\""+request.getRequestURI()+"?minid="+_minid+ SuperPage.getQueryAnd(request,"minid")+ "\">更多</a>");
    }
    out.println("<br><br>-----------------------over<br><br>");

   /* result=iphprpcService.userFollowDelete("ami",30);
    if(result == null) {
        out.println("result=null");
    } else {
        out.println(result.getRet() + "|" + result.getMsg());
    }
    out.println("<br>-------------------------<br>");
*/

    /*List<Comment> list= PHPRPCService.getComment();
    out.println(list.size());*/

%>