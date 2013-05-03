<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="java.io.File" %>
<%@ page import="tool.*" %>
<%@ page import="my.mysystem.tool.ContextUtil" %>
<%@ page import="my.myproperties.tool.PropUtil" %>
<%
    //System.out.println(MD5Util.encode());

    boolean isLogin=SuperString.notNullTrim((String)session.getAttribute("sword.my.constant.sessionid")).equals("1");
    String act=SuperString.notNull(request.getParameter("act"));

    String content = "";
    String rcharset = SuperString.notNullTrim(request.getParameter("rcharset"));
    String wcharset = SuperString.notNullTrim(request.getParameter("wcharset"));
    if(rcharset.length() == 0) rcharset = "UTF-8";
    if(wcharset.length() == 0) wcharset = "UTF-8";
    String filename = ContextUtil.rootPath + "WEB-INF/constant.properties";

    if(request.getMethod().equalsIgnoreCase("post")&&act.equals("login")){
        String loginpwd=SuperString.notNullTrim(request.getParameter("loginpwd"));
        if("".equals(loginpwd)||!MD5Util.encode(loginpwd).equals("4b14ade55cf2243661b1f2919884edfe")){
            out.println("登录失败！");
            return;
        }else{
            session.setAttribute("sword.my.constant.sessionid","1");
            response.sendRedirect(request.getRequestURI()+ SuperPage.getQueryQ(request,"act,id"));
            return;
        }
    }

    if(isLogin){
        if(request.getMethod().equalsIgnoreCase("post")) {
            content = SuperFile.enUnicodeForConstant(SuperString.notNullTrim(request.getParameter("content")),"#");
            String filenameTemp=filename+".temp";
            SuperFile.writeHtmlFileCharset(filenameTemp, content, wcharset);
            File fileTemp = new File(filenameTemp);
            if(fileTemp.exists() && fileTemp.isFile() && fileTemp.length() > 0) {
                SuperFile.fileCopy(null, filenameTemp, filename);
                PropUtil.loadProperties(filename);
                response.sendRedirect(request.getRequestURI() + SuperPage.getQueryQ(request, ""));
                return;
            } else {
                out.println("**** Error:PropTemp File is Error!****");
                return;
            }
        } else {
            content = SuperFile.deUnicode(SuperFile.readHtmlFileCharset(filename, rcharset));
        }

    }

%>

<html>
<head>
    <meta http-equiv="Content-Language" content="zh-cn">
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <title>配置文件管理</title>
    <link href="css/common.css" rel="stylesheet" type="text/css">
</head>

<body>
<center>
<%if(!isLogin){%>
    <form action="constant.jsp?act=login<%=SuperPage.getQueryAnd(request,"act")%>" method="post">
        <br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>
        输入密码：<input type="password" name="loginpwd" value="">
        <input type="submit" value=" 提交 " >
    </form>

<%}else{%>
    <form action="constant.jsp<%=SuperPage.getQueryQ(request,"act")%>" method="post">

        <textarea name="content" rows="30" style="width:100%; height:700px; font-family: 'Courier New', Courier, mono; font-size:12px; border:1px solid #999999; padding:5px; background-color: #FFFDEC;"><%=content%></textarea>

        <br>
        <input type="submit" value="     保 存 设 置     " class="BUTTON2" style="font-weight:bold ">
    </form>
<%}%>
    <br>
</center>


</body>
</html>