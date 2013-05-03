package com.speed.dao.base;
import java.sql.ResultSet;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import my.myorm.tool.jdbc.RowMapper;
import my.myorm.tool.jdbc.SQLExecutor;
import my.myorm.tool.jdbc.SQLManager;
import my.myorm.tool.orm.TableBeanMap;

import org.json.JSONObject;

import com.speed.bean.SysLog;
public abstract class  SysLogDaoBase implements RowMapper{
      private String toSaveSql(SQLExecutor dbconn,SysLog sysLog,String split) throws Exception{
       if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(sysLog);
      Map<String,Object> dirtyProperties=sysLog.getDirtyProperties();
      if(dirtyProperties.size() == 0) return "";
        StringBuffer sb=new StringBuffer();
        sb.append("insert into " + TableBeanMap.getBeanTable(sysLog.getClass()) + split + " (");
        String[] props = dirtyProperties.keySet().toArray(new String[0]);
        for(int i = 0; i < props.length; i++) {
            String prop = props[i];
            String column = TableBeanMap.getColumnName(TableBeanMap.getBeanTable(sysLog.getClass()), prop);
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
    private String toUpdateSql(SQLExecutor dbconn,String byProperty,SysLog sysLog,String split) throws Exception{
       if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(sysLog);
       Map<String,Object> dirtyProperties=sysLog.getDirtyProperties();
       if(dirtyProperties.size() == 0) return "";
        String byColumn = TableBeanMap.getColumnName(sysLog.getClass(), byProperty);
        if(byColumn == null) throw new Exception("byColumn==null");
        StringBuffer sb=new StringBuffer();
        sb.append("update  " + TableBeanMap.getBeanTable(sysLog.getClass()) + split + " set ");
        String[] props = dirtyProperties.keySet().toArray(new String[0]);
        for(int i = 0; i < props.length; i++) {
            String prop = props[i];
            String column = TableBeanMap.getColumnName(TableBeanMap.getBeanTable(sysLog.getClass()), prop);
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
    private String toDeleteSql(String byProperty,SysLog sysLog,String split) throws Exception{
      if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(sysLog);
        String byColumn=TableBeanMap.getColumnName(sysLog.getClass(),byProperty);
        if(byColumn==null) throw new Exception("byColumn==null");
        String sql="delete from "+TableBeanMap.getBeanTable(sysLog.getClass())+split+" where "+byColumn+"=?";
        return sql;
    }
    public void save(SysLog sysLog,boolean withoutTransaction,String split) throws Exception{
             if(sysLog==null) return;
             SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
              String sql=this.toSaveSql(dbconn,sysLog,split);
              if(sql.equals("")) return;
            sysLog.setId((int)dbconn.execute(sql,true));
    }
    public void save(SysLog sysLog,boolean withoutTransaction) throws Exception{
            save(sysLog,withoutTransaction,null);
    }
    public void save(SysLog sysLog,String split) throws Exception{
            if(sysLog==null) return;
             SQLExecutor dbconn= SQLManager.getSQLExecutor();
              String sql=this.toSaveSql(dbconn,sysLog,split);
              if(sql.equals("")) return;
            sysLog.setId((int)dbconn.execute(sql,true));
    }
    public void save(SysLog sysLog) throws Exception{
            save(sysLog,null);
    }
    public int update(SysLog sysLog,String split) throws Exception{
           return update(null,sysLog,split);
    }
    public int update(SysLog sysLog) throws Exception{
           return update(sysLog,null);
    }
     public int update(String byProperty,SysLog sysLog,String split) throws Exception{
        if(sysLog==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor();
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toUpdateSql(dbconn,byProperty,sysLog,split);
        if(sql.equals("")) return 0;
        dbconn.addParam(getPropertyValue(sysLog,byProperty));
        int result= (int)dbconn.execute(sql);
         return result;
    }
    public int update(String byProperty,SysLog sysLog) throws Exception{
        return update(byProperty,sysLog,null);
    }
     public int update(SysLog sysLog,boolean withoutTransaction,String split) throws Exception{
            return update(null,sysLog,withoutTransaction,split);
    }
     public int update(SysLog sysLog,boolean withoutTransaction) throws Exception{
            return update(sysLog,withoutTransaction,null);
    }
    public int update(String byProperty,SysLog sysLog,boolean withoutTransaction,String split) throws Exception{
        if(sysLog==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toUpdateSql(dbconn,byProperty,sysLog,split);
        if(sql.equals("")) return 0;
        dbconn.addParam(getPropertyValue(sysLog,byProperty));
        int result= (int)dbconn.execute(sql);
         return result;
    }
    public int update(String byProperty,SysLog sysLog,boolean withoutTransaction) throws Exception{
       return  update(byProperty,sysLog,withoutTransaction,null);
    }
    public int delete(String byProperty,SysLog sysLog,String split) throws Exception{
          if(sysLog==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor();
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toDeleteSql(byProperty,sysLog,split);
        dbconn.addParam(getPropertyValue(sysLog,byProperty));
        int result=(int)dbconn.execute(sql);
        return result;
    }
    public int delete(String byProperty,SysLog sysLog) throws Exception{
          return  delete(byProperty,sysLog,null);
    }
     public int delete(SysLog sysLog,String split) throws Exception{
          return delete(null,sysLog,split);
    }
    public int delete(SysLog sysLog) throws Exception{
          return delete(sysLog,null);
    }
    public int delete(SysLog sysLog,boolean withoutTransaction,String split) throws Exception{
         return delete(null,sysLog,withoutTransaction,split);
    }
    public int delete(SysLog sysLog,boolean withoutTransaction) throws Exception{
         return delete(sysLog, withoutTransaction,null);
    }
    public int delete(String byProperty,SysLog sysLog,boolean withoutTransaction,String split) throws Exception{
        if(sysLog==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toDeleteSql(byProperty,sysLog,split);
        dbconn.addParam(getPropertyValue(sysLog,byProperty));
        int result=(int)dbconn.execute(sql);
        return result;
    }
    public int delete(String byProperty,SysLog sysLog,boolean withoutTransaction) throws Exception{
        return  delete(byProperty,sysLog,withoutTransaction,null);
    }
     public SysLog findSysLog(Object param) throws Exception{
           return findSysLog(null,param,null);
    }
    public SysLog findSysLog(String byProperty,Object param,String split) throws Exception{
        if(byProperty==null||byProperty.trim().equals("")) byProperty=getDefaultKeyProperty();
        if(split==null) split="";
        split=split.trim();
        String byColumn = TableBeanMap.getColumnName(SysLog.class, byProperty);
        String sql="select * from "+TableBeanMap.getBeanTable(SysLog.class)+split+" where "+byColumn+"=?";
        SQLExecutor dbconn= SQLManager.getSQLExecutor(true);
        dbconn.addParam(param);
        return dbconn.queryForBean(sql,SysLog.class);
    }
    private String getDefaultKeyProperty(){
           return "id";
    }
    private Object getPropertyValue(SysLog sysLog,String propertyName) throws Exception{
    if(propertyName==null||propertyName.equals("")) return null;
           if(propertyName.equalsIgnoreCase("id")){
                return sysLog.getId();
           }
           if(propertyName.equalsIgnoreCase("userName")){
                return sysLog.getUserName();
           }
           if(propertyName.equalsIgnoreCase("operate")){
                return sysLog.getOperate();
           }
           if(propertyName.equalsIgnoreCase("content")){
                return sysLog.getContent();
           }
           if(propertyName.equalsIgnoreCase("iP")){
                return sysLog.getIP();
           }
           if(propertyName.equalsIgnoreCase("createTime")){
                return sysLog.getCreateTime();
           }
           return null;
    }
    public Object mapRow(ResultSet rs, Map<String, String> columns) throws Exception {
        SysLog sysLog=new SysLog();
         Map<String,Object> valueMap=new HashMap<String,Object>();
         String[] lables=columns.keySet().toArray(new String[0]);
         String table=TableBeanMap.getBeanTable(sysLog.getClass());
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
         loadFromMap(valueMap,sysLog);
         valueMap=null;
         return sysLog;
    }
    private void loadFromMap(Map<String,Object> valueMap,SysLog sysLog) throws Exception{
        if(valueMap.containsKey("id")){
             sysLog.setId((java.lang.Integer)valueMap.get("id"));
        }
        if(valueMap.containsKey("userName")){
             sysLog.setUserName((java.lang.String)valueMap.get("userName"));
        }
        if(valueMap.containsKey("operate")){
             sysLog.setOperate((java.lang.String)valueMap.get("operate"));
        }
        if(valueMap.containsKey("content")){
             sysLog.setContent((java.lang.String)valueMap.get("content"));
        }
        if(valueMap.containsKey("iP")){
             sysLog.setIP((java.lang.String)valueMap.get("iP"));
        }
        if(valueMap.containsKey("createTime")){
             sysLog.setCreateTime((java.util.Date)valueMap.get("createTime"));
        }
    }
    public JSONObject toJSON(SysLog sysLog)  throws Exception{
        if(sysLog==null) return null;
        JSONObject json=new JSONObject();
        SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        json.put("id", sysLog.getId());
        if(sysLog.getUserName()!=null){
           json.put("userName",sysLog.getUserName());
        }
        if(sysLog.getOperate()!=null){
           json.put("operate",sysLog.getOperate());
        }
        if(sysLog.getContent()!=null){
           json.put("content",sysLog.getContent());
        }
        if(sysLog.getIP()!=null){
           json.put("iP",sysLog.getIP());
        }
        if(sysLog.getCreateTime()!=null){
             json.put("createTime",formatter.format(sysLog.getCreateTime()));
        }
        return json;
    }
    public SysLog fromJSON(String str) throws Exception{
       if(str==null||str.trim().equals("")) return null;
       JSONObject json=new JSONObject(str);
       SysLog sysLog=new  SysLog();
       SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
       if(json.get("id")!=null){
             sysLog.setId(Integer.parseInt(json.get("id")+""));
        }
        if(json.get("userName")!=null){
             sysLog.setUserName(json.get("userName")+"");
        }
        if(json.get("operate")!=null){
             sysLog.setOperate(json.get("operate")+"");
        }
        if(json.get("content")!=null){
             sysLog.setContent(json.get("content")+"");
        }
        if(json.get("iP")!=null){
             sysLog.setIP(json.get("iP")+"");
        }
        if(json.get("createTime")!=null){
             sysLog.setCreateTime(formatter.parse(json.get("createTime")+""));
        }
        return sysLog;
    }
    public abstract String getSplit(SysLog sysLog) throws Exception;
}
