package tool;

import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.Date;
import java.util.Enumeration;
import java.util.Random;
import java.io.IOException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.struts.util.RequestUtils;

/**
 * Created by IntelliJ IDEA.
 * User: Xu Jianbo sword20@263.net
 * Date: 2004-6-25
 * Time: 15:22:17
 * Shanghai 2004
 */
public class SuperPage {
    public SuperPage() {

    }

    public static String getCurURL(HttpServletRequest request, String hoststr) {
        return URLEncode(hoststr + request.getRequestURI() + getQueryString(request), "utf-8");
    }

    public static String getCurURL(HttpServletRequest request) {
        return URLEncode(request.getRequestURI() + getQueryString(request), "utf-8");
    }

    public static String getQueryString(HttpServletRequest request) {
        String q = request.getQueryString();
        return q == null ? "" : "?" + q;
    }

    public static String getRequestQueryString(HttpServletRequest request, String droppara) {
        return getRequestQueryString(request, droppara, false);
    }

    public static String getRequestQueryString(HttpServletRequest request, String droppara, boolean urlEncode) {
        return getRequestQueryString(request, droppara, urlEncode, null, false);
    }

    public static String getRequestQueryString(HttpServletRequest request,
                                               String droppara,
                                               boolean urlEncode,
                                               String urlEncodeCharSet,
                                               boolean dropEmptyParam) {
        String[] aquery = SuperString.notNull(request.getQueryString()).split("&");
        if(aquery == null) return "";
        StringBuffer sb = new StringBuffer();
        for(String query : aquery) {
            if(query == null || "".equals(query.trim())) continue;
            query = query.trim();
            int _pos = query.indexOf("=");
            if(_pos < 0 && dropEmptyParam) continue;
            String _name = query;
            String _value = "";
            if(_pos > 0) {
                _name = query.substring(0, _pos);
                _value = query.substring(_pos + 1);
            }
            if(droppara != null && ("," + droppara + ",").indexOf("," + _name + ",") > -1) continue;
            if("".equals(_value) && dropEmptyParam) continue;
            if(!"".equals(sb.toString())) sb.append("&");
            sb.append(_name);
            if(_pos > 0) sb.append("=");
            sb.append(urlEncode ? URLEncode(_value, urlEncodeCharSet) : _value);
        }
        return sb.toString();
    }

    public static String getQueryQ(HttpServletRequest request, String droppara) {
        String r = getRequestQueryString(request, droppara);
        return (r.length() > 0) ? "?" + r : "";
    }

    public static String getQueryAnd(HttpServletRequest request, String droppara) {
        String r = getRequestQueryString(request, droppara);
        return (r.length() > 0) ? "&" + r : "";
    }

    public static String getQueryParamQ(HttpServletRequest request, String droppara) {
        String r = getRequestQueryString(request, droppara, true);
        return (r.length() > 0) ? "?" + r : "";
    }

    public static String getQueryParamAnd(HttpServletRequest request, String droppara) {
        String r = getRequestQueryString(request, droppara, true);
        return (r.length() > 0) ? "&" + r : "";
    }

    public static String getRequestParamString(HttpServletRequest request, String droppara, String urlencode) {
        Enumeration names = request.getParameterNames();
        droppara += ",";
        String rstr = "";
        try {
            while(names.hasMoreElements()) {
                String name = (String) names.nextElement();
                if(name != null && name.length() > 0 && droppara.indexOf(name + ",") < 0) {
                    rstr += (rstr.length() > 0 ? "&" : "") +
                            name + "=" + (urlencode.length() == 0 ?
                            SuperString.notNull(request.getParameter(name)) :
                            URLEncoder.encode(SuperString.notNull(request.getParameter(name)), urlencode));
                }
            }
        } catch(Exception e) {
        }
        return rstr;
    }

    public static String getParamQ(HttpServletRequest request, String droppara) {
        String r = getRequestParamString(request, droppara, "");
        return (r.length() > 0) ? "?" + r : "";
    }

    public static String getParamAnd(HttpServletRequest request, String droppara) {
        String r = getRequestParamString(request, droppara, "");
        return (r.length() > 0) ? "&" + r : "";
    }

    public static String getParamQ(HttpServletRequest request, String droppara, String urlencode) {
        String r = getRequestParamString(request, droppara, urlencode);
        return (r.length() > 0) ? "?" + r : "";
    }

    public static String getParamAnd(HttpServletRequest request, String droppara, String urlencode) {
        String r = getRequestParamString(request, droppara, urlencode);
        return (r.length() > 0) ? "&" + r : "";
    }


