<%@page import="tool.SuperPage"%>
<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<%@page import="tool.SuperString"%>
<%@page import="com.speed.dao.DAOManager"%>
<%@page import="com.speed.bean.Speed"%>
<%@page import="java.util.Date"%>
<%@page import="tool.SuperFile"%>
<%@include file="commcheck.jsp"%>
<%
	String curFile = SuperPage.getCurURLFile(request);
	String modFile = SuperFile.getFileName(curFile) + "mod.jsp";
	String modFileDo = SuperFile.getFileName(curFile) + "moddo.jsp";
	String act = SuperString.notNullTrim(request.getParameter("act"));
	String info="";
	String speedDomain=Util.getProperties("SPEED_KEY_DOMAIN");
	String speedLink="";
	if(request.getMethod().equalsIgnoreCase("post")) {
	    if("create".equals(act)) {
	    	try{
	    		Speed speed=new Speed();
	    		String speedKey=SuperString.getSpeedKeys();
	        	speed.setSpeedKey(speedKey);
	        	speed.setCreateTime(new Date());
	        	speed.setOperator(Constant_UserName);
	    		DAOManager.speedDao.save(speed);
	    		speedLink=speedDomain+speedKey;
	   		    info="生成测速连接成功";
	    	}catch(Exception e){
	    		e.printStackTrace();
	    		info="生成测速连接失败";
	    	}
	    }
	}
%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>无标题文档</title>
<link href="css/common.css" rel="stylesheet" type="text/css">
<script language="javascript" src="js/comm.js"></script>
<script type="text/javascript" language="JavaScript">
function _do_copy() {  
	var copytext=document.getElementById('speedlink').value;
     if(window.clipboardData) {  
             window.clipboardData.clearData();  
             window.clipboardData.setData("Text", copytext);  
     } else if(navigator.userAgent.indexOf("Opera") != -1) {  
          window.location = copytext;  
     } else if (window.netscape) {  
          try {  
               netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");  
          } catch (e) {  
               alert("被浏览器拒绝！\n请在浏览器地址栏输入'about:config'并回车\n然后将'signed.applets.codebase_principal_support'设置为'true'");  
          }  
          var clip = Components.classes['@mozilla.org/widget/clipboard;1'].createInstance(Components.interfaces.nsIClipboard);  
          if (!clip)  
               return;  
          var trans = Components.classes['@mozilla.org/widget/transferable;1'].createInstance(Components.interfaces.nsITransferable);  
          if (!trans)  
               return;  
          trans.addDataFlavor('text/unicode');  
          var str = new Object();  
          var len = new Object();  
          var str = Components.classes["@mozilla.org/supports-string;1"].createInstance(Components.interfaces.nsISupportsString);  
          str.data = copytext;  
          trans.setTransferData("text/unicode",str,copytext.length*2);  
          var clipid = Components.interfaces.nsIClipboard;  
          if (!clip)  
               return false;  
          clip.setData(trans,null,clipid.kGlobalClipboard);  
          //alert("复制成功！") ;
     }  
  alert("复制成功，请用Ctrl+V粘贴！") ;
} 
</script>
</head>
<body>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
    <tr>
        <td class="title_text"> 生 成 连 接 管 理 </td>
    </tr>
</table>
<div style="height:30px;"/>
<table width="100%" border="0" cellpadding="3" cellspacing="1" class="table_bord">
  <tr class="table_bord_ttl">
    <td>生 成 连 接 </td>
  </tr>
  <tr bgcolor="#FFFFFF" class="tr_class1">
    <td>
	<form name="form1" method="post" action="<%=curFile%>?act=create">
		<table width="100%"  border="0" cellspacing="1" cellpadding="2">
	      <tr>
	        <td width="15%">&nbsp;</td>
	        <td width="70%" align="center"><font color="red" size="4"><b><%=info%></b></font></td>
	        <td width="15%">&nbsp;</td>
	      </tr>
	      <tr>
	        <td width="15%">&nbsp;</td>
	        <td width="70%">&nbsp;</td>
	        <td width="15%">&nbsp;</td>
	      </tr>
	      <tr>
	        <td height="22" align="center">测速连接：</td>
	        <td><input name="speedlink" id="speedlink" type="text" size="120" value="<%=speedLink%>"></td>
	        <td>&nbsp;</td>
	      </tr>
	      <tr>
	        <td align="center">&nbsp;</td>
	        <td height="50">
				<input name="Submit" type="submit" class="BUTTON2" value="   生 成 测 速 连 接     ">
				<input name="copy" type="button" class="BUTTON2" value=" 复制 " onClick="_do_copy();">
				<input name="back" type="button" class="BUTTON2" value=" 返回 " onClick="location.href='speed.jsp'">
			</td>
	        <td>&nbsp;</td>
	      </tr>
	      <tr>
	        <td align="center">&nbsp;</td>
	        <td>&nbsp;</td>
	        <td>&nbsp;</td>
	      </tr>
	    </table>
	</form>
	</td>
  </tr>
</table>
</body>
</html>