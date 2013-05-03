<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ page import="java.util.Date" %>
<%@ page import="java.util.Random" %>
<%@ page import="tool.*" %>
<%@ page import="tool.jdbc.SQLExecutor" %>
<%@ page import="com.speed.bean.EggCard" %>
<%@ page import="com.speed.comm.Util" %>
<%@include file="../commnocache.jsp"%>
<%!
    public static String generateCardNo() {
        StringBuffer sb = new StringBuffer();
        sb.append("P");
        sb.append(new SuperDate().getFormatDateTime("MMddHHmmss"));
        Random rnd = new Random();
        for(int i = 0; i < 6; i++) sb.append(String.valueOf(rnd.nextInt(10)));
        return sb.toString();
    }
%>
<%
    response.setBufferSize(0);
    out.flush();
    out.println("<font style=\"font-size:12px;font-family:Tahoma;\">");

    String md5key=SuperString.notNullTrim(Util.getProperties("EGG_MD5_KEY"));

    SuperDate sdNow=new SuperDate();

    String act = SuperString.notNullTrim(request.getParameter("act"));
    String username = SuperString.notNullTrim(request.getParameter("username"));
    String url=SuperString.notNullTrim(request.getParameter("url"));
    String ret="";
    if("start".equals(act)) {//start
        String time=String.valueOf(new Date().getTime());
        String md5str=Util.encodeUrl(username)+time+md5key;
        String openurl=url+"?username="+Util.encodeUrl(username)+"&time="+time+"&md5="+ MD5Util.encode(md5str)+
                "&rnd="+new Date().getTime()+""+new Random().nextInt(1000);
        ret= Util.openURL(openurl);
        out.println("<br>结口返回结果：<br>");
        out.println("<textarea rows=\"5\" cols=\"60\">"+SuperString.escapeHTMLTagsSpe(ret)+"</textarea>");
        out.println("<br><br>");
        out.flush();
    }

    String url_confirm=SuperString.notNullTrim(request.getParameter("url_confirm"));
    int type=SuperString.getInt(request.getParameter("type"),2);
    String no=SuperString.notNullTrim(request.getParameter("no"));
    if("put".equals(act)){
        String time=String.valueOf(new Date().getTime());
        String md5str=Util.encodeUrl(username)+String.valueOf(type)+no+time+md5key;
        ret= Util.openURL(url_confirm+"?username="+Util.encodeUrl(username)+"&ret="+type+"&no="+no+
                "&time="+time+"&md5="+MD5Util.encode(md5str)+
                "&rnd="+new Date().getTime()+""+new Random().nextInt(1000));
        out.println("<br>结口返回结果：<br>");
        out.println("<textarea rows=\"5\" cols=\"60\">"+SuperString.escapeHTMLTagsSpe(ret)+"</textarea>");
        out.println("<br><br>");
        out.flush();
    }

    if("dellog".equals(act)){
        SQLExecutor dbconn = new SQLExecutor();
        String sql = "delete from egg_log where username=? and createtime>=?";
        dbconn.addParam(username);
        dbconn.addParam(new SuperDate(new SuperDate().getDateString(),true).getDateTimeString());
        dbconn.execute(sql);
        out.println("<br>操作结果：<br>");
        out.println("<font color=blue><b>用户【"+username+"】今日砸蛋记录已删除，可以再次抽奖！</b></font>");
        out.println("<br><br>");
        out.flush();
    }

    if("createcard".equals(act)){
        int count= SuperString.getInt(request.getParameter("count"));
        if(count<0) count=0;
        out.println("<br>需要生成"+count+"张8折会员充值卡<br>------------------------------<br>");
        out.flush();
        for(int i = 0; i < count; i++) {
            out.println("["+(i+1)+"] create cardno .... ");
            String cardno = generateCardNo();
            out.println("<font color=blue>"+cardno+"</font> ... ");
            EggCard eggCard = new EggCard();
            eggCard.setCardNo(cardno);
            eggCard.setType(2);
            eggCard.setUserName(username);
            eggCard.setStatus(1);
            eggCard.setUpdateTime(sdNow);
            eggCard.setCreateTime(sdNow);
            eggCard.save(null);
            out.println("<font color=blue>OK</font>");
            out.println("<br>");
            out.flush();
        }

        out.println("<br><br>------------------over<bR><br>");
        out.flush();

    }

    String awardparam="";
    if("createurl".equals(act)){
        String time=String.valueOf(new Date().getTime()/1000);
        String md5str=Util.encodeUrl(username)+time+md5key;
        awardparam="?username="+Util.encodeUrl(username)+"&time="+time+"&md5="+MD5Util.md5Hex(md5str);
    }

    if("".equals(url))url= SuperPage.getHostURL(request)+"xihttp_egg_run.jsp";
    if("".equals(username)) username="saker";

    if("".equals(url_confirm)) url_confirm=SuperPage.getHostURL(request)+"xihttp_egg_put.jsp";

    String awardurl=SuperPage.getHostURL(request)+"xihttp_egg_award.jsp"+awardparam;
    

%>

<b>砸蛋接口模拟调用</b><br>
---------------------------<bR>
<form style="display:inline;" action="<%=request.getRequestURI()%>" name="form1" method="post">
    接口：<input name="url" type="text" value="<%=url%>" size="60"/><br>
    用户名:
      <input name="username" type="text" value="<%=username%>" size="20"/>
    <input type="submit" name="Submit" value="开始游戏"/>
    <input type="hidden" name="act" value="start">
</form>

<br><br><b>砸中奖项确认</b><br>
---------------------------<bR>
<form style="display:inline;" action="<%=request.getRequestURI()%>" name="form2" method="post">
    接口：<input name="url_confirm" type="text" value="<%=url_confirm%>" size="60"/><br>
    奖项类型:
      <input name="type" type="text" value="<%=type%>" size="5"/>
    中奖卡号:
       <input name="no" type="text" value="<%=no%>" size="20"/>
    <input type="submit" name="Submit" value=" 确认 "/>
    <input type="hidden" name="act" value="put">
    <input type="hidden" name="username" value="<%=username%>">
</form>

<br><br><b>获取用户砸蛋信息</b><br>
---------------------------<bR>
<form style="display:inline;" action="<%=request.getRequestURI()%>" name="form2" method="post">
    用户名:
      <input name="username" type="text" value="<%=username%>" size="20"/>
    <input type="hidden" name="act" value="createurl">
    <input type="submit" name="Submit" value=" 生成请求URL "/>
</form>

<%if("createurl".equals(act)){%>
<br><br>
<a href="<%=awardurl%>" target="_blank"><%=awardurl%></a>
<%}%>

<br><br><b>删除用户今日砸蛋记录</b><br>
---------------------------<bR>
<form style="display:inline;" action="<%=request.getRequestURI()%>" name="form2" method="post">
    用户名:
      <input name="username" type="text" value="<%=username%>" size="20"/>
    <input type="hidden" name="act" value="dellog">
    <input type="submit" name="Submit" value=" 删除今日砸蛋记录 "/>
</form>

<br><br><b>手动生成8折会员卡</b><br>
---------------------------<bR>
<form style="display:inline;" action="<%=request.getRequestURI()%>" name="form2" method="post">
    用户名:
      <input name="username" type="text" value="<%=username%>" size="20"/>
    数量：
      <input name="count" type="text" value="80" size="5"/>
    <input type="hidden" name="act" value="createcard">
    <input type="submit" name="Submit" value=" 手动生成8折会员卡 "/>
</form>
