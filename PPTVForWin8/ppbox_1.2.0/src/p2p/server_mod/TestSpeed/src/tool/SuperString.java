package tool;

import org.apache.commons.lang.StringUtils;

import javax.servlet.ServletContext;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.regex.Pattern;

/**
 * Created by IntelliJ IDEA.
 * User: XuJianbo
 * Date: 2007-4-6
 * Time: 10:21:04
 * To change this template use File | Settings | File Templates.
 */

public class SuperString {
    public static final String EMPTY = "";
    public static final int NUMERIC_SHORT = 0;
    public static final int NUMERIC_INT = 1;
    public static final int NUMERIC_LONG = 2;
    public static final int NUMERIC_FLOAT = 3;
    public static final int NUMERIC_DOUBLE = 4;

    public static String notNull(String strTemp) {
        if(strTemp == null) {
            return new String("");
        } else {
            return strTemp;
        }
    }

    public static String notNullTrim(String strTemp) {
        if(strTemp == null) {
            return new String("");
        }else if(strTemp.trim().equalsIgnoreCase("null")){
            return new String("");
        }
        else {
            return strTemp.trim();
        }
    }

    public static boolean isBlank(String str) {
        return (notNullTrim(str).length() == 0);
    }

    public static String getQuoteDel(String strTemp) {
        return replace(notNull(strTemp), "\"", "");
    }

    public static int getInt(String strTemp) {
        strTemp = notNull(strTemp);
        if(strTemp.equals("")) {
            return 0;
        }
        try {
            return (int) Math.floor(Double.parseDouble(strTemp));
        }
        catch(Exception e) {
            return 0;
        }
    }

    public static int getInt(String strTemp, int def) {
        strTemp = notNull(strTemp);
        if(strTemp.equals("")) {
            return def;
        }
        try {
            return (int) Math.floor(Double.parseDouble(strTemp));
        }
        catch(Exception e) {
            return def;
        }
    }

    public static long getLong(String strTemp) {
        strTemp = notNull(strTemp);
        if(strTemp.equals("")) {
            return 0L;
        }
        try {
            return (long) Math.floor(Double.parseDouble(strTemp));
        }
        catch(Exception e) {
            return 0L;
        }
    }

    public static float getFloat(String strTemp) {
        strTemp = notNull(strTemp);
        if(strTemp.equals("")) {
            return 0f;
        }
        try {
            return Float.parseFloat(strTemp);
        }
        catch(Exception e) {
            return 0f;
        }
    }

    public static double getDouble(String strTemp) {
        strTemp = notNull(strTemp);
        strTemp = SuperString.replace(strTemp, ",", "");
        if(strTemp.equals("")) {
            return 0d;
        }
        try {
            return Double.parseDouble(strTemp);
        }
        catch(Exception e) {
            return 0d;
        }
    }

    public static double getDouble(String strTemp, double defaultVal) {
        strTemp = notNull(strTemp);
        strTemp = SuperString.replace(strTemp, ",", "");
        if(strTemp.equals("")) {
            return 0d;
        }
        try {
            return Double.parseDouble(strTemp);
        }
        catch(Exception e) {
            return defaultVal;
        }
    }

    public static String getRandString(int minlen, int maxlen) {
        String s = "";
        if(minlen > maxlen) {
            minlen = maxlen;
        }
        for(; s.length() < maxlen; s = ("" + Math.random()).substring(2) + s) {
            ;
        }
        s = s.substring(s.length() - maxlen);
        int n = maxlen - minlen;
        if(n > 0) {
            n = (int) Math.round((double) n * Math.random());
        }
        if(n > 0) {
            s = s.substring(n);
        }
        return s;
    }

    /**
     * 返回字符串的长度，汉字按两个字符长度计算
     *
     * @param s 需要计算的字符串
     */
    public static int getLength(String s) {
        int len = 0;
        String strchar = "";
        for(int i = 0; i < s.length(); i++) {
            strchar = String.valueOf(s.charAt(i));
            if(strchar.length() == strchar.getBytes().length) {
                len++;
            } else {
                len += 2;
            }
        }
        return len;
    }

    public static String getShortString(String s, int len, String fillstr) {
        if(s.length() > len) {
            return s.substring(0, len) + fillstr;
        } else {
            return s;
        }
    }

