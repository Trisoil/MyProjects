<%@ page import="org.apache.commons.codec.binary.Base64" %>
<%@ page import="tool.DESUtil" %>
<%@ page import="tool.cookie.CryptogramHelper" %>
<%@ page contentType="text/html; charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%

    //String key="7F5rQ5LOXtYjs55JXnqDYfj4s4Pmbp7f";
    //key= DESUtil.generateKey();
    //out.println("<br>key="+key);
    //byte[] bkey= Base64.decodeBase64(key.getBytes());
    //String hexKey= DESUtil.byte2hex(bkey);
    String hexKey="9B2915A72F8329A2FE6B681C8AAE1F97ABA8D9D58576AB20";
    out.println("<br>HexKey="+hexKey);

    String hexiv= "???";
    out.println("<br>hexiv="+hexiv);
    byte[] byteIv= DESUtil.hex2byte(hexiv);
    //out.println("<br>striv="+striv);
    String str="pplive中国2010";
    out.println("<br>str="+str);
    String enstr= CryptogramHelper.Encrypt(str,hexKey,byteIv);
    out.println("<br>enstr="+enstr);

    out.println("<br>=========================<br>");

    String destr= CryptogramHelper.Decrypt(enstr,hexKey,byteIv);
    out.println("<br>destr="+destr);


%>