    public static int[] getPageRecordsNumber(int recordsCount, HttpServletRequest request, String stripage, int perPageCount) {
        int[] rint = {0, 0, 0};
        int ipage = 1;
        if(SuperString.notNull(stripage).equals(""))
            stripage = SuperString.notNull(request.getParameter("ipage"));
        if(stripage.equals("")) stripage = "1";
        try {
            ipage = Integer.parseInt(stripage);
        }
        catch(Exception e) {
            ipage = 1;
        }

        if(recordsCount <= 0) {
            rint[2] = -1;
            return rint;
        }
        int pageCount = (int) Math.ceil((double) recordsCount / perPageCount);
        if(ipage <= 0) ipage = 1;//将ipage保证在合理范围
        if(ipage > pageCount) ipage = pageCount;
        if(perPageCount < recordsCount) rint[0] = 1;
        rint[1] = (ipage - 1) * perPageCount;
        rint[2] = ipage * perPageCount - 1;
        if(rint[1] < 0) rint[1] = 0;
        if(rint[2] < 0) rint[2] = 0;
        if(rint[1] > (recordsCount - 1)) rint[1] = (recordsCount - 1);
        if(rint[2] > (recordsCount - 1)) rint[2] = (recordsCount - 1);
        return rint;
    }

    public static String generatePageNavSingle(int recordsCount, HttpServletRequest request, String stripage, int perPageCount, String unitText) {
        int pageCount = (int) Math.ceil((double) recordsCount / perPageCount);
        if(pageCount < 1) return "";

        String rstr = "";//返回值

        int ipage = 1;
        if(SuperString.notNull(stripage).equals(""))
            stripage = SuperString.notNull(request.getParameter("ipage"));
        if(stripage.equals("")) stripage = "1";
        try {
            ipage = Integer.parseInt(stripage);
        }
        catch(Exception e) {
            ipage = 1;
        }
        if(ipage <= 0) ipage = 1;//将ipage保证在合理范围
        if(ipage > pageCount) ipage = pageCount;
        int iPageWidth = 11;//每页显示的数字页数
        String strInputName = "inputpage_" + new Date().getTime();
        Random rnd = new Random();
        strInputName += rnd.nextInt(100);
        strInputName += rnd.nextInt(100);
        strInputName += rnd.nextInt(100);

        //得到QueryString，ipage去掉
        String strQueryPara = getRequestQueryString(request, "ipage");
        String requestURL = request.getRequestURI();
        strQueryPara = strQueryPara.equals("") ? strQueryPara : "&" + strQueryPara;

        //几个图片的地址及名称
        String imgfirst = "<a href=\"" + requestURL + "?ipage=1" + strQueryPara + "\" title=\"首页\">" +
                "首页</a>";
        String imgpre = "<a href=\"" + requestURL + "?ipage=" + (ipage - 1) + strQueryPara + "\" title=\"前一页\">" +
                "前一页</a>";
        String imgnext = "<a href=\"" + requestURL + "?ipage=" + (ipage + 1) + strQueryPara + "\" title=\"后一页\">" +
                "后一页</a>";
        String imglast = "<a href=\"" + requestURL + "?ipage=" + pageCount + strQueryPara + "\" title=\"尾页\">" +
                "尾页</a>";
        if(ipage <= 1) {
            imgfirst = "<span>首页</span>";
            imgpre = "<span>前一页</span>";
        }
        if(ipage >= pageCount) {
            imgnext = "<span>后一页</span>";
            imglast = "<span>尾页</span>";
        }

        //中间部分导航页码的生成
        int pageBegin = ipage - iPageWidth / 2;
        int pageEnd = ipage + iPageWidth / 2;
        if(pageEnd < iPageWidth) pageEnd = iPageWidth;
        if(pageEnd > pageCount) {
            pageBegin = pageBegin - (pageEnd - pageCount);
            pageEnd = pageCount;
        }
        if(pageBegin < 1) pageBegin = 1;
        String strPageNumNav = "";
        for(int i = pageBegin; i <= pageEnd; i++) {
            if(i != pageBegin) strPageNumNav += " ";
            if(i == ipage)
                strPageNumNav += "[<b>" + i + "</b>]";
            else
                strPageNumNav += "<a href=\"" + requestURL + "?ipage=" + i +
                        strQueryPara + "\">" + i + "</a>";
        }
        
        
        String filename=request.getRequestURI();
        //int last=filename.lastIndexOf("/");
        //filename=filename.substring(last+1);
        //生成最后的东西
        rstr += "<span class=ipagenav>"+ imgfirst + "&nbsp;" + imgpre + "&nbsp;&nbsp;" +
                strPageNumNav +
                "&nbsp;&nbsp;" +
                imgnext + "&nbsp;" + imglast +"&nbsp;&nbsp;&nbsp;"+"跳到第<input type='text' id='thepage' size=3 onkeydown='if(event.keyCode==13){self.open(\""+filename+"?ipage=\""+"+document.getElementById(\"thepage\").value+"+"\""+SuperPage.getParamAnd(request,"ipage")+"\",\"_self\");}'>页"+"&nbsp;&nbsp;&nbsp;" +
                "共" + recordsCount + unitText + "" + pageCount + "页" +
                "</span>";
        return rstr;
    }


