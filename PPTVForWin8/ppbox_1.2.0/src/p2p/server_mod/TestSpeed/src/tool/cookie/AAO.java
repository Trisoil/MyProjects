// Decompiled by DJ v2.9.9.61 Copyright 2000 Atanas Neshkov  Date: 2008-3-31 20:03:21
// Home Page : http://members.fortunecity.com/neshkov/dj.html  - Check often for new version!
// Decompiler options: packimports(3) 
// Source File Name:   AAO.java

package tool.cookie;

import java.util.Vector;
import java.net.URLDecoder;
import javax.servlet.http.*;

// Referenced classes of package com.synacast.ppboss.ppaao:
//            TimeHelper, CryptogramHelper, ConfigInfo, UserInfo, 
//            CookieHelper

public class AAO {

    public AAO() {
    }

    public static String GenerateAPTokenRequest(String ReturnURL,ConfigInfo configInfo)
            throws Exception {
        String TimeStamp = TimeHelper.GetTimeStamp();
        String Digest = CryptogramHelper.GenerateDigest((new StringBuilder(String.valueOf(ReturnURL))).append("$").append(TimeStamp).toString());
        String originalStr = (new StringBuilder(String.valueOf(ReturnURL))).append("$").append(TimeStamp).append("$").append(Digest).toString();
        String resultStr = (new StringBuilder(String.valueOf(configInfo.getApID()))).append("$").append(CryptogramHelper.Encrypt(originalStr, configInfo.getApKEY(), configInfo.getApIVB())).toString();
        return (new StringBuilder(String.valueOf(configInfo.getPpLoginUrl()))).append("?APTokenRequest=").append(CryptogramHelper.URLEncode(resultStr)).toString();
    }

    public static String GenerateLogoutURL(String ReturnURL, ConfigInfo configInfo) {
        try {
            return (new StringBuilder(configInfo.getPpLogoutUrl()).append("?APReturnURL=").append(CryptogramHelper.URLEncode(ReturnURL))).toString();
        } catch(Exception e) {
            return "";
        }
    }

    public static String MyParseAPToken(String strTobeParseAPToken, ConfigInfo configInfo)
            throws Exception {
        String tempStr = CryptogramHelper.URLDecode(strTobeParseAPToken);
        String APID = tempStr.substring(0, tempStr.indexOf("$"));
        String tobeDecrypt = tempStr.substring(tempStr.indexOf("$") + 1);
        String strResult = CryptogramHelper.Decrypt(tobeDecrypt, configInfo.getApKEY(), configInfo.getApIVB());
        return (new StringBuilder(String.valueOf(APID))).append("$").append(strResult).toString();
    }

    public static Vector ParseAPToken_bak(String strAPTokenResponse, ConfigInfo configInfo)
            throws Exception {
        Vector resultArray = new Vector(3);
        Boolean isValid = Boolean.FALSE;
        String apid = strAPTokenResponse.substring(0, strAPTokenResponse.indexOf("$"));
        String tobeDecrypt = strAPTokenResponse.substring(strAPTokenResponse.indexOf("$") + 1);
        String strResult = CryptogramHelper.Decrypt(tobeDecrypt, configInfo.getApKEY(), configInfo.getApIVB());
        String oldDigest = strResult.substring(strResult.lastIndexOf("$") + 1);
        String originalStr = strResult.substring(0, strResult.lastIndexOf("$"));
        String newDigest = CryptogramHelper.GenerateDigest(originalStr);
        AAOUserInfo ui = new AAOUserInfo(originalStr);
        isValid = !newDigest.equals(oldDigest) || !TimeHelper.CheckTimeStamp(ui.getExpireTime()) || !ui.getResult().equals("0") ? Boolean.FALSE : Boolean.TRUE;
        resultArray.add(0, isValid);
        resultArray.add(1, (new StringBuilder(String.valueOf(apid))).append("$").append(originalStr).toString());
        resultArray.add(2, ui);
        return resultArray;
    }

