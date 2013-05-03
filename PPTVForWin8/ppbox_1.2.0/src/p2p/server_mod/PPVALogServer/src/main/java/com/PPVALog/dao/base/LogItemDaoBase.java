package com.PPVALog.dao.base;
import my.myorm.tool.jdbc.RowMapper;
import my.myorm.tool.jdbc.SQLExecutor;
import my.myorm.tool.jdbc.SQLManager;
import my.myorm.tool.orm.TableBeanMap;
import java.sql.ResultSet;
import java.util.Map;
import java.util.HashMap;
import java.util.Date;
import org.json.JSONObject;
import java.text.SimpleDateFormat;
import java.util.Iterator;

import com.PPVALog.bean.LogItem;
public abstract class  LogItemDaoBase implements RowMapper{
      private String toSaveSql(SQLExecutor dbconn,LogItem class20110627,String split) throws Exception{
       if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(class20110627);
      Map<String,Object> dirtyProperties=class20110627.getDirtyProperties();
      if(dirtyProperties.size() == 0) return "";
        StringBuffer sb=new StringBuffer();
        sb.append("insert into " + TableBeanMap.getBeanTable(class20110627.getClass()) + split + " (");
        String[] props = dirtyProperties.keySet().toArray(new String[0]);
        for(int i = 0; i < props.length; i++) {
            String prop = props[i];
            String column = TableBeanMap.getColumnName(TableBeanMap.getBeanTable(class20110627.getClass()), prop);
            sb.append(" ").append(column);
            dbconn.addParam(dirtyProperties.get(prop));
            if(i < props.length - 1) {
                sb.append(",");
            }
        }
        sb.append(" ) values (");
        for(int i = 0; i < props.length; i++) {
           sb.append("?");
            if(i < props.length - 1) {
                sb.append(",");
            }
        }
        sb.append(")");
        //dirtyProperties.clear();
        return sb.toString();
    }
    private String toUpdateSql(SQLExecutor dbconn,String byProperty,LogItem class20110627,String split) throws Exception{
       if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(class20110627);
       Map<String,Object> dirtyProperties=class20110627.getDirtyProperties();
       if(dirtyProperties.size() == 0) return "";
        String byColumn = TableBeanMap.getColumnName(class20110627.getClass(), byProperty);
        if(byColumn == null) throw new Exception("byColumn==null");
        StringBuffer sb=new StringBuffer();
        sb.append("update  " + TableBeanMap.getBeanTable(class20110627.getClass()) + split + " set ");
        String[] props = dirtyProperties.keySet().toArray(new String[0]);
        for(int i = 0; i < props.length; i++) {
            String prop = props[i];
            String column = TableBeanMap.getColumnName(TableBeanMap.getBeanTable(class20110627.getClass()), prop);
            sb.append(" ").append(column).append("=?");
            dbconn.addParam(dirtyProperties.get(prop));
            if(i < props.length - 1) {
                sb.append(",");
            }
        }
        sb.append(" where " + byColumn + "=?");
        //dirtyProperties.clear();
        return sb.toString();
    }
    private String toDeleteSql(String byProperty,LogItem class20110627,String split) throws Exception{
      if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(class20110627);
        String byColumn=TableBeanMap.getColumnName(class20110627.getClass(),byProperty);
        if(byColumn==null) throw new Exception("byColumn==null");
        String sql="delete from "+TableBeanMap.getBeanTable(class20110627.getClass())+split+" where "+byColumn+"=?";
        return sql;
    }
    public void save(LogItem class20110627,boolean withoutTransaction,String split) throws Exception{
             if(class20110627==null) return;
             SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
              String sql=this.toSaveSql(dbconn,class20110627,split);
              if(sql.equals("")) return;
            class20110627.setId((int)dbconn.execute(sql,true));
    }
    public void save(LogItem class20110627,boolean withoutTransaction) throws Exception{
            save(class20110627,withoutTransaction,null);
    }
    public void save(LogItem class20110627,String split) throws Exception{
            if(class20110627==null) return;
             SQLExecutor dbconn= SQLManager.getSQLExecutor();
              String sql=this.toSaveSql(dbconn,class20110627,split);
              if(sql.equals("")) return;
            class20110627.setId((int)dbconn.execute(sql,true));
    }
    public void save(LogItem class20110627) throws Exception{
            save(class20110627,null);
    }
    public int update(LogItem class20110627,String split) throws Exception{
           return update(null,class20110627,split);
    }
    public int update(LogItem class20110627) throws Exception{
           return update(class20110627,null);
    }
     public int update(String byProperty,LogItem class20110627,String split) throws Exception{
        if(class20110627==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor();
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toUpdateSql(dbconn,byProperty,class20110627,split);
        if(sql.equals("")) return 0;
        dbconn.addParam(getPropertyValue(class20110627,byProperty));
        int result= (int)dbconn.execute(sql);
         return result;
    }
    public int update(String byProperty,LogItem class20110627) throws Exception{
        return update(byProperty,class20110627,null);
    }
     public int update(LogItem class20110627,boolean withoutTransaction,String split) throws Exception{
            return update(null,class20110627,withoutTransaction,split);
    }
     public int update(LogItem class20110627,boolean withoutTransaction) throws Exception{
            return update(class20110627,withoutTransaction,null);
    }
    public int update(String byProperty,LogItem class20110627,boolean withoutTransaction,String split) throws Exception{
        if(class20110627==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toUpdateSql(dbconn,byProperty,class20110627,split);
        if(sql.equals("")) return 0;
        dbconn.addParam(getPropertyValue(class20110627,byProperty));
        int result= (int)dbconn.execute(sql);
         return result;
    }
    public int update(String byProperty,LogItem class20110627,boolean withoutTransaction) throws Exception{
       return  update(byProperty,class20110627,withoutTransaction,null);
    }
    public int delete(String byProperty,LogItem class20110627,String split) throws Exception{
          if(class20110627==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor();
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toDeleteSql(byProperty,class20110627,split);
        dbconn.addParam(getPropertyValue(class20110627,byProperty));
        int result=(int)dbconn.execute(sql);
        return result;
    }
    public int delete(String byProperty,LogItem class20110627) throws Exception{
          return  delete(byProperty,class20110627,null);
    }
     public int delete(LogItem class20110627,String split) throws Exception{
          return delete(null,class20110627,split);
    }
    public int delete(LogItem class20110627) throws Exception{
          return delete(class20110627,null);
    }
    public int delete(LogItem class20110627,boolean withoutTransaction,String split) throws Exception{
         return delete(null,class20110627,withoutTransaction,split);
    }
    public int delete(LogItem class20110627,boolean withoutTransaction) throws Exception{
         return delete(class20110627, withoutTransaction,null);
    }
    public int delete(String byProperty,LogItem class20110627,boolean withoutTransaction,String split) throws Exception{
        if(class20110627==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toDeleteSql(byProperty,class20110627,split);
        dbconn.addParam(getPropertyValue(class20110627,byProperty));
        int result=(int)dbconn.execute(sql);
        return result;
    }
    public int delete(String byProperty,LogItem class20110627,boolean withoutTransaction) throws Exception{
        return  delete(byProperty,class20110627,withoutTransaction,null);
    }
     public LogItem findClass20110627(Object param) throws Exception{
           return findClass20110627(null,param,null);
    }
    public LogItem findClass20110627(String byProperty,Object param,String split) throws Exception{
        if(byProperty==null||byProperty.trim().equals("")) byProperty=getDefaultKeyProperty();
        if(split==null) split="";
        split=split.trim();
        String byColumn = TableBeanMap.getColumnName(LogItem.class, byProperty);
        String sql="select * from "+TableBeanMap.getBeanTable(LogItem.class)+split+" where "+byColumn+"=?";
        SQLExecutor dbconn= SQLManager.getSQLExecutor(true);
        dbconn.addParam(param);
        return dbconn.queryForBean(sql,LogItem.class);
    }
    private String getDefaultKeyProperty(){
           return "id";
    }
    private Object getPropertyValue(LogItem class20110627,String propertyName) throws Exception{
    if(propertyName==null||propertyName.equals("")) return null;
           if(propertyName.equalsIgnoreCase("id")){
                return class20110627.getId();
           }
           if(propertyName.equalsIgnoreCase("address")){
                return class20110627.getAddress();
           }
           if(propertyName.equalsIgnoreCase("peer_version")){
                return class20110627.getPeer_version();
           }
           if(propertyName.equalsIgnoreCase("peer_guid")){
                return class20110627.getPeer_guid();
           }
           if(propertyName.equalsIgnoreCase("time")){
                return class20110627.getTime();
           }
           if(propertyName.equalsIgnoreCase("log_file_md5")){
                return class20110627.getLog_file_md5();
           }
           if(propertyName.equalsIgnoreCase("resource_id")){
                return class20110627.getResource_id();
           }
           if(propertyName.equalsIgnoreCase("resource_name")){
                return class20110627.getResource_name();
           }
           if(propertyName.equalsIgnoreCase("statistics_id")){
                return class20110627.getStatistics_id();
           }
           return null;
    }
    public Object mapRow(ResultSet rs, Map<String, String> columns) throws Exception {
        LogItem class20110627=new LogItem();
         Map<String,Object> valueMap=new HashMap<String,Object>();
         String[] lables=columns.keySet().toArray(new String[0]);
         String table=TableBeanMap.getBeanTable(class20110627.getClass());
         for(int i=0;i<lables.length;i++){
              String propertyName= TableBeanMap.getPropertName(table,columns.get(lables[i]));
              Object value=rs.getObject(lables[i]);
              if(value==null) continue;
              if(value instanceof java.sql.Date){
                          java.sql.Date o=(java.sql.Date)value;
                          value=new Date(o.getTime());
              }else if(value instanceof java.sql.Time){
                          java.sql.Time o=(java.sql.Time)value;
                          value=new Date(o.getTime());
              }else if(value instanceof java.sql.Timestamp){
                          java.sql.Timestamp o=(java.sql.Timestamp)value;
                          value=new Date(o.getTime());
               }else if(value instanceof java.math.BigDecimal){
                          value=((java.math.BigDecimal)value).doubleValue();
               }else if(value instanceof java.math.BigInteger){
                          value=((java.math.BigInteger)value).longValue();
               }
               valueMap.put(propertyName,value);
         }
         loadFromMap(valueMap,class20110627);
         valueMap=null;
         return class20110627;
    }
    private void loadFromMap(Map<String,Object> valueMap,LogItem class20110627) throws Exception{
        if(valueMap.containsKey("id")){
             class20110627.setId((java.lang.Integer)valueMap.get("id"));
        }
        if(valueMap.containsKey("address")){
             class20110627.setAddress((java.lang.String)valueMap.get("address"));
        }
        if(valueMap.containsKey("peer_version")){
             class20110627.setPeer_version((java.lang.String)valueMap.get("peer_version"));
        }
        if(valueMap.containsKey("peer_guid")){
             class20110627.setPeer_guid((java.lang.String)valueMap.get("peer_guid"));
        }
        if(valueMap.containsKey("time")){
             class20110627.setTime((java.util.Date)valueMap.get("time"));
        }
        if(valueMap.containsKey("log_file_md5")){
             class20110627.setLog_file_md5((java.lang.String)valueMap.get("log_file_md5"));
        }
        if(valueMap.containsKey("resource_id")){
             class20110627.setResource_id((java.lang.String)valueMap.get("resource_id"));
        }
        if(valueMap.containsKey("resource_name")){
             class20110627.setResource_name((java.lang.String)valueMap.get("resource_name"));
        }
        if(valueMap.containsKey("statistics_id")){
             class20110627.setStatistics_id((java.lang.String)valueMap.get("statistics_id"));
        }
    }
    public JSONObject toJSON(LogItem class20110627)  throws Exception{
        if(class20110627==null) return null;
        return class20110627.toJSON();
    }
    public LogItem fromJSON(JSONObject json) throws Exception{
       if(json==null) return null;
       LogItem class20110627=new  LogItem();
        class20110627.fromJSON(json);
        return class20110627;
    }
   public LogItem fromMap(Map<String,String> paramMap) throws Exception{
       LogItem class20110627=new  LogItem();
       if(paramMap!=null){
           Map<String,String> beanmap=new HashMap<String,String>();
           Iterator it = paramMap.entrySet().iterator();
            while (it.hasNext()) {
                Map.Entry entry = (Map.Entry) it.next();
                String key = (String) entry.getKey();
                String value =(String)entry.getValue();
                if(key==null) continue;
                int dotindex=key.indexOf(".");
                if(dotindex>-1){
                    String beanname=key.substring(0,dotindex);
                    String propname=key.substring(dotindex+1);
                    if(beanname.equalsIgnoreCase("Class20110627")){
                        beanmap.put(propname.toLowerCase(),value);
                    }
                }else{
                    beanmap.put(key.toLowerCase(),value);
                }
            }
           class20110627.fromMap(beanmap);
       }
        return class20110627;
    }
    public abstract String getSplit(LogItem class20110627) throws Exception;
}