    public static String generatePageNav(int recordsCount, HttpServletRequest request, String stripage, int perPageCount) {
        int pageCount = (int) Math.ceil((double) recordsCount / perPageCount);
        if(pageCount <= 1) return "";

        String rstr = "";//返回值

        int ipage = 1;
        if(SuperString.notNull(stripage).equals(""))
            stripage = SuperString.notNull(request.getParameter("ipage"));
        if(stripage.equals("")) stripage = "1";
        try {
            ipage = Integer.parseInt(stripage);
        }
        catch(Exception e) {
            ipage = 1;
        }
        if(ipage <= 0) ipage = 1;//将ipage保证在合理范围
        if(ipage > pageCount) ipage = pageCount;
        int iPageWidth = 11;//没页显示的数字页数
        String strInputName = "inputpage_" + new Date().getTime();
        Random rnd = new Random();
        strInputName += rnd.nextInt(100);
        strInputName += rnd.nextInt(100);
        strInputName += rnd.nextInt(100);

        //得到QueryString，ipage去掉
        String strQueryPara = getRequestQueryString(request, "ipage");
        String requestURL = request.getRequestURI();
        strQueryPara = strQueryPara.equals("") ? strQueryPara : "&" + strQueryPara;

        //几个图片的地址及名称
        /*
                String imgfirst="<a href=\""+requestURL+"?ipage=1"+strQueryPara+"\">" +
                        "<img src=\"/images/album/pagefirst.gif\" border=0 width=\"14\" height=\"10\"></a>";
                String imgpre="<a href=\""+requestURL+"?ipage="+(ipage-1)+strQueryPara+"\">" +
                        "<img src=\"/images/album/pagepre.gif\" border=0 width=\"14\" height=\"10\"></a>";
                String imgnext="<a href=\""+requestURL+"?ipage="+(ipage+1)+strQueryPara+"\">" +
                        "<img src=\"/images/album/pagenext.gif\" border=0 width=\"14\" height=\"10\"></a>";
                String imglast="<a href=\""+requestURL+"?ipage="+pageCount+strQueryPara+"\">" +
                        "<img src=\"/images/album/pagelast.gif\" border=0 width=\"14\" height=\"10\"></a>";
                if(ipage<=1){
                    imgfirst="<img src=\"/images/album/pagefirstgray.gif\" width=\"14\" height=\"10\">";
                    imgpre="<img src=\"/images/album/pagepregray.gif\" width=\"14\" height=\"10\">";
                }
                if(ipage>=pageCount){
                    imgnext="<img src=\"/images/album/pagenextgray.gif\" width=\"14\" height=\"10\">";
                    imglast="<img src=\"/images/album/pagelastgray.gif\" width=\"14\" height=\"10\">";
                }
        */
        String imgfirst = "<a href=\"" + requestURL + "?ipage=1" + strQueryPara + "\" class=link_pagearrow title=\"首页\">" +
                "9</a>";
        String imgpre = "<a href=\"" + requestURL + "?ipage=" + (ipage - 1) + strQueryPara + "\" class=link_pagearrow title=\"前一页\">" +
                "7</a>";
        String imgnext = "<a href=\"" + requestURL + "?ipage=" + (ipage + 1) + strQueryPara + "\" class=link_pagearrow title=\"后一页\">" +
                "8</a>";
        String imglast = "<a href=\"" + requestURL + "?ipage=" + pageCount + strQueryPara + "\" class=link_pagearrow title=\"尾页\">" +
                ":</a>";
        if(ipage <= 1) {
            imgfirst = "<span class=pagearrow>9</span>";
            imgpre = "<span class=pagearrow>7</span>";
        }
        if(ipage >= pageCount) {
            imgnext = "<span class=pagearrow>8</span>";
            imglast = "<span class=pagearrow>:</span>";
        }

        //中间部分导航页码的生成
        int pageBegin = ipage - iPageWidth / 2;
        int pageEnd = ipage + iPageWidth / 2;
        if(pageEnd < iPageWidth) pageEnd = iPageWidth;
        if(pageEnd > pageCount) {
            pageBegin = pageBegin - (pageEnd - pageCount);
            pageEnd = pageCount;
        }
        if(pageBegin < 1) pageBegin = 1;
        String strPageNumNav = "";
        for(int i = pageBegin; i <= pageEnd; i++) {
            if(i != pageBegin) strPageNumNav += " ";
            if(i == ipage)
                strPageNumNav += "<font color=\"#FF0000\">" + i + "</font>";
            else
                strPageNumNav += "<a href=\"" + requestURL + "?ipage=" + i +
                        strQueryPara + "\" class=\"link_pagenum\">" + i + "</a>";
        }

        //生成最后的东西
        rstr += "<SCRIPT LANGUAGE=javascript>\n" +
                "<!--\n" +
                "function checkpagenum(urlfile,urlpara,inputname){\n" +
                "    //alert(event.keyCode);\n" +
                "    if(event.keyCode==13) gopage(urlfile,urlpara,inputname);\n" +
                "    return true;\n" +
                "    var strValidKey=new String(\"[8][27][37][39]\");\n" +
                "    if(strValidKey.indexOf(\"[\"+event.keyCode+\"]\",0)>=0) return true;\n" +
                "    else if((event.keyCode<48)||((event.keyCode>57)&&(event.keyCode<96))||" +
                "(event.keyCode>105)) return false;\n" +
                "    else return true;\n" +
                "}\n" +
                "\n" +
                "function gopage(urlfile,urlpara,inputname){\n" +
                "    var objpage=document.all(inputname);\n" +
                "    if(objpage!=null){\n" +
                "        var ipage=objpage.value;\n" +
                "        if((ipage!='')&&!isNaN(ipage)){\n" +
                "            var gourl=urlfile+\"?ipage=\"+ipage;\n" +
                "            if((urlpara!=null)&&urlpara!='') gourl+=urlpara;\n" +
                "            location.href=gourl;\n" +
                "        }\n" +
                "    }\n" +
                "}\n" +
                "//-->\n" +
                "</SCRIPT>";
        rstr += "<table width=\"100\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">" +
                "<tr>" +
                "<td width=\"50\" class=\"pagenaven\" nowrap>" +
                "<font color=\"#336699\">&nbsp;Total:<b>" +
                recordsCount +
                "</b>&nbsp;&nbsp;" +
                "<font color=\"#336699\">Pages:</font><b>" +
                ipage +
                "</b>/<b>" +
                pageCount +
                "</b></font>&nbsp;&nbsp;&nbsp;&nbsp;" +
                "</td>" +
                "<td width=\"35\" class=\"pagenaven\" align=center nowrap>" +
                "" +
                imgfirst + "&nbsp;" + imgpre +
                "</td>" +
                "<td width=\"1%\" class=\"pagenumnaven\" align=center nowrap>" +
                strPageNumNav +
                "</td>" +
                "<td width=\"35\" class=\"pagenaven\" align=center nowrap>" +
                imgnext + "&nbsp;" + imglast +
                "</td>" +
                "<td width=\"30\" nowrap>&nbsp;</td>" +
                "<td width=\"1\" nowrap>" +
                "<input name=\"" + strInputName + "\" id=\"" + strInputName + "\" type=\"text\" " +
                "size=\"3\" maxlength=\"3\"" +
                " class=\"pageinput\" " +
                " onkeydown=\"return checkpagenum('" + requestURL + "','" + strQueryPara + "','" + strInputName + "');\">" +
                "</td>" +
                "<td width=\"1\" nowrap>" +
                "<a href=\"javascript:gopage('" + requestURL + "','" + strQueryPara + "','" + strInputName + "');\" " +
                "onmousemove=\"setStatus('直接跳转到指定页');\" onmouseout=\"setStatus('');\" " +
                "class=\"link_pagego\">GO</a>" +
                "</td>" +
                "<td width=\"95%\">&nbsp;</td>" +
                "</tr>" +
                "</table>";
        return rstr;
    }


