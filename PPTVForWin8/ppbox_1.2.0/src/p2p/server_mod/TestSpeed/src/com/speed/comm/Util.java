package com.speed.comm;

import my.myproperties.tool.PropUtil;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.net.URL;
import java.net.URLEncoder;
import java.net.URLDecoder;
import java.util.*;
import javax.servlet.http.HttpServletRequest;

import com.speed.comm.Constant;

import tool.*;


/**
 * Created by IntelliJ IDEA.
 *
 * @version 1.0
 *          Date: 2005-11-30
 *          Time: 23:58:36
 */
public class Util {

    /*通用系统信息显示方法*/
    public static void callInfo(String strMessage,
                                javax.servlet.http.HttpServletRequest request,
                                javax.servlet.http.HttpServletResponse response) {
        callInfo(strMessage, true, null, request, response);
    }

    public static void callInfo(String strMessage,
                                String backURL,
                                javax.servlet.http.HttpServletRequest request,
                                javax.servlet.http.HttpServletResponse response) {
        callInfo(strMessage, true, backURL, request, response);
    }

    public static void callInfo(String strMessage,
                                boolean showBack, String backURL,
                                javax.servlet.http.HttpServletRequest request,
                                javax.servlet.http.HttpServletResponse response) {
        try {
            request.setAttribute("commMessage", strMessage);
            if(!showBack) request.setAttribute("showback", "N");
            else request.setAttribute("showback_url", backURL);
            javax.servlet.RequestDispatcher disp = request.getRequestDispatcher("comminfo.jsp");
            disp.forward(request, response);
        } catch(Exception e) {
            try {
                response.sendRedirect("comminfo.jsp");
            } catch(Exception ex) {
            }
        }
    }

    public static String encodeUrl(String url) throws UnsupportedEncodingException {
        return encodeUrl(url, Constant.CHARSET);
    }

    public static String decodeUrl(String url) throws UnsupportedEncodingException {
        return URLDecoder.decode(url, Constant.CHARSET);
    }

    public static String encodeUrl(String url, String charset) throws UnsupportedEncodingException {
        return URLEncoder.encode(url, charset);
    }

    public static String decodeUrl(String url, String charset) throws UnsupportedEncodingException {
        return URLDecoder.decode(url, charset);
    }


    public static String generateHourOption(int defaultValue) {
        String r = "";
        SuperNumber.fillChar = '0';
        for(int i = 0; i < 24; i++) {
            r += "<option " + (defaultValue == i ? "selected " : "") + ">" + SuperNumber.format(i, "2.0") + "</option>";
        }
        return r;
    }

    public static String generateMinuteOption(int defaultValue) {
        String r = "";
        SuperNumber.fillChar = '0';
        for(int i = 0; i < 60; i++) {
            r += "<option " + (defaultValue == i ? "selected " : "") + ">" + SuperNumber.format(i, "2.0") + "</option>";
        }
        return r;
    }

    public static SuperDate getDateFromRequest(String prefix, HttpServletRequest request) {
        String date = SuperString.notNull(request.getParameter(prefix + "date"));
        String hour = SuperString.notNull(request.getParameter(prefix + "hour"));
        String min = SuperString.notNull(request.getParameter(prefix + "min"));
        String sec = SuperString.notNull(request.getParameter(prefix + "sec"));
        String datestr = date + " " + hour + ":" + min + ":" + sec;
        try {
            return new SuperDate(datestr);
        } catch(Exception e) {
            return null;
        }
    }


    public static String generateSelectList(String name,
                                            String[] arrText,
                                            int seletedValue,
                                            int width,
                                            String firstRowText) {
        StringBuffer sb = new StringBuffer();
        sb.append("<select name=\"").append(name)
                .append("\" id=\"").append(name)
                .append("\" class=\"input_list_01\"");
        if(width > 0) sb.append(" style=\"width:").append(width).append("px\"");
        sb.append(">\n");
        if(firstRowText != null && firstRowText.length() > 0)
            sb.append("<option value=\"\">").append(firstRowText).append("</option>\n");
        for(int i = 0; i < arrText.length; i++) {
            sb.append("<option value=\"").append(String.valueOf(i)).append("\"");
            if(seletedValue == i) sb.append(" selected class=\"input_list_01sel\"");
            sb.append(">").append(arrText[i]).append("</option>\n");
        }
        sb.append("</select>\n");
        return sb.toString();
    }

