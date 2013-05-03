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
import com.speed.bean.Speed;
public abstract class  SpeedDaoBase implements RowMapper{
      private String toSaveSql(SQLExecutor dbconn,Speed speed,String split) throws Exception{
       if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(speed);
      Map<String,Object> dirtyProperties=speed.getDirtyProperties();
      if(dirtyProperties.size() == 0) return "";
        StringBuffer sb=new StringBuffer();
        sb.append("insert into " + TableBeanMap.getBeanTable(speed.getClass()) + split + " (");
        String[] props = dirtyProperties.keySet().toArray(new String[0]);
        for(int i = 0; i < props.length; i++) {
            String prop = props[i];
            String column = TableBeanMap.getColumnName(TableBeanMap.getBeanTable(speed.getClass()), prop);
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
    private String toUpdateSql(SQLExecutor dbconn,String byProperty,Speed speed,String split) throws Exception{
       if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(speed);
       Map<String,Object> dirtyProperties=speed.getDirtyProperties();
       if(dirtyProperties.size() == 0) return "";
        String byColumn = TableBeanMap.getColumnName(speed.getClass(), byProperty);
        if(byColumn == null) throw new Exception("byColumn==null");
        StringBuffer sb=new StringBuffer();
        sb.append("update  " + TableBeanMap.getBeanTable(speed.getClass()) + split + " set ");
        String[] props = dirtyProperties.keySet().toArray(new String[0]);
        for(int i = 0; i < props.length; i++) {
            String prop = props[i];
            String column = TableBeanMap.getColumnName(TableBeanMap.getBeanTable(speed.getClass()), prop);
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
    private String toDeleteSql(String byProperty,Speed speed,String split) throws Exception{
      if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(speed);
        String byColumn=TableBeanMap.getColumnName(speed.getClass(),byProperty);
        if(byColumn==null) throw new Exception("byColumn==null");
        String sql="delete from "+TableBeanMap.getBeanTable(speed.getClass())+split+" where "+byColumn+"=?";
        return sql;
    }
    public void save(Speed speed,boolean withoutTransaction,String split) throws Exception{
             if(speed==null) return;
             SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
              String sql=this.toSaveSql(dbconn,speed,split);
              if(sql.equals("")) return;
            speed.setId((int)dbconn.execute(sql,true));
    }
    public void save(Speed speed,boolean withoutTransaction) throws Exception{
            save(speed,withoutTransaction,null);
    }
    public void save(Speed speed,String split) throws Exception{
            if(speed==null) return;
             SQLExecutor dbconn= SQLManager.getSQLExecutor();
              String sql=this.toSaveSql(dbconn,speed,split);
              if(sql.equals("")) return;
            speed.setId((int)dbconn.execute(sql,true));
    }
    public void save(Speed speed) throws Exception{
            save(speed,null);
    }
    public int update(Speed speed,String split) throws Exception{
           return update(null,speed,split);
    }
    public int update(Speed speed) throws Exception{
           return update(speed,null);
    }
     public int update(String byProperty,Speed speed,String split) throws Exception{
        if(speed==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor();
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toUpdateSql(dbconn,byProperty,speed,split);
        if(sql.equals("")) return 0;
        dbconn.addParam(getPropertyValue(speed,byProperty));
        int result= (int)dbconn.execute(sql);
         return result;
    }
    public int update(String byProperty,Speed speed) throws Exception{
        return update(byProperty,speed,null);
    }
     public int update(Speed speed,boolean withoutTransaction,String split) throws Exception{
            return update(null,speed,withoutTransaction,split);
    }
     public int update(Speed speed,boolean withoutTransaction) throws Exception{
            return update(speed,withoutTransaction,null);
    }
    public int update(String byProperty,Speed speed,boolean withoutTransaction,String split) throws Exception{
        if(speed==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toUpdateSql(dbconn,byProperty,speed,split);
        if(sql.equals("")) return 0;
        dbconn.addParam(getPropertyValue(speed,byProperty));
        int result= (int)dbconn.execute(sql);
         return result;
    }
    public int update(String byProperty,Speed speed,boolean withoutTransaction) throws Exception{
       return  update(byProperty,speed,withoutTransaction,null);
    }
    public int delete(String byProperty,Speed speed,String split) throws Exception{
          if(speed==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor();
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toDeleteSql(byProperty,speed,split);
        dbconn.addParam(getPropertyValue(speed,byProperty));
        int result=(int)dbconn.execute(sql);
        return result;
    }
    public int delete(String byProperty,Speed speed) throws Exception{
          return  delete(byProperty,speed,null);
    }
     public int delete(Speed speed,String split) throws Exception{
          return delete(null,speed,split);
    }
    public int delete(Speed speed) throws Exception{
          return delete(speed,null);
    }
    public int delete(Speed speed,boolean withoutTransaction,String split) throws Exception{
         return delete(null,speed,withoutTransaction,split);
    }
    public int delete(Speed speed,boolean withoutTransaction) throws Exception{
         return delete(speed, withoutTransaction,null);
    }
    public int delete(String byProperty,Speed speed,boolean withoutTransaction,String split) throws Exception{
        if(speed==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toDeleteSql(byProperty,speed,split);
        dbconn.addParam(getPropertyValue(speed,byProperty));
        int result=(int)dbconn.execute(sql);
        return result;
    }
    public int delete(String byProperty,Speed speed,boolean withoutTransaction) throws Exception{
        return  delete(byProperty,speed,withoutTransaction,null);
    }
     public Speed findSpeed(Object param) throws Exception{
           return findSpeed(null,param,null);
    }
    public Speed findSpeed(String byProperty,Object param,String split) throws Exception{
        if(byProperty==null||byProperty.trim().equals("")) byProperty=getDefaultKeyProperty();
        if(split==null) split="";
        split=split.trim();
        String byColumn = TableBeanMap.getColumnName(Speed.class, byProperty);
        String sql="select * from "+TableBeanMap.getBeanTable(Speed.class)+split+" where "+byColumn+"=?";
        SQLExecutor dbconn= SQLManager.getSQLExecutor(true);
        dbconn.addParam(param);
        return dbconn.queryForBean(sql,Speed.class);
    }
    private String getDefaultKeyProperty(){
           return "id";
    }
    private Object getPropertyValue(Speed speed,String propertyName) throws Exception{
    if(propertyName==null||propertyName.equals("")) return null;
           if(propertyName.equalsIgnoreCase("id")){
                return speed.getId();
           }
           if(propertyName.equalsIgnoreCase("speedKey")){
                return speed.getSpeedKey();
           }
           if(propertyName.equalsIgnoreCase("createTime")){
                return speed.getCreateTime();
           }
           if(propertyName.equalsIgnoreCase("resultID")){
                return speed.getResultID();
           }
           if(propertyName.equalsIgnoreCase("resultValue")){
                return speed.getResultValue();
           }
           if(propertyName.equalsIgnoreCase("completeTime")){
                return speed.getCompleteTime();
           }
           if(propertyName.equalsIgnoreCase("note")){
                return speed.getNote();
           }
           if(propertyName.equalsIgnoreCase("userIp")){
                return speed.getUserIp();
           }
           if(propertyName.equalsIgnoreCase("operator")){
                return speed.getOperator();
           }
           return null;
    }
    public Object mapRow(ResultSet rs, Map<String, String> columns) throws Exception {
        Speed speed=new Speed();
         Map<String,Object> valueMap=new HashMap<String,Object>();
         String[] lables=columns.keySet().toArray(new String[0]);
         String table=TableBeanMap.getBeanTable(speed.getClass());
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
         loadFromMap(valueMap,speed);
         valueMap=null;
         return speed;
    }
    private void loadFromMap(Map<String,Object> valueMap,Speed speed) throws Exception{
        if(valueMap.containsKey("id")){
             speed.setId((java.lang.Integer)valueMap.get("id"));
        }
        if(valueMap.containsKey("speedKey")){
             speed.setSpeedKey((java.lang.String)valueMap.get("speedKey"));
        }
        if(valueMap.containsKey("createTime")){
             speed.setCreateTime((java.util.Date)valueMap.get("createTime"));
        }
        if(valueMap.containsKey("resultID")){
             speed.setResultID((java.lang.String)valueMap.get("resultID"));
        }
        if(valueMap.containsKey("resultValue")){
             speed.setResultValue((java.lang.Integer)valueMap.get("resultValue"));
        }
        if(valueMap.containsKey("completeTime")){
             speed.setCompleteTime((java.util.Date)valueMap.get("completeTime"));
        }
        if(valueMap.containsKey("note")){
             speed.setNote((java.lang.String)valueMap.get("note"));
        }
        if(valueMap.containsKey("userIp")){
             speed.setUserIp((java.lang.String)valueMap.get("userIp"));
        }
        if(valueMap.containsKey("operator")){
             speed.setOperator((java.lang.String)valueMap.get("operator"));
        }
    }
    public JSONObject toJSON(Speed speed)  throws Exception{
        if(speed==null) return null;
        return speed.toJSON();
    }
    public Speed fromJSON(JSONObject json) throws Exception{
       if(json==null) return null;
       Speed speed=new  Speed();
        speed.fromJSON(json);
        return speed;
    }
   public Speed fromMap(Map<String,String> paramMap) throws Exception{
       Speed speed=new  Speed();
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
                    if(beanname.equalsIgnoreCase("Speed")){
                        beanmap.put(propname.toLowerCase(),value);
                    }
                }else{
                    beanmap.put(key.toLowerCase(),value);
                }
            }
           speed.fromMap(beanmap);
       }
        return speed;
    }
    public abstract String getSplit(Speed speed) throws Exception;
}