    /**
     * 这个方法是个特别的方法，返回指定长度的字符串，重要的是汉字算两个字符长度
     */
    public static String getSpeShortString(String s, int len, String fillstr) {
        int ilen = Math.max(0, len - fillstr.length());
        char ch = ' ';
        int reallen = 0;
        for(int i = 0; i < s.length(); i++) {
            ch = s.charAt(i);
            if(((int) ch > 32) && ((int) ch < 128)) {
                reallen++;
            } else {
                reallen += 2;
            }
        }
        if(reallen <= len) {
            return s;
        }
        StringBuffer buf = new StringBuffer();
        reallen = 0;
        for(int i = 0; i < s.length(); i++) {
            ch = s.charAt(i);
            buf.append(ch);
            if(((int) ch > 32) && ((int) ch < 128)) {
                reallen++;
            } else {
                reallen += 2;
            }
            if(reallen >= ilen) {
                return buf.toString() + fillstr;
            }
        }
        return buf.toString();
    }

    //是不是全英文
    public static boolean isEnglish(String str) {
        if(str == null) {
            return true;
        } else {
            if(str.length() == getLength(str)) {
                return true;
            } else {
                return false;
            }
        }
    }

    /**
     * 判断字符串是否是数字
     */
    public static boolean isNumeric(String str, int intType) {
        try {
            switch(intType) {
                case NUMERIC_SHORT:
                    short shortN = Short.parseShort(str);
                    break;
                case NUMERIC_INT:
                    int intN = Integer.parseInt(str);
                    break;
                case NUMERIC_LONG:
                    long longN = Long.parseLong(str);
                    break;
                case NUMERIC_FLOAT:
                    float floatN = Float.parseFloat(str);
                    break;
                case NUMERIC_DOUBLE:
                    double doubleN = Double.parseDouble(str);
                    break;
            }
        }
        catch(Exception e) {
            return false;
        }
        return true;
    }

    /**
     * 判断是否是手机
     */
    public static boolean isMobile(String mobile) {
        return match("^[1][3][0-9]{9}", mobile);
    }

    /**
     * 判断是否是email地址
     */
    public static boolean isEmail(String email) {
        return match("^[\\w-]+(\\.[\\w-]+)*@[\\w-]+(\\.[\\w-]+)+$", email);
    }

    public static boolean isCharAndNum(String s) {
        return match("[a-zA-Z0-9]+", s);
    }

    public static boolean isDirName(String s) {
        return match("[a-zA-Z0-9_.-]+", s);
    }

    public static boolean isDateTimeString(String s) {
        return match("(\\d{4})-(\\d{2})-(\\d{2}) (\\d{2}):(\\d{2}):(\\d{2})", s);
    }

    public static boolean isDateString(String s) {
        return match("(\\d{4})-(\\d{2})-(\\d{2})", s);
    }

    /**
     * 替换掉本网站模板文本中的变量$()
     *
     * @param text    要替换的文本全文
     * @param varname 变量名 $(name) name为变量名
     * @param value   替换的值
     */
    public static String replaceTemplateTag(String text, String varname,
                                            String value) {
        return SuperString.replace(text, "${" + varname + "}", value);
    }

    public static String replaceTemplateTag(String text, Map map) {
        for(Object o : map.keySet()) {
            String key = (String) o;
            if(key != null && map.containsKey(key)) {
                text = replaceTemplateTag(text, key, (String) map.get(key));
            }
        }
        return text;
    }

    /**
     * 一个替换方法，替换字符串中从第几个到第几个字符为指定的字符（串）
     */
    public static String replaceCharFromTo(String s, int from, int to,
                                           String rechar) {
        s = notNull(s);
        if(from > to) {
            from += to;
            to = from - to;
            from -= to;
        }
        if(from > s.length() || to < s.length()) {
            return s;
        }
        char ch = ' ';
        StringBuffer buf = new StringBuffer();
        for(int i = 0; i < s.length(); i++) {
            ch = s.charAt(i);
            if(i + 1 < from || i + 1 > to) {
                buf.append(ch);
            } else {
                buf.append(rechar);
            }
        }
        return buf.toString();
    }

    public static String replaceCharFromTo(String s, int from, String rechar) {
        return replaceCharFromTo(s, from, s.length(), rechar);
    }

