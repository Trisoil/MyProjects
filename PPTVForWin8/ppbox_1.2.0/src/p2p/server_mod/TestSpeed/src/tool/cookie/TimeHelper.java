// Decompiled by DJ v2.9.9.61 Copyright 2000 Atanas Neshkov  Date: 2008-3-31 20:04:34
// Home Page : http://members.fortunecity.com/neshkov/dj.html  - Check often for new version!
// Decompiler options: packimports(3) 
// Source File Name:   TimeHelper.java

package tool.cookie;

import java.text.SimpleDateFormat;
import java.util.Date;

public class TimeHelper {

    public TimeHelper() {
    }

    private static String FormatDate(Date inDate) {
        SimpleDateFormat formater = new SimpleDateFormat(DateStyle);
        return formater.format(inDate).toString();
    }

    private static Date FormatDate(String inDateStr)
            throws Exception {
        SimpleDateFormat formater = new SimpleDateFormat(DateStyle);
        return formater.parse(inDateStr);
    }

    public static String GetTimeStamp(String strInDateTime)
            throws Exception {
        SimpleDateFormat s = new SimpleDateFormat(DateStyle);
        Date d = s.parse(strInDateTime);
        return String.valueOf(d.getTime());
    }

    public static String GetTimeStamp() {
        Date d = new Date();
        return String.valueOf(d.getTime());
    }

    public static boolean CheckTimeStamp(String strInTimeStamp) {
        boolean flag = false;
        Date d = new Date();
        long nowTimeStamp = d.getTime();
        long inTimeStamp = Long.parseLong(strInTimeStamp);
        flag = nowTimeStamp < inTimeStamp;
        return flag;
    }

    public static String GetDateTimeFromTimeStamp(String strInTimeStamp) {
        long intimgstamp = Long.parseLong(strInTimeStamp);
        Date d = new Date();
        d.setTime(intimgstamp);
        return FormatDate(d);
    }

    public static String ResetTimeStamp(String strInTimeStamp, long lInTokenTimeSpan) {
        long intimestamp = Long.parseLong(strInTimeStamp);
        long newtimestamp = intimestamp + lInTokenTimeSpan * 1000L;
        return String.valueOf(newtimestamp);
    }

    public static String GetDateTimeString(Date inDTval) {
        Date d = inDTval != null ? inDTval : new Date();
        return FormatDate(d);
    }

    public static Date GetDateTimeByString(String DateTimeString)
            throws Exception {
        return FormatDate(DateTimeString);
    }

    private static String DateStyle = "yyyy-MM-dd HH:mm:ss";

}