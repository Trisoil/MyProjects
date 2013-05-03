package my.myorm.tool.orm;

import my.myorm.tool.jdbc.DataSourceFactory;
import my.myorm.tool.jdbc.SQLExecutor;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-3-11
 * Time: 10:21:08
 * To change this template use File | Settings | File Templates.
 */
public class ORMInitializer {
    public static void init(String configFilePath, DataSourceFactory dataSourceFactory) throws Exception{
        if(dataSourceFactory==null) throw new Exception("dataSourceFactory==null");
        TableBeanMap.load(configFilePath);
        DataTypeMap.init();
        SQLExecutor.setDataSourceFactory(dataSourceFactory);
    }
}