    /**
     * <p>Joins the elements of the provided array into a single String
     * containing the provided list of elements.</p>
     * <p/>
     * <p>No delimiter is added before or after the list.
     * A <code>null</code> separator is the same as an empty String ("").
     * Null objects or empty strings within the array are represented by
     * empty strings.</p>
     * <p/>
     * <pre>
     * StringUtils.join(null, *)                = null
     * StringUtils.join([], *)                  = ""
     * StringUtils.join([null], *)              = ""
     * StringUtils.join(["a", "b", "c"], "--")  = "a--b--c"
     * StringUtils.join(["a", "b", "c"], null)  = "abc"
     * StringUtils.join(["a", "b", "c"], "")    = "abc"
     * StringUtils.join([null, "", "a"], ',')   = ",,a"
     * </pre>
     *
     * @param array     the array of values to join together, may be null
     * @param separator the separator character to use, null treated as ""
     * @return the joined String, <code>null</code> if null array input
     */
    public static String join(Object[] array, String separator) {
        if(array == null) {
            return null;
        }
        if(separator == null) {
            separator = EMPTY;
        }
        int arraySize = array.length;

        // ArraySize ==  0: Len = 0
        // ArraySize > 0:   Len = NofStrings *(len(firstString) + len(separator))
        //           (Assuming that all Strings are roughly equally long)
        int bufSize
                = ((arraySize == 0) ? 0
                : arraySize * ((array[0] == null ? 16 : array[0].toString().length())
                + ((separator != null) ? separator.length() : 0)));

        StringBuffer buf = new StringBuffer(bufSize);

        for(int i = 0; i < arraySize; i++) {
            if((separator != null) && (i > 0)) {
                buf.append(separator);
            }
            if(array[i] != null) {
                buf.append(array[i]);
            }
        }
        return buf.toString();
    }

    public static final String recoverEnter(String input) {
        if(input == null || input.length() == 0) {
            return input;
        }
        input = replace(input, "\\n", "\n");
        input = replace(input, "\\r", "\r");
        return input;
    }

    public static final String recoverHTMLTags(String input) {
        if(input == null || input.length() == 0) {
            return input;
        }
        input = replace(input, "&lt;", "<");
        input = replace(input, "&gt;", ">");
        input = replace(input, "&amp;", "&");
        input = replace(input, "&nbsp;", " ");
        input = replace(input, "&nbsp;", " ");
        input = replace(input, "&quot;", "\"");
        input = replace(input, "<br>", "\r\n");
        return input;
    }

    /**
     * This method takes a string which may contain HTML tags (ie, &lt;b&gt;,
     * &lt;table&gt;, etc) and converts the '&lt'' and '&gt;' characters to
     * their HTML escape sequences.
     *
     * @param input the text to be converted.
     * @return the input string with the characters '&lt;' and '&gt;' replaced
     *         with their HTML escape sequences.
     */
    public static final String escapeHTMLTags(String input) {
        //Check if the string is null or zero length -- if so, return
        //what was sent in.
        if(input == null || input.length() == 0) {
            return input;
        }
        input = SuperString.replace(input, "\\r", "");
        //Use a StringBuffer in lieu of String concatenation -- it is
        //much more efficient this way.
        StringBuffer buf = new StringBuffer(input.length());
        char ch = ' ';
        for(int i = 0; i < input.length(); i++) {
            ch = input.charAt(i);
            if(ch == '<') {
                buf.append("&lt;");
            } else if(ch == '>') {
                buf.append("&gt;");
            } else if(ch == '&') {
                buf.append("&amp;");
            } else if((int) ch == 32) {
                buf.append("&nbsp;");
            } else if((int) ch == 34) {
                buf.append("&quot;");
            } else if((int) ch == 10) {
                buf.append("<br>");
            } else if((int) ch == 13) {
                buf.append("");
            } else {
                buf.append(ch);
            }
        }
        return buf.toString();
    }

