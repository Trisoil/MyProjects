package my.myorm.tool.jdbc;

import javax.sql.DataSource;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-7
 * Time: 16:26:22
 * To change this template use File | Settings | File Templates.
 */
public interface DataSourceFactory {
        public DataSource getDataSource() throws Exception;
}