    public static String generatePageNavForIkan2(int recordsCount,
                                         HttpServletRequest request,
                                         HttpServletResponse response,
                                         int perPageCount,
                                         String unitText,
                                         boolean alwaysShow) {
        int pageCount = (int) Math.ceil((double) recordsCount / perPageCount);
        if(pageCount < 1) return "";
        if(pageCount == 1 && !alwaysShow) return "";

        StringBuffer sb = new StringBuffer();
        sb.append("<div class=\"vpager_a\">\r\n");
        sb.append("<ul>\r\n");

        int iPageBorder = 2;//边缘页数
        int iPageMiddle = 5;//中间叶数
        String[] arrPageText = {"首页", "前一页", "后一页", "尾页"};

        int ipage = SuperString.getInt(request.getParameter("ipage"));
        if(ipage <= 0) ipage = 1;//将ipage保证在合理范围
        if(ipage > pageCount) ipage = pageCount;
        int iPageWidth = iPageBorder * 2 + iPageMiddle;//每页显示的数字页数

        //得到QueryString，ipage去掉
        String strQueryPara = getRequestQueryString(request, "ipage", false, null, true);
        String requestURL = request.getRequestURI();
        strQueryPara = "".equals(strQueryPara) ? "?" : "?" + strQueryPara + "&";

        /*
                String imgfirst = "<kbd" +
                        (ipage <= 1 ? " class=disable" : "") +
                        "><a href=\"" +
                        (ipage <= 1 ? "#1" : response.encodeURL(requestURL + strQueryPara + "ipage=1")) +
                        "\">" + arrPageText[0] + "</a></kbd>\r\n";
        */
        String imgpre = "<li><a href=\"" +
                (ipage <= 1 ? "javascript:void(null);\" class=\"disable\"" :
                        response.encodeURL(requestURL + strQueryPara + "ipage=" + (ipage - 1)) +
                                "\"") +
                ">" + arrPageText[1] + "</a></li>\r\n";
        String imgnext = "<li><a href=\"" +
                (ipage >= pageCount ? "javascript:void(null);\" class=\"disable\"" :
                        response.encodeURL(requestURL + strQueryPara + "ipage=" + (ipage + 1)) +
                                "\"") +
                ">" + arrPageText[2] + "</a></li>\r\n";
        /*
                String imglast = "<a href=\"" +
                        (ipage <= 1 ? "javascript:void(null);\" class=\"bx-01 dis\"" :
                                response.encodeURL(requestURL + strQueryPara + "ipage=" + pageCount) +
                                "\" class=\"bx-01\"")+
                        ">" + arrPageText[3] + "</a>\r\n";
        */

        //中间部分导航页码的生成
        int iMaxBorderLeft = Math.min(pageCount, iPageBorder);
        int iMinBorderRight = Math.max(1, pageCount - iPageBorder + 1);
        if(iMinBorderRight <= iMaxBorderLeft) iMinBorderRight = Math.min(pageCount, iMaxBorderLeft + 1);
        if(iMaxBorderLeft == pageCount) iMinBorderRight = pageCount + 1;
        int iMiddleBegin = ipage - iPageMiddle / 2;
        if(iMiddleBegin <= iMaxBorderLeft) iMiddleBegin = iMaxBorderLeft + 1;
        int iMiddleEnd = iMiddleBegin + iPageMiddle - 1;
        if(iMiddleEnd >= iMinBorderRight) iMiddleEnd = iMinBorderRight - 1;
        if(iMiddleEnd - iMiddleBegin < iPageMiddle)
            iMiddleBegin = Math.max(iMaxBorderLeft + 1, iMiddleEnd - iPageMiddle + 1);

        if(iMiddleBegin > iMiddleEnd) {
            iMiddleBegin = 0;
            iMiddleEnd = 0;
        }

        if(ipage > 0) {
            //sb.append(imgfirst);
            sb.append(imgpre);
        }

        for(int i = 1; i <= iMaxBorderLeft; i++) {
            sb.append("<li><a href=\"");
            if(i == ipage) {
                sb.append("javascript:void(null);\" class=\"active\"");
            } else {
                sb.append(response.encodeURL(requestURL + strQueryPara + "ipage=" + i));
                sb.append("\"");
            }
            sb.append(">");
            sb.append(String.valueOf(i));
            sb.append("</a></li>\r\n");
            if(i == iMaxBorderLeft && i < iMiddleBegin - 1) {
                sb.append("<li class=\"ellipsis\">...</li>\r\n");
            }
        }
        if(iMiddleBegin > 0) {
            for(int i = iMiddleBegin; i <= iMiddleEnd; i++) {
                sb.append("<li><a href=\"");
                if(i == ipage) {
                    sb.append("javascript:void(null);\" class=\"active\"");
                } else {
                    sb.append(response.encodeURL(requestURL + strQueryPara + "ipage=" + i));
                    sb.append("\"");
                }
                sb.append(">");
                sb.append(String.valueOf(i));
                sb.append("</a></li>\r\n");
            }
        }

        for(int i = iMinBorderRight; i <= pageCount; i++) {
            if(i == iMinBorderRight && iMiddleEnd > 0 && i > iMiddleEnd + 1) {
                sb.append("<li class=\"ellipsis\">...</li>\r\n");
            }
            sb.append("<li><a href=\"");
            if(i == ipage) {
                sb.append("javascript:void(null);\" class=\"active\"");
            } else {
                sb.append(response.encodeURL(requestURL + strQueryPara + "ipage=" + i));
                sb.append("\"");
            }
            sb.append(">");
            sb.append(String.valueOf(i));
            sb.append("</a></li>\r\n");
        }

        if(ipage <= pageCount) {
            sb.append(imgnext);
            //sb.append(imglast);
        }

            sb.append("</ul>\r\n");
        sb.append("</div>");

        return sb.toString();

    }