    public static final String escapeHTMLTags(String input, boolean escapeSpace) {
        //Check if the string is null or zero length -- if so, return
        //what was sent in.
        if(input == null || input.length() == 0) {
            return "";
        }
        input = SuperString.replace(input, "\\r", "");
        //Use a StringBuffer in lieu of String concatenation -- it is
        //much more efficient this way.
        StringBuffer buf = new StringBuffer(input.length());
        char ch = ' ';
        for(int i = 0; i < input.length(); i++) {
            ch = input.charAt(i);
            if(ch == '<') {
                buf.append("&lt;");
            } else if(ch == '>') {
                buf.append("&gt;");
            } else if(ch == '&') {
                buf.append("&amp;");
            } else if((int) ch == 32) {
                if(escapeSpace) {
                    buf.append("&nbsp;");
                } else {
                    buf.append(ch);
                }
            } else if((int) ch == 34) {
                buf.append("&quot;");
            } else if((int) ch == 10) {
                buf.append("<br>");
            } else if((int) ch == 13) {
                buf.append("");
            } else {
                buf.append(ch);
            }
        }
        return buf.toString();
    }

    public static String removeEnter(String input) {
        if(input == null || input.length() == 0) {
            return "";
        }
        while(input.indexOf("\\r") >= 0) {
            input = SuperString.replace(input, "\\r", "");
        }
        return input;
    }

    public static final String escapeHTMLTagsSpe(String input) {
        if(input == null || input.length() == 0) {
            return "";
        }
        if(input.getBytes().length > input.length()) {
            return escapeHTMLTags(input);
        } else {
            return escapeHTMLTags(input, false);
        }
    }

    public static final String escapeTagsRemainHTML(String input,
                                                    boolean escapeSpace) {
        if(input == null || input.length() == 0) {
            return "";
        }
        input = SuperString.removeEnter(input);
        StringBuffer buf = new StringBuffer(input.length());
        char ch = ' ';
        for(int i = 0; i < input.length(); i++) {
            ch = input.charAt(i);
            if(ch == '&') {
                buf.append("&amp;");
            } else if((int) ch == 32) {
                if(escapeSpace) {
                    buf.append("&nbsp;");
                } else {
                    buf.append(ch);
                }
            } else if((int) ch == 34) {
                buf.append("&quot;");
            } else if((int) ch == 10) {
                buf.append("<br>");
            } else {
                buf.append(ch);
            }
        }
        return buf.toString();
    }

    public static final String escapeTagsRemainHTMLSpe(String input) {
        if(input == null || input.length() == 0) {
            return "";
        }
        if(input.getBytes().length > input.length()) {
            return escapeTagsRemainHTML(input, true);
        } else {
            return escapeTagsRemainHTML(input, false);
        }
    }

    public static String convertHTMLTagsOnlyEnter(String input) {
        if(input == null || input.length() == 0) {
            return "";
        }
        input = replace(input, "\r", "<br>");
        input = replace(input, "\n", "");
        return input;
    }

    // Replacing
    //-----------------------------------------------------------------------

    /**
     * <p>Replaces a String with another String inside a larger String, once.</p>
     * <p/>
     * <p>A <code>null</code> reference passed to this method is a no-op.</p>
     * <p/>
     * <pre>
     * StringUtils.replaceOnce(null, *, *)        = null
     * StringUtils.replaceOnce("", *, *)          = ""
     * StringUtils.replaceOnce("aba", null, null) = "aba"
     * StringUtils.replaceOnce("aba", null, null) = "aba"
     * StringUtils.replaceOnce("aba", "a", null)  = "aba"
     * StringUtils.replaceOnce("aba", "a", "")    = "aba"
     * StringUtils.replaceOnce("aba", "a", "z")   = "zba"
     * </pre>
     *
     * @param text text to search and replace in, may be null
     * @param repl the String to search for, may be null
     * @param with the String to replace with, may be null
     * @return the text with any replacements processed,
     *         <code>null</code> if null String input
     * @see #replace(String text, String repl, String with, int max)
     */
    public static String replaceOnce(String text, String repl, String with) {
        return replace(text, repl, with, 1);
    }

    public static String removeEnterAndBr(String input, ServletContext sc) {
        if(input == null || input.length() == 0) {
            return input;
        }
        StringBuffer buf = new StringBuffer(input.length());
        char ch = ' ';
        for(int i = 0; i < input.length(); i++) {
            ch = input.charAt(i);
            if(((int) ch != 10) && ((int) ch != 13)) {
                buf.append(ch);
            }
        }
        return buf.toString();
    }

