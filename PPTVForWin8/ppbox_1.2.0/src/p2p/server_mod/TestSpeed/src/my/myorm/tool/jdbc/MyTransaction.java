package my.myorm.tool.jdbc;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-9
 * Time: 15:06:08
 * To change this template use File | Settings | File Templates.
 */
public class MyTransaction {
    private SQLExecutor executor=null;
    //已经开始-1，已经结束-2
    private int status=1;

    public SQLExecutor getExecutor() {
        return executor;
    }

    public void setExecutor(SQLExecutor executor) {
        this.executor = executor;
    }

    public int getStatus() {
        return status;
    }

    public void setStatus(int status) {
        this.status = status;
    }
}
