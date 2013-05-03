package my.myorm.tool.orm;

import org.jdom.input.SAXBuilder;
import org.jdom.Document;
import org.jdom.Element;

import java.util.Map;
import java.util.HashMap;
import java.util.List;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;

import my.myorm.tool.jdbc.RowMapper;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-3-11
 * Time: 10:29:05
 * To change this template use File | Settings | File Templates.
 */
public class TableBeanMap {
    private static Map<String, Class> tableBeanMap = new HashMap<String, Class>();
    private static Map<Class, String> beanTableMap = new HashMap<Class, String>();
    private static Map<Class, RowMapper> RowMapperMap = new HashMap<Class, RowMapper>();
    private static Map<String, String> columnPropertyMap = new HashMap<String, String>();
    private static Map<String, String> propertyColumnMap = new HashMap<String, String>();
    private static Map<String, String> ignorCasePropertyColumnMap = new HashMap<String, String>();
    private static String splitSpec="";

    public static void load(String mapFile) throws Exception {
        File file = new File(mapFile);
        if(!file.exists()){
            System.out.println("找不到bean映射文件:"+mapFile);
            return;
        }
        SAXBuilder sb = new SAXBuilder(false);
        InputStream is = new FileInputStream(file);
        try {
            Document doc = sb.build(is);
            splitSpec= doc.getRootElement().getAttributeValue("splitspec");
            if(splitSpec==null) splitSpec=""; 
            List<Element> tables = doc.getRootElement().getChildren();
            for (Element table : tables) {
                String tablename = table.getAttributeValue("tableName");
                String beanname = table.getAttributeValue("beanName");
                Class beanClass = Class.forName(beanname);
                tableBeanMap.put(tablename.trim().toLowerCase(), beanClass);
                beanTableMap.put(beanClass, tablename.trim().toLowerCase());
                RowMapperMap.put(beanClass,(RowMapper)Class.forName(getDaoPath(beanClass)).newInstance());
                List<Element> columns = table.getChildren();
                for (Element column : columns) {
                    String columnname = column.getAttributeValue("columnName");
                    String propertyname = column.getAttributeValue("propertyName");
                    columnPropertyMap.put(tablename.trim().toLowerCase() + ":" + columnname.trim().toLowerCase(), propertyname.trim());
                    propertyColumnMap.put(tablename.trim().toLowerCase() + ":" + propertyname.trim(), columnname.trim().toLowerCase());
                    ignorCasePropertyColumnMap.put(tablename.trim().toLowerCase() + ":" + propertyname.trim().toLowerCase(), columnname.trim().toLowerCase());
                }
            }
        }catch(Exception ex){
            throw ex;
        }finally {
            is.close();
            sb = null;
        }

    }
    public static Class getBeanClass(String tableName){
        if(tableName==null) return null;
        return tableBeanMap.get(tableName.trim().toLowerCase());
    }
    public static RowMapper getRowMapper(Class clazz){
        if(clazz==null) return null;
        return RowMapperMap.get(clazz);
    }
     public static String getBeanTable(Class clazz){
        if(clazz==null) return null;
        return beanTableMap.get(clazz);
    }
    public static String getPropertName(String table,String column){
        if(table==null||column==null) return null;
        return columnPropertyMap.get(table.trim().toLowerCase()+":"+column.trim().toLowerCase());
    }
    public static String getColumnName(String table,String property){
        if(table==null||property==null) return null;
        return   propertyColumnMap.get(table.trim().toLowerCase()+":"+property.trim());
    }
    public static String getColumnName(Class clazz,String ignorCaseProperty){
        if(clazz==null||ignorCaseProperty==null) return null;
        String table=getBeanTable(clazz);
        if(table==null) return null;
        return   ignorCasePropertyColumnMap.get(table.trim().toLowerCase()+":"+ignorCaseProperty.trim().toLowerCase());
    }

    private static String getDaoPath(Class beanClass){
        String beanFullPath=beanClass.getCanonicalName();
        int lastPoint=beanFullPath.lastIndexOf(".");
        String beanName=beanFullPath.substring(lastPoint+1);
        String daoName=beanName+"Dao";
        String beanPath=beanFullPath.substring(0,lastPoint);
        int lastPoint1=beanPath.lastIndexOf(".");
        String basicPath=beanPath.substring(0,lastPoint1);
        return basicPath+".dao."+daoName;
    }
    public static String  getSplitSpec(){
        return splitSpec;
    }
}
