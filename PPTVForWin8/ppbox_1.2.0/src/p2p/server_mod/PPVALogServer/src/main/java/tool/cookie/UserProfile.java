package tool.cookie;

/**
 * Created by IntelliJ IDEA.
 *
 * @author Xu Jianbo
 * @version 1.0
 *          Date: 2008-4-6
 *          Time: 16:00:27
 */
public class UserProfile {

    int gender;
    int ppNum;
    int expNum;
    String levelName = "";
    String nextLevelName = "";
    int nextLevelExpNum;
    String area = "";
    String headpic = "";
    String email = "";
    String onlineTime = "";

    public int getGender() {
        return gender;
    }

    public void setGender(int gender) {
        this.gender = gender;
    }

    public int getPpNum() {
        return ppNum;
    }

    public void setPpNum(int ppNum) {
        this.ppNum = ppNum;
    }

    public int getExpNum() {
        return expNum;
    }

    public void setExpNum(int expNum) {
        this.expNum = expNum;
    }

    public String getLevelName() {
        return levelName;
    }

    public void setLevelName(String levelName) {
        this.levelName = levelName;
    }

    public String getNextLevelName() {
        return nextLevelName;
    }

    public void setNextLevelName(String nextLevelName) {
        this.nextLevelName = nextLevelName;
    }

    public int getNextLevelExpNum() {
        return nextLevelExpNum;
    }

    public void setNextLevelExpNum(int nextLevelExpNum) {
        this.nextLevelExpNum = nextLevelExpNum;
    }

    public String getArea() {
        return area;
    }

    public void setArea(String area) {
        this.area = area;
    }

    public String getHeadpic() {
        return headpic;
    }

    public void setHeadpic(String headpic) {
        this.headpic = headpic;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }

    public String getOnlineTime() {
        return onlineTime;
    }

    public void setOnlineTime(String onlineTime) {
        this.onlineTime = onlineTime;
    }
}
