// Decompiled by DJ v2.9.9.61 Copyright 2000 Atanas Neshkov  Date: 2008-3-31 20:03:45
// Home Page : http://members.fortunecity.com/neshkov/dj.html  - Check often for new version!
// Decompiler options: packimports(3) 
// Source File Name:   AAOConfig.java

package tool.cookie;

import java.io.*;
import java.util.Properties;
import javax.servlet.ServletContext;

public class AAOConfig {

    public AAOConfig() {
    }

    public static void setPropertyFile(String path) {
        PropertyFile = path;
    }

    public static String getConfigInfo(ServletContext context, String KeyWord)
            throws FileNotFoundException, IOException, Exception {
        String returnValue = null;
        File fObj = null;
        InputStream input = null;
        try {
            fObj = new File(context.getRealPath(PropertyFile));
            input = new FileInputStream(fObj);
        }
        catch(FileNotFoundException e) {
            input = AAOConfig.class.getResourceAsStream("/AAOSetting.properties");
        }
        Properties pro = new Properties();
        pro.load(input);
        returnValue = new String(pro.getProperty(KeyWord).getBytes("ISO8859_1"));
        pro.clear();
        input.close();
        return returnValue;
    }

    private static String PropertyFile = "WEB-INF/AAOSetting.properties";
    private static final String defaultPropertiesFilePath = "/AAOSetting.properties";

}