    public static String openURL(String _url) {
        try {
            URL url = new URL(_url);
            InputStream is = url.openStream();
            BufferedReader reader = new BufferedReader(new InputStreamReader(is));
            StringBuffer sb = new StringBuffer();
            String input_info = "";
            while((input_info = reader.readLine()) != null) {
                sb.append(input_info.trim());
            }
            reader.close();
            is.close();
            return SuperString.notNullTrim(sb.toString());
        } catch(Exception e) {
            e.printStackTrace();
            return "";
        }
    }

    public static String openURL(String _url, String charset) {
        try {
            URL url = new URL(_url);
            InputStream is = url.openStream();
            BufferedReader reader = new BufferedReader(new InputStreamReader(is, charset));
            StringBuffer sb = new StringBuffer();
            String input_info = "";
            while((input_info = reader.readLine()) != null) {
                sb.append(input_info.trim());
            }
            reader.close();
            is.close();
            return SuperString.notNullTrim(sb.toString());
        } catch(Exception e) {
            e.printStackTrace();
            return "";
        }
    }

    public static String getURLText(String _url) {
        try {
            URL url = new URL(_url);
            InputStream is = url.openStream();
            BufferedReader reader = new BufferedReader(new InputStreamReader(is));
            String content = "";
            String inLine = "";
            while((inLine = reader.readLine()) != null) {
                content += (content.length() > 0 ? "\n" : "") + inLine;
            }
            reader.close();
            is.close();
            return content;
        } catch(Exception e) {
            e.printStackTrace();
            return "";
        }
    }


    public static String dealCodeIDs(String ids) {
        if(ids == null) return "";
        while(ids.startsWith(",")) ids = ids.substring(1);
        while(ids.endsWith(",")) ids = ids.substring(0, ids.length() - 1);
        return "";
    }

    public static boolean existInStrs(String strs, String str) {
        return SuperString.strToList(strs).contains(str);
    }

    public static String addToStrs(String strs, String str) {
        List<String> list = SuperString.strToList(strs);
        if(!list.contains(str)) list.add(str);
        return SuperString.listToStr(list);
    }

    public static String removeFromStrs(String strs, String str) {
        List<String> list = SuperString.strToList(strs);
        list.remove(str);
        return SuperString.listToStr(list);
    }

    public static String getAreaCode(String mobile) {
        if(mobile == null || mobile.length() == 0 ||
                mobile.length() < 10 || !mobile.startsWith("0")) return "";
        if(mobile.startsWith("01") || mobile.startsWith("02")) {
            return mobile.substring(0, 3);
        } else {
            return mobile.substring(0, 4);
        }
    }

    public static SuperDate getSuperDate(int imonth) {
        String mstr = String.valueOf(imonth);
        if(mstr.length() != 6) return null;
        return new SuperDate(mstr.substring(0, 4) + "-" + mstr.substring(4, 6) + "-01 00:00:00");
    }

    public static int getPrevMonth(int imonth) {
        SuperDate sdtemp = getSuperDate(imonth);
        if(sdtemp == null) return 0;
        sdtemp.add(Calendar.MONTH, -1);
        return SuperString.getInt(sdtemp.getYearMonthValue());
    }

    public static int getNextMonth(int imonth) {
        SuperDate sdtemp = getSuperDate(imonth);
        if(sdtemp == null) return 0;
        sdtemp.add(Calendar.MONTH, 1);
        return SuperString.getInt(sdtemp.getYearMonthValue());
    }

    public static String getTableSuffix(HttpServletRequest request) {
        String datamonth = SuperString.notNull(request.getParameter("datamonth"));
        if(datamonth.length() == 0) return "";
        return "_" + String.valueOf(datamonth);
    }

