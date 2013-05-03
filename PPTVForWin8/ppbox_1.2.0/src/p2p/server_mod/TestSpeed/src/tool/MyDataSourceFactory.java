package tool;

import my.myorm.tool.jdbc.DataSourceFactory;

import javax.sql.DataSource;

import my.myspring.tool.SpringBeanProxy;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-7
 * Time: 19:21:09
 * To change this template use File | Settings | File Templates.
 */
public class MyDataSourceFactory implements DataSourceFactory {

    private static ThreadLocal threadLocal = new ThreadLocal();

    public DataSource getDataSource() throws Exception {
        String s = (String) threadLocal.get();
        if(s == null || "".equals(s)) s = "dataSource1";
        return (DataSource) SpringBeanProxy.getBean(s);
    }

    public static void setDataSource(String s) {
        threadLocal.set(s);
    }
}
