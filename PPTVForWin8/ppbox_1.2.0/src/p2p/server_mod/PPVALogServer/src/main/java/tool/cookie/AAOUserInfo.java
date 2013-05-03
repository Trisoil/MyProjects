// Decompiled by DJ v2.9.9.61 Copyright 2000 Atanas Neshkov  Date: 2008-3-31 20:04:46
// Home Page : http://members.fortunecity.com/neshkov/dj.html  - Check often for new version!
// Decompiler options: packimports(3) 
// Source File Name:   UserInfo.java

package tool.cookie;


public class AAOUserInfo {

    public String getPpUID() {
        return ppUID;
    }

    public void setPpUID(String ppUID) {
        this.ppUID = ppUID;
    }

    public String getUserMail() {
        return userMail;
    }

    public void setUserMail(String userMail) {
        this.userMail = userMail;
    }

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public String getPpNum() {
        return ppNum;
    }

    public void setPpNum(String ppNum) {
        this.ppNum = ppNum;
    }

    public String getUserType() {
        return userType;
    }

    public void setUserType(String userType) {
        this.userType = userType;
    }

    public String getUserProfile() {
        return userProfile;
    }

    public void setUserProfile(String userProfile) {
        this.userProfile = userProfile;
    }

    public String getResult() {
        return result;
    }

    public void setResult(String result) {
        this.result = result;
    }

    public String getTimeStamp() {
        return timeStamp;
    }

    public void setTimeStamp(String timeStamp) {
        this.timeStamp = timeStamp;
    }

    public String getExpireTime() {
        return expireTime;
    }

    public void setExpireTime(String expireTime) {
        this.expireTime = expireTime;
    }

    public String getUserFace() {
        return userFace;
    }

    public void setUserFace(String userFace) {
        this.userFace = userFace;
    }

    public AAOUserInfo() {
        ppUID = "";
        userMail = "";
        userName = "";
        ppNum = "";
        userType = "00";
        userProfile = "";
        result = "";
        timeStamp = "-1L";
        expireTime = "-1L";
        userFace="";

    }

    public AAOUserInfo(String apTokenValueReal) {
        String spItems[] = (String[]) null;
        spItems = apTokenValueReal.split("\\$");
        if(spItems.length == 9) {
            ppUID = spItems[0];
            userMail = spItems[1];
            userName = spItems[2];
            ppNum = spItems[3];
            userType = spItems[4];
            timeStamp = spItems[5];
            expireTime = spItems[6];
            userProfile = spItems[7];
            result = spItems[8];
        }
    }

    private String ppUID;
    private String userMail;
    private String userName;
    private String ppNum;
    private String userType;
    private String userProfile;
    private String result;
    private String timeStamp;
    private String expireTime;
    private String userFace;
}