    /**
     * 返回参数后面的所有值
     */
    public static String getSpePara(String paraname, HttpServletRequest request) {
        if(SuperString.notNull(paraname).equals("")) return "";
        String paravalue = SuperString.notNull(request.getParameter(paraname));
        String strquery = SuperString.notNull(request.getQueryString());
        if(!paravalue.equals("") && !strquery.equals(""))
            return strquery.substring(strquery.indexOf(paraname + "=") + (paraname + "=").length());
        return "";
    }


    public static String generateHintHTML(String hintText, int width, int left, int top) {
        if(width == 0) width = 180;
        if(left == 0) left = 10;
        if(top == 0) top = 10;
        String r = " onMouseOver=\"initLayerHintSize('layerHint'," + width + ",12);" +
                "initHintData('" + hintText + "');showLayer('layerHint'," + left + "," + top + ")\" " +
                "onMouseMove=\"moveLayer('layerHint'," + left + "," + top + ")\" " +
                "onMouseOut=\"hideLayer('layerHint')\" ";
        return r;
    }

    public static String generateLimitHintHTML(int top, int left, int len) {
        return "onMouseOver=\"showLayer('layerInputHint'," + top + "," + left + ");showInputLength(this)\" \n" +
                "onMouseMove=\"moveLayer('layerInputHint'," + top + "," + left + ")\" \n" +
                "onMouseOut=\"hideLayer('layerInputHint')\" \n" +
                "onKeyUp=\"showLayer('layerInputHint'," + top + "," + left + ");showInputLength(this); \n" +
                "return limitInputLength(this," + len + ")\" \n" +
                "onKeyDown=\"return limitInputLength(this," + len + ")\" ";
    }

