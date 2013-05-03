package my.myorm.tool.jdbc;

import java.util.Map;
import java.util.HashMap;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.lang.reflect.Field;

import my.myorm.tool.orm.TableUtil;
import my.myorm.tool.orm.TableBeanMap;

/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2010-3-2
 * Time: 15:18:08
 * To change this template use File | Settings | File Templates.
 */
public class ResultSetHandler {
    private static Field field=null;
    static{
        try {
            field = com.mysql.jdbc.ResultSetMetaData.class.getDeclaredField("fields");
            field.setAccessible(true);
        } catch (Exception e) {
            e.printStackTrace();
        }

    }
    private String defaultTableName="_default_table_name_";
    protected ResultList handleResult(ResultSet rs,boolean isAll) throws Exception{
           ResultList results=new ResultList();

           Map<String,String> tableMap=new HashMap<String,String>();
           Map<String,Map<String,String>> columnsMap=new HashMap<String,Map<String,String>>();
           columnsMap.put(defaultTableName,new HashMap<String,String>());
           ResultSetMetaData metadata=rs.getMetaData();

           com.mysql.jdbc.Field[] fields=(com.mysql.jdbc.Field[])field.get(metadata);

           //表名和别名的映射,字段名和字段别名的映射
           if(fields!=null){
               for(int i=0;i<fields.length;i++){
                    com.mysql.jdbc.Field f=fields[i];
                    String oriTableName=f.getOriginalTableName();
                    String tableName=f.getTableName();
                    String columnName="";
                    String label=f.getName();
                    if(oriTableName==null||oriTableName.trim().equals("")){//不属于任何表
                         oriTableName=defaultTableName;
                         label=label.toLowerCase().trim();
                         
                         if( tableName.equals("ppva_log") ) {
                        	 tableName = "ppva_log";
                        	 columnName=f.getFullOriginalName().substring(f.getFullOriginalName().lastIndexOf(".")+1).trim();
                         } else {
                             tableName=defaultTableName;
                             columnName="";
                         }
                    }else{
                         oriTableName=oriTableName.trim().toLowerCase();
                         tableName=tableName.trim().toLowerCase();
                         label=label.trim().toLowerCase();
                         columnName=f.getFullOriginalName().substring(f.getFullOriginalName().lastIndexOf(".")+1).trim();
                    }
                    tableMap.put(tableName,oriTableName);
                    if(!columnsMap.containsKey(tableName)){
                        columnsMap.put(tableName,new HashMap<String,String>());
                    }
                    columnsMap.get(tableName).put(tableName+"."+label,columnName);
                    if(isAll){
                         columnsMap.get(defaultTableName).put(tableName+"."+label,columnName);
                    }
                    results.getListColumns().add(columnName);
               }
           }
           String[] tablenames=tableMap.keySet().toArray(new String[0]);
           while(rs.next()){
               RecordMap record=new RecordMap();
               for(int i=0;i<tablenames.length;i++){
                   if(tablenames[i].equals(defaultTableName) && !tablenames[i].equals("ppva_log")) continue;
                   String oriTableName=tableMap.get(tablenames[i]);
                   Map<String,String> columns=columnsMap.get(tablenames[i]);
                   if( (oriTableName.getBytes()[4] == '_' && oriTableName.getBytes()[7] == '_') || tablenames[i].equals("ppva_log") ) {
                	   oriTableName = "ppva_log";
                   }
                   
                   Class clazz= TableBeanMap.getBeanClass(TableUtil.getSummary(oriTableName));
                   if(clazz!=null) {//找到对应的bean
                       RowMapper mapper=TableBeanMap.getRowMapper(clazz);
                       Object obj=mapper.mapRow(rs,columns);
                       record.put(clazz,obj,tablenames[i]);
                   }else{//找不到，则使用DefaultBean,将该table对应的hashmap加入到默认map中
                      columnsMap.get(defaultTableName).putAll(columns);
                   }
               }
                DefaultBean defaultBean=new DefaultBean();
                defaultBean.mapRow(rs,columnsMap.get(defaultTableName));
                record.put(defaultBean.getClass(),defaultBean);
               results.add(record);
           }
           tableMap=null;
           columnsMap=null;
           metadata=null;
           fields=null;
           tablenames=null;
           return results;
    }
}
