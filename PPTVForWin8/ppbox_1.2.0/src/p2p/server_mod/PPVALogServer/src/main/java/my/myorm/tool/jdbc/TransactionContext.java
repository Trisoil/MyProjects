package my.myorm.tool.jdbc;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-9
 * Time: 15:02:22
 * To change this template use File | Settings | File Templates.
 */
public class TransactionContext {
    private static ThreadLocal<MyTransaction> transactions=new ThreadLocal<MyTransaction>();
    public static SQLExecutor getSQLExecutorInCurrentTransaction() throws  Exception{
            MyTransaction transaction=transactions.get();
            if(transaction==null||transaction.getStatus()!=1){
                return null;
            }
            return transaction.getExecutor();
    }
    public static void beginTransaction() throws Exception{
            MyTransaction transaction=transactions.get();
            if(transaction==null){
                MyTransaction tran=new MyTransaction();
                tran.setStatus(1);
                tran.setExecutor(new SQLExecutor(true));
                transactions.set(tran);
            }else{
                if(transaction.getStatus()!=4){
                    throw new Exception("last transaction is not over");
                }else{
                    transaction.setExecutor(new SQLExecutor(true));
                    transaction.setStatus(1);
                }
            }
    }
    public static void commitTransaction() throws Exception{
           MyTransaction transaction=transactions.get();
           if(transaction==null){
               throw new Exception("no transaction to commit");
           }
           if(transaction.getStatus()!=1){
               throw new Exception("invalid transaction status");
           }
           transaction.getExecutor().commit();
           transaction.setStatus(2);
    }

    public static void rollbackTransaction() throws Exception{
           MyTransaction transaction=transactions.get();
           if(transaction==null){
               throw new Exception("no transaction to rollback");
           }
           if(transaction.getStatus()!=1){
               throw new Exception("invalid transaction status");
           }
           transaction.getExecutor().rollBack();
           transaction.setStatus(3);
    }

     public static void endTransaction() throws Exception{
           MyTransaction transaction=transactions.get();
           if(transaction==null){
               throw new Exception("no transaction to rollback");
           }
           transaction.getExecutor().close();
           transaction.setStatus(4);
    }
}