    /**
     * 返回path（目录或文件）的HTTP完整路径，包括主机名
     */
    public static String getHostURL(HttpServletRequest request, String path) {
        try {
            return RequestUtils.absoluteURL(request, path).toString();
        } catch(Exception e) {
            e.printStackTrace();
            return "";
        }
    }

    public static String getHostURL(HttpServletRequest request) {
        return getHostURL(request, "/");
    }

    /**
     * 返回表单验证的脚本
     */
    public static String getOnSubmitCode(String checkStr, String userfun) {
        if(SuperString.isBlank(checkStr)) return "";
        String[] arrStr = checkStr.split(";");
        String rstr = "";
        for(int i = 0; i < arrStr.length; i++) {
            String unitstr = arrStr[i];
            String[] arrunitstr = unitstr.split(",");
            if(arrunitstr.length == 2) {
                rstr += (rstr.length() == 0 ? "" : ",") + "'" + arrunitstr[0] + "','','" + arrunitstr[1] + "'";
            }
        }
        if(rstr.length() > 0) {
            userfun = SuperString.notNullTrim(userfun);
            if(userfun.length() > 0 && !userfun.endsWith(";")) userfun += ";";
            rstr = "onSubmit=\"MM_validateForm(" + rstr + ");" + userfun + "return document.MM_returnValue;\"";
        }
        return rstr;
    }

    public static String URLEncode(String url) {
        return URLEncode(url, "gbk");
    }

    public static String URLEncode(String url, String charset) {
        if(charset == null) return URLDecode(url);
        try {
            return URLEncoder.encode(url, charset);
        } catch(Exception e) {
            e.printStackTrace();
            return "";
        }
    }

    public static String URLDecode(String url) {
        return URLDecode(url, "gbk");
    }

    public static String URLDecode(String url, String charset) {
        try {
            return URLDecoder.decode(url, charset);
        } catch(Exception e) {
            e.printStackTrace();
            return "";
        }
    }

    public static String getCurURLFile(HttpServletRequest request) {
        String file = request.getRequestURI();
        if(file.lastIndexOf("/") >= 0) file = file.substring(file.lastIndexOf("/") + 1);
        return file;
    }

