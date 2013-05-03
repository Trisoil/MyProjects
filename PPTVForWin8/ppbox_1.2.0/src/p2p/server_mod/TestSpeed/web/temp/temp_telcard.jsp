<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ page import="tool.SuperString" %>
<%@ page import="java.util.Random" %>
<%@ page import="tool.SuperDate" %>
<%@ page import="com.speed.bean.EggCard" %>
<%@ page import="com.speed.comm.Constant" %>
<%@ page import="java.io.BufferedReader" %>
<%@ page import="java.io.InputStreamReader" %>
<%@ page import="java.io.FileInputStream" %>
<%!
    public static String generateTelCardNo() {
        StringBuffer sb = new StringBuffer();
        sb.append("T");
        sb.append(new SuperDate().getFormatDateTime("MMddHHmmss"));
        Random rnd = new Random();
        for(int i = 0; i < 6; i++) sb.append(String.valueOf(rnd.nextInt(10)));
        return sb.toString();
    }
%>
<%
    response.setBufferSize(0);
    out.flush();
    out.println("<font style=\"font-size:12px;\">");

    String act = SuperString.notNullTrim(request.getParameter("act"));
    int count = SuperString.getInt(request.getParameter("count"));
    if("rnd".equals(act)) {//随机生成
        out.println("<br>开始生成电信卡号：" + count + "<br>");
        out.println("---------------------------------<br>");
        for(int i = 0; i < count; i++) {
            String cardno = generateTelCardNo();
            out.println("[" + (i + 1) + "] " + cardno + " .... ");
            EggCard card = new EggCard();
            card.setCardNo(cardno);
            card.setType(3);
            card.setStatus(-1);//未使用
            card.setCreateTime(new SuperDate());
            card.save(null);
            out.println("OK");
            out.println("<br>");
            out.flush();
        }
        out.println("<br><br>---------------------over!<br><br>");
        out.flush();
    }

    String cardfile = SuperString.notNullTrim(request.getParameter("cardfile"));
    if("loadfile".equals(act)) {
        if("".equals(cardfile)) {
            out.println("请填写文件！");
            return;
        }
        BufferedReader reader = new BufferedReader(new InputStreamReader(
                new FileInputStream(cardfile), "UTF-8"));
        int allcount = 0;
        int suc = 0;
        int err = 0;
        for(String inLine = reader.readLine(); inLine != null; inLine = reader.readLine()) {
            inLine = SuperString.notNullTrim(inLine);
            if("".equals(inLine)) continue;
            allcount++;
            out.println("[" + allcount + "] " + inLine + " .... ");
            EggCard card = new EggCard();
            card.setCardNo(inLine);
            card.setType(3);
            card.setStatus(-1);//未使用
            card.setCreateTime(new SuperDate());
            card.save(null);
            out.println("OK");
            out.println("<br>");
            out.flush();
            suc++;
        }
        reader.close();
        out.println("===========over,all:" + allcount + ",suc:" + suc + ",err:" + err + "<br>");
        out.flush();
    }

    if("".equals(cardfile)) cardfile = Constant.ROOTPATH + "temp/telcard.txt";


%>

<p><b>电信爱冲印消费卷</b><br>
    ---------------------------------------</p>

<form class="formline" action="<%=request.getRequestURI()%>" name="form1" method="post">
    数量:
    <input name="count" type="text" value="1000" size="10"/>
    <input type="submit" name="Submit" value="随机生成卡号"/>
    <input type="hidden" name="act" value="rnd">
</form>
<form class="formline" action="<%=request.getRequestURI()%>" name="form2" method="post">
    文件：
    <input name="cardfile" type="text" size="60" value="<%=cardfile%>"/>
    <input type="submit" name="Submit" value="从文件载入"/>
    <input type="hidden" name="act" value="loadfile">
</form>