<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@ page import="java.net.URLEncoder" %>
<%@ page import="tool.SuperString" %>
<%@ page import="tool.DESUtil" %>
<%@ page import="com.speed.comm.Constant" %>
<%@ page import="tool.SuperPage" %>
<%

    String act = SuperString.notNullTrim(request.getParameter("act"));
    String key = SuperString.notNullTrim(request.getParameter("key"));
    String str = SuperString.notNullTrim(request.getParameter("str"));
    String enstr = SuperString.notNullTrim(request.getParameter("enstr"));
    boolean encode=SuperString.notNullTrim(request.getParameter("encode")).equalsIgnoreCase("1");
    boolean decode=SuperString.notNullTrim(request.getParameter("decode")).equalsIgnoreCase("1");
    if(key.equalsIgnoreCase("")) key = Constant.DES_KEY;

    String charset=SuperString.notNull(request.getParameter("charset"));
    if(charset.equals("")) charset="utf-8";

    if(act.equalsIgnoreCase("create")) {
        response.sendRedirect("deskey.jsp?key=" + URLEncoder.encode(DESUtil.generateKey(), "gbk"));
        return;
    }
    if(act.equalsIgnoreCase("en")) {
        enstr = DESUtil.encodeString(str, key);
        if(encode) enstr= SuperPage.URLEncode(enstr,charset);
    }
    if(act.equalsIgnoreCase("urlen")){
        enstr= SuperPage.URLEncode(str,charset);
    }
    if(act.equalsIgnoreCase("de")) {
        str = DESUtil.decodeString(decode? SuperPage.URLDecode(enstr,charset):enstr, key);
    }
    if(act.equalsIgnoreCase("urlde")){
        str= SuperPage.URLDecode(enstr,charset);
    }

%>
<html>
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <title>temp</title>
    <link href="../css/style.css" rel="stylesheet" type="text/css">
    <script language="javascript">
        function submitme(a) {
            with(document.form1) {
                if(a == 0) act.value = "create";
                if(a == 1) act.value = "en";
                if(a == 2) act.value = "de";
                if(a==3) act.value="urlen";
                if(a==4) act.value="urlde";
                submit();
            }
        }

    </script>

</head>

<body>

<div align="center">
    <p>&nbsp;</p>

    <p>&nbsp;</p>

    <form name="form1" method="post" action="deskey.jsp">
        <input type="hidden" name="act" value="">

        <p>&nbsp;</p>

        <p>密钥：
            <input name="key" type="text" id="key" size="40" value="<%=key%>">
            &nbsp;
            <input type="button" name="Submit" value="生成随机密钥" onClick="submitme(0)">
        </p>

        <p>明文：
            <input name="str" type="text" id="str" size="40" value="<%=str%>">
            <input name="encode" type="checkbox" id="encode" value="1">
            URLEncode&nbsp;
            <input type="button" name="Submit" value="   加 密   " onClick="submitme(1)">
            <input type="button" name="Submit" value="URLEncode" onClick="submitme(3)">
        </p>

        <p>密文：
            <input name="enstr" type="text" id="enstr" size="40" value="<%=enstr%>">
             <input name="decode" type="checkbox" id="decode" value="1">
URLDecode&nbsp;
            <input type="button" name="Submit" value="   解 密   " onClick="submitme(2)">
            <input type="button" name="Submit" value="URLDecode" onClick="submitme(4)">
        </p>

        <p>&nbsp;</p>

        <p>&nbsp;      </p>
    </form>
</div>
</body>
</html>