    public static String generateProgramOptions() {
        StringBuffer sb = new StringBuffer();
        return sb.toString();
    }


    public static String getParamsStrs(ArrayList params) {
        if(params == null || params.size() <= 0) return "";
        String r = "";
        for(int i = 0; i < params.size(); i++) {
            Object param = params.get(i);
            r += String.valueOf(i) + "=" + param.toString() + "|";
        }
        return r;
    }

    public static String dealSQLText(String sql, ArrayList params) {
        if(sql == null || sql.length() == 0) return "";
        int index = 0;
        while(sql.indexOf("?") >= 0) {
            sql = SuperString.replaceOnce(sql, "?", "${" + String.valueOf(index) + "}");
            index++;
        }
        if(params != null) {
            for(int i = 0; i < params.size(); i++) {
                Object param = params.get(i);
                String ovalue = "";
                if(param instanceof Integer) {
                    ovalue = String.valueOf(((Integer) param).intValue());
                } else if(param instanceof String) {
                    ovalue = "'" + dealSQLStringChar((String) param) + "'";
                } else if(param instanceof Double) {
                    ovalue = String.valueOf(((Double) param).doubleValue());
                } else if(param instanceof Float) {
                    ovalue = String.valueOf(((Float) param).floatValue());
                } else if(param instanceof Long) {
                    ovalue = String.valueOf(((Long) param).longValue());
                } else if(param instanceof Boolean) {
                    ovalue = String.valueOf(((Boolean) param).booleanValue());
                } else if(param instanceof Date) {
                    ovalue = "'" + new SuperDate((Date) param).getDateTimeString() + "'";
                } else {
                    ovalue = "'" + dealSQLStringChar(SuperString.notNull(param.toString())) + "'";
                }
                ovalue = SuperString.replace(ovalue, "?", "\\?");
                sql = SuperString.replace(sql, "${" + String.valueOf(i) + "}", ovalue);
            }
        }
        return sql;
    }

    public static String getOrderByImgHTML(String order, String cur_order_by) {
        String imgHTML = "<img src=[src] width=10 height=11 border=0 align=absmiddle>";
        return cur_order_by.startsWith(order) ? SuperString.replace(imgHTML, "[src]", "images/comm/" +
                (cur_order_by.endsWith("desc") ? "desc.gif" : "asc.gif")) :
                "";
    }

    public static String getOrderUrl(String order, String cur_order_by, HttpServletRequest request) {
        String orderby = cur_order_by.startsWith(order) || (cur_order_by.length() > 0 && order.startsWith(cur_order_by)) ?
                (cur_order_by.endsWith("desc") || order.endsWith(" desc") ?
                        (order.endsWith(" desc") && cur_order_by.endsWith(" desc") ? order.substring(0, order.length() - " desc".length()) : order) : order + " desc") :
                (order.endsWith(" 2desc") ? order.substring(0, order.length() - " desc".length()) : order);
        return request.getRequestURI() + "?order=" + orderby + SuperPage.getQueryAnd(request, "order");
    }

    public static String dealSQLStringChar(String s) {
        s = SuperString.replace(s, "'", "\\'");
        return s;
    }

    public static String getActText(String act) {
        if(act.equalsIgnoreCase("add")) return "增加";
        if(act.equalsIgnoreCase("mod")) return "修改";
        if(act.equalsIgnoreCase("del")) return "删除";
        return "";
    }


    public static String dealScore(float score, String class1, String class2) {
        StringBuffer sb = new StringBuffer();
        String _score = SuperNumber.format(score, "0.1");
        sb.append("<span class=").append(class1).append(">")
                .append(_score.substring(0, _score.indexOf(".")))
                .append("</span><span class=").append(class2).append(">")
                .append(_score.substring(_score.indexOf(".")))
                .append("</span>");
        return sb.toString();
    }


    public static String dealAnonymousIP(String ip) {
        if(ip == null || "".equals(ip)) return "";
        String[] aip = ip.split("\\.");
        StringBuffer sb = new StringBuffer();
        for(int i = 0; i < aip.length; i++) {
            sb.append(aip[i]).append(".");
            if(i >= 1) {
                sb.append("*");
                break;
            }
        }
        return sb.toString();
    }


