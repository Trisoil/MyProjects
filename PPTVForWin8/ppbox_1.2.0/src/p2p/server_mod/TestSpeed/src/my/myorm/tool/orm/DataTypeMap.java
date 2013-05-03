package my.myorm.tool.orm;

import java.util.Map;
import java.util.HashMap;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2009-9-24
 * Time: 14:44:57
 * To change this template use File | Settings | File Templates.
 */
public class DataTypeMap {
    private static Map<Class,Class> typeMap=new HashMap<Class,Class>();
    public static String DEFAULT_DATE="1970-01-01";
    public static byte DEFAULT_BYTE=0;
    public static int DEFAULT_INT=0;
    public static long DEFAULT_LONG=0;
    public static float DEFAULT_FLOAT=0;
    public static double DEFAULT_DOUBLE=0;
    public static Boolean DEFAULT_BOOLEAN=false;
    public static String DEFAULT_STRING="";

   public static void init() throws Exception{
       typeMap.put(java.lang.Byte.class,java.lang.Byte.class);
       typeMap.put(java.lang.Integer.class,java.lang.Integer.class);
       typeMap.put(java.lang.Long.class,java.lang.Long.class);
       typeMap.put(java.lang.Float.class,java.lang.Float.class);
       typeMap.put(java.lang.Double.class,java.lang.Double.class);
       typeMap.put(java.lang.String.class,java.lang.String.class);
       typeMap.put(java.lang.Boolean.class,java.lang.Boolean.class);
       typeMap.put(java.math.BigDecimal.class,java.lang.Double.class);
       typeMap.put(java.sql.Date.class, java.util.Date.class);
       typeMap.put(java.sql.Time.class,java.util.Date.class);
       typeMap.put(java.sql.Timestamp.class, java.util.Date.class);
   }

   public static Class getDataTypeClass(Class clazz){
       return typeMap.get(clazz);
   }
}
