package my.mysystem.tool;
import javax.servlet.ServletContext;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-9
 * Time: 10:52:09
 * To change this template use File | Settings | File Templates.
 */
public class ContextUtil {
    public static String rootPath = "";

    public static void initRootPath(ServletContext sc) throws Exception{
        if (rootPath == null || rootPath.equals("")) {
            //获取根路径
            String rootpath = sc.getRealPath("/");
            if (rootpath != null) rootpath = rootpath.replaceAll("\\\\", "/");
            else rootpath = "/";
            if (!rootpath.endsWith("/")) rootpath = rootpath + "/";
            rootPath = rootpath;
            System.out.println("##########################################################");
            System.out.println("############### Web Root Path=>" + rootpath);
            System.out.println("##########################################################");
        }
    }
}
