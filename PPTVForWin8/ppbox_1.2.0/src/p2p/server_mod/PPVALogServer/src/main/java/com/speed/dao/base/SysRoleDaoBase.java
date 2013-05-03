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

import com.speed.bean.SysRole;
public abstract class  SysRoleDaoBase implements RowMapper{
      private String toSaveSql(SQLExecutor dbconn,SysRole sysRole,String split) throws Exception{
       if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(sysRole);
      Map<String,Object> dirtyProperties=sysRole.getDirtyProperties();
      if(dirtyProperties.size() == 0) return "";
        StringBuffer sb=new StringBuffer();
        sb.append("insert into " + TableBeanMap.getBeanTable(sysRole.getClass()) + split + " (");
        String[] props = dirtyProperties.keySet().toArray(new String[0]);
        for(int i = 0; i < props.length; i++) {
            String prop = props[i];
            String column = TableBeanMap.getColumnName(TableBeanMap.getBeanTable(sysRole.getClass()), prop);
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
    private String toUpdateSql(SQLExecutor dbconn,String byProperty,SysRole sysRole,String split) throws Exception{
       if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(sysRole);
       Map<String,Object> dirtyProperties=sysRole.getDirtyProperties();
       if(dirtyProperties.size() == 0) return "";
        String byColumn = TableBeanMap.getColumnName(sysRole.getClass(), byProperty);
        if(byColumn == null) throw new Exception("byColumn==null");
        StringBuffer sb=new StringBuffer();
        sb.append("update  " + TableBeanMap.getBeanTable(sysRole.getClass()) + split + " set ");
        String[] props = dirtyProperties.keySet().toArray(new String[0]);
        for(int i = 0; i < props.length; i++) {
            String prop = props[i];
            String column = TableBeanMap.getColumnName(TableBeanMap.getBeanTable(sysRole.getClass()), prop);
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
    private String toDeleteSql(String byProperty,SysRole sysRole,String split) throws Exception{
      if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(sysRole);
        String byColumn=TableBeanMap.getColumnName(sysRole.getClass(),byProperty);
        if(byColumn==null) throw new Exception("byColumn==null");
        String sql="delete from "+TableBeanMap.getBeanTable(sysRole.getClass())+split+" where "+byColumn+"=?";
        return sql;
    }
    public void save(SysRole sysRole,boolean withoutTransaction,String split) throws Exception{
             if(sysRole==null) return;
             SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
              String sql=this.toSaveSql(dbconn,sysRole,split);
              if(sql.equals("")) return;
            sysRole.setId((int)dbconn.execute(sql,true));
    }
    public void save(SysRole sysRole,boolean withoutTransaction) throws Exception{
            save(sysRole,withoutTransaction,null);
    }
    public void save(SysRole sysRole,String split) throws Exception{
            if(sysRole==null) return;
             SQLExecutor dbconn= SQLManager.getSQLExecutor();
              String sql=this.toSaveSql(dbconn,sysRole,split);
              if(sql.equals("")) return;
            sysRole.setId((int)dbconn.execute(sql,true));
    }
    public void save(SysRole sysRole) throws Exception{
            save(sysRole,null);
    }
    public int update(SysRole sysRole,String split) throws Exception{
           return update(null,sysRole,split);
    }
    public int update(SysRole sysRole) throws Exception{
           return update(sysRole,null);
    }
     public int update(String byProperty,SysRole sysRole,String split) throws Exception{
        if(sysRole==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor();
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toUpdateSql(dbconn,byProperty,sysRole,split);
        if(sql.equals("")) return 0;
        dbconn.addParam(getPropertyValue(sysRole,byProperty));
        int result= (int)dbconn.execute(sql);
         return result;
    }
    public int update(String byProperty,SysRole sysRole) throws Exception{
        return update(byProperty,sysRole,null);
    }
     public int update(SysRole sysRole,boolean withoutTransaction,String split) throws Exception{
            return update(null,sysRole,withoutTransaction,split);
    }
     public int update(SysRole sysRole,boolean withoutTransaction) throws Exception{
            return update(sysRole,withoutTransaction,null);
    }
    public int update(String byProperty,SysRole sysRole,boolean withoutTransaction,String split) throws Exception{
        if(sysRole==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toUpdateSql(dbconn,byProperty,sysRole,split);
        if(sql.equals("")) return 0;
        dbconn.addParam(getPropertyValue(sysRole,byProperty));
        int result= (int)dbconn.execute(sql);
         return result;
    }
    public int update(String byProperty,SysRole sysRole,boolean withoutTransaction) throws Exception{
       return  update(byProperty,sysRole,withoutTransaction,null);
    }
    public int delete(String byProperty,SysRole sysRole,String split) throws Exception{
          if(sysRole==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor();
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toDeleteSql(byProperty,sysRole,split);
        dbconn.addParam(getPropertyValue(sysRole,byProperty));
        int result=(int)dbconn.execute(sql);
        return result;
    }
    public int delete(String byProperty,SysRole sysRole) throws Exception{
          return  delete(byProperty,sysRole,null);
    }
     public int delete(SysRole sysRole,String split) throws Exception{
          return delete(null,sysRole,split);
    }
    public int delete(SysRole sysRole) throws Exception{
          return delete(sysRole,null);
    }
    public int delete(SysRole sysRole,boolean withoutTransaction,String split) throws Exception{
         return delete(null,sysRole,withoutTransaction,split);
    }
    public int delete(SysRole sysRole,boolean withoutTransaction) throws Exception{
         return delete(sysRole, withoutTransaction,null);
    }
    public int delete(String byProperty,SysRole sysRole,boolean withoutTransaction,String split) throws Exception{
        if(sysRole==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toDeleteSql(byProperty,sysRole,split);
        dbconn.addParam(getPropertyValue(sysRole,byProperty));
        int result=(int)dbconn.execute(sql);
        return result;
    }
    public int delete(String byProperty,SysRole sysRole,boolean withoutTransaction) throws Exception{
        return  delete(byProperty,sysRole,withoutTransaction,null);
    }
     public SysRole findSysRole(Object param) throws Exception{
           return findSysRole(null,param,null);
    }
    public SysRole findSysRole(String byProperty,Object param,String split) throws Exception{
        if(byProperty==null||byProperty.trim().equals("")) byProperty=getDefaultKeyProperty();
        if(split==null) split="";
        split=split.trim();
        String byColumn = TableBeanMap.getColumnName(SysRole.class, byProperty);
        String sql="select * from "+TableBeanMap.getBeanTable(SysRole.class)+split+" where "+byColumn+"=?";
        SQLExecutor dbconn= SQLManager.getSQLExecutor(true);
        dbconn.addParam(param);
        return dbconn.queryForBean(sql,SysRole.class);
    }
    private String getDefaultKeyProperty(){
           return "id";
    }
    private Object getPropertyValue(SysRole sysRole,String propertyName) throws Exception{
    if(propertyName==null||propertyName.equals("")) return null;
           if(propertyName.equalsIgnoreCase("id")){
                return sysRole.getId();
           }
           if(propertyName.equalsIgnoreCase("roleName")){
                return sysRole.getRoleName();
           }
           if(propertyName.equalsIgnoreCase("permission")){
                return sysRole.getPermission();
           }
           if(propertyName.equalsIgnoreCase("createTime")){
                return sysRole.getCreateTime();
           }
           return null;
    }
    public Object mapRow(ResultSet rs, Map<String, String> columns) throws Exception {
        SysRole sysRole=new SysRole();
         Map<String,Object> valueMap=new HashMap<String,Object>();
         String[] lables=columns.keySet().toArray(new String[0]);
         String table=TableBeanMap.getBeanTable(sysRole.getClass());
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
         loadFromMap(valueMap,sysRole);
         valueMap=null;
         return sysRole;
    }
    private void loadFromMap(Map<String,Object> valueMap,SysRole sysRole) throws Exception{
        if(valueMap.containsKey("id")){
             sysRole.setId((java.lang.Integer)valueMap.get("id"));
        }
        if(valueMap.containsKey("roleName")){
             sysRole.setRoleName((java.lang.String)valueMap.get("roleName"));
        }
        if(valueMap.containsKey("permission")){
             sysRole.setPermission((java.lang.String)valueMap.get("permission"));
        }
        if(valueMap.containsKey("createTime")){
             sysRole.setCreateTime((java.util.Date)valueMap.get("createTime"));
        }
    }
    public JSONObject toJSON(SysRole sysRole)  throws Exception{
        if(sysRole==null) return null;
        JSONObject json=new JSONObject();
        SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        json.put("id", sysRole.getId());
        if(sysRole.getRoleName()!=null){
           json.put("roleName",sysRole.getRoleName());
        }
        if(sysRole.getPermission()!=null){
           json.put("permission",sysRole.getPermission());
        }
        if(sysRole.getCreateTime()!=null){
             json.put("createTime",formatter.format(sysRole.getCreateTime()));
        }
        return json;
    }
    public SysRole fromJSON(String str) throws Exception{
       if(str==null||str.trim().equals("")) return null;
       JSONObject json=new JSONObject(str);
       SysRole sysRole=new  SysRole();
       SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
       if(json.get("id")!=null){
             sysRole.setId(Integer.parseInt(json.get("id")+""));
        }
        if(json.get("roleName")!=null){
             sysRole.setRoleName(json.get("roleName")+"");
        }
        if(json.get("permission")!=null){
             sysRole.setPermission(json.get("permission")+"");
        }
        if(json.get("createTime")!=null){
             sysRole.setCreateTime(formatter.parse(json.get("createTime")+""));
        }
        return sysRole;
    }
    public abstract String getSplit(SysRole sysRole) throws Exception;
}
