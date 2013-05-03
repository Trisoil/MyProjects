// Decompiled by DJ v2.9.9.61 Copyright 2000 Atanas Neshkov  Date: 2008-3-31 20:03:56
// Home Page : http://members.fortunecity.com/neshkov/dj.html  - Check often for new version!
// Decompiler options: packimports(3) 
// Source File Name:   ConfigInfo.java
package tool.cookie;


public class ConfigInfo {

    public ConfigInfo() {
    }

    public String getApID() {
        return apID;
    }

    public void setApID(String apID) {
        this.apID = apID;
    }

    public String getApIV() {
        return apIV;
    }

    public void setApIV(String apIV) {
        this.apIV = apIV;
    }

    public String getApKEY() {
        return apKEY;
    }

    public void setApKEY(String apKEY) {
        this.apKEY = apKEY;
    }

    public String getCookieDomain() {
        return cookieDomain;
    }

    public void setCookieDomain(String cookieDomain) {
        this.cookieDomain = cookieDomain;
    }

    public String getCookieExpireSecond() {
        return cookieExpireSecond;
    }

    public void setCookieExpireSecond(String cookieExpireSecond) {
        this.cookieExpireSecond = cookieExpireSecond;
    }

    public String getPpLoginUrl() {
        return ppLoginUrl;
    }

    public void setPpLoginUrl(String ppLoginUrl) {
        this.ppLoginUrl = ppLoginUrl;
    }

    public String getPpLogoutUrl() {
        return ppLogoutUrl;
    }

    public void setPpLogoutUrl(String ppLogoutUrl) {
        this.ppLogoutUrl = ppLogoutUrl;
    }

    public String getApLoginUrl() {
        return apLoginUrl;
    }

    public void setApLoginUrl(String apLoginUrl) {
        this.apLoginUrl = apLoginUrl;
    }

    public String getApLogoutUrl() {
        return apLogoutUrl;
    }

    public void setApLogoutUrl(String apLogoutUrl) {
        this.apLogoutUrl = apLogoutUrl;
    }

    public byte[] getApIVB() {
        return apIVB;
    }

    public void setApIVB(byte apIVB[]) {
        this.apIVB = apIVB;
    }

    public int getCookieExpireSecondI() {
        return CookieExpireSecondI;
    }

    public void setCookieExpireSecondI(int cookieExpireSecondI) {
        CookieExpireSecondI = cookieExpireSecondI;
    }

    public String getPpRegistUrl() {
        return ppRegistUrl;
    }

    public void setPpRegistUrl(String ppRegistUrl) {
        this.ppRegistUrl = ppRegistUrl;
    }

    private String apID;
    private String apIV;
    private String apKEY;
    private String cookieDomain;
    private String cookieExpireSecond;
    private String ppRegistUrl;
    private String ppLoginUrl;
    private String ppLogoutUrl;
    private String apLoginUrl;
    private String apLogoutUrl;
    private byte apIVB[];
    private int CookieExpireSecondI;
}