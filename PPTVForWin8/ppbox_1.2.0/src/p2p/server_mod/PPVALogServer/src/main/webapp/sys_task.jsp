<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<%@ page import="tool.SuperPage" %>
<%@ page import="tool.SuperString" %>
<%@ include file="commcheck.jsp" %>
<%

    String curFile = SuperPage.getCurURLFile(request);

    String act = SuperString.notNullTrim(request.getParameter("act"));

    if(request.getMethod().equalsIgnoreCase("post")) {

        String info="运行成功！";

        if("cachedel".equals(act)) {

            String url= SuperPage.getHostURL(request)+"xihttp_comment_cache_delete.jsp";
            String ret=Util.openURL(url);
            if("ok".equals(ret)){
                info="运行成功，返回："+ret;
            }else{
                info="失败："+ret;
            }

        }

        Util.callInfo(info, request, response);
        return;

    }


%>
<html>
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <title>欢迎进入管理中心</title>
    <link href="css/style.css" rel="stylesheet" type="text/css">
    <script type="text/javascript" src="js/comm.js"></script>
</head>

<body>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
    <tr>
        <td class="title_text">自动任务管理</td>
    </tr>
</table>

<form name="form1" method="post" action="<%=curFile%>?act=cachedel">
    <div align="center">
        <input name="Submit" type="submit" class="BUTTON2" value=" 手动清理评论Cache表中过期数据 ">
        <font color="#006699">过期天数:<%=SuperString.getInt(Util.getProperties("COMMENT_CACHE_DELETE_DAYS"), 30)%></font></div>
</form>


<p>&nbsp;</p>
</body>
</html>
