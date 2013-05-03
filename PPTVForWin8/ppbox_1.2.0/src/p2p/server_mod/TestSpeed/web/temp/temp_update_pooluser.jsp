<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ page import="tool.SuperString" %>
<%@ page import="tool.SuperPage" %>
<%@ page import="com.speed.comm.Util" %>
<%@ page import="com.speed.service.DataUtil" %>
<%@ page import="com.speed.bean.PoolUser" %>
<%@ page import="com.speed.comm.Constant" %>
<%@ page import="org.json.JSONObject" %>
<%@ page import="java.util.*" %>
<%@ page import="org.json.JSONArray" %>
<%@ page import="com.speed.bean.User" %>
<%@ page import="com.speed.dao.DAOManager" %>
<%@ include file="../commnocache.jsp" %>
<%
    response.setBufferSize(0);
    out.flush();
    out.println("<font style=\"font-size:12px;font-family:Tahoma;\">");

    List<PoolUser> list = DataUtil.getListPoolUser();
    out.println("<b>共" + list.size() + "个用户需要处理...</b><br>--------------------------<br>");

    out.println("<br>正在组织usernames ...... ");
    StringBuffer sb = new StringBuffer();
    for(PoolUser poolUser : list) sb.append(",").append(poolUser.getUserName());
    String usernames = sb.toString();
    if(usernames.startsWith(",")) usernames = usernames.substring(1);
    out.println("ok<br>");
    out.flush();

    out.println("<br>正在向通行证请求获取用户资料 ....... ");
    out.flush();
    String url = "http://passport.pptv.com/getUserDetail_iuwkk.jsp?password=uid54pok009&usernames=" +
            Util.encodeUrl(usernames);
    String ret = Util.openURL(url, Constant.CHARSET);
    JSONObject json = new JSONObject(ret);
    Map<String, String> mapHeadPics = new HashMap<String, String>();
    Iterator iter = json.keys();
    int count = 0;
    while(iter.hasNext()) {
        String key = (String) iter.next();
        JSONArray jsonArray = (JSONArray) json.get(key);
        String headpic = (String) jsonArray.get(0);
        headpic=SuperString.replace(headpic,"http://face.passport.pplive.com/","");
        mapHeadPics.put(key, headpic);
        count++;
    }
    out.println("<br>ok,count:" + count + "<br>");
    out.flush();

    out.println("<br>开始检查PoolUser ...... <br><br>");
    out.flush();

    for(int i = 0; i < list.size(); i++) {
        PoolUser poolUser = list.get(i);
        out.println("[" + (i + 1) + "] " + poolUser.getUserName() + " ...... ");
        String headpic = mapHeadPics.get(poolUser.getUserName());
        if(headpic == null || "".equals(headpic)) {
            out.println("<font color=red>no headpic</font>");
            out.println("<br>");
            out.flush();
            continue;
        }

        User user = DAOManager.userDao.findUserByUserName(poolUser.getUserName(), false);
        if(user == null) {//add
            user = new User();
            user.setUserName(poolUser.getUserName());
            user.setHeadPic(headpic);
            DAOManager.userDao.save(user);
            out.println("<font color=#0033ff>create ok,headpic=" + headpic + "</font>");
        } else {//update
            if(user.getHeadPic().equalsIgnoreCase(headpic)) {
                out.println("<font color=#999999>headpic no change: "+headpic+"</font>");
                out.println("<br>");
                out.flush();
                continue;
            }else{
                user.setHeadPic(headpic);
                DAOManager.userDao.update(user);
                out.println("<font color=#006699>update ok,headpic=" + headpic + "</font>");
            }
            
        }
        out.println("<br>");
        out.flush();
    }

    out.println("<bR><br>==================over<br><br>");


%>
