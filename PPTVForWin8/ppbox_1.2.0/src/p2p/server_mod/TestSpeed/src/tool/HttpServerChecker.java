package tool;

import java.net.URL;
import java.net.MalformedURLException;
import java.net.URLConnection;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2009-12-16
 * Time: 13:27:00
 * To change this template use File | Settings | File Templates.
 */
public class HttpServerChecker {
    public static boolean isRunning(String ip) throws Exception {
        boolean isRun=false;
        try{
             URL url=new URL("http://"+ip);
             URLConnection conn=url.openConnection();
             conn.connect();
             isRun=true;
        }catch(Exception ex){
            if(ex instanceof MalformedURLException){
                throw ex;
            }
        }
        return isRun;
    }
    public static void main(String[] args) throws Exception {
        System.out.println(isRunning("127.0.0.1:8081"));
    }
}
