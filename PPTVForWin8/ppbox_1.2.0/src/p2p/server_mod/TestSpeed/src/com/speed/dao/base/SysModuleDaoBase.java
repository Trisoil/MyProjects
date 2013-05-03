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

import com.speed.bean.SysModule;
public abstract class  SysModuleDaoBase implements RowMapper{
      private String toSaveSql(SQLExecutor dbconn,SysModule sysModule,String split) throws Exception{
       if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(sysModule);
      Map<String,Object> dirtyProperties=sysModule.getDirtyProperties();
      if(dirtyProperties.size() == 0) return "";
        StringBuffer sb=new StringBuffer();
        sb.append("insert into " + TableBeanMap.getBeanTable(sysModule.getClass()) + split + " (");
        String[] props = dirtyProperties.keySet().toArray(new String[0]);
        for(int i = 0; i < props.length; i++) {
            String prop = props[i];
            String column = TableBeanMap.getColumnName(TableBeanMap.getBeanTable(sysModule.getClass()), prop);
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
    private String toUpdateSql(SQLExecutor dbconn,String byProperty,SysModule sysModule,String split) throws Exception{
       if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(sysModule);
       Map<String,Object> dirtyProperties=sysModule.getDirtyProperties();
       if(dirtyProperties.size() == 0) return "";
        String byColumn = TableBeanMap.getColumnName(sysModule.getClass(), byProperty);
        if(byColumn == null) throw new Exception("byColumn==null");
        StringBuffer sb=new StringBuffer();
        sb.append("update  " + TableBeanMap.getBeanTable(sysModule.getClass()) + split + " set ");
        String[] props = dirtyProperties.keySet().toArray(new String[0]);
        for(int i = 0; i < props.length; i++) {
            String prop = props[i];
            String column = TableBeanMap.getColumnName(TableBeanMap.getBeanTable(sysModule.getClass()), prop);
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
    private String toDeleteSql(String byProperty,SysModule sysModule,String split) throws Exception{
      if(split==null) split="";
      if(split.trim().equals("")) split=getSplit(sysModule);
        String byColumn=TableBeanMap.getColumnName(sysModule.getClass(),byProperty);
        if(byColumn==null) throw new Exception("byColumn==null");
        String sql="delete from "+TableBeanMap.getBeanTable(sysModule.getClass())+split+" where "+byColumn+"=?";
        return sql;
    }
    public void save(SysModule sysModule,boolean withoutTransaction,String split) throws Exception{
             if(sysModule==null) return;
             SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
              String sql=this.toSaveSql(dbconn,sysModule,split);
              if(sql.equals("")) return;
            sysModule.setId((int)dbconn.execute(sql,true));
    }
    public void save(SysModule sysModule,boolean withoutTransaction) throws Exception{
            save(sysModule,withoutTransaction,null);
    }
    public void save(SysModule sysModule,String split) throws Exception{
            if(sysModule==null) return;
             SQLExecutor dbconn= SQLManager.getSQLExecutor();
              String sql=this.toSaveSql(dbconn,sysModule,split);
              if(sql.equals("")) return;
            sysModule.setId((int)dbconn.execute(sql,true));
    }
    public void save(SysModule sysModule) throws Exception{
            save(sysModule,null);
    }
    public int update(SysModule sysModule,String split) throws Exception{
           return update(null,sysModule,split);
    }
    public int update(SysModule sysModule) throws Exception{
           return update(sysModule,null);
    }
     public int update(String byProperty,SysModule sysModule,String split) throws Exception{
        if(sysModule==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor();
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toUpdateSql(dbconn,byProperty,sysModule,split);
        if(sql.equals("")) return 0;
        dbconn.addParam(getPropertyValue(sysModule,byProperty));
        int result= (int)dbconn.execute(sql);
         return result;
    }
    public int update(String byProperty,SysModule sysModule) throws Exception{
        return update(byProperty,sysModule,null);
    }
     public int update(SysModule sysModule,boolean withoutTransaction,String split) throws Exception{
            return update(null,sysModule,withoutTransaction,split);
    }
     public int update(SysModule sysModule,boolean withoutTransaction) throws Exception{
            return update(sysModule,withoutTransaction,null);
    }
    public int update(String byProperty,SysModule sysModule,boolean withoutTransaction,String split) throws Exception{
        if(sysModule==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toUpdateSql(dbconn,byProperty,sysModule,split);
        if(sql.equals("")) return 0;
        dbconn.addParam(getPropertyValue(sysModule,byProperty));
        int result= (int)dbconn.execute(sql);
         return result;
    }
    public int update(String byProperty,SysModule sysModule,boolean withoutTransaction) throws Exception{
       return  update(byProperty,sysModule,withoutTransaction,null);
    }
    public int delete(String byProperty,SysModule sysModule,String split) throws Exception{
          if(sysModule==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor();
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toDeleteSql(byProperty,sysModule,split);
        dbconn.addParam(getPropertyValue(sysModule,byProperty));
        int result=(int)dbconn.execute(sql);
        return result;
    }
    public int delete(String byProperty,SysModule sysModule) throws Exception{
          return  delete(byProperty,sysModule,null);
    }
     public int delete(SysModule sysModule,String split) throws Exception{
          return delete(null,sysModule,split);
    }
    public int delete(SysModule sysModule) throws Exception{
          return delete(sysModule,null);
    }
    public int delete(SysModule sysModule,boolean withoutTransaction,String split) throws Exception{
         return delete(null,sysModule,withoutTransaction,split);
    }
    public int delete(SysModule sysModule,boolean withoutTransaction) throws Exception{
         return delete(sysModule, withoutTransaction,null);
    }
    public int delete(String byProperty,SysModule sysModule,boolean withoutTransaction,String split) throws Exception{
        if(sysModule==null) return 0;
        SQLExecutor dbconn= SQLManager.getSQLExecutor(withoutTransaction);
        if(byProperty==null||byProperty.equals("")){
            byProperty=getDefaultKeyProperty();
        }
        String sql=this.toDeleteSql(byProperty,sysModule,split);
        dbconn.addParam(getPropertyValue(sysModule,byProperty));
        int result=(int)dbconn.execute(sql);
        return result;
    }
    public int delete(String byProperty,SysModule sysModule,boolean withoutTransaction) throws Exception{
        return  delete(byProperty,sysModule,withoutTransaction,null);
    }
     public SysModule findSysModule(Object param) throws Exception{
           return findSysModule(null,param,null);
    }
    public SysModule findSysModule(String byProperty,Object param,String split) throws Exception{
        if(byProperty==null||byProperty.trim().equals("")) byProperty=getDefaultKeyProperty();
        if(split==null) split="";
        split=split.trim();
        String byColumn = TableBeanMap.getColumnName(SysModule.class, byProperty);
        String sql="select * from "+TableBeanMap.getBeanTable(SysModule.class)+split+" where "+byColumn+"=?";
        SQLExecutor dbconn= SQLManager.getSQLExecutor(true);
        dbconn.addParam(param);
        return dbconn.queryForBean(sql,SysModule.class);
    }
    private String getDefaultKeyProperty(){
           return "id";
    }
    private Object getPropertyValue(SysModule sysModule,String propertyName) throws Exception{
    if(propertyName==null||propertyName.equals("")) return null;
           if(propertyName.equalsIgnoreCase("id")){
                return sysModule.getId();
           }
           if(propertyName.equalsIgnoreCase("moduleID")){
                return sysModule.getModuleID();
           }
           if(propertyName.equalsIgnoreCase("moduleName")){
                return sysModule.getModuleName();
           }
           if(propertyName.equalsIgnoreCase("parentID")){
                return sysModule.getParentID();
           }
           if(propertyName.equalsIgnoreCase("link")){
                return sysModule.getLink();
           }
           if(propertyName.equalsIgnoreCase("linkClass")){
                return sysModule.getLinkClass();
           }
           if(propertyName.equalsIgnoreCase("status")){
                return sysModule.getStatus();
           }
           if(propertyName.equalsIgnoreCase("memo")){
                return sysModule.getMemo();
           }
           if(propertyName.equalsIgnoreCase("createTime")){
                return sysModule.getCreateTime();
           }
           return null;
    }
    public Object mapRow(ResultSet rs, Map<String, String> columns) throws Exception {
        SysModule sysModule=new SysModule();
         Map<String,Object> valueMap=new HashMap<String,Object>();
         String[] lables=columns.keySet().toArray(new String[0]);
         String table=TableBeanMap.getBeanTable(sysModule.getClass());
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
         loadFromMap(valueMap,sysModule);
         valueMap=null;
         return sysModule;
    }
    private void loadFromMap(Map<String,Object> valueMap,SysModule sysModule) throws Exception{
        if(valueMap.containsKey("id")){
             sysModule.setId((java.lang.Integer)valueMap.get("id"));
        }
        if(valueMap.containsKey("moduleID")){
             sysModule.setModuleID((java.lang.String)valueMap.get("moduleID"));
        }
        if(valueMap.containsKey("moduleName")){
             sysModule.setModuleName((java.lang.String)valueMap.get("moduleName"));
        }
        if(valueMap.containsKey("parentID")){
             sysModule.setParentID((java.lang.String)valueMap.get("parentID"));
        }
        if(valueMap.containsKey("link")){
             sysModule.setLink((java.lang.String)valueMap.get("link"));
        }
        if(valueMap.containsKey("linkClass")){
             sysModule.setLinkClass((java.lang.String)valueMap.get("linkClass"));
        }
        if(valueMap.containsKey("status")){
             sysModule.setStatus((java.lang.Integer)valueMap.get("status"));
        }
        if(valueMap.containsKey("memo")){
             sysModule.setMemo((java.lang.String)valueMap.get("memo"));
        }
        if(valueMap.containsKey("createTime")){
             sysModule.setCreateTime((java.util.Date)valueMap.get("createTime"));
        }
    }
    public JSONObject toJSON(SysModule sysModule)  throws Exception{
        if(sysModule==null) return null;
        JSONObject json=new JSONObject();
        SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        json.put("id", sysModule.getId());
        if(sysModule.getModuleID()!=null){
           json.put("moduleID",sysModule.getModuleID());
        }
        if(sysModule.getModuleName()!=null){
           json.put("moduleName",sysModule.getModuleName());
        }
        if(sysModule.getParentID()!=null){
           json.put("parentID",sysModule.getParentID());
        }
        if(sysModule.getLink()!=null){
           json.put("link",sysModule.getLink());
        }
        if(sysModule.getLinkClass()!=null){
           json.put("linkClass",sysModule.getLinkClass());
        }
        json.put("status", sysModule.getStatus());
        if(sysModule.getMemo()!=null){
           json.put("memo",sysModule.getMemo());
        }
        if(sysModule.getCreateTime()!=null){
             json.put("createTime",formatter.format(sysModule.getCreateTime()));
        }
        return json;
    }
    public SysModule fromJSON(String str) throws Exception{
       if(str==null||str.trim().equals("")) return null;
       JSONObject json=new JSONObject(str);
       SysModule sysModule=new  SysModule();
       SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
       if(json.get("id")!=null){
             sysModule.setId(Integer.parseInt(json.get("id")+""));
        }
        if(json.get("moduleID")!=null){
             sysModule.setModuleID(json.get("moduleID")+"");
        }
        if(json.get("moduleName")!=null){
             sysModule.setModuleName(json.get("moduleName")+"");
        }
        if(json.get("parentID")!=null){
             sysModule.setParentID(json.get("parentID")+"");
        }
        if(json.get("link")!=null){
             sysModule.setLink(json.get("link")+"");
        }
        if(json.get("linkClass")!=null){
             sysModule.setLinkClass(json.get("linkClass")+"");
        }
       if(json.get("status")!=null){
             sysModule.setStatus(Integer.parseInt(json.get("status")+""));
        }
        if(json.get("memo")!=null){
             sysModule.setMemo(json.get("memo")+"");
        }
        if(json.get("createTime")!=null){
             sysModule.setCreateTime(formatter.parse(json.get("createTime")+""));
        }
        return sysModule;
    }
    public abstract String getSplit(SysModule sysModule) throws Exception;
}
