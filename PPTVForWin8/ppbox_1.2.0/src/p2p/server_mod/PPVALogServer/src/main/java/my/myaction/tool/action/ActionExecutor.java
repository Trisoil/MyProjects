package my.myaction.tool.action;

import net.sf.cglib.reflect.FastMethod;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-12
 * Time: 17:39:41
 * To change this template use File | Settings | File Templates.
 */
public class ActionExecutor {
     public static void execute(String className,String methodName) throws Exception {
         String actionpackage=ActionInitializer.actionpackage;
        if(actionpackage.equals("")) actionpackage+=".";
        String fullname=(actionpackage+className).toLowerCase();
        String fullMethodName=(fullname+"."+methodName).toLowerCase();
        Object obj=ActionInitializer.objs.get(fullname);
        FastMethod method=ActionInitializer.methods.get(fullMethodName);
        if(obj==null||method==null) throw new Exception("invalid params");
         method.invoke(obj,null);
    }
}
