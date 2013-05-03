// Decompiled by DJ v2.9.9.61 Copyright 2000 Atanas Neshkov  Date: 2008-3-31 20:02:15
// Home Page : http://members.fortunecity.com/neshkov/dj.html  - Check often for new version!
// Decompiler options: packimports(3) 
// Source File Name:   ConfigHelper.java

package tool.cookie;

import java.io.*;
import java.util.Properties;
import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;

// Referenced classes of package com.synacast.ppboss.ppaao:
//            AAOConfig, ConfigInfo, CryptogramHelper

public class ConfigHelper {

    private static final String defaultPropertiesFilePath = "/WEB-INF/classes/AAOSetting.properties";
    private static final String APID = "APID";
    private static final String APIV = "APIV";
    private static final String APKEY = "APKEY";
    private static final String CookieDomain = "CookieDomain";
    private static final String CookieExpireSecond = "CookieExpireSecond";
    private static final String PPLoginUrl = "PPLoginUrl";
    private static final String PPLogoutUrl = "PPLogoutUrl";
    private static final String APLoginUrl = "APLoginUrl";
    private static final String APLogoutUrl = "APLogoutUrl";
    private static ConfigInfo cinfo = null;


    public ConfigHelper() {
    }

    public static ConfigInfo getConfigInfo(ServletContext context, HttpServletRequest request, String hostURL)
            throws FileNotFoundException, IOException, Exception {
        if(cinfo != null) return cinfo;
        else return getConfigInfo(context, request, hostURL, null);
    }

    public static ConfigInfo getConfigInfo(ServletContext context, HttpServletRequest request,
                                           String hostURL, String path)
            throws FileNotFoundException, IOException, Exception {
        ConfigInfo configInfo = null;
        FileInputStream input = null;
        if(path == null || "".equals(path))
            path = defaultPropertiesFilePath;
        path = context.getRealPath(path);
        try {
            File fObj = new File(path);
            input = new FileInputStream(fObj);
        }
        catch(FileNotFoundException e) {
            e.printStackTrace();
        }
        Properties pro = new Properties();
        configInfo = new ConfigInfo();
        pro.load(input);
        configInfo.setApID(new String(pro.getProperty("APID").getBytes("ISO8859_1")));
        configInfo.setApIV(new String(pro.getProperty("APIV").getBytes("ISO8859_1")));
        configInfo.setApKEY(new String(pro.getProperty("APKEY").getBytes("ISO8859_1")));
        configInfo.setCookieDomain(new String(pro.getProperty("CookieDomain").getBytes("ISO8859_1")));
        configInfo.setCookieExpireSecond(new String(pro.getProperty("CookieExpireSecond").getBytes("ISO8859_1")));
        configInfo.setPpRegistUrl(new String(pro.getProperty("PPRegistUrl").getBytes("ISO8859_1")));
        configInfo.setPpLoginUrl(new String(pro.getProperty("PPLoginUrl").getBytes("ISO8859_1")));
        configInfo.setPpLogoutUrl(new String(pro.getProperty("PPLogoutUrl").getBytes("ISO8859_1")));
        configInfo.setApLoginUrl(hostURL + new String(pro.getProperty("APLoginUrl").getBytes("ISO8859_1")));
        configInfo.setApLogoutUrl(hostURL + new String(pro.getProperty("APLogoutUrl").getBytes("ISO8859_1")));
        configInfo.setApIVB(CryptogramHelper.HexStringToByteArray(configInfo.getApIV()));
        configInfo.setCookieExpireSecondI(Integer.parseInt(configInfo.getCookieExpireSecond()));
        pro.clear();
        input.close();
        cinfo = configInfo;
        return configInfo;
    }

}