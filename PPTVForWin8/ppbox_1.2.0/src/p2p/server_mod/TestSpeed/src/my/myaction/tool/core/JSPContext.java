package my.myaction.tool.core;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-1
 * Time: 16:50:10
 * To change this template use File | Settings | File Templates.
 */
public class JSPContext {
    private static ThreadLocal local=new ThreadLocal();
    protected static void put(HttpServletRequest request, HttpServletResponse response){

        Object[] params=new Object[2];
        params[0]=request;
        params[1]=response;
        local.set(params);
    }
    public static HttpServletRequest getRequest(){
       Object[] params=(Object[])local.get();
       if(params==null) return null;
       return (HttpServletRequest)params[0];
    }

    public static HttpServletResponse getResponse(){
       Object[] params=(Object[])local.get();
       if(params==null) return null;
       return  (HttpServletResponse)params[1];
    }   
}
