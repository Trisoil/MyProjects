package my.myaction.tool.action;

import net.sf.cglib.reflect.FastMethod;
import net.sf.cglib.reflect.FastClass;

import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.Map;
import java.util.HashMap;
import java.io.File;

import my.mysystem.tool.ContextUtil;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-4-1
 * Time: 15:29:52
 * To change this template use File | Settings | File Templates.
 */
public class ActionInitializer {
    protected static Map<String, Object> objs=new HashMap<String,Object>();
    protected static Map<String, FastMethod> methods=new HashMap<String,FastMethod>();
    protected static String actionpackage="";
    public static void init(String actionpackage) throws Exception{
        init(actionpackage,"");
    }
    private static void init(String actionpackage,String subactionpackage) throws Exception{
        ActionInitializer.actionpackage=actionpackage;
        String filePath= ContextUtil.rootPath +"WEB-INF/classes/"+replaceAllWith(actionpackage,".","/")+replaceAllWith(subactionpackage,".","/");
        if(!actionpackage.equals("")&&!actionpackage.endsWith(".")) actionpackage+=".";
        File file=new File(filePath);
        if(!file.exists()||!file.isDirectory()) return;
        String[] filenames=file.list();
        for(int i=0;i<filenames.length;i++){
            File subfile=new File(file.getPath(),filenames[i]);
            if(subfile.isDirectory()){
                init(actionpackage,subactionpackage.equals("")?filenames[i]:subactionpackage+"."+filenames[i]);
                continue;
            }
            int index=filenames[i].indexOf("\\$");
            if(index>-1) continue;
            index=filenames[i].indexOf(".class");
            if(index<=-1) continue;
            String fullClass=actionpackage+(subactionpackage.equals("")?"":subactionpackage+".")+filenames[i].substring(0,index);
            Class clazz=Class.forName(fullClass);
            int modifierIndex=clazz.getModifiers();
            String modifierStr= Modifier.toString(modifierIndex);
            if(!modifierStr.trim().startsWith("public")){//跳过非public的类
                continue;
            }
            if(modifierStr.indexOf(" static")>-1){//跳过static类
                continue;
            }
             if(modifierStr.indexOf(" abstract")>-1){//跳过抽象类
                continue;
            }
             if(clazz.isInterface()){//跳过接口类
                continue;
            }
            FastClass fastClazz=FastClass.create(clazz);
            Object obj=fastClazz.newInstance();
            objs.put(fullClass.toLowerCase(),obj);
            Method[] ms=clazz.getMethods();
            for(int j=0;j<ms.length;j++){
                 Method method=ms[j];
                 modifierIndex=clazz.getModifiers();
                 modifierStr= Modifier.toString(modifierIndex);
                 if(!modifierStr.trim().startsWith("public")){//跳过非public方法
                     continue;
                 }
                 if(modifierStr.indexOf(" static")>-1){//跳过static方法
                     continue;
                 }
                if(modifierStr.indexOf(" abstract")>-1){//跳过抽象方法
                     continue;
                 }
                Class[] paramsTypes=method.getParameterTypes();
                if(paramsTypes==null||paramsTypes.length>0){//跳过有参数的方法
                    continue;
                }
                 methods.put((fullClass+"."+method.getName()).toLowerCase(),fastClazz.getMethod(method));
            }
        }
    }
    private static String replaceAllWith(String str,String repl,String with){
         if(str==null) return "";
         while(str.indexOf(repl)>-1){
             str=str.replace(repl,with);
         }
         return str;
    }
}
