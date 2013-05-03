package my.myproperties.tool;

import java.util.Properties;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-16
 * Time: 16:17:35
 * To change this template use File | Settings | File Templates.
 */
public interface PropertyLoadCaller {
    public void onLoad(String fileName) throws Exception;
}