    /**
     * <p>Replaces all occurances of a String within another String.</p>
     * <p/>
     * <p>A <code>null</code> reference passed to this method is a no-op.</p>
     * <p/>
     * <pre>
     * StringUtils.replace(null, *, *)        = null
     * StringUtils.replace("", *, *)          = ""
     * StringUtils.replace("aba", null, null) = "aba"
     * StringUtils.replace("aba", null, null) = "aba"
     * StringUtils.replace("aba", "a", null)  = "aba"
     * StringUtils.replace("aba", "a", "")    = "aba"
     * StringUtils.replace("aba", "a", "z")   = "zbz"
     * </pre>
     *
     * @param text text to search and replace in, may be null
     * @param repl the String to search for, may be null
     * @param with the String to replace with, may be null
     * @return the text with any replacements processed,
     *         <code>null</code> if null String input
     * @see #replace(String text, String repl, String with, int max)
     */
    public static String replace(String text, String repl, String with) {
        return replace(text, repl, with, -1);
    }

    public static String replaceIgnoreCase(String text, String repl, String with) {
        return replaceIgnoreCase(text, repl, with, -1);
    }

    /**
     * <p>Replaces a String with another String inside a larger String,
     * for the first <code>max</code> values of the search String.</p>
     * <p/>
     * <p>A <code>null</code> reference passed to this method is a no-op.</p>
     * <p/>
     * <pre>
     * StringUtils.replace(null, *, *, *)         = null
     * StringUtils.replace("", *, *, *)           = ""
     * StringUtils.replace("abaa", null, null, 1) = "abaa"
     * StringUtils.replace("abaa", null, null, 1) = "abaa"
     * StringUtils.replace("abaa", "a", null, 1)  = "abaa"
     * StringUtils.replace("abaa", "a", "", 1)    = "abaa"
     * StringUtils.replace("abaa", "a", "z", 0)   = "abaa"
     * StringUtils.replace("abaa", "a", "z", 1)   = "zbaa"
     * StringUtils.replace("abaa", "a", "z", 2)   = "zbza"
     * StringUtils.replace("abaa", "a", "z", -1)  = "zbzz"
     * </pre>
     *
     * @param text text to search and replace in, may be null
     * @param repl the String to search for, may be null
     * @param with the String to replace with, may be null
     * @param max  maximum number of values to replace, or <code>-1</code> if no maximum
     * @return the text with any replacements processed,
     *         <code>null</code> if null String input
     */
    public static String replace(String text, String repl, String with, int max) {
        if(text == null || repl == null || with == null || repl.length() == 0 ||
                max == 0) {
            return text;
        }

        StringBuffer buf = new StringBuffer(text.length());
        int start = 0, end = 0;
        while((end = text.indexOf(repl, start)) != -1) {
            buf.append(text.substring(start, end)).append(with);
            start = end + repl.length();

            if(--max == 0) {
                break;
            }
        }
        buf.append(text.substring(start));
        return buf.toString();
    }

    public static String replaceIgnoreCase(String text, String repl, String with, int max) {
        if(text == null || repl == null || with == null || repl.length() == 0 ||
                max == 0) {
            return text;
        }

        StringBuffer buf = new StringBuffer(text.length());
        int start = 0, end = 0;
        while((end = text.toLowerCase().indexOf(repl.toLowerCase(), start)) != -1) {
            buf.append(text.substring(start, end)).append(with);
            start = end + repl.length();

            if(--max == 0) {
                break;
            }
        }
        buf.append(text.substring(start));
        return buf.toString();
    }

    /**
     * 将文件名中的汉字转为UTF8编码的串
     *
     * @param s 字符串
     * @return 重新编码后的字符串
     */
    public static String toUtf8String(String s) {
        StringBuffer sb = new StringBuffer();
        for(int i = 0; i < s.length(); i++) {
            char c = s.charAt(i);
            if(c >= 0 && c <= 255) {
                sb.append(c);
            } else {
                byte[] b;
                try {
                    b = Character.toString(c).getBytes("utf-8");
                }
                catch(Exception ex) {
                    //System.out.println(ex);
                    b = new byte[0];
                }
                for(int j = 0; j < b.length; j++) {
                    int k = b[j];
                    if(k < 0) {
                        k += 256;
                    }
                    sb.append("%" + Integer.toHexString(k).
                            toUpperCase());
                }
            }
        }
        return sb.toString();
    }

