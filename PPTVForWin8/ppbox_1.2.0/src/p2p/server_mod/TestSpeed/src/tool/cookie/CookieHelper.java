// Decompiled by DJ v2.9.9.61 Copyright 2000 Atanas Neshkov  Date: 2008-3-31 20:04:12
// Home Page : http://members.fortunecity.com/neshkov/dj.html  - Check often for new version!
// Decompiler options: packimports(3) 
// Source File Name:   CookieHelper.java

package tool.cookie;

import javax.servlet.http.*;

// Referenced classes of package com.synacast.ppboss.ppaao:
//            ConfigInfo, UserInfo, CryptogramHelper

public class CookieHelper {

    public CookieHelper() {
    }

    public static String getCookieValue(HttpServletRequest request, String name) {
        Cookie cookie = getCookie(request, name);
        return cookie == null ? "" : cookie.getValue();
    }

    public static Cookie getCookie(HttpServletRequest request, String name) {
        Cookie cookies[] = request.getCookies();
        if(cookies == null || name == null || name.length() == 0)
            return null;
        Cookie cookie = null;
        for(Cookie cooky : cookies) {
            if(!cooky.getName().equalsIgnoreCase(name))
                continue;
            cookie = cooky;
            if(request.getServerName().equalsIgnoreCase(cookie.getDomain()))
                break;
        }

        return cookie;
    }

    public static void setCookie(HttpServletResponse response, ConfigInfo configInfo, String strAPTokenResponse)
            throws Exception {
        String cookieName = (new StringBuilder("APToken@")).append(configInfo.getApID()).toString();
        Cookie cookie = new Cookie(cookieName, strAPTokenResponse);
        cookie.setPath(configInfo.getCookieDomain());
        cookie.setMaxAge(configInfo.getCookieExpireSecondI());
        response.addCookie(cookie);
    }

    public static void setCookie(HttpServletResponse response, ConfigInfo configInfo, AAOUserInfo userInfo)
            throws Exception {
        String cookieName = (new StringBuilder("APToken@")).append(configInfo.getApID()).toString();
        StringBuffer buffer = new StringBuffer();
        buffer.append(userInfo.getPpUID());
        buffer.append("$");
        buffer.append(userInfo.getUserMail());
        buffer.append("$");
        buffer.append(userInfo.getUserName());
        buffer.append("$");
        buffer.append(userInfo.getPpNum());
        buffer.append("$");
        buffer.append(userInfo.getUserType());
        buffer.append("$");
        buffer.append(userInfo.getTimeStamp());
        buffer.append("$");
        buffer.append(userInfo.getExpireTime());
        buffer.append("$");
        buffer.append(userInfo.getUserProfile());
        buffer.append("$");
        buffer.append(userInfo.getResult());
        String tokenValue = buffer.toString();
        String digest = CryptogramHelper.GenerateDigest(tokenValue);
        buffer.append("$");
        buffer.append(digest);
        String orginalString = buffer.toString();
        String cookieValue = (new StringBuilder(String.valueOf(configInfo.getApID()))).append("$").append(CryptogramHelper.Encrypt(orginalString, configInfo.getApKEY(), configInfo.getApIVB())).toString();
        Cookie cookie = new Cookie(cookieName, cookieValue);
        cookie.setPath(configInfo.getCookieDomain());
        cookie.setMaxAge(configInfo.getCookieExpireSecondI());
        response.addCookie(cookie);
    }

    public static void clearCookie(HttpServletResponse response, ConfigInfo configInfo)
            throws Exception {
        String cookieName = (new StringBuilder("APToken@")).append(configInfo.getApID()).toString();
        Cookie cookie = new Cookie(cookieName, "");
        cookie.setPath(configInfo.getCookieDomain());
        cookie.setMaxAge(0);
        response.addCookie(cookie);
    }
}