package my.myproperties.tool;
import java.io.FileInputStream;
import java.io.File;
import java.util.Properties;
import java.util.Map;
import java.util.HashMap;


/**
 * Created by IntelliJ IDEA.
 *
 * @version 1.0
 *          Date: 2005-12-4
 *          Time: 16:03:48
 */
public class PropUtil {
    
    private static   PropertyLoadCaller onloadCaller=null;
    public static void setPropertyLoadCaller(PropertyLoadCaller onloadCaller){
           PropUtil.onloadCaller=onloadCaller;
    }
    public static PropertyLoadCaller getPropertyLoadCaller(){
          return  onloadCaller;
    }
    private static Map<String, Properties> propsM = new HashMap<String, Properties>();
    public static Properties getInstance(String fileName) {
        return propsM.get(fileName+".properties");
    }

    public static void loadProperties(String filePath) throws Exception {
        Properties properties = new Properties();
        File file=new File(filePath);
        FileInputStream fi=new  FileInputStream(file);
        properties.load(fi);
        fi.close();
        propsM.put(file.getName(),properties);
        if(onloadCaller!=null){
            onloadCaller.onLoad(file.getName());
        }

    }
}
