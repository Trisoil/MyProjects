package my.myorm.tool.jdbc;
import my.myorm.tool.orm.DataTypeMap;

import java.util.*;
import java.sql.ResultSet;
import java.sql.Timestamp;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-3-2
 * Time: 12:18:42
 * To change this template use File | Settings | File Templates.
 */
public class DefaultBean{
    private Map<String, List<Object[]>> data=new HashMap<String,List<Object[]>>();
    private Object get(Object key) throws Exception {
         if (key == null)
			return null;
		if (!(key instanceof String))
			return null;
		String thekey = (key+"").toLowerCase();
		List<Object[]> list = data.get(thekey);
		if (list != null) {// 说明传过来的是column
			if (list.size() == 0)
				return null;
			else
				return list.get(0)[1];
		}
		// 说明传过来的不是column
		String table = "";
		String column = "";
		int index = thekey.indexOf(".");
		if (index < 0) {
			column = thekey;
		} else {
			table = thekey.substring(0, index);
			column = thekey.substring(index + 1);
		}
		list = data.get(column);
		if (list == null || list.size() == 0)
			return null;
		if (list.size() == 1)
			return list.get(0)[1];
		if (table.equals(""))
			return list.get(0)[1];
		for (int i = 0; i < list.size(); i++) {
			Object[] objs = list.get(i);
			if ((objs[0] + "").equals(table)) {
				return objs[1];
			}
		}
		return null;
    }
    private void put(Object key,Object value) throws Exception {
         if(key==null) throw new Exception("key must not be null");
         if(!(key instanceof String)) throw new Exception("key should  be String");
         String thekey=(key+"").toLowerCase();
         int index= thekey.indexOf(".");
         if(index<0){
             throw new Exception("param err,at least one point");
         }
         index= thekey.indexOf(".");
         String table=thekey.substring(0,index);
         String column=thekey.substring(index+1);
         if(data.get(column)==null){
             data.put(column,new LinkedList<Object[]>());
         }
        List<Object[]> list=data.get(column);
        list.add(new Object[]{table,value});
    }

    public int getInt(String key) throws Exception {
        Object o=get(key);
        if(o==null){
            return DataTypeMap.DEFAULT_INT;
        }else{
           try{
                return Integer.parseInt(o.toString().trim());
            }catch(Exception ex){
                return  DataTypeMap.DEFAULT_INT;
            }
        }
    }
     public long getLong(String key) throws Exception {
        Object o=get(key);
        if(o==null){
            return DataTypeMap.DEFAULT_LONG;
        }else{
             try{
                return Long.parseLong(o.toString().trim());
            }catch(Exception ex){
                return  DataTypeMap.DEFAULT_LONG;
            }
        }
    }
     public float getFloat(String key) throws Exception {
        Object o=get(key);
        if(o==null){
            return DataTypeMap.DEFAULT_FLOAT;
        }else{
            try{
                return Float.parseFloat(o.toString().trim());
            }catch(Exception ex){
                return  DataTypeMap.DEFAULT_FLOAT;
            }
        }
    }

    public double getDouble(String key) throws Exception {
        Object o=get(key);
        if(o==null){
            return DataTypeMap.DEFAULT_DOUBLE;
        }else{
            try{
                return Double.parseDouble(o.toString().trim());
            }catch(Exception ex){
                return  DataTypeMap.DEFAULT_DOUBLE;
            }
        }
    }
     public boolean getBoolean(String key) throws Exception {
        Object o=get(key);
        if(o==null||!(o instanceof Boolean)){
            return DataTypeMap.DEFAULT_BOOLEAN;
        }else{
             try{
                return Boolean.parseBoolean(o.toString().trim());
            }catch(Exception ex){
                return  DataTypeMap.DEFAULT_BOOLEAN;
            }
        }
    }
     public String getString(String key) throws Exception {
        Object o=get(key);
        if(o==null){
            return DataTypeMap.DEFAULT_STRING;
        }else{
            return o.toString().trim();
        }
    }
    public Date getDate(String key) throws Exception {
        Object o=get(key);
        if(o==null||!((o instanceof java.sql.Date)||(o instanceof java.sql.Time)||(o instanceof Timestamp))){
            return null;
        }else{
            if(o instanceof java.sql.Date){
                return new Date(((java.sql.Date)o).getTime());
            }else if(o instanceof java.sql.Time){
                return new Date(((java.sql.Time)o).getTime());
            }else{
                return new Date(((java.sql.Timestamp)o).getTime());
            }
        }
    }

    public void mapRow(ResultSet rs, Map<String, String> columns) throws Exception {
        String[] labels=columns.keySet().toArray(new String[0]);
        for(int i=0;i<labels.length;i++){
            String table=labels[i].substring(0,labels[i].indexOf("."));
            if(table.equals("_default_table_name_")){
                 put(labels[i],rs.getObject(labels[i].substring(table.length()+1)));
            }else{
                 put(labels[i],rs.getObject(labels[i]));
            }
        }
        labels=null;
    }
}