    public static String getIpAddr(HttpServletRequest request) {
        String ip = request.getHeader("x-forwarded-for");
        if(ip == null || ip.length() == 0) {
            ip = request.getRemoteAddr();
        }
        if(ip != null && ip.indexOf(",") > 0) {
            ip = ip.substring(0, ip.indexOf(","));
        }
        return ip;
    }

    public static String getIPRootPath(HttpServletRequest request) {
        String rootpath = request.getScheme() + "://" + request.getServerName()
                + ":" + request.getServerPort() + request.getContextPath();
        return rootpath;
    }

    public static String getRootPath(HttpServletRequest request) {
        /*String rootpath = request.getScheme() + "://" + request.getServerName()
			+ ":" + request.getServerPort()+request.getContextPath();*/
        return getPropertiesPath("VIP_WEB_URL", true);
    }


    public static String getWhereSQL(StringBuffer sbw) {
        String strwhere = SuperString.notNull(sbw.toString());
        while(strwhere.startsWith(" ")) strwhere = strwhere.substring(1);
        if(strwhere.startsWith("and ")) strwhere = strwhere.substring("and ".length());
        if(strwhere.length() > 0) strwhere = " where " + strwhere;
        return strwhere;
    }


    public static boolean isCompletedHTML(String str) {
        if(str == null || "".equals(str)) return false;
        int b = str.indexOf("<!--{header}//-->");
        int e = str.indexOf("<!--{footer}//-->");
        return b > 0 && e > 0 && b < e;
    }

    public static String getHiddenCharString(String str, String fillchar,
                                             boolean fromEnd, int offset, int len) {
        if(str == null) str = "";
        if(str.length() < offset) return str;
        if(fromEnd && len > offset) len = offset;
        if(!fromEnd && offset + len > str.length()) len = str.length() - offset;
        StringBuffer sb = new StringBuffer();
        for(int i = 0; i < len; i++) sb.append(fillchar);
        if(fromEnd) return str.substring(0, str.length() - offset) + sb.toString() +
                str.substring(str.length() - offset + len);
        else return str.substring(0, offset) + sb.toString() + str.substring(offset + len);
    }

    public static String getProperties(String key) {
        return PropUtil.getInstance("constant").getProperty(key);
    }

    public static int getPropertiesInt(String key) {
        return SuperString.getInt(PropUtil.getInstance("constant").getProperty(key));
    }

    public static int getPropertiesInt(String key, int iDefValue) {
        return SuperString.getInt(PropUtil.getInstance("constant").getProperty(key), iDefValue);
    }

    public static boolean getPropertiesBoolean(String key) {
        return Boolean.valueOf(PropUtil.getInstance("constant").getProperty(key));
    }

    public static String getPropertiesPath(String key, boolean incsuffix) {
        String p = SuperString.notNullTrim(getProperties(key));
        if(p.length() > 0 && incsuffix && !p.endsWith("/")) p += "/";
        return p;
    }


    public static String convertQuotText(String t) {
        if(t == null || "".equals(t)) return "";
        String strbegin = SuperString.notNull(Util.getProperties("COMMENT_QUOT_BEGINSTR"));
        String strend = SuperString.notNull(Util.getProperties("COMMENT_QUOT_ENDSTR"));
        String repbegin = SuperString.notNull(Util.getProperties("COMMENT_QUOT_REP_BEGINSTR"));
        String repend = SuperString.notNull(Util.getProperties("COMMENT_QUOT_REP_ENDSTR"));
        if("".equals(strbegin)) strbegin = "[quot=";
        if("".equals(strend)) strend = "[/quot]";
        int lastpos = t.length();
        while(t.lastIndexOf(strbegin, lastpos) >= 0) {
            lastpos = t.lastIndexOf(strbegin, lastpos);
            String tpart1 = t.substring(0, lastpos);
            String tpart2 = t.substring(lastpos);
            int curendpos = tpart2.indexOf(strend);
            String tnew = tpart2;
            if(curendpos > strbegin.length()) {//找到对应的end，需要替换
                tnew = repbegin + tpart2.substring(strbegin.length(), curendpos) + repend;
                if(tnew.indexOf(":]") > 0) {
                    tnew = SuperString.replace(tnew, ":]", tnew.indexOf(":]" + repbegin) < 0 ? ":<br/>" : ":");
                }
                tnew += tpart2.substring(curendpos + strend.length());
                tnew = SuperString.replace(tnew, repend + "<br>", repend);
            }
            t = tpart1 + tnew;
            lastpos--;
        }
        return t;
    }