    public static String generatePageNav(int recordsCount,
                                         HttpServletRequest request,
                                         HttpServletResponse response,
                                         int perPageCount,
                                         String unitText,
                                         boolean alwaysShow) {
        int pageCount = (int) Math.ceil((double) recordsCount / perPageCount);
        if(pageCount < 1) return "";
        if(pageCount == 1 && !alwaysShow) return "";

        StringBuffer sb = new StringBuffer();
        sb.append("<dd id=vpager>\r\n");

        int iPageBorder = 2;//边缘页数
        int iPageMiddle = 5;//中间叶数
        String[] arrPageText = {"首页", "前一页", "后一页", "尾页"};

        int ipage = SuperString.getInt(request.getParameter("ipage"));
        if(ipage <= 0) ipage = 1;//将ipage保证在合理范围
        if(ipage > pageCount) ipage = pageCount;
        int iPageWidth = iPageBorder * 2 + iPageMiddle;//每页显示的数字页数

        //得到QueryString，ipage去掉
        String strQueryPara = getRequestQueryString(request, "ipage", false, null, true);
        String requestURL = request.getRequestURI();
        strQueryPara = "".equals(strQueryPara) ? "?" : "?" + strQueryPara + "&";

        String imgfirst = "<kbd" +
                (ipage <= 1 ? " class=disable" : "") +
                "><a href=\"" +
                (ipage <= 1 ? "#1" : response.encodeURL(requestURL + strQueryPara + "ipage=1")) +
                "\">" + arrPageText[0] + "</a></kbd>\r\n";
        String imgpre = "<kbd" +
                (ipage <= 1 ? " class=disable" : "") +
                "><a href=\"" +
                (ipage <= 1 ? "#1" : response.encodeURL(requestURL + strQueryPara + "ipage=" + (ipage - 1))) +
                "\">" + arrPageText[1] + "</a></kbd>\r\n";
        String imgnext = "<dfn" +
                (ipage >= pageCount ? " class=disable" : "") +
                "><a href=\"" +
                (ipage >= pageCount ? "#1" : response.encodeURL(requestURL + strQueryPara + "ipage=" + (ipage + 1))) +
                "\">" + arrPageText[2] + "</a></dfn>\r\n";
        String imglast = "<dfn" +
                (ipage >= pageCount ? " class=disable" : "") +
                "><a href=\"" +
                (ipage >= pageCount ? "#1" : response.encodeURL(requestURL + strQueryPara + "ipage=" + pageCount)) +
                "\">" + arrPageText[3] + "</a></dfn>\r\n";

        //中间部分导航页码的生成
        int iMaxBorderLeft = Math.min(pageCount, iPageBorder);
        int iMinBorderRight = Math.max(1, pageCount - iPageBorder + 1);
        if(iMinBorderRight <= iMaxBorderLeft) iMinBorderRight = Math.min(pageCount, iMaxBorderLeft + 1);
        if(iMaxBorderLeft == pageCount) iMinBorderRight = pageCount + 1;
        int iMiddleBegin = ipage - iPageMiddle / 2;
        if(iMiddleBegin <= iMaxBorderLeft) iMiddleBegin = iMaxBorderLeft + 1;
        int iMiddleEnd = iMiddleBegin + iPageMiddle - 1;
        if(iMiddleEnd >= iMinBorderRight) iMiddleEnd = iMinBorderRight - 1;
        if(iMiddleEnd - iMiddleBegin < iPageMiddle)
            iMiddleBegin = Math.max(iMaxBorderLeft + 1, iMiddleEnd - iPageMiddle + 1);

        if(iMiddleBegin > iMiddleEnd) {
            iMiddleBegin = 0;
            iMiddleEnd = 0;
        }

        if(ipage > 0) {
            sb.append(imgfirst);
            sb.append(imgpre);
        }

        sb.append("<code>\r\n");

        for(int i = 1; i <= iMaxBorderLeft; i++) {
            sb.append("<a href=\"");
            if(i == ipage) {
                sb.append("#1\" class=active");
            } else {
                sb.append(response.encodeURL(requestURL + strQueryPara + "ipage=" + i));
                sb.append("\"");
            }
            sb.append(">");
            sb.append(String.valueOf(i));
            sb.append("</a>\r\n");
            if(i == iMaxBorderLeft && i < iMiddleBegin - 1) {
                sb.append("<i>...</i>\r\n");
            }
        }
        if(iMiddleBegin > 0) {
            for(int i = iMiddleBegin; i <= iMiddleEnd; i++) {
                sb.append("<a href=\"");
                if(i == ipage) {
                    sb.append("#1\" class=active");
                } else {
                    sb.append(response.encodeURL(requestURL + strQueryPara + "ipage=" + i));
                    sb.append("\"");
                }
                sb.append(">");
                sb.append(String.valueOf(i));
                sb.append("</a>\r\n");
            }
        }

        for(int i = iMinBorderRight; i <= pageCount; i++) {
            if(i == iMinBorderRight && iMiddleEnd > 0 && i > iMiddleEnd + 1) {
                sb.append("<i>...</i>");
            }
            sb.append("<a href=\"");
            if(i == ipage) {
                sb.append("#1\" class=active");
            } else {
                sb.append(response.encodeURL(requestURL + strQueryPara + "ipage=" + i));
                sb.append("\"");
            }
            sb.append(">");
            sb.append(String.valueOf(i));
            sb.append("</a>\r\n");
        }

        sb.append("</code>\r\n");

        if(ipage <= pageCount) {
            sb.append(imgnext);
            sb.append(imglast);
        }

        sb.append("</dd>");

        return sb.toString();

    }

