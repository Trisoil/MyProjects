package tool.cookie;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.Cookie;
import java.util.Vector;
import java.net.URLDecoder;

/**
 * Created by IntelliJ IDEA.
 *
 * @author Xu Jianbo
 * @version 1.0
 *          Date: 2008-4-6
 *          Time: 16:10:05
 */
public class UserHelper {

    public static UserProfile GetUserProfile(HttpServletRequest request) {
        try {
            Cookie cookie = CookieHelper.getCookie(request, "UDI");
            if(cookie == null) return null;
            String udi = URLDecoder.decode(cookie.getValue(), "utf-8");
            if(udi == null) return null;
            String[] audi = udi.split("\\$");
            if(audi.length < 12) return null;
            UserProfile userProfile = new UserProfile();
            userProfile.setGender(Integer.valueOf(audi[0]));
            userProfile.setPpNum(Integer.valueOf(audi[1]));
            userProfile.setExpNum(Integer.valueOf(audi[2]));
            userProfile.setLevelName(audi[3]);
            userProfile.setNextLevelName(audi[4]);
            userProfile.setNextLevelExpNum(Integer.valueOf(audi[5]));
            userProfile.setArea(audi[6]);
            userProfile.setHeadpic(audi[9]);
            userProfile.setEmail(audi[10]);
            userProfile.setOnlineTime(audi[11]);
            return userProfile;
        } catch(Exception e) {
            return null;
        }
    }

}