    /**
     * 替换自定义标签
     *
     * @param s
     */
    public static String replaceSelfTag(String s, int defimgw) {
        String rtnStr = s;

        rtnStr = replaceSelfFontTag(rtnStr, "b");
        rtnStr = replaceSelfFontTag(rtnStr, "i");
        rtnStr = replaceSelfFontTag(rtnStr, "u");
        rtnStr = replaceSelfImageTag(rtnStr);
        rtnStr = replaceSelfSImageTag(rtnStr, defimgw);

        return rtnStr;
    }

    /**
     * 替换自定义文字标签
     *
     * @param str
     * @param tag
     */
    public static String replaceSelfFontTag(String str, String tag) {
        String rtnStr = str;

        int start = 0, end = 0;
        String sFind = null, sReplace = null;
        while((start = rtnStr.indexOf("[" + tag + "]", start)) != -1) {
            end = rtnStr.indexOf("[/" + tag + "]", start + 4);
            if(end != -1) {
                sFind = rtnStr.substring(start, end + 4);
                sReplace = sFind.replaceAll("\\[" + tag + "\\]", "<" + tag + ">").
                        replaceAll("\\[/" + tag + "\\]", "</" + tag + ">");
                sFind = sFind.replaceAll("\\[", "\\\\[").replaceAll("\\]", "\\\\]");
                rtnStr = rtnStr.replaceAll(sFind, sReplace);
                //System.out.println(sFind + "-" + sReplace + "-" + rtnStr);
            } else {
                break;
            }

            start = end + 5;
        }

        return rtnStr;
    }

    /**
     * 替换自定义图片标签
     *
     * @param str
     */
    public static String replaceSelfImageTag(String str) {
        String rtnStr = str;

        int start = 0, end = 0;
        String sFind = null, sReplace = null;
        while((start = rtnStr.indexOf("[img]", start)) != -1) {
            end = rtnStr.indexOf("[/img]", start + 4);
            if(end != -1) {
                sFind = rtnStr.substring(start, end + 6);
                sReplace = "<br><img src='" + sFind.substring(5, sFind.lastIndexOf("[/")) +
                        "' onload=\"javascript:if(this.width>500){this.width=500}\"><br>";
                //sFind = sFind.replaceAll("\\[", "\\\\[");
                //sFind = sFind.replaceAll("\\]", "\\\\]");
                //rtnStr = rtnStr.replaceAll(sFind, sReplace);
                rtnStr = rtnStr.substring(0, start) + sReplace +
                        rtnStr.substring(end + 6, rtnStr.length());
                //System.out.println(sFind + "-" + sReplace + "-" + rtnStr);
            } else {
                break;
            }

            //start = end + 5;
        }

        return rtnStr;
    }

    public static String replaceSelfSImageTag(String str, int defimgw) {
        String rtnStr = str;
        int start = 0, end = 0;
        String prefix = "[simg]";
        String suffix = "[/simg]";
        String sFind = null, sReplace = null;
        while((start = rtnStr.indexOf(prefix, start)) != -1) {
            end = rtnStr.indexOf(suffix, start + prefix.length());
            if(end != -1) {
                sFind = rtnStr.substring(start, end + suffix.length());
                String sImage = sFind.substring(prefix.length(),
                        sFind.length() - suffix.length());
                String imgWStr = "";
                String imgFile = sImage;
                int intwpos = sImage.indexOf("w_");
                if(intwpos > 0 && sImage.indexOf("h_") > 0) {
                    imgFile = sImage.substring(sImage.indexOf("h_") + 2);
                    int intw = SuperString.getInt(sImage.substring(0, intwpos));
                    if(intw > 0) {
                        imgWStr = " width=\"" +
                                (intw > defimgw ? String.valueOf(defimgw) : String.valueOf(intw)) +
                                "\" ";
                    } else {
                        imgWStr =
                                " onload=\"javascript:if(this.width>500){this.width=500}\" ";
                    }
                }
                sReplace = "<br><a href=\"" + imgFile + "\" target=_blank>" +
                        "<img src=\"" + imgFile + "\" " + imgWStr + " border=0>" +
                        "</a><br>";
                rtnStr = rtnStr.substring(0, start) + sReplace +
                        rtnStr.substring(end + suffix.length(), rtnStr.length());
            } else {
                break;
            }
        }
        return rtnStr;
    }

