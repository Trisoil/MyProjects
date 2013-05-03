<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="org.apache.commons.codec.digest.DigestUtils" %>
<%
    /*String ret= Util.openURL("http://192.168.27.69:8080/xihttp_binding_post.jsp?act=bind&username=hxpterry&appname=sina&appid=620826&apptoken=73b05fa0200607f301b80b531aca0dd3","utf-8");

    out.print("ret="+ret);*/

    out.println("d8d527de565bfa19d6b5856e9e90d408<br>");

    String str="$晚";

    out.println(DigestUtils.md5Hex(str));
    out.println("<br>"+ System.currentTimeMillis());

%>