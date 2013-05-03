<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="com.speed.service.IPHPRPCService" %>
<%@ page import="org.phprpc.PHPRPC_Client" %>
<%@ page import="com.speed.bean.Result" %>
<%@ page import="tool.SuperPage" %>
<%@ page import="com.speed.dao.DAOManager" %>
<%@page import="tool.MD5Util"%>
<%

    //PHPRPC_Client client = new PHPRPC_Client(SuperPage.getHostURL(request)+"xihttp_phprpc_service.jsp");
    PHPRPC_Client client = new PHPRPC_Client("http://60.28.216.242:8080/xihttp_phprpc_service.jsp");
    client.setTimeout(50000);

    IPHPRPCService iphprpcService = (IPHPRPCService) client.useService(IPHPRPCService.class);
    /*Result result = iphprpcService.postComment(0,0,-1,0,0,"","ippgame",
            "e/7/c/e7ce3af6-3f09-4a60-838d-6fc5c6d03385.jpg?OWGFSV7K",1,"",
            "什么情况？");*/ 
    Result result = iphprpcService.postComment(0,1,10021,21,30616,"圆月弯刀","saker24","",1,"192.168.0.27",
            "人生啊1111，理想啊，就这样没了。。。。");
    //Result result = iphprpcService.userFollow("msprite",10);
    out.println(result.getRet() + "|" + result.getMsg());
    if(result.getListComment().size()>0) out.println("<Br>"+ DAOManager.commentDao.toJSON(result.getListComment().get(0)).toString());
    out.println("<br>-------------------------<br>");
	
    
    
    /*out.println("d8d527de565bfa19d6b5856e9e90d408<br/>");
    out.println(MD5Util.encode("$晚".getBytes("UTF-8")));*/
%>