    public static String generatePageNav(int recordsCount,
                                         HttpServletRequest request,
                                         HttpServletResponse response,
                                         String stripage, int perPageCount,
                                         String unitText,
                                         String style, String pos,
                                         boolean alwaysShow,
                                         boolean dropEmptyParam
    ) {
        int pageCount = (int) Math.ceil((double) recordsCount / perPageCount);
        if(pageCount < 1) return "";
        if(pageCount == 1 && !alwaysShow) return "";

        pos = pos == null || "".equals(pos) ? "" : "_" + pos;

        StringBuffer sb = new StringBuffer();
        sb.append("<div class=");
        sb.append(style == null || "".equals(style) ? "sspage" : style);
        sb.append(">");
        sb.append("<div class=divpage");
        sb.append(pos);
        sb.append(">");
        int iPageBorder = 2;//边缘页数
        int iPageMiddle = 5;//中间叶数
        String[] arrPageText = {"首页", "前一页", "后一页", "尾页"};

        int ipage = SuperString.getInt(request.getParameter("ipage"));
        if(ipage <= 0) ipage = 1;//将ipage保证在合理范围
        if(ipage > pageCount) ipage = pageCount;
        int iPageWidth = iPageBorder * 2 + iPageMiddle;//每页显示的数字页数

        //得到QueryString，ipage去掉
        String strQueryPara = getRequestQueryString(request, "ipage", false, null, true);
        String requestURL = request.getRequestURI();
        strQueryPara = "".equals(strQueryPara) ? "?" : "?" + strQueryPara + "&";

        String imgfirst = "<span><a class=ptlink href=\"" + response.encodeURL(requestURL + strQueryPara + "ipage=1") + "\">" + arrPageText[0] +
                "</a></span>";
        String imgpre = "<span><a class=ptlink href=\"" + response.encodeURL(requestURL + strQueryPara + "ipage=" + (ipage - 1)) + "\">" + arrPageText[1] +
                "</a></span>";
        String imgnext = "<span><a class=ptlink href=\"" + response.encodeURL(requestURL + strQueryPara + "ipage=" + (ipage + 1)) + "\">" + arrPageText[2] +
                "</a></span>";
        String imglast = "<span><a class=ptlink href=\"" + response.encodeURL(requestURL + strQueryPara + "ipage=" + pageCount) + "\">" + arrPageText[3] +
                "</a></span>";
        if(ipage <= 1) {
            imgfirst = "<span class=ptnolink>" + arrPageText[0] + "</span>";
            imgpre = "<span class=ptnolink>" + arrPageText[1] + "</span>";
        }
        if(ipage >= pageCount) {
            imgnext = "<span class=ptnolink>" + arrPageText[2] + "</span>";
            imglast = "<span class=ptnolink>" + arrPageText[3] + "</span>";
        }

        //中间部分导航页码的生成
        int iMaxBorderLeft = Math.min(pageCount, iPageBorder);
        int iMinBorderRight = Math.max(1, pageCount - iPageBorder + 1);
        if(iMinBorderRight <= iMaxBorderLeft) iMinBorderRight = Math.min(pageCount, iMaxBorderLeft + 1);
        if(iMaxBorderLeft == pageCount) iMinBorderRight = pageCount + 1;
        int iMiddleBegin = ipage - iPageMiddle / 2;
        if(iMiddleBegin <= iMaxBorderLeft) iMiddleBegin = iMaxBorderLeft + 1;
        int iMiddleEnd = iMiddleBegin + iPageMiddle - 1;
        if(iMiddleEnd >= iMinBorderRight) iMiddleEnd = iMinBorderRight - 1;
        if(iMiddleEnd - iMiddleBegin < iPageMiddle)
            iMiddleBegin = Math.max(iMaxBorderLeft + 1, iMiddleEnd - iPageMiddle + 1);

        if(iMiddleBegin > iMiddleEnd) {
            iMiddleBegin = 0;
            iMiddleEnd = 0;
        }

        if(ipage > 0) {
            sb.append(imgfirst);
            sb.append(imgpre);
        }

        for(int i = 1; i <= iMaxBorderLeft; i++) {
            if(i == ipage) {
                sb.append("<span class=curnum>");
                sb.append(String.valueOf(i));
            } else {
                sb.append("<span><a class=num href=\"");
                sb.append(response.encodeURL(requestURL + strQueryPara + "ipage=" + i));
                sb.append("\">");
                sb.append(String.valueOf(i));
                sb.append("</a>");
            }
            sb.append("</span>");
            if(i == iMaxBorderLeft && i < iMiddleBegin - 1) {
                sb.append("<span class=dot>...</span>");
            }
        }
        if(iMiddleBegin > 0) {
            for(int i = iMiddleBegin; i <= iMiddleEnd; i++) {
                if(i == ipage) {
                    sb.append("<span class=curnum>");
                    sb.append(String.valueOf(i));
                } else {
                    sb.append("<span><a class=num href=\"");
                    sb.append(response.encodeURL(requestURL + strQueryPara + "ipage=" + i));
                    sb.append("\">");
                    sb.append(String.valueOf(i));
                    sb.append("</a>");
                }
                sb.append("</span>");
            }
        }

        for(int i = iMinBorderRight; i <= pageCount; i++) {
            if(i == iMinBorderRight && iMiddleEnd > 0 && i > iMiddleEnd + 1) {
                sb.append("<span class=dot>...</span>");
            }
            if(i == ipage) {
                sb.append("<span class=curnum>");
                sb.append(String.valueOf(i));
            } else {
                sb.append("<span><a class=num href=\"");
                sb.append(response.encodeURL(requestURL + strQueryPara + "ipage=" + i));
                sb.append("\">");
                sb.append(String.valueOf(i));
                sb.append("</a>");
            }
            sb.append("</span>");
        }

        if(ipage <= pageCount) {
            sb.append(imgnext);
            sb.append(imglast);
        }

        sb.append("</div>");

        sb.append("<div class=divtotal");
        sb.append(pos);
        sb.append(">");
        sb.append("<span class=totalnum>");
        sb.append("共<span class=num>");
        sb.append(recordsCount);
        sb.append("</span>");
        sb.append(unitText);
        sb.append("</span>");
        sb.append("<span class=totalpage><span class=num>");
        sb.append(pageCount);
        sb.append("</span>");
        sb.append("页");
        sb.append("</span>");
        sb.append("</div>");

        sb.append("</div>");

        return sb.toString();
    }


    public static String getFullUrl(HttpServletRequest request,String droppa){
       return   getCurURLFile(request)+"?abc123lkf=1"+getParamAnd(request,droppa);
    }

    public static void redirectTo(HttpServletResponse response,String url) throws IOException {
          response.getWriter().write("<script>window.location.href='"+url+"';</script>");
    }

}