    public static String lineBreak(String txt) {
        if(txt != null) {
            txt = txt.replaceAll("\r\n", "<br>");
            return txt;
        } else {
            return "";
        }
    }

    public static boolean match(String regex, String str) {
        return Pattern.matches(regex, str);
        //Pattern pattern = Pattern.compile(regex);
        //Matcher matcher = pattern.matcher(str);
        //return matcher.matches();
    }

    public static boolean isLegalLoginName(String s, int lens, int lene) {
        return match("^[A-Za-z0-9]{" + lens + "," + lene + "}", s);
    }

    public static boolean isLegalPassword(String pwd) {
        boolean islegalchar = match("[\\S]+", pwd);
        if(!islegalchar) {
            return false;
        }
        return pwd.length() == pwd.getBytes().length;
    }

    public static boolean isAllowIP(String userIP, String businessAllowIP) {
        if(StringUtils.isBlank(businessAllowIP)) {
            return false;
        } else {
            String[] ips = businessAllowIP.split(",");
            for(int i = 0; i < ips.length; i++) {
                if(ips[i].equals(userIP)) {
                    return true;
                }
            }
            return false;
        }
    }

    public static String clearText(String text) {
        text = replace(text, "'", "");
        text = replace(text, "\"", "");
        text = replace(text, "\r", "");
        text = replace(text, "\n", "");
        text = replace(text, "\t", "");
        return text;
    }

    public static List<String> strToList(String str, String seperator) {
        List<String> list = new ArrayList<String>();
        if(str == null || str.length() == 0 || seperator == null) return list;
        list.addAll(Arrays.asList(str.split(seperator)));
        return list;
    }

    public static List<String> strToList(String str) {
        return strToList(str, ",");
    }

    public static String listToStr(List<String> list, String seperator) {
        StringBuffer sb = new StringBuffer();
        for(int i = 0; i < list.size(); i++) {
            if(i > 0) sb.append(seperator);
            sb.append(list.get(i));
        }
        return sb.toString();
    }

    public static String listToStr(List<String> list) {
        return listToStr(list, ",");
    }

    public static String addCharToListStr(String str, String chr) {
        StringBuffer sb = new StringBuffer();
        List<String> list = strToList(str, ",");
        for(int i = 0; i < list.size(); i++) {
            if(i > 0) sb.append(",");
            sb.append(chr).append(list.get(i)).append(chr);
        }
        return sb.toString();
    }

    public static String escapeInValidXMLChars(String str) {
        StringBuffer out = new StringBuffer();
        char current;
        if(str == null || ("".equals(str))) return "";
        for(int i = 0; i < str.length(); i++) {
            current = str.charAt(i);
            if((current == 0x9) || (current == 0xA) || (current == 0xD)
                    || ((current >= 0x20) && (current <= 0xD7FF))
                    || ((current >= 0xE000) && (current <= 0xFFFD))
                    || ((current >= 0x10000) && (current <= 0x10FFFF)))
                out.append(current);
        }
        return out.toString();
    }

    public static String upcaseFirstChar(String str){
        if(str==null||str.equals("")) return str;
        return str.substring(0,1).toUpperCase()+str.substring(1);
    }
    

    public static int getCharCount(String str,char ch){
        int count=0;
        StringBuilder sb=new  StringBuilder(str);
        for(int i=0;i<sb.length();i++){
            if(sb.charAt(i)==ch){
                count++;
            }
        }
        return count;
    }
    /**
	 * markwang
	 * @param num
	 * @return
	 */
	public static String getSpeedKeys() {
		String timeStr=new SimpleDateFormat("yyyyMMddHHmmsss").format(new Date());
		int randomStr=(int)Math.floor(Math.random()*100000);
		String codeStr=timeStr+randomStr;
        return codeStr;
    }

    public static void main(String[] args) {
        //System.out.println(match("(13[0-9]{9}[,]{0,1}){1,5}[#].*",
        //        "13917504705,13917504705,13917504705,13917504705,13917504705,13917504705#test"));
        //System.out.println(match("[\\S]+", "1d我"));
        /*
               java.util.List list = new java.util.ArrayList();
               list.add("1");
               list.add("2");
               System.out.println(list.size());
        */
    }

}
