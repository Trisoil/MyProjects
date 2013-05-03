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

import com.speed.bean.SysUser;
public abstract class  SysUserDaoBase implements RowMapper{
      private String toSaveSql(SQLExecutor dbconn,SysUser sysUser,String split) throws Exception{
       if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(sysUser);
      Map<String,Object> dirtyProperties=sysUser.getDirtyProperties();
      if(dirtyProperties.size() == 0) return "";
        StringBuffer sb=new StringBuffer();
        sb.append("insert into " + TableBeanMap.getBeanTable(sysUser.getClass()) + split + " (");
        String[] props = dirtyProperties.keySet().toArray(new String[0]);
        for(int i = 0; i < props.length; i++) {
            String prop = props[i];
            String column = TableBeanMap.getColumnName(TableBeanMap.getBeanTable(sysUser.getClass()), prop);
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
    private String toUpdateSql(SQLExecutor dbconn,String byProperty,SysUser sysUser,String split) throws Exception{
       if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(sysUser);
       Map<String,Object> dirtyProperties=sysUser.getDirtyProperties();
       if(dirtyProperties.size() == 0) return "";
        String byColumn = TableBeanMap.getColumnName(sysUser.getClass(), byProperty);
        if(byColumn == null) throw new Exception("byColumn==null");
        StringBuffer sb=new StringBuffer();
        sb.append("update  " + TableBeanMap.getBeanTable(sysUser.getClass()) + split + " set ");
        String[] props = dirtyProperties.keySet().toArray(new String[0]);
        for(int i = 0; i < props.length; i++) {
            String prop = props[i];
            String column = TableBeanMap.getColumnName(TableBeanMap.getBeanTable(sysUser.getClass()), prop);
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
    private String toDeleteSql(String byProperty,SysUser sysUser,String split) throws Exception{
      if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(sysUser);
        String byColumn=TableBeanMap.getColumnName(sysUser.getClass(),byProperty);
        if(byColumn==null) throw new Exception("byColumn==null");
        String sql="delete from "+TableBeanMap.getBeanTable(sysUser.getClass())+split+" where "+byColumn+"=?";
        return sql;
    }
    public void save(SysUser sysUser,boolean withoutTransaction,String split) throws Exception{
             if(sysUser==null) return;
             SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
              String sql=this.toSaveSql(dbconn,sysUser,split);
              if(sql.equals("")) return;
            sysUser.setId((int)dbconn.execute(sql,true));
    }
    public void save(SysUser sysUser,boolean withoutTransaction) throws Exception{
            save(sysUser,withoutTransaction,null);
    }
    public void save(SysUser sysUser,String split) throws Exception{
            if(sysUser==null) return;
             SQLExecutor dbconn= SQLManager.getSQLExecutor();
              String sql=this.toSaveSql(dbconn,sysUser,split);
              if(sql.equals("")) return;
            sysUser.setId((int)dbconn.execute(sql,true));
    }
    public void save(SysUser sysUser) throws Exception{
            save(sysUser,null);
    }
    public int update(SysUser sysUser,String split) throws Exception{
           return update(null,sysUser,split);
    }
    public int update(SysUser sysUser) throws Exception{
           return update(sysUser,null);
    }
     public int update(String byProperty,SysUser sysUser,String split) throws Exception{
        if(sysUser==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor();
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toUpdateSql(dbconn,byProperty,sysUser,split);
        if(sql.equals("")) return 0;
        dbconn.addParam(getPropertyValue(sysUser,byProperty));
        int result= (int)dbconn.execute(sql);
         return result;
    }
    public int update(String byProperty,SysUser sysUser) throws Exception{
        return update(byProperty,sysUser,null);
    }
     public int update(SysUser sysUser,boolean withoutTransaction,String split) throws Exception{
            return update(null,sysUser,withoutTransaction,split);
    }
     public int update(SysUser sysUser,boolean withoutTransaction) throws Exception{
            return update(sysUser,withoutTransaction,null);
    }
    public int update(String byProperty,SysUser sysUser,boolean withoutTransaction,String split) throws Exception{
        if(sysUser==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toUpdateSql(dbconn,byProperty,sysUser,split);
        if(sql.equals("")) return 0;
        dbconn.addParam(getPropertyValue(sysUser,byProperty));
        int result= (int)dbconn.execute(sql);
         return result;
    }
    public int update(String byProperty,SysUser sysUser,boolean withoutTransaction) throws Exception{
       return  update(byProperty,sysUser,withoutTransaction,null);
    }
    public int delete(String byProperty,SysUser sysUser,String split) throws Exception{
          if(sysUser==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor();
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toDeleteSql(byProperty,sysUser,split);
        dbconn.addParam(getPropertyValue(sysUser,byProperty));
        int result=(int)dbconn.execute(sql);
        return result;
    }
    public int delete(String byProperty,SysUser sysUser) throws Exception{
          return  delete(byProperty,sysUser,null);
    }
     public int delete(SysUser sysUser,String split) throws Exception{
          return delete(null,sysUser,split);
    }
    public int delete(SysUser sysUser) throws Exception{
          return delete(sysUser,null);
    }
    public int delete(SysUser sysUser,boolean withoutTransaction,String split) throws Exception{
         return delete(null,sysUser,withoutTransaction,split);
    }
    public int delete(SysUser sysUser,boolean withoutTransaction) throws Exception{
         return delete(sysUser, withoutTransaction,null);
    }
    public int delete(String byProperty,SysUser sysUser,boolean withoutTransaction,String split) throws Exception{
        if(sysUser==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toDeleteSql(byProperty,sysUser,split);
        dbconn.addParam(getPropertyValue(sysUser,byProperty));
        int result=(int)dbconn.execute(sql);
        return result;
    }
    public int delete(String byProperty,SysUser sysUser,boolean withoutTransaction) throws Exception{
        return  delete(byProperty,sysUser,withoutTransaction,null);
    }
     public SysUser findSysUser(Object param) throws Exception{
           return findSysUser(null,param,null);
    }
    public SysUser findSysUser(String byProperty,Object param,String split) throws Exception{
        if(byProperty==null||byProperty.trim().equals("")) byProperty=getDefaultKeyProperty();
        if(split==null) split="";
        split=split.trim();
        String byColumn = TableBeanMap.getColumnName(SysUser.class, byProperty);
        String sql="select * from "+TableBeanMap.getBeanTable(SysUser.class)+split+" where "+byColumn+"=?";
        SQLExecutor dbconn= SQLManager.getSQLExecutor(true);
        dbconn.addParam(param);
        return dbconn.queryForBean(sql,SysUser.class);
    }
    private String getDefaultKeyProperty(){
           return "id";
    }
    private Object getPropertyValue(SysUser sysUser,String propertyName) throws Exception{
    if(propertyName==null||propertyName.equals("")) return null;
           if(propertyName.equalsIgnoreCase("id")){
                return sysUser.getId();
           }
           if(propertyName.equalsIgnoreCase("userName")){
                return sysUser.getUserName();
           }
           if(propertyName.equalsIgnoreCase("password")){
                return sysUser.getPassword();
           }
           if(propertyName.equalsIgnoreCase("roleID")){
                return sysUser.getRoleID();
           }
           if(propertyName.equalsIgnoreCase("permission")){
                return sysUser.getPermission();
           }
           if(propertyName.equalsIgnoreCase("memo")){
                return sysUser.getMemo();
           }
           if(propertyName.equalsIgnoreCase("createTime")){
                return sysUser.getCreateTime();
           }
           return null;
    }
    public Object mapRow(ResultSet rs, Map<String, String> columns) throws Exception {
        SysUser sysUser=new SysUser();
         Map<String,Object> valueMap=new HashMap<String,Object>();
         String[] lables=columns.keySet().toArray(new String[0]);
         String table=TableBeanMap.getBeanTable(sysUser.getClass());
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
         loadFromMap(valueMap,sysUser);
         valueMap=null;
         return sysUser;
    }
    private void loadFromMap(Map<String,Object> valueMap,SysUser sysUser) throws Exception{
        if(valueMap.containsKey("id")){
             sysUser.setId((java.lang.Integer)valueMap.get("id"));
        }
        if(valueMap.containsKey("userName")){
             sysUser.setUserName((java.lang.String)valueMap.get("userName"));
        }
        if(valueMap.containsKey("password")){
             sysUser.setPassword((java.lang.String)valueMap.get("password"));
        }
        if(valueMap.containsKey("roleID")){
             sysUser.setRoleID((java.lang.Integer)valueMap.get("roleID"));
        }
        if(valueMap.containsKey("permission")){
             sysUser.setPermission((java.lang.String)valueMap.get("permission"));
        }
        if(valueMap.containsKey("memo")){
             sysUser.setMemo((java.lang.String)valueMap.get("memo"));
        }
        if(valueMap.containsKey("createTime")){
             sysUser.setCreateTime((java.util.Date)valueMap.get("createTime"));
        }
    }
    public JSONObject toJSON(SysUser sysUser)  throws Exception{
        if(sysUser==null) return null;
        JSONObject json=new JSONObject();
        SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        json.put("id", sysUser.getId());
        if(sysUser.getUserName()!=null){
           json.put("userName",sysUser.getUserName());
        }
        if(sysUser.getPassword()!=null){
           json.put("password",sysUser.getPassword());
        }
        json.put("roleID", sysUser.getRoleID());
        if(sysUser.getPermission()!=null){
           json.put("permission",sysUser.getPermission());
        }
        if(sysUser.getMemo()!=null){
           json.put("memo",sysUser.getMemo());
        }
        if(sysUser.getCreateTime()!=null){
             json.put("createTime",formatter.format(sysUser.getCreateTime()));
        }
        return json;
    }
    public SysUser fromJSON(String str) throws Exception{
       if(str==null||str.trim().equals("")) return null;
       JSONObject json=new JSONObject(str);
       SysUser sysUser=new  SysUser();
       SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
       if(json.get("id")!=null){
             sysUser.setId(Integer.parseInt(json.get("id")+""));
        }
        if(json.get("userName")!=null){
             sysUser.setUserName(json.get("userName")+"");
        }
        if(json.get("password")!=null){
             sysUser.setPassword(json.get("password")+"");
        }
       if(json.get("roleID")!=null){
             sysUser.setRoleID(Integer.parseInt(json.get("roleID")+""));
        }
        if(json.get("permission")!=null){
             sysUser.setPermission(json.get("permission")+"");
        }
        if(json.get("memo")!=null){
             sysUser.setMemo(json.get("memo")+"");
        }
        if(json.get("createTime")!=null){
             sysUser.setCreateTime(formatter.parse(json.get("createTime")+""));
        }
        return sysUser;
    }
    public abstract String getSplit(SysUser sysUser) throws Exception;
}