    public static void reloadDataForOtherWeb(String type) {
        List<String> list = SuperString.strToList(SuperString.notNullTrim(getProperties("WEB_APPS")));
        String reload_url = SuperString.notNullTrim(getProperties("DATA_RELOAD_URL"));
        for(String ip : list) {
            if(ip != null && !"".equals(ip)) {
                String url = "http://" + ip + "/" + reload_url + "?type=" + type + "&rnd=" + new Date().getTime();
                String ret = openURL(url);
                System.out.println("### open url : " + url + "==>" + ret);
            }
        }

    }

    /*public static String getTableSplit(long cid){
        try {
            String str=MD5Util.encode(String.valueOf(cid)).toLowerCase().trim();
            if("".equals(str))return "0";
            return str.substring(str.length()-1);
        } catch (Exception e) {
            return "0";
        }
    }*/

    public static String getTableSplit(long cid) {
        return String.valueOf(Math.abs(cid) % 100);
    }

    public static void main(String[] args) {
        /*Map<String,Long> map=new HashMap<String,Long>();
        List<String> list=new ArrayList<String>();
        for(int i = 10000; i < 100000; i++) {
            String str=getTableSplit(i);
            str=str.substring(str.length()-1);
            long count=map.containsKey(str)?map.get(str):0;
            count++;
            map.put(str, count);
            if(!list.contains(str)) list.add(str);
        }

        for(String str:list){
            System.out.println(str+" | "+map.get(str));
        }*/
        /*String[] arr = "0,1,2,3,4,5,6,7,8,9,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z".split(",");*/
        for(int i = 0; i < 100 ; i++) {
            System.out.println("CREATE TABLE `ppt_comment_" + i + "` (\n" +
                    "  `ID` bigint(21) NOT NULL auto_increment,\n" +
                    "  `RootID` bigint(21) NOT NULL default '0',\n" +
                    "  `BKType` tinyint(4) NOT NULL default '0',\n" +
                    "  `BKID` bigint(21) default '0',\n" +
                    "  `CatID` int(11) NOT NULL default '0',\n" +
                    "  `ChannelID` bigint(21) NOT NULL default '0',\n" +
                    "  `ChannelName` varchar(50) default NULL,\n" +
                    "  `fb` varchar(30) default NULL,\n" +
                    "  `UserType` int(2) NOT NULL default '0',\n" +
                    "  `UserID` bigint(21) NOT NULL default '0',\n" +
                    "  `UserName` varchar(30) default NULL,\n" +
                    "  `IsVIP` tinyint(4) NOT NULL default '0',\n" +
                    "  `Content` varchar(500) default NULL,\n" +
                    "  `UpCount` int(11) NOT NULL default '0',\n" +
                    "  `Status` int(2) NOT NULL default '0',\n" +
                    "  `BsjStatus` int(2) NOT NULL default '-1',\n" +
                    "  `UserIP` varchar(20) default NULL,\n" +
                    "  `UserArea` varchar(20) default NULL,\n" +
                    "  `CreateTime` datetime NOT NULL,\n" +
                    "  PRIMARY KEY  (`ID`),\n" +
                    "  KEY `Index_1` (`ChannelID`,`Status`),\n" +
                    "  KEY `Index_2` (`UserID`,`Status`),\n" +
                    "  KEY `Status` (`Status`),\n" +
                    "  KEY `RootID` (`RootID`)\n" +
                    ") ENGINE=InnoDB DEFAULT CHARSET=utf8;\n\n\n");
        }

    }

}
