package my.myorm.tool.jdbc;

import tool.MyDataSourceFactory;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-9
 * Time: 16:03:29
 * To change this template use File | Settings | File Templates.
 */
public class SQLManager {
    public SQLManager() {
    }

    public static SQLExecutor getSQLExecutor(String dsname) throws Exception {
        //MyLocal.put("datasource",dsname);
        /*if(dsname.equals("")) */
        //System.out.println("##dsname="+dsname);
        SQLExecutor executor = TransactionContext.getSQLExecutorInCurrentTransaction();
        if (executor != null) return executor;
        return new SQLExecutor(dsname);
    }

    public static SQLExecutor getSQLExecutorForQuery() throws Exception {
        return getSQLExecutor("dataSource2");
    }
    
    public static SQLExecutor getSQLExecutor() throws Exception {
        return getSQLExecutor("");
    }

    public static SQLExecutor getSQLExecutor(boolean flag)
            throws Exception {
        if(flag) {
            return new SQLExecutor();
        } else {
            SQLExecutor sqlexecutor = TransactionContext.getSQLExecutorInCurrentTransaction();
            return sqlexecutor;
        }
    }
}
