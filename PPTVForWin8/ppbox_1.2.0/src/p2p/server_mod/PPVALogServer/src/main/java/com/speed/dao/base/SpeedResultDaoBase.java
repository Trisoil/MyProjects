package com.speed.dao.base;
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
import com.speed.bean.SpeedResult;
public abstract class  SpeedResultDaoBase implements RowMapper{
      private String toSaveSql(SQLExecutor dbconn,SpeedResult speedResult,String split) throws Exception{
       if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(speedResult);
      Map<String,Object> dirtyProperties=speedResult.getDirtyProperties();
      if(dirtyProperties.size() == 0) return "";
        StringBuffer sb=new StringBuffer();
        sb.append("insert into " + TableBeanMap.getBeanTable(speedResult.getClass()) + split + " (");
        String[] props = dirtyProperties.keySet().toArray(new String[0]);
        for(int i = 0; i < props.length; i++) {
            String prop = props[i];
            String column = TableBeanMap.getColumnName(TableBeanMap.getBeanTable(speedResult.getClass()), prop);
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
    private String toUpdateSql(SQLExecutor dbconn,String byProperty,SpeedResult speedResult,String split) throws Exception{
       if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(speedResult);
       Map<String,Object> dirtyProperties=speedResult.getDirtyProperties();
       if(dirtyProperties.size() == 0) return "";
        String byColumn = TableBeanMap.getColumnName(speedResult.getClass(), byProperty);
        if(byColumn == null) throw new Exception("byColumn==null");
        StringBuffer sb=new StringBuffer();
        sb.append("update  " + TableBeanMap.getBeanTable(speedResult.getClass()) + split + " set ");
        String[] props = dirtyProperties.keySet().toArray(new String[0]);
        for(int i = 0; i < props.length; i++) {
            String prop = props[i];
            String column = TableBeanMap.getColumnName(TableBeanMap.getBeanTable(speedResult.getClass()), prop);
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
    private String toDeleteSql(String byProperty,SpeedResult speedResult,String split) throws Exception{
      if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(speedResult);
        String byColumn=TableBeanMap.getColumnName(speedResult.getClass(),byProperty);
        if(byColumn==null) throw new Exception("byColumn==null");
        String sql="delete from "+TableBeanMap.getBeanTable(speedResult.getClass())+split+" where "+byColumn+"=?";
        return sql;
    }
    public void save(SpeedResult speedResult,boolean withoutTransaction,String split) throws Exception{
             if(speedResult==null) return;
             SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
              String sql=this.toSaveSql(dbconn,speedResult,split);
              if(sql.equals("")) return;
            speedResult.setId((int)dbconn.execute(sql,true));
    }
    public void save(SpeedResult speedResult,boolean withoutTransaction) throws Exception{
            save(speedResult,withoutTransaction,null);
    }
    public void save(SpeedResult speedResult,String split) throws Exception{
            if(speedResult==null) return;
             SQLExecutor dbconn= SQLManager.getSQLExecutor();
              String sql=this.toSaveSql(dbconn,speedResult,split);
              if(sql.equals("")) return;
            speedResult.setId((int)dbconn.execute(sql,true));
    }
    public void save(SpeedResult speedResult) throws Exception{
            save(speedResult,null);
    }
    public int update(SpeedResult speedResult,String split) throws Exception{
           return update(null,speedResult,split);
    }
    public int update(SpeedResult speedResult) throws Exception{
           return update(speedResult,null);
    }
     public int update(String byProperty,SpeedResult speedResult,String split) throws Exception{
        if(speedResult==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor();
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toUpdateSql(dbconn,byProperty,speedResult,split);
        if(sql.equals("")) return 0;
        dbconn.addParam(getPropertyValue(speedResult,byProperty));
        int result= (int)dbconn.execute(sql);
         return result;
    }
    public int update(String byProperty,SpeedResult speedResult) throws Exception{
        return update(byProperty,speedResult,null);
    }
     public int update(SpeedResult speedResult,boolean withoutTransaction,String split) throws Exception{
            return update(null,speedResult,withoutTransaction,split);
    }
     public int update(SpeedResult speedResult,boolean withoutTransaction) throws Exception{
            return update(speedResult,withoutTransaction,null);
    }
    public int update(String byProperty,SpeedResult speedResult,boolean withoutTransaction,String split) throws Exception{
        if(speedResult==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toUpdateSql(dbconn,byProperty,speedResult,split);
        if(sql.equals("")) return 0;
        dbconn.addParam(getPropertyValue(speedResult,byProperty));
        int result= (int)dbconn.execute(sql);
         return result;
    }
    public int update(String byProperty,SpeedResult speedResult,boolean withoutTransaction) throws Exception{
       return  update(byProperty,speedResult,withoutTransaction,null);
    }
    public int delete(String byProperty,SpeedResult speedResult,String split) throws Exception{
          if(speedResult==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor();
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toDeleteSql(byProperty,speedResult,split);
        dbconn.addParam(getPropertyValue(speedResult,byProperty));
        int result=(int)dbconn.execute(sql);
        return result;
    }
    public int delete(String byProperty,SpeedResult speedResult) throws Exception{
          return  delete(byProperty,speedResult,null);
    }
     public int delete(SpeedResult speedResult,String split) throws Exception{
          return delete(null,speedResult,split);
    }
    public int delete(SpeedResult speedResult) throws Exception{
          return delete(speedResult,null);
    }
    public int delete(SpeedResult speedResult,boolean withoutTransaction,String split) throws Exception{
         return delete(null,speedResult,withoutTransaction,split);
    }
    public int delete(SpeedResult speedResult,boolean withoutTransaction) throws Exception{
         return delete(speedResult, withoutTransaction,null);
    }
    public int delete(String byProperty,SpeedResult speedResult,boolean withoutTransaction,String split) throws Exception{
        if(speedResult==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toDeleteSql(byProperty,speedResult,split);
        dbconn.addParam(getPropertyValue(speedResult,byProperty));
        int result=(int)dbconn.execute(sql);
        return result;
    }
    public int delete(String byProperty,SpeedResult speedResult,boolean withoutTransaction) throws Exception{
        return  delete(byProperty,speedResult,withoutTransaction,null);
    }
     public SpeedResult findSpeedResult(Object param) throws Exception{
           return findSpeedResult(null,param,null);
    }
    public SpeedResult findSpeedResult(String byProperty,Object param,String split) throws Exception{
        if(byProperty==null||byProperty.trim().equals("")) byProperty=getDefaultKeyProperty();
        if(split==null) split="";
        split=split.trim();
        String byColumn = TableBeanMap.getColumnName(SpeedResult.class, byProperty);
        String sql="select * from "+TableBeanMap.getBeanTable(SpeedResult.class)+split+" where "+byColumn+"=?";
        SQLExecutor dbconn= SQLManager.getSQLExecutor(true);
        dbconn.addParam(param);
        return dbconn.queryForBean(sql,SpeedResult.class);
    }
    private String getDefaultKeyProperty(){
           return "id";
    }
    private Object getPropertyValue(SpeedResult speedResult,String propertyName) throws Exception{
    if(propertyName==null||propertyName.equals("")) return null;
           if(propertyName.equalsIgnoreCase("id")){
                return speedResult.getId();
           }
           if(propertyName.equalsIgnoreCase("speedKey")){
                return speedResult.getSpeedKey();
           }
           if(propertyName.equalsIgnoreCase("resultID")){
                return speedResult.getResultID();
           }
           if(propertyName.equalsIgnoreCase("resultValue")){
                return speedResult.getResultValue();
           }
           if(propertyName.equalsIgnoreCase("completeTime")){
                return speedResult.getCompleteTime();
           }
           if(propertyName.equalsIgnoreCase("note")){
                return speedResult.getNote();
           }
           if(propertyName.equalsIgnoreCase("userIp")){
                return speedResult.getUserIp();
           }
           return null;
    }
    public Object mapRow(ResultSet rs, Map<String, String> columns) throws Exception {
        SpeedResult speedResult=new SpeedResult();
         Map<String,Object> valueMap=new HashMap<String,Object>();
         String[] lables=columns.keySet().toArray(new String[0]);
         String table=TableBeanMap.getBeanTable(speedResult.getClass());
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
         loadFromMap(valueMap,speedResult);
         valueMap=null;
         return speedResult;
    }
    private void loadFromMap(Map<String,Object> valueMap,SpeedResult speedResult) throws Exception{
        if(valueMap.containsKey("id")){
             speedResult.setId((java.lang.Integer)valueMap.get("id"));
        }
        if(valueMap.containsKey("speedKey")){
             speedResult.setSpeedKey((java.lang.String)valueMap.get("speedKey"));
        }
        if(valueMap.containsKey("resultID")){
             speedResult.setResultID((java.lang.String)valueMap.get("resultID"));
        }
        if(valueMap.containsKey("resultValue")){
             speedResult.setResultValue((java.lang.Integer)valueMap.get("resultValue"));
        }
        if(valueMap.containsKey("completeTime")){
             speedResult.setCompleteTime((java.util.Date)valueMap.get("completeTime"));
        }
        if(valueMap.containsKey("note")){
             speedResult.setNote((java.lang.String)valueMap.get("note"));
        }
        if(valueMap.containsKey("userIp")){
             speedResult.setUserIp((java.lang.String)valueMap.get("userIp"));
        }
    }
    public JSONObject toJSON(SpeedResult speedResult)  throws Exception{
        if(speedResult==null) return null;
        return speedResult.toJSON();
    }
    public SpeedResult fromJSON(JSONObject json) throws Exception{
       if(json==null) return null;
       SpeedResult speedResult=new  SpeedResult();
        speedResult.fromJSON(json);
        return speedResult;
    }
   public SpeedResult fromMap(Map<String,String> paramMap) throws Exception{
       SpeedResult speedResult=new  SpeedResult();
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
                    if(beanname.equalsIgnoreCase("SpeedResult")){
                        beanmap.put(propname.toLowerCase(),value);
                    }
                }else{
                    beanmap.put(key.toLowerCase(),value);
                }
            }
           speedResult.fromMap(beanmap);
       }
        return speedResult;
    }
    public abstract String getSplit(SpeedResult speedResult) throws Exception;
}