    public static Vector ParseAPToken(String strAPTokenResponse, ConfigInfo configInfo)
            throws Exception {
        Vector resultArray = new Vector(3);
        Boolean isValid = Boolean.FALSE;
        String apid = "";
        String strResult = CryptogramHelper.Decrypt(URLDecoder.decode(strAPTokenResponse, "utf-8"), configInfo.getApKEY(), configInfo.getApIVB());
        String oldDigest = strResult.substring(strResult.lastIndexOf("$") + 1);
        String originalStr = strResult.substring(0, strResult.lastIndexOf("$"));
        String newDigest = CryptogramHelper.GenerateDigest(originalStr);
        AAOUserInfo ui = new AAOUserInfo(originalStr);
        isValid = !newDigest.equals(oldDigest) || !TimeHelper.CheckTimeStamp(ui.getExpireTime()) || !ui.getResult().equals("0") ? Boolean.FALSE : Boolean.TRUE;
        resultArray.add(0, isValid);
        resultArray.add(1, (new StringBuilder(String.valueOf(apid))).append("$").append(originalStr).toString());
        resultArray.add(2, ui);
        return resultArray;
    }

    public static boolean ValidAPToken(HttpServletRequest request, HttpServletResponse response, ConfigInfo configInfo)
            throws Exception {
        Cookie cookie = CookieHelper.getCookie(request, (new StringBuilder("APToken@")).append(configInfo.getApID()).toString());
        if(cookie == null)
            return false;
        Vector vr = ParseAPToken(cookie.getValue(), configInfo);
        if((Boolean) vr.get(0) == Boolean.TRUE) {
            //CookieHelper.setCookie(response, configInfo, (AAOUserInfo) vr.get(2));
            return true;
        } else {
            CookieHelper.clearCookie(response, configInfo);
            return false;
        }
    }

    public static AAOUserInfo GetUserInfo(HttpServletRequest request, HttpServletResponse response, ConfigInfo configInfo) {
        try {
            //Cookie cookie = CookieHelper.getCookie(request, (new StringBuilder("APToken@")).append(configInfo.getApID()).toString());
             //获取udi中的用户头像
            Cookie cookie = CookieHelper.getCookie(request, "UDI");
            String userFace="";
            if(cookie!=null){
                try{
                     String udistr=URLDecoder.decode(cookie.getValue(), "utf-8");
                     String[] udiarr=udistr.split("\\$");
                     userFace=udiarr[9];
                }catch(Exception ex){
                    ex.printStackTrace();
                }
            }

            
            cookie = CookieHelper.getCookie(request, "PPKey");
            if(cookie != null) {
                Vector vr = ParseAPToken(cookie.getValue(), configInfo);
                if((Boolean) vr.get(0) == Boolean.TRUE) {
                    //CookieHelper.setCookie(response, configInfo, (AAOUserInfo) vr.get(2));
                    AAOUserInfo ui= (AAOUserInfo) vr.get(2);
                    ui.setUserFace(userFace);
                    return  ui;
                } else {
                    CookieHelper.clearCookie(response, configInfo);
                }
            } else {
                cookie = CookieHelper.getCookie(request, "PPName");
                if(cookie != null) {
                    String ppnamestr = cookie.getValue();
                    if(ppnamestr == null || "".equals(ppnamestr)) return null;
                    ppnamestr = URLDecoder.decode(ppnamestr, "utf-8");
                    if(ppnamestr.indexOf("$") > 0) ppnamestr = ppnamestr.substring(0, ppnamestr.indexOf("$"));
                    AAOUserInfo ui = new AAOUserInfo();
                    ui.setUserName(ppnamestr);
                    ui.setUserFace(userFace);
                    return ui;
                }
            }
        } catch(Exception e) {
            e.printStackTrace();
        }
        return null